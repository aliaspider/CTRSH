
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
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
#define DEBUG_VAR(X) printf( #X" : 0x%08lX\n", (uint32_t)(X))

#define CTR_IP          IP2INT(10, 42, 0, 237)
#define CTR_PORT        5000
#define HISTORY_FILE    "ctrsh.hist"
#define OFFLINE_TEST



#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
#define KGRY  "\x1B[90m"
#define KLRD  "\x1B[91m"
#define KLGR  "\x1B[92m"
#define KLYL  "\x1B[93m"
#define KLBL  "\x1B[94m"
#define KLPR  "\x1B[95m"
#define KTRQ  "\x1B[96m"

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

typedef struct
{
   ctrsh_dirent** entries;
   int count;
}ctrsh_dirent_list_t;


struct
{
   void* buffer;
   size_t size;
} rgui;

static void ctrsh_dirent_list_new(ctrsh_dirent* first, ctrsh_dirent_list_t* out)
{
   int i;
   ctrsh_dirent* dir = first;
   out->count = 0;
   while(dir->entry_size)
   {
      dir = (ctrsh_dirent*)((uintptr_t)dir + dir->entry_size);
      out->count++;
   }

   out->entries = malloc(out->count * sizeof(ctrsh_dirent*));

   dir = first;
   for(i = 0; i < out->count; i++)
   {
      out->entries[i] = dir;
      dir = (ctrsh_dirent*)((uintptr_t)dir + dir->entry_size);
   }
}

static void ctrsh_dirent_list_sort(ctrsh_dirent_list_t* list)
{
   int i;
   bool finished = false;
   while(!finished)
   {
      finished = true;
      for(i = 0; i < list->count - 1; i++)
      {
         if(strcasecmp(list->entries[i]->name, list->entries[i + 1]->name) > 0)
         {
            ctrsh_dirent* tmp = list->entries[i];
            list->entries[i] = list->entries[i + 1];
            list->entries[i + 1] = tmp;
            finished = false;
         }
      }
   }
}

static inline int send_command(int socket, uint32_t command_id)
{
   return write(socket, &command_id, 4);
}

void command_send(int sockfd, const char* args)
{
   printf("executing send with argument %s\n", args);

   printf("sending file\n");
#ifndef OFFLINE_TEST
   DEBUG_ERROR(send_command(sockfd, CTRSH_COMMAND_DISPLAY_IMAGE));
   DEBUG_ERROR(write(sockfd, &rgui.size, 4));
   DEBUG_ERROR(write(sockfd, rgui.buffer, rgui.size));
#endif
}
void command_ls(int sockfd, const char* args)
{
   int i, j;
   printf("executing ls with argument %s\n", args);
#ifndef OFFLINE_TEST
   DEBUG_ERROR(send_command(sockfd, CTRSH_COMMAND_DIRENT));
   uint32_t buffer_size;
   DEBUG_ERROR(read(sockfd, &buffer_size, 4));

   if (!buffer_size)
      return;

   uint8_t* buffer = malloc(buffer_size);
   ssize_t bytes_read = 0;

   while (bytes_read < buffer_size)
   {
      ssize_t ret = read(sockfd, buffer + bytes_read, buffer_size - bytes_read);
      DEBUG_ERROR(ret);
      bytes_read += ret;
   }

#if 1
   FILE* fp = fopen("dirent_test.bin", "wb");
   fwrite(buffer, 1, buffer_size, fp);
   fclose(fp);
#endif
#else
   FILE* fp = fopen("dirent_test.bin", "rb");
   fseek(fp, 0, SEEK_END);
   uint32_t buffer_size = ftell(fp);
   uint8_t* buffer = malloc(buffer_size);
   fseek(fp, 0, SEEK_SET);
   fread(buffer, 1, buffer_size, fp);
   fclose(fp);
#endif

   ctrsh_dirent_list_t dir_list;
   ctrsh_dirent_list_new((ctrsh_dirent*)buffer, &dir_list);
   ctrsh_dirent_list_sort(&dir_list);

#if 1
   int term_w;
   rl_get_screen_size(NULL, &term_w);

   const int col_spacing = 2;
   const int max_colums = 20;
   int col_widths[max_colums];
   int col_entries[max_colums];

   int dir;
   int w_remaining;
   int columns = max_colums;

   do
   {
      dir = 0;
      w_remaining = term_w;
      for (i = 0; i < columns; i++)
         col_entries[i] = dir_list.count / columns;
      for (i = 0; i < (dir_list.count % columns); i++)
         col_entries[i]++;

      for (i = 0; i < columns; i++)
      {
         col_widths[i] = 0;
         for(j = 0; j < col_entries[i]; j++)
         {
            if(col_widths[i] < dir_list.entries[dir]->mbslen)
               col_widths[i] = dir_list.entries[dir]->mbslen;
            dir++;
         }
         w_remaining -= col_widths[i] + col_spacing;
         if(w_remaining < 0)
            break;
      }
   }
   while((w_remaining < 0) && (--columns > 1));
   
   int k;
   for(j = 0; j < col_entries[0]; j++)
   {
      dir = 0;
      for(k = 0; k < j; k++)
         dir++;

      for (i = 0; (i < columns) && (dir < dir_list.count); i++)
      {
         if (dir_list.entries[dir]->is_directory)
            printf(KLYL "%s" KNRM, dir_list.entries[dir]->name);
         else
            printf("%s",dir_list.entries[dir]->name);

         for(k = 0; k < (col_widths[i] + col_spacing - dir_list.entries[dir]->mbslen); k++)
            putchar(' ');
         for(k = 0; k < col_entries[i]; k++)
            dir++;
      }
      printf("\n");
   }

   
   
#else

   dir = (ctrsh_dirent*)buffer;

   while (dir->entry_size)
   {
      if (dir->is_directory)
         printf(" ----------  ##  " KLYL "%s \n" KNRM, dir->name);

      else
         printf(" %10lli  ##  %s \n", dir->file_size, dir->name);

      dir = (ctrsh_dirent*)((uintptr_t)dir + dir->entry_size);
   }

#endif




   free(dir_list.entries);
   free(buffer);

}

