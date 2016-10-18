
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

#ifdef __GLIBC__
   optind = 0;
#else
   optind = 1;
#endif
   optopt = 0;
   optarg = NULL;

   char **vals = calloc(count + argc, sizeof(*vals));

   char short_opts[1 + (count * 2) + 3 + 1];
   char* s_ptr = short_opts;
   struct option long_opts[count + 2];
   struct option* l_ptr = long_opts;
   *s_ptr++ = ':';
   while(opt && opt->id)
   {
      assert(opt->id != 'h');
      assert(opt->id != '?');
      *s_ptr++ = opt->id;
      if(opt->has_arg)
         *s_ptr++ = ':';

      if(opt->id_long)
      {
         l_ptr->name = opt->id_long;
         l_ptr->has_arg = !!opt->has_arg;
         l_ptr->flag = NULL;
         l_ptr->val = opt->id;
         l_ptr++;
      }
      opt++;
   }
   *s_ptr++ = 'h';
   *s_ptr++ = ':';
   *s_ptr++ = ':';
   *s_ptr = 0;
   l_ptr->name = "help";
   l_ptr->has_arg = optional_argument;
   l_ptr->flag = NULL;
   l_ptr->val = 'h';
   l_ptr++;
   memset(l_ptr, 0, sizeof(*l_ptr));

   int opt_id;
   while ((opt_id = getopt_long(argc, argv, short_opts, long_opts, NULL)) != -1)
   {
      rl_printf_debug("found option :%c\n", opt_id);

      if(opt_id == 'h')
         goto print_help;

      if(opt_id == ':')
      {
         rl_printf_error("missing argument for option :%s\n", argv[optind - 1]);
         goto print_help;
      }

      if(!options || opt_id == '?')
      {
         if(optopt)
            rl_printf_error("unknown option :%c\n", optopt);
         else
            rl_printf_error("unknown option :%s\n", argv[optind - 1]);
         goto print_help;
      }

      int i = 0;
      while(i < count && options[i].id != opt_id)
         i++;

      assert(i < count);

      if(options[i].has_arg)
      {
         if(!optarg)
         {
            rl_printf_error("required argument missing for option %c", options[i].id);
            if(options[i].id_long)
               rl_printf_error("[%s]", options[i].id_long);
            rl_printf_error("\n");
            goto error;
         }
         vals[i] = optarg;
      }
      else
         vals[i] = "";
   }

   argc -= optind;
   argv += optind;
   memcpy(vals + count, argv, argc * sizeof(*vals));
   vals[count + argc] = NULL;

   return vals;

print_help:
   rl_printf_info("\nusage : %s%s\n\n", argv[0], options?" [options]":"");
   if(options)
   {
      int i;
      int longopt_len = 0;

      for(i = 0; i < count; i++)
         if(options[i].id_long && longopt_len < strlen(options[i].id_long))
            longopt_len = strlen(options[i].id_long);

      if(longopt_len && longopt_len < strlen("help"))
         longopt_len = strlen("help");

      char fmt[128];
      char fmt_long[128];
      sprintf(fmt, "    -%%c%%-%is  %%s\n", longopt_len ? longopt_len + 4: 0);
      sprintf(fmt_long, "    -%%c, --%%-%is  %%s\n", longopt_len);

      for(i = 0; i < count; i++)
         rl_printf_info(options[i].id_long? fmt_long : fmt, options[i].id,
                   options[i].id_long? options[i].id_long: "",
                   options[i].help);

      rl_printf_info(longopt_len? fmt_long : fmt, 'h',
                longopt_len? "help": "",
                "print help");
      rl_printf_info("\n");
   }

error:
   free(vals);
   return NULL;

}
