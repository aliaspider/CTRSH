
#include <3ds.h>
#include <malloc.h>
#include <string.h>
#include <arpa/inet.h>
#include "ctr_net.h"
#include "ctr_debug.h"

static Handle ctrnet_sharedmem_handle;
static void* ctrnet_sharedmem_buffer;
static Handle ctrnet_handle;

Result ctrnet_socU_Init(Handle memhandle, u32 memsize)
{
	Result ret = 0;
	u32 *cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = IPC_MakeHeader(0x1,1,4); // 0x10044
	cmdbuf[1] = memsize;
	cmdbuf[2] = IPC_Desc_CurProcessHandle();
	cmdbuf[4] = IPC_Desc_SharedHandles(1);
	cmdbuf[5] = memhandle;

	ret = svcSendSyncRequest(ctrnet_handle);
	if(ret)
		return ret;

	return cmdbuf[1];
}


Result ctrnet_init(u32 sharedmem_size)
{
   Result ret;
   sharedmem_size += 0xFFF;
   sharedmem_size &= ~0xFFF;
   ctrnet_sharedmem_buffer = memalign(0x1000, sharedmem_size);

   if(!(ret = svcCreateMemoryBlock(&ctrnet_sharedmem_handle, (u32)ctrnet_sharedmem_buffer, sharedmem_size, 0, 3)))
   {
      if(!(ret = srvGetServiceHandle(&ctrnet_handle, "soc:U")))
      {
         if(!(ret = ctrnet_socU_Init(ctrnet_sharedmem_handle, sharedmem_size)))
            return 0;

         svcCloseHandle(ctrnet_handle);
      }
      svcCloseHandle(ctrnet_sharedmem_handle);
   }

   return ret;
}


static Result ctrnet_ShutdownSockets(void)
{
	Result ret = 0;
	u32 *cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = IPC_MakeHeader(0x19,0,0); // 0x190000

	ret = svcSendSyncRequest(ctrnet_handle);
	if(ret)
		return ret;

	return cmdbuf[1];
}

Result ctrnet_exit(void)
{
	Result ret;

	if((ret = svcCloseHandle(ctrnet_sharedmem_handle)))
      return ret;

   free(ctrnet_sharedmem_buffer);

	if((ret = ctrnet_ShutdownSockets()))
      return ret;

	ret = svcCloseHandle(ctrnet_handle);
	return ret;
}


Result ctrnet_gethostid(u32* ip_out)
{
	int ret = 0;
	u32 *cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = IPC_MakeHeader(0x16,0,0); // 0x160000

	ret = svcSendSyncRequest(ctrnet_handle);
	if(ret)
		return ret;

	*ip_out = cmdbuf[2];

	return cmdbuf[1];
}

Result ctrnet_socket(Handle* socket_out)
{
	int ret = 0;
	u32 *cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = IPC_MakeHeader(0x2,3,2); // 0x200C2
	cmdbuf[1] = AF_INET;
	cmdbuf[2] = SOCK_STREAM;
	cmdbuf[3] = 0;
	cmdbuf[4] = IPC_Desc_CurProcessHandle();

	ret = svcSendSyncRequest(ctrnet_handle);
	if(ret)
		return ret;

	*socket_out = cmdbuf[2];
	return cmdbuf[1];
}

Result ctrnet_bind(Handle socket, u32 ip, u16 port)
{
	int ret;
	u32 *cmdbuf = getThreadCommandBuffer();

   ctrnet_sockaddr_t addr;
   addr.size = sizeof(addr);
   addr.family = AF_INET;
   addr.port = htons(port);
   addr.ip = ip;

	cmdbuf[0] = IPC_MakeHeader(0x5,2,4); // 0x50084
	cmdbuf[1] = (u32)socket;
	cmdbuf[2] = sizeof(addr);
	cmdbuf[3] = IPC_Desc_CurProcessHandle();
	cmdbuf[5] = IPC_Desc_StaticBuffer(sizeof(addr),0);
	cmdbuf[6] = (u32)&addr;

	ret = svcSendSyncRequest(ctrnet_handle);
	if(ret)
		return ret;

   if(cmdbuf[1])
      return cmdbuf[1];

   return cmdbuf[2];
}

Result ctrnet_listen(Handle socket, int max_connections)
{
	int ret;
	u32 *cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = IPC_MakeHeader(0x3,2,2); // 0x30082
	cmdbuf[1] = (u32)socket;
	cmdbuf[2] = (u32)max_connections;
	cmdbuf[3] = IPC_Desc_CurProcessHandle();

	ret = svcSendSyncRequest(ctrnet_handle);
	if(ret)
		return ret;

   if(cmdbuf[1])
      return cmdbuf[1];

   return cmdbuf[2];
}

Result ctrnet_accept(Handle socket, Handle* client_handle, ctrnet_sockaddr_t* client_addr)
{
	Result ret;
	u32 *cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = IPC_MakeHeader(0x4,2,2); // 0x40082
	cmdbuf[1] = (u32)socket;
	cmdbuf[2] = (u32)sizeof(*client_addr);
	cmdbuf[3] = IPC_Desc_CurProcessHandle();

	u32 * staticbufs = getThreadStaticBuffers();

	staticbufs[0] = IPC_Desc_StaticBuffer(sizeof(*client_addr),0);
	staticbufs[1] = (u32)client_addr;

	ret = svcSendSyncRequest(ctrnet_handle);
   if(ret)
      return ret;

   *client_handle = cmdbuf[2];

   return cmdbuf[1];
}



