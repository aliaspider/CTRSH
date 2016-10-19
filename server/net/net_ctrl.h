#ifndef NET_CTRL_H
#define NET_CTRL_H

#include <3ds/types.h>

void server_init(void);
void server_exit(void);

Result send_from_buffer(void* buffer, u32 size);

Result ctrsh_recv_to_buffer(void** buffer);
Result recv_to_file(Handle file);
Result recv_u32(u32* out);

#endif // NET_CTRL_H
