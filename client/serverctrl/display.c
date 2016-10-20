#include "common.h"
#include "server_ctrl.h"
#include "server_internal.h"
#include "server_internal.h"

void server_display_image(const void* buffer, uint32_t size)
{
   rl_printf_debug("sending image\n");
   DEBUG_ERROR(server_send_command(CTRSH_COMMAND_DISPLAY_IMAGE));
   DEBUG_ERROR(server_send_data(buffer, size));
}
