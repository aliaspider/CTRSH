
#include "ctrsh.h"

ctrsh_t ctrsh =
{
   .commands = {
      [CTRSH_COMMAND_INVALID] = {NULL},
      [CTRSH_COMMAND_EXIT] = {ctrsh_command_exit},
      [CTRSH_COMMAND_DIRENT] = {ctrsh_command_dirent},
      [CTRSH_COMMAND_DISPLAY_IMAGE] = {ctrsh_command_display_image},
   }
};
