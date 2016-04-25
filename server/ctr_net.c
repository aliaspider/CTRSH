
#include <3ds.h>
#include <malloc.h>
#include <string.h>
#include <arpa/inet.h>
#include "ctr_ipc.h"
#include "ctr_net.h"
#include "ctr_debug.h"

#define CTRNET_TRANSFER_SIZE_THRESHOLD 0x2000

__attribute((noinline))
static Result ctrnet_sharedmem_init(Handle memhandle, u32 memsize)
{
   ipc_command_t* command = IPCCMD_New(0x1);
   IPCCMD_Add_Param(command, memsize);
   IPCCMD_Add_Desc_CurProcessHandle(command);
   IPCCMD_Add_Desc_SharedHandles(command, 1, &memhandle);

   return IPCCMD_Send_Wait_Reply(command, ctrnet.handle, NULL, NULL);
}

Result ctrnet_init(u32 sharedmem_size)
{
   Result ret;
   ctrnet.sharedmem_size = (sharedmem_size + 0xFFF) & ~0xFFF;
   ctrnet.sharedmem_buffer = memalign(0x1000, ctrnet.sharedmem_size);

   if (!(ret = svcCreateMemoryBlock(&ctrnet.sharedmem_handle, (u32)ctrnet.sharedmem_buffer, ctrnet.sharedmem_size, 0, 3)))
   {
      if (!(ret = srvGetServiceHandle(&ctrnet.handle, "soc:U")))
      {
         if (!(ret = ctrnet_sharedmem_init(ctrnet.sharedmem_handle, ctrnet.sharedmem_size)))
            return 0;

         svcCloseHandle(ctrnet.handle);
      }

      svcCloseHandle(ctrnet.sharedmem_handle);
   }

   return ret;
}

static Result ctrnet_sharedmem_deinit(void)
{
   return IPCCMD_Send_Wait_Reply(IPCCMD_New(0x19), ctrnet.handle, NULL, NULL);
}

Result ctrnet_exit(void)
{
   Result ret;

   if ((ret = ctrnet_sharedmem_deinit()))
      return ret;

   if ((ret = svcCloseHandle(ctrnet.handle)))
      return ret;

   ret = svcCloseHandle(ctrnet.sharedmem_handle);
   free(ctrnet.sharedmem_buffer);
   return ret;
}


Result ctrnet_gethostid(u32* ip_out)
{
   return IPCCMD_Send_Wait_Reply(IPCCMD_New(0x16), ctrnet.handle, ip_out, NULL);
}

Result ctrnet_socket(Handle* socket_out)
{
   ipc_command_t* command = IPCCMD_New(0x2);

   IPCCMD_Add_Param(command, AF_INET);
   IPCCMD_Add_Param(command, SOCK_STREAM);
   IPCCMD_Add_Param(command, 0);
   IPCCMD_Add_Desc_CurProcessHandle(command);

   return IPCCMD_Send_Wait_Reply(command, ctrnet.handle, socket_out, NULL);
}

Result ctrnet_bind(Handle socket, ctrnet_sockaddr_in_t* addr)
{
   ipc_command_t* command = IPCCMD_New(0x5);

   IPCCMD_Add_Param(command, socket);
   IPCCMD_Add_Param(command, sizeof(*addr));
   IPCCMD_Add_Desc_CurProcessHandle(command);
   IPCCMD_Add_Desc_StaticBuffer(command, 0, addr, sizeof(*addr));

   return IPCCMD_Send_Wait_POSIX_Reply(command, ctrnet.handle, NULL);
}

Result ctrnet_listen(Handle socket, int max_connections)
{
   ipc_command_t* command = IPCCMD_New(0x3);
   IPCCMD_Add_Param(command, socket);
   IPCCMD_Add_Param(command, max_connections);
   IPCCMD_Add_Desc_CurProcessHandle(command);

   return IPCCMD_Send_Wait_POSIX_Reply(command, ctrnet.handle, NULL);
}

Result ctrnet_accept(Handle socket, Handle* client_handle, ctrnet_sockaddr_in_t* client_addr)
{
   ipc_command_t* command = IPCCMD_New(0x4);
   IPCCMD_Add_Param(command, socket);
   IPCCMD_Add_Param(command, sizeof(*client_addr));
   IPCCMD_Add_Desc_CurProcessHandle(command);
   IPCCMD_Set_StaticBuffer(command, 0, client_addr, sizeof(*client_addr));

   return IPCCMD_Send_Wait_Reply(command, ctrnet.handle, client_handle, NULL);
}

