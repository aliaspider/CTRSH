#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <readline/readline.h>

#include "common.h"


int rl_printf(const char* fmt, ...)
{
   va_list va;
   va_start(va, fmt);
   rl_vprintf(fmt, va);
   va_end(va);
}

int rl_vprintf(const char* fmt, va_list va)
{
   char spaces[256];
   int count = strlen(rl_prompt) + strlen(rl_line_buffer);

   if (count > 254)
      count = 254;

   memset(spaces, ' ', count);
   spaces[count] = '\0';
   printf("\r%s\r", spaces);
   vprintf(fmt, va);
   printf("%s%s", rl_prompt, rl_line_buffer);
}
