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

typedef enum
{
   CTRNET_MSG_OOB       = BIT(0),
   CTRNET_MSG_PEEK      = BIT(1),
   CTRNET_MSG_DONTWAIT  = BIT(2)
}ctrnet_transfer_flags;


Result ctrnet_init(u32 sharedmem_size);
Result ctrnet_exit(void);
Result ctrnet_gethostid(u32* ip_out);
Result ctrnet_socket(Handle* socket_out);
Result ctrnet_bind(Handle socket, ctrnet_sockaddr_in_t *addr);
Result ctrnet_listen(Handle socket, int max_connections);
Result ctrnet_accept(Handle socket, Handle* client_handle, ctrnet_sockaddr_in_t* client_addr);
Result ctrnet_recv(Handle socket, void *buf, size_t len, ctrnet_transfer_flags flags, ctrnet_sockaddr_in_t *src_addr);
Result ctrnet_send(Handle socket, void *buf, size_t len, ctrnet_transfer_flags flags, ctrnet_sockaddr_in_t *dst_addr);
Result ctrnet_close(Handle socket);
Result ctrnet_getsockopt(Handle socket, u32 level, u32 optname, u32* optval, u32* optlen);
Result ctrnet_setsockopt(Handle socket, u32 level, u32 optname, u32* optval, u32 optlen);
Result ctrnet_close_sockets(void);

const char* ctrnet_sa_to_cstr(ctrnet_sockaddr_in_t* addr);

#ifdef __cplusplus
}
#endif

#endif // CTR_NET_H