Result ctrnet_recv(Handle socket, void* buf, size_t len, ctrnet_transfer_flags flags, ctrnet_sockaddr_in_t* src_addr)
{
   ipc_command_t* command = IPCCMD_New(len < CTRNET_TRANSFER_SIZE_THRESHOLD ? 0x8 : 0x7);
   IPCCMD_Add_Param(command, socket);
   IPCCMD_Add_Param(command, len);
   IPCCMD_Add_Param(command, flags);
   IPCCMD_Add_Param(command, sizeof(*src_addr));
   IPCCMD_Add_Desc_CurProcessHandle(command);

   if (len < CTRNET_TRANSFER_SIZE_THRESHOLD)
   {
      IPCCMD_Set_StaticBuffer(command, 0, buf, len);
      IPCCMD_Set_StaticBuffer(command, 1, src_addr, sizeof(*src_addr));
   }
   else
   {
      IPCCMD_Add_Desc_Buffer(command, buf, len, IPC_BUFFER_W);
      IPCCMD_Set_StaticBuffer(command, 0, src_addr, sizeof(*src_addr));
   }

   return IPCCMD_Send_Wait_POSIX_Reply(command, ctrnet.handle, NULL);
}

Result ctrnet_send(Handle socket, void* buf, size_t len, ctrnet_transfer_flags flags, ctrnet_sockaddr_in_t* dst_addr)
{
   ipc_command_t* command = IPCCMD_New(len < CTRNET_TRANSFER_SIZE_THRESHOLD ? 0xA : 0x9);
   IPCCMD_Add_Param(command, socket);
   IPCCMD_Add_Param(command, len);
   IPCCMD_Add_Param(command, flags);
   IPCCMD_Add_Param(command, sizeof(*dst_addr));
   IPCCMD_Add_Desc_CurProcessHandle(command);

   if (len < CTRNET_TRANSFER_SIZE_THRESHOLD)
   {
      IPCCMD_Add_Desc_StaticBuffer(command, 2, buf, len);
      IPCCMD_Add_Desc_StaticBuffer(command, 1, dst_addr, sizeof(*dst_addr));
   }
   else
   {
      IPCCMD_Add_Desc_StaticBuffer(command, 1, dst_addr, sizeof(*dst_addr));
      IPCCMD_Add_Desc_Buffer(command, buf, len, IPC_BUFFER_R);
   }

   return IPCCMD_Send_Wait_POSIX_Reply(command, ctrnet.handle, NULL);
}

Result ctrnet_close(Handle socket)
{
   ipc_command_t* command = IPCCMD_New(0xB);
   IPCCMD_Add_Param(command, socket);
   IPCCMD_Add_Desc_CurProcessHandle(command);

   return IPCCMD_Send_Wait_POSIX_Reply(command, ctrnet.handle, NULL);
}

Result ctrnet_getsockopt(Handle socket, u32 level, u32 optname, u32* optval, u32* optlen)
{
   ipc_command_t* command = IPCCMD_New(0x11);
   IPCCMD_Add_Param(command, socket);
   IPCCMD_Add_Param(command, level);
   IPCCMD_Add_Param(command, optname);
   IPCCMD_Add_Param(command, *optlen);
   IPCCMD_Add_Desc_CurProcessHandle(command);
   IPCCMD_Set_StaticBuffer(command, 0, optval, *optlen);

   return IPCCMD_Send_Wait_POSIX_Reply(command, ctrnet.handle, optlen);
}

Result ctrnet_setsockopt(Handle socket, u32 level, u32 optname, u32* optval, u32 optlen)
{
   ipc_command_t* command = IPCCMD_New(0x12);
   IPCCMD_Add_Param(command, socket);
   IPCCMD_Add_Param(command, level);
   IPCCMD_Add_Param(command, optname);
   IPCCMD_Add_Param(command, optlen);
   IPCCMD_Add_Desc_CurProcessHandle(command);
   IPCCMD_Add_Desc_StaticBuffer(command, 9, optval, optlen);

   return IPCCMD_Send_Wait_POSIX_Reply(command, ctrnet.handle, NULL);
}


Result ctrnet_close_sockets(void)
{
   ipc_command_t* command = IPCCMD_New(0x21);
   IPCCMD_Add_Desc_CurProcessHandle(command);

   return IPCCMD_Send_Wait_Reply(command, ctrnet.handle, NULL, NULL);
}

const char* ctrnet_sa_to_cstr(ctrnet_sockaddr_in_t* addr)
{
   static char buffer[0x100];
   u8* ip = (u8*)&addr->addr;
   snprintf(buffer, sizeof(buffer), "%i.%i.%i.%i:%i", ip[0], ip[1], ip[2], ip[3], ntohs(addr->port));
   return buffer;
}
