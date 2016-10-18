
#include "server_ctrl.h"
#include "server_internal.h"

void server_exit(void)
{
   DEBUG_ERROR(send_command(CTRSH_COMMAND_EXIT));
}


void server_put(const void* buffer, uint32_t size)
{
   DEBUG_ERROR(send_command(CTRSH_COMMAND_PUT));
   DEBUG_ERROR(send_data(&size, 4));
   DEBUG_ERROR(send_data(buffer, size));

}
