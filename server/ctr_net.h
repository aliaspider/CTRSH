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
   u32 ip;
}ctrnet_sockaddr_t;


Result ctrnet_init(u32 sharedmem_size);
Result ctrnet_exit(void);
Result ctrnet_gethostid(u32* ip_out);
Result ctrnet_socket(Handle* socket_out);
Result ctrnet_bind(Handle socket, u32 ip, u16 port);
Result ctrnet_listen(Handle socket, int max_connections);
Result ctrnet_accept(Handle socket, Handle* client_handle, ctrnet_sockaddr_t* client_addr);
Result ctrnet_recv(Handle socket, void *buf, size_t len, u32 flags, ctrnet_sockaddr_t *src_addr);
Result ctrnet_send(Handle socket, void *buf, size_t len, u32 flags, ctrnet_sockaddr_t *dst_addr);
Result ctrnet_close(Handle socket);
Result ctrnet_close_sockets(void);


#ifdef __cplusplus
}
#endif

#endif // CTR_NET_H
