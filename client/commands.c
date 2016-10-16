#include <stdlib.h>
#include "commands.h"


void command_exit(int sockfd, int argc, char* const* argv);
void command_ls(int sockfd, int argc, char* const* argv);
void command_put(int sockfd, int argc, char* const* argv);
void command_quit(int sockfd, int argc, char* const* argv);
void command_send(int sockfd, int argc, char* const* argv);

command_t ctrsh_commands[] =
{
   {"exit", command_exit},
   {"ls", command_ls},
   {"put", command_put},
   {"quit", command_quit},
   {"send", command_send},
   {NULL}
};

