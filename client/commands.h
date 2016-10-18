#ifndef COMMANDS_H
#define COMMANDS_H

#include "utils/options.h"

typedef void(*command_fn_t)(char* const* options);

typedef struct
{
   const char* name;
   command_fn_t fn;
   option_t* options;
} command_t;

extern command_t ctrsh_commands[];

void execute_command(char* line);

#endif // COMMANDS_H
