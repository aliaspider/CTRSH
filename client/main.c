
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <sys/cdefs.h>
#include <limits.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdarg.h>
#include <stdbool.h>

#include "../server/common.h"
#define IP2INT(a, b, c, d) (a | (b << 8) | (c << 16) | (d <<24))
#define DEBUG_ERROR(X) do{int res_ = (int)(intptr_t)(X); if(res_ < 0){printf("error %i @%s (%s:%d).\n%s\n", res_, __FUNCTION__, __FILE__, __LINE__,strerror(errno)); exit(0);}}while(0)
#define DEBUG_ERROR_stay(X) do{int res_ = (int)(intptr_t)(X); if(res_ < 0){printf("error %i @%s (%s:%d).\n%s\n", res_, __FUNCTION__, __FILE__, __LINE__,strerror(errno));}}while(0)

#define CTR_IP          IP2INT(10, 42, 0, 237)
#define CTR_PORT        5000
#define HISTORY_FILE    "ctrsh.hist"




typedef void(*command_fn_t)(int sockfd, const char*);
typedef struct
{
   const char* name;
   command_fn_t fn;
} command_t;


struct
{
   char history_file[PATH_MAX];
   bool running;
   bool server_running;
} ctrsh;



struct
{
   void* buffer;
   size_t size;
}rgui;

static inline int send_command(int socket, uint32_t command_id)
{
   return write(socket, &command_id, 4);
}

void command_send(int sockfd, const char* args)
{
   printf("executing send with argument %s\n", args);

   printf("sending file\n");

   DEBUG_ERROR(send_command(sockfd, CTRSH_COMMAND_DISPLAY_IMAGE));
   DEBUG_ERROR(write(sockfd, &rgui.size, 4));
   DEBUG_ERROR(write(sockfd, rgui.buffer, rgui.size));

}
void command_ls(int sockfd, const char* args)
{
   int i;
   printf("executing ls with argument %s\n", args);
   DEBUG_ERROR(send_command(sockfd, CTRSH_COMMAND_DIRENT));
   int dircount;
   char dirname[0x200];
   DEBUG_ERROR(read(sockfd, &dircount, 4));

   for (i = 0; i < dircount; i++)
   {
      int len;
      DEBUG_ERROR(read(sockfd, &len, 4));
      DEBUG_ERROR(read(sockfd, &dirname, len));
      printf("%s\n", dirname);
   }

}
void command_exit(int sockfd, const char* args)
{
   printf("executing exit with argument %s\n", args);
   DEBUG_ERROR(send_command(sockfd, CTRSH_COMMAND_EXIT));
   ctrsh.server_running = false;
}

void command_quit(int sockfd, const char* args)
{
   printf("executing exit with argument %s\n", args);
   DEBUG_ERROR(send_command(sockfd, CTRSH_COMMAND_EXIT));
   ctrsh.server_running = false;
   ctrsh.running = false;

}

command_t ctrsh_commands[] =
{
   {"send", command_send},
   {"ls", command_ls},
   {"exit", command_exit},
   {"quit", command_quit},
   {NULL}
};


char* ctrsh_completion_commands(const char* text, int id)
{
   static int index = 0;

   if (id == 0)
      index = 0;

   while (ctrsh_commands[index].name)
   {
      if (!strncmp(text, ctrsh_commands[index].name, strlen(text)))
         return strdup(ctrsh_commands[index++].name);

      index++;
   }

   return NULL;
}



static int rl_printf(const char* fmt, ...)
{
   va_list va;
   char spaces[256];
   int count = strlen(rl_prompt) + strlen(rl_line_buffer);

   if (count > 254)
      count = 254;

   memset(spaces, ' ', count);
   spaces[count] = '\0';
   printf("\r%s\r", spaces);
   va_start(va, fmt);
   vprintf(fmt, va);
   va_end(va);
   printf("%s%s", rl_prompt, rl_line_buffer);
}


static char** ctrsh_completion_function(const char* str, int start, int end)
{
   if (start == 0)
      return rl_completion_matches(str, ctrsh_completion_commands);

   rl_completion_matches(str, rl_filename_completion_function);
}

int main(int argc, char* argv[])
{
   printf("loading file\n");
   FILE* rgui_fp = fopen("./rgui.dat", "rb");
   DEBUG_ERROR(rgui_fp);
   fseek(rgui_fp, 0, SEEK_END);
   rgui.size = ftell(rgui_fp);
   rgui.buffer = malloc(rgui.size);
   fseek(rgui_fp, 0, SEEK_SET);
   fread(rgui.buffer, 1, rgui.size, rgui_fp);
   fclose(rgui_fp);

   //    *(int*)0 = 0;
   const char* home_path = getenv("HOME");

   if (home_path)
      snprintf(ctrsh.history_file, sizeof(ctrsh.history_file), "%s/" HISTORY_FILE, home_path);
   else
      strncpy(ctrsh.history_file, HISTORY_FILE, sizeof(ctrsh.history_file));

   read_history(ctrsh.history_file);
   history_set_pos(history_length);


   rl_attempted_completion_function = ctrsh_completion_function;

   //    rl_callback_handler_install("", cli_handler);
   //    rl_basic_word_break_characters = "\t\n ";
   //    rl_completer_word_break_characters = "\t\n ";


   ctrsh.running = true;
   while (ctrsh.running)
   {
      ctrsh.server_running = true;
      int sockfd,  n;
      struct sockaddr_in serv_addr = {0};
      DEBUG_ERROR(sockfd = socket(AF_INET, SOCK_STREAM, 0));
      int sockopt_val = 0x40000;
      setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &sockopt_val, 4);

      serv_addr.sin_family = AF_INET;
      serv_addr.sin_addr.s_addr = CTR_IP;
      serv_addr.sin_port = htons(CTR_PORT);
      int ret;

      do
      {
         ret = connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

         //          DEBUG_ERROR_stay(ret);
         if (ret < 0)
            usleep(10000);
      }
      while (ret < 0);

      while (ctrsh.server_running)
      {
         char* line = readline("test:/> ");
         if(line && *line)
            add_history(line);
         printf("executing command : %s\n", line);
         command_t* cmd = ctrsh_commands;

         while (cmd->name)
         {
            if (!strncmp(line, cmd->name, strlen(cmd->name)))
            {
               cmd->fn(sockfd, line + strlen(cmd->name) + 1);
               break;
            }

            cmd++;
         }

         free(line);
      }

      close(sockfd);
      sleep(1);
   }

   free(rgui.buffer);
   write_history(ctrsh.history_file);

   return 0;
}
