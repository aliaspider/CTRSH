#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <readline/readline.h>

#include "common.h"

void rl_vprintf(const char* fmt, va_list va)
{
   int rl_point_org;
   char* rl_text_org = NULL;

   if ((rl_readline_state & RL_STATE_READCMD) > 0)
   {
      rl_text_org = rl_copy_text(0, rl_end);
      rl_point_org = rl_point;
      rl_save_prompt();
      rl_replace_line("", 0);
      rl_redisplay();
   }

   vprintf(fmt, va);

   if(rl_text_org)
   {
      rl_restore_prompt();
      rl_replace_line(rl_text_org, 0);
      rl_point = rl_point_org;
      rl_redisplay();
      free(rl_text_org);
   }
}

void rl_vprintf_color(const char* color, const char* fmt, va_list va)
{
   int new_len = strlen(fmt) + strlen(color) + strlen(KNRM);
   char* fmt_new = malloc (new_len + 1);

   char* dst = fmt_new;
   const char* src;

   src = color;
   while(*src)
      *dst++ = *src++;

   src = fmt;
   while(*src)
      *dst++ = *src++;

   src = KNRM;
   while(*src)
      *dst++ = *src++;

   *dst = 0;

   rl_vprintf(fmt_new, va);

   free(fmt_new);

}

void rl_vprintf_ex(const char* color, const char* prefix, const char* fmt, va_list va)
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

   rl_vprintf(fmt_new, va);

   free(fmt_new);

}

void rl_printf_ex(const char* color, const char* prefix, const char* fmt, ...)
{
   va_list va;
   va_start(va, fmt);
   rl_vprintf_ex(color, prefix, fmt, va);
   va_end(va);

}

void rl_printf(const char* fmt, ...)
{
   va_list va;
   va_start(va, fmt);
   rl_vprintf(fmt, va);
   va_end(va);
}

void rl_printf_info(const char* fmt, ...)
{
   va_list va;
   va_start(va, fmt);
   rl_vprintf_color(ctrsh.console.colors.info, fmt, va);
   va_end(va);
}

void rl_printf_error(const char* fmt, ...)
{
   va_list va;
   va_start(va, fmt);
   rl_vprintf_color(ctrsh.console.colors.error, fmt, va);
   va_end(va);
}

void rl_printf_debug(const char* fmt, ...)
{
#ifndef NDEBUG
   va_list va;
   va_start(va, fmt);
   rl_vprintf_color(ctrsh.console.colors.debug, fmt, va);
   va_end(va);
#endif
}
