#include "common.h"
#include "server_ctrl.h"

void server_display_image(const void* buffer, uint32_t size)
{
   rl_printf_debug("sending image\n");
   DEBUG_ERROR(send_command(CTRSH_COMMAND_DISPLAY_IMAGE));
   DEBUG_ERROR(send_data(&size, 4));
   DEBUG_ERROR(send_data(buffer, size));

}