#ifndef NET_CTRL_H
#define NET_CTRL_H

#include <3ds/types.h>

void netprint_init(void);

void server_init(void);
void server_deinit(void);

Result ctrsh_send_from_buffer(void* buffer, u32 size);

Result ctrsh_recv_to_buffer(void** buffer);
Result ctrsh_recv_to_file(Handle file);


#endif // NET_CTRL_H