Result ctrnet_recvfrom_other(Handle socket, void *buf, size_t len, u32 flags, ctrnet_sockaddr_t *src_addr)
{
	int ret = 0;
	u32 *cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = IPC_MakeHeader(0x7,4,4); // 0x70104
	cmdbuf[1] = (u32)socket;
	cmdbuf[2] = (u32)len;
	cmdbuf[3] = (u32)flags;
	cmdbuf[4] = (u32)sizeof(*src_addr);
	cmdbuf[5] = IPC_Desc_CurProcessHandle();
	cmdbuf[7] = IPC_Desc_Buffer(len,IPC_BUFFER_W);
	cmdbuf[8] = (u32)buf;

	cmdbuf[0x40] = IPC_Desc_StaticBuffer(sizeof(*src_addr),0);
	cmdbuf[0x41] = (u32)src_addr;

	ret = svcSendSyncRequest(ctrnet_handle);

   if(ret)
		return ret;

   if(cmdbuf[1])
      return (Result)cmdbuf[1];

   return cmdbuf[2]; // cmdbuf[3]?
}

Result ctrnet_recvfrom(Handle socket, void *buf, size_t len, u32 flags, ctrnet_sockaddr_t *src_addr)
{
	Handle ret;
	u32 *cmdbuf = getThreadCommandBuffer();

	cmdbuf[0x0] = IPC_MakeHeader(0x8,4,2); // 0x80102
	cmdbuf[0x1] = (u32)socket;
	cmdbuf[0x2] = (u32)len;
	cmdbuf[0x3] = (u32)flags;
	cmdbuf[0x4] = (u32)sizeof(*src_addr);
	cmdbuf[0x5] = IPC_Desc_CurProcessHandle();

	cmdbuf[0x40] = (((u32)len)<<14) | 2;
	cmdbuf[0x41] = (u32)buf;
	cmdbuf[0x42] = ((sizeof(*src_addr))<<14) | 2;
	cmdbuf[0x43] = (u32)src_addr;

	ret = svcSendSyncRequest(ctrnet_handle);
	if(ret)
		return ret;

   if(cmdbuf[1])
      return (Result)cmdbuf[1];

   return cmdbuf[2]; // cmdbuf[3]?
}

Result ctrnet_recv(Handle socket, void *buf, size_t len, u32 flags, ctrnet_sockaddr_t *src_addr)
{
	if(len < 0x2000)
		return ctrnet_recvfrom(socket, buf, len, flags, src_addr);
	return ctrnet_recvfrom_other(socket, buf, len, flags, src_addr);
}

Result ctrnet_sendto_other(Handle socket, void *buf, size_t len, u32 flags, ctrnet_sockaddr_t *dst_addr)
{
	Result ret;
	u32 *cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = IPC_MakeHeader(0x9,4,6); // 0x90106
	cmdbuf[1] = (u32)socket;
	cmdbuf[2] = len;
	cmdbuf[3] = flags;
	cmdbuf[4] = sizeof(*dst_addr);
	cmdbuf[5] = IPC_Desc_CurProcessHandle();
	cmdbuf[7] = IPC_Desc_StaticBuffer(sizeof(*dst_addr),1);
	cmdbuf[8] = (u32)dst_addr;
	cmdbuf[9] = IPC_Desc_Buffer(len,IPC_BUFFER_R);
	cmdbuf[10] = (u32)buf;

	ret = svcSendSyncRequest(ctrnet_handle);
	if(ret)
		return ret;

   if(cmdbuf[1])
      return (Result)cmdbuf[1];

   return cmdbuf[2]; // cmdbuf[3]?
}

Result ctrnet_sendto(Handle socket, void *buf, size_t len, u32 flags, ctrnet_sockaddr_t *dst_addr)
{
	int ret = 0;
	u32 *cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = IPC_MakeHeader(0xA,4,6); // 0xA0106
	cmdbuf[1] = (u32)socket;
	cmdbuf[2] = len;
	cmdbuf[3] = flags;
	cmdbuf[4] = sizeof(*dst_addr);
	cmdbuf[5] = IPC_Desc_CurProcessHandle();
	cmdbuf[7] = IPC_Desc_StaticBuffer(len,2);
	cmdbuf[8] = (u32)buf;
	cmdbuf[9] = IPC_Desc_StaticBuffer(sizeof(*dst_addr),1);
	cmdbuf[10] = (u32)dst_addr;

	ret = svcSendSyncRequest(ctrnet_handle);
	if(ret)
		return ret;

   if(cmdbuf[1])
      return (Result)cmdbuf[1];

   return cmdbuf[2]; // cmdbuf[3]?
}

Result ctrnet_send(Handle socket, void *buf, size_t len, u32 flags, ctrnet_sockaddr_t *dst_addr)
{
	if(len < 0x2000)
		return ctrnet_sendto(socket, buf, len, flags, dst_addr);
	return ctrnet_sendto_other(socket, buf, len, flags, dst_addr);
}

Result ctrnet_close(Handle socket)
{
	int ret = 0;
	u32 *cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = IPC_MakeHeader(0xB,1,2); // 0xB0042
	cmdbuf[1] = socket;
	cmdbuf[2] = IPC_Desc_CurProcessHandle();

	ret = svcSendSyncRequest(ctrnet_handle);
	if(ret)
		return ret;

   if(cmdbuf[1])
      return (Result)cmdbuf[1];

   return cmdbuf[2];
}


Result ctrnet_close_sockets(void)
{
	u32 *cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = IPC_MakeHeader(0x21,0,2); // 0x210002;
	cmdbuf[1] = IPC_Desc_CurProcessHandle();

	int ret = svcSendSyncRequest(ctrnet_handle);
	if(ret)
      return ret;
	return cmdbuf[1];
}


