#ifndef COMMANDS_H
#define COMMANDS_H

typedef void(*command_fn_t)(int sockfd, int argc, char* const* argv);
typedef struct
{
   const char* name;
   command_fn_t fn;
} command_t;

extern command_t ctrsh_commands[];

#endif // COMMANDS_H
