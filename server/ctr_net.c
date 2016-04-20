
#include <3ds.h>
#include <malloc.h>
#include <string.h>
#include <arpa/inet.h>
#include "ctr_net.h"
#include "ctr_debug.h"

#define CTRNET_TRANSFER_SIZE_THRESHOLD 0x100

static struct
{
   Handle handle;
   Handle sharedmem_handle;
   void* sharedmem_buffer;
} ctrnet;

typedef struct
{
   u32 header;
   union
   {
      struct
      {
         u32 params[0x3F];
         u32 static_buffer[0x20];
      };
      struct
      {
         Result result;
         u32 val0;
         u32 val1;
      } reply;
   };
} ipc_command_t;

__attribute((always_inline))
static inline ipc_command_t* IPC_CommandNew(u32 id, u32 normal_params, u32 translate_params)
{
   ipc_command_t* cmd = (ipc_command_t*)((u32)getThreadLocalStorage() + 0x80);
   cmd->header = IPC_MakeHeader(id, normal_params, translate_params);
   return cmd;
}

static Result ctrnet_sharedmem_init(Handle memhandle, u32 memsize)
{
   ipc_command_t* command = IPC_CommandNew(0x1, 1, 4);

   command->params[0] = memsize;
   command->params[1] = IPC_Desc_CurProcessHandle();
   command->params[3] = IPC_Desc_SharedHandles(1);
   command->params[4] = memhandle;

   Result res = svcSendSyncRequest(ctrnet.handle);

   if (res)
      return res;

   return command->reply.result;
}

Result ctrnet_init(u32 sharedmem_size)
{
   Result ret;
   sharedmem_size += 0xFFF;
   sharedmem_size &= ~0xFFF;
   ctrnet.sharedmem_buffer = memalign(0x1000, sharedmem_size);

   if (!(ret = svcCreateMemoryBlock(&ctrnet.sharedmem_handle, (u32)ctrnet.sharedmem_buffer, sharedmem_size, 0, 3)))
   {
      if (!(ret = srvGetServiceHandle(&ctrnet.handle, "soc:U")))
      {
         if (!(ret = ctrnet_sharedmem_init(ctrnet.sharedmem_handle, sharedmem_size)))
            return 0;

         svcCloseHandle(ctrnet.handle);
      }

      svcCloseHandle(ctrnet.sharedmem_handle);
   }

   return ret;
}

static Result ctrnet_sharedmem_deinit(void)
{
   ipc_command_t* command = IPC_CommandNew(0x19, 0, 0);

   Result res = svcSendSyncRequest(ctrnet.handle);

   if (res)
      return res;

   return command->reply.result;
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
   ipc_command_t* command = IPC_CommandNew(0x16, 0, 0);

   Result res = svcSendSyncRequest(ctrnet.handle);

   if (res)
      return res;

   *ip_out = command->reply.val0;

   return command->reply.result;
}

Result ctrnet_socket(Handle* socket_out)
{
   ipc_command_t* command = IPC_CommandNew(0x2, 3, 2);

   command->params[0] = AF_INET;
   command->params[1] = SOCK_STREAM;
   command->params[2] = 0;
   command->params[3] = IPC_Desc_CurProcessHandle();

   Result res = svcSendSyncRequest(ctrnet.handle);

   if (res)
      return res;

   *socket_out = command->reply.val0;
   return command->reply.result;
}

Result ctrnet_bind(Handle socket, ctrnet_sockaddr_in_t* addr)
{
   ipc_command_t* command = IPC_CommandNew(0x5, 2, 4);

   command->params[0] = (u32)socket;
   command->params[1] = sizeof(*addr);
   command->params[2] = IPC_Desc_CurProcessHandle();
   command->params[4] = IPC_Desc_StaticBuffer(sizeof(*addr), 0);
   command->params[5] = (u32)addr;

   Result res = svcSendSyncRequest(ctrnet.handle);

   if (res)
      return res;

   if (command->reply.result)
      return command->reply.result;

   return command->reply.val0;
}

Result ctrnet_listen(Handle socket, int max_connections)
{
   ipc_command_t* command = IPC_CommandNew(0x3, 2, 2);
   command->params[0] = (u32)socket;
   command->params[1] = (u32)max_connections;
   command->params[2] = IPC_Desc_CurProcessHandle();

   Result res = svcSendSyncRequest(ctrnet.handle);

   if (res)
      return res;

   if (command->reply.result)
      return command->reply.result;

   return command->reply.val0;
}

