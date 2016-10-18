#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "common.h"
#include "commands.h"
#include "utils/file_list.h"
#include "serverctrl/server_ctrl.h"


option_t command_ls_options[] =
{
   {'l', false, NULL, "detailed view"},
   {0}
};

typedef struct
{
   const char* detailed_view;
   const char* path;
}ls_options_t;

void command_ls(char* const* options)
{
   int i, j;
   ls_options_t* opt = (ls_options_t*)options;

   filelist_t* filelist = server_get_filelist(NULL);

   if (opt->detailed_view)
      filelist_print_detailed(filelist);
   else
      filelist_print(filelist);

   filelist_free(filelist);
}

