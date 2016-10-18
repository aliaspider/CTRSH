#include "c_str.h"

#include <ctype.h>

char* clean_whitespace(char* str)
{
   char* line = str;
   char* last = line;

   while (*line)
   {
      if (!isspace(*line))
         last = line + 1;

      line++;
   }

   *last = '\0';

   line = str;

   while (isspace(*line))
      line++;

   return line;
}
