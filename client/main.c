
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <netdb.h>
#include <errno.h>
#include <sys/cdefs.h>
#include <limits.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdbool.h>
#include <getopt.h>
#include <arpa/inet.h>

#include "common.h"
#include "commands.h"
#include "serverctrl/server_ctrl.h"
#include "utils/file_list.h"
#include "utils/c_str.h"

#define CTR_IP          IP2INT(10, 42, 0, 237)
//#define CTR_IP          IP2INT(192, 168, 2, 240)
#define CTR_PORT        12000
#define HISTORY_FILE    ".ctrsh.hist"

ctrsh_t ctrsh;

char* ctrsh_completion_commands(const char* text, int id);

static char** ctrsh_completion_function(const char* str, int start, int end)
{
   int i;
   (void)end;
   for(i = 0; i < start; i++)
      if(!isspace(rl_line_buffer[i]))
         return rl_completion_matches(str, rl_filename_completion_function);

   return rl_completion_matches(str, ctrsh_completion_commands);

}


option_def_t main_options_defs[] =
{
   {'a', true,  "address", "server address"},
   {'p', true,  "port",    "server port"},
   {'s', true,  "history", "set history file"},
   {'x', true,  "3dsx",    "3dsx file to send with 3dslink before connecting (optional)"},
   {'v', false, "version", "print version"},
   {0}
};

typedef struct
{
   const char* address;
   const char* port;
   const char* history_file;
   const char* _3dsx;
   const char* version;
}main_options_t;

int main(int argc, char* argv[])
{
   opterr = 0;

   ctrsh.server.ip = CTR_IP;
   ctrsh.server.port = CTR_PORT;
   ctrsh.console.colors.info = KNRM;
   ctrsh.console.colors.error = KLRD;
#ifndef NDEBUG
   ctrsh.console.colors.debug = KMAG;
#endif


   main_options_t* opts = (main_options_t*) parse_options(argc, argv,  main_options_defs);

   if(!opts)
      return 0;

   ctrsh.console.colors.info = KLBL;

   if(opts->version)
   {
      printf("CTRSH version 0.01\n");
      return 0;
   }

   if(opts->history_file && *opts->history_file)
      strncpy(ctrsh.history_file, opts->history_file, sizeof(ctrsh.history_file));
   else
   {
      const char* home_path = getenv("HOME");

      if (home_path)
         snprintf(ctrsh.history_file, sizeof(ctrsh.history_file), "%s/" HISTORY_FILE, home_path);
      else
         strncpy(ctrsh.history_file, HISTORY_FILE, sizeof(ctrsh.history_file));
   }

   if(opts->address)
   {
      ctrsh.server.ip = inet_addr(opts->address);

      if (ctrsh.server.ip == -1)
      {
         rl_printf_error("invalid ip : %s\n", opts->address);
         return 1;
      }
   }

   if(opts->port)
   {
      ctrsh.server.port = atoi(opts->port);

      if (!ctrsh.server.port || ctrsh.server.port  > 0xFFFF)
      {
         rl_printf_error("invalid port number : %s\n", opts->port);
         return 1;
      }
   }

   if(opts->_3dsx)
      run_server_3dsx(opts->_3dsx);

   free(opts);

   read_history(ctrsh.history_file);
   history_set_pos(history_length);
   rl_attempted_completion_function = ctrsh_completion_function;

   ctrsh.running = true;

   while (ctrsh.running)
   {
      server_connect();

      while (ctrsh.server.connected)
      {
         char* line_buffer = readline("test:/> ");
         char* line = clean_whitespace(line_buffer);

         if (*line)
         {
            add_history(line);
            rl_printf_debug("executing command : %s\n", line);
            execute_command(line);
         }
         free(line_buffer);
      }

      sleep(1);
   }

   write_history(ctrsh.history_file);

   return 0;
}