Result ctrnet_accept(Handle socket, Handle* client_handle, ctrnet_sockaddr_in_t* client_addr)
{
   ipc_command_t* command = IPC_CommandNew(0x4, 2, 2);
   command->params[0] = (u32)socket;
   command->params[1] = (u32)sizeof(*client_addr);
   command->params[2] = IPC_Desc_CurProcessHandle();
   command->static_buffer[0] = IPC_Desc_StaticBuffer(sizeof(*client_addr), 0);
   command->static_buffer[1] = (u32)client_addr;

   Result res = svcSendSyncRequest(ctrnet.handle);

   if (res)
      return res;

   *client_handle = command->reply.val0;

   return command->reply.result;
}

Result ctrnet_recv(Handle socket, void* buf, size_t len, u32 flags, ctrnet_sockaddr_in_t* src_addr)
{
   ipc_command_t* command = (len < CTRNET_TRANSFER_SIZE_THRESHOLD) ? IPC_CommandNew(0x8, 4, 2) : IPC_CommandNew(0x7, 4, 4);
   command->params[0] = (u32)socket;
   command->params[1] = (u32)len;
   command->params[2] = (u32)flags;
   command->params[3] = (u32)sizeof(*src_addr);
   command->params[4] = IPC_Desc_CurProcessHandle();

   if (len < CTRNET_TRANSFER_SIZE_THRESHOLD)
   {
      command->static_buffer[0] = (((u32)len) << 14) | 2;
      command->static_buffer[1] = (u32)buf;
      command->static_buffer[2] = ((sizeof(*src_addr)) << 14) | 2;
      command->static_buffer[3] = (u32)src_addr;
   }
   else
   {
      command->params[6] = IPC_Desc_Buffer(len, IPC_BUFFER_W);
      command->params[7] = (u32)buf;
      command->static_buffer[0] = IPC_Desc_StaticBuffer(sizeof(*src_addr), 0);
      command->static_buffer[1] = (u32)src_addr;
   }

   Result res = svcSendSyncRequest(ctrnet.handle);

   if (res)
      return res;

   if (command->reply.result)
      return command->reply.result;

   return command->reply.val0; // command->params[3]?
}

Result ctrnet_send(Handle socket, void* buf, size_t len, u32 flags, ctrnet_sockaddr_in_t* dst_addr)
{
   ipc_command_t* command = IPC_CommandNew((len < CTRNET_TRANSFER_SIZE_THRESHOLD) ? 0xA : 0x9, 4, 6);
   command->params[0] = socket;
   command->params[1] = len;
   command->params[2] = flags;
   command->params[3] = sizeof(*dst_addr);
   command->params[4] = IPC_Desc_CurProcessHandle();

   if (len < CTRNET_TRANSFER_SIZE_THRESHOLD)
   {
      command->params[6] = IPC_Desc_StaticBuffer(len, 2);
      command->params[7] = (u32)buf;
      command->params[8] = IPC_Desc_StaticBuffer(sizeof(*dst_addr), 1);
      command->params[9] = (u32)dst_addr;
   }
   else
   {
      command->params[6] = IPC_Desc_StaticBuffer(sizeof(*dst_addr), 1);
      command->params[7] = (u32)dst_addr;
      command->params[8] = IPC_Desc_Buffer(len, IPC_BUFFER_R);
      command->params[9] = (u32)buf;
   }

   Result res = svcSendSyncRequest(ctrnet.handle);

   if (res)
      return res;

   if (command->reply.result)
      return command->reply.result;

   return command->reply.val0; // command->reply.val1?
}

Result ctrnet_close(Handle socket)
{
   ipc_command_t* command = IPC_CommandNew(0xB, 1, 2);
   command->params[0] = socket;
   command->params[1] = IPC_Desc_CurProcessHandle();

   Result res = svcSendSyncRequest(ctrnet.handle);

   if (res)
      return res;

   if (command->reply.result)
      return command->reply.result;

   return command->reply.val0;
}

Result ctrnet_close_sockets(void)
{
   ipc_command_t* command = IPC_CommandNew(0x21, 0, 2);
   command->params[0] = IPC_Desc_CurProcessHandle();

   Result res = svcSendSyncRequest(ctrnet.handle);

   if (res)
      return res;

   return command->reply.result;
}

const char* ctrnet_sa_to_cstr(ctrnet_sockaddr_in_t* addr)
{
   static char buffer[0x100];
   u8* ip = (u8*)&addr->addr;
   snprintf(buffer, sizeof(buffer), "%u.%u.%u.%u:%u", (u32)ip[0], (u32)ip[1], (u32)ip[2], (u32)ip[3], ntohs(addr->port));
   return buffer;
}
