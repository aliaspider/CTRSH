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
   int rl_point_org = rl_point;
   char* rl_text_org = rl_copy_text(0, rl_end);

   rl_save_prompt();
   rl_replace_line("", 0);
   rl_redisplay();

   vprintf(fmt, va);

   rl_restore_prompt();
   rl_replace_line(rl_text_org, 0);
   rl_point = rl_point_org;
   rl_redisplay();

   free(rl_text_org);
}

int rl_printf_ex(const char* color, const char* prefix, const char* fmt, ...)
{
   int new_len = strlen(fmt);
   if(color)
      new_len += strlen(color) + strlen(KNRM);

   const char* src = fmt;

   if(prefix)
      new_len += strlen(prefix);

   while (prefix && *src)
   {
      if(*src == '\n')
         new_len += strlen(prefix);
      src++;
   }

   char* fmt_new = malloc (new_len + 1);
   char* dst = fmt_new;
   if(color)
   {
      strcpy(dst, color);
      dst += strlen(color);
   }
   if(prefix)
   {
      strcpy(dst, prefix);
      dst += strlen(prefix);
   }

   src = fmt;

   while(*src)
   {
      *dst++ = *src++;
      if(prefix && src[-1] == '\n' && *src)
      {
         strcpy(dst, prefix);
         dst += strlen(prefix);
      }
   }

   *dst = 0;
   if(color)
      strcpy(dst, KNRM);


   va_list va;
   va_start(va, fmt);
   rl_vprintf(fmt_new, va);
   va_end(va);

   free(fmt_new);
}
