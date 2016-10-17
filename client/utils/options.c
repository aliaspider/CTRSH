
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <assert.h>
#include "options.h"
#include "common.h"

char** parse_options(int argc, char **argv, option_t* options)
{

   option_t* opt = options;
   int count = 0;

   while(options && options[count].id)
      count++;

   optind = 1;
   optopt = 0;
   optarg = NULL;

   char **vals = calloc(count + argc, sizeof(*vals));

   if(options)
   {
      char short_opts[(count * 2) + 1];
      char* s_ptr = short_opts;
      struct option long_opts[count + 1];
      struct option* l_ptr = long_opts;
      while(opt->id)
      {
         assert(opt->id != 'h');
         assert(opt->id != '?');
         *s_ptr++ = opt->id;
         if(opt->has_arg)
            *s_ptr++ = ':';

         if(opt->id_long)
         {
            l_ptr->name = opt->id_long;
            l_ptr->has_arg = opt->has_arg;
            l_ptr->flag = NULL;
            l_ptr->val = opt->id;
            l_ptr++;
         }
         opt++;
      }
      *s_ptr = 0;
      memset(l_ptr, 0, sizeof(*l_ptr));

      int opt_id;
      while ((opt_id = getopt_long(argc, argv, short_opts, long_opts, NULL)) != -1)
      {
         int i = 0;
         while(i < count && options[i].id != opt_id)
            i++;

         if(i == count)
         {
            free(vals);
            return NULL;
         }

         rl_printf("found option :%c\n", opt_id);
         if(options[i].has_arg)
         {
            if(!optarg)
            {
               printf("required argument missing for option %c", options[i].id);
               if(options[i].id_long)
                  printf("[%s]", options[i].id_long);
               printf("\n");
               free(vals);
               return NULL;
            }
            vals[i] = optarg;
         }
         else
            vals[i] = "";
      }
   }

   argc -= optind;
   argv += optind;
   memcpy(vals + count, argv, argc * sizeof(*vals));
   vals[count + argc] = NULL;

   return vals;
}
