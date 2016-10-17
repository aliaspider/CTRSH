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
   return vprintf(fmt, va);

   char spaces[256];
   int count = strlen(rl_prompt) + rl_end;

   if (count > 254)
      count = 254;

   memset(spaces, ' ', count);
   spaces[count] = '\0';
   printf("\r%s\r", spaces);
   vprintf(fmt, va);
   puts(rl_prompt);
   if(rl_end && rl_line_buffer)
      puts(rl_line_buffer);
}