void command_put(int sockfd, const char* args)
{
   printf("executing put with argument %s\n", args);
#ifndef OFFLINE_TEST
   DEBUG_ERROR(send_command(sockfd, CTRSH_COMMAND_PUT));
   uint32_t filesize = 0x1000000;
   DEBUG_ERROR(write(sockfd, &filesize, 4));
   void* buffer =  malloc(filesize);
   DEBUG_ERROR(write(sockfd, buffer, filesize));
   free(buffer);
#endif
}

void command_exit(int sockfd, const char* args)
{
   printf("executing exit with argument %s\n", args);
#ifndef OFFLINE_TEST
   DEBUG_ERROR(send_command(sockfd, CTRSH_COMMAND_EXIT));
#endif
   ctrsh.server_running = false;
}

void command_quit(int sockfd, const char* args)
{
   printf("executing exit with argument %s\n", args);
#ifndef OFFLINE_TEST
   DEBUG_ERROR(send_command(sockfd, CTRSH_COMMAND_EXIT));
#endif
   ctrsh.server_running = false;
   ctrsh.running = false;

}

command_t ctrsh_commands[] =
{
   {"send", command_send},
   {"ls", command_ls},
   {"put", command_put},
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
#if 0

   while (true)
   {
      int sockfd,  n;
      struct sockaddr_in serv_addr = {0};
      DEBUG_ERROR(sockfd = socket(AF_INET, SOCK_STREAM, 0));
      int sockopt_val = 0x40000;
      setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &sockopt_val, 4);
      setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &sockopt_val, 4);

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

      int i;

      for (i = 0; i < 20; i++)
      {
         DEBUG_ERROR(send_command(sockfd, CTRSH_COMMAND_DISPLAY_IMAGE));
         DEBUG_ERROR(write(sockfd, &rgui.size, 4));
         DEBUG_ERROR(write(sockfd, rgui.buffer, rgui.size));
      }


      //      sleep(3);
      DEBUG_ERROR(send_command(sockfd, CTRSH_COMMAND_EXIT));

      close(sockfd);
      sleep(1);
   }

#else
   ctrsh.running = true;

   while (ctrsh.running)
   {
      ctrsh.server_running = true;
      int sockfd,  n;
      struct sockaddr_in serv_addr = {0};
      DEBUG_ERROR(sockfd = socket(AF_INET, SOCK_STREAM, 0));
      int sockopt_val = 0x40000;
      setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &sockopt_val, 4);
      setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &sockopt_val, 4);

      serv_addr.sin_family = AF_INET;
      serv_addr.sin_addr.s_addr = CTR_IP;
      serv_addr.sin_port = htons(CTR_PORT);
      int ret;
#ifndef OFFLINE_TEST
      do
      {
         ret = connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

         //          DEBUG_ERROR_stay(ret);
         if (ret < 0)
            usleep(10000);
      }
      while (ret < 0);
#endif
      while (ctrsh.server_running)
      {
         char* line = readline("test:/> ");

         if (line && *line)
            add_history(line);

         printf("executing command : %s\n", line);
         command_t* cmd = ctrsh_commands;

         while (cmd->name)
         {
            if (!strncmp(line, cmd->name, strlen(cmd->name)))
            {
               cmd->fn(sockfd,
                       strlen(line) > strlen(cmd->name) ? line + strlen(cmd->name) + 1 : NULL);
               break;
            }

            cmd++;
         }

         free(line);
      }

      close(sockfd);
#ifndef OFFLINE_TEST
      sleep(1);
#endif
   }

#endif
   free(rgui.buffer);
   write_history(ctrsh.history_file);

   return 0;
}
