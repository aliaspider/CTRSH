#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "common.h"
#include "commands.h"
#include "utils/file_list.h"
#include "serverctrl/server_ctrl.h"

typedef struct
{
   const char* detailed_view;
   const char* path;
}ls_options_t;

static option_def_t command_ls_options_def[] =
{
   {'l', false, NULL, "detailed view"},
   {0}
};


static void command_ls_entry(ls_options_t* opt)
{
   int i, j;

   filelist_t* filelist = server_get_filelist(NULL);

   if (opt->detailed_view)
      filelist_print_detailed(filelist);
   else
      filelist_print(filelist);

   filelist_free(filelist);
}

command_t command_ls =
{
   "ls",
   (command_fn_t)command_ls_entry,
   command_ls_options_def
};
