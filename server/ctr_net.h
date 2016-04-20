#ifndef CTR_NET_H
#define CTR_NET_H

#include <3ds/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
   u8 size;
   u8 family;
   u16 port;
   u32 addr;
}ctrnet_sockaddr_in_t;

Result ctrnet_init(u32 sharedmem_size);
Result ctrnet_exit(void);
Result ctrnet_gethostid(u32* ip_out);
Result ctrnet_socket(Handle* socket_out);
Result ctrnet_bind(Handle socket, ctrnet_sockaddr_in_t *addr);
Result ctrnet_listen(Handle socket, int max_connections);
Result ctrnet_accept(Handle socket, Handle* client_handle, ctrnet_sockaddr_in_t* client_addr);
Result ctrnet_recv(Handle socket, void *buf, size_t len, u32 flags, ctrnet_sockaddr_in_t *src_addr);
Result ctrnet_send(Handle socket, void *buf, size_t len, u32 flags, ctrnet_sockaddr_in_t *dst_addr);
Result ctrnet_close(Handle socket);
Result ctrnet_close_sockets(void);

const char* ctrnet_sa_to_cstr(ctrnet_sockaddr_in_t* addr);

#ifdef __cplusplus
}
#endif

#endif // CTR_NET_H
