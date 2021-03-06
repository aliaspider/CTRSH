#ifndef CTR_IPC_H
#define CTR_IPC_H

#include <3ds/types.h>
#include <3ds/svc.h>
#include <3ds/ipc.h>

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
static inline ipc_command_t* IPCCMD_New(u32 id)
{
   ipc_command_t* cmd = (ipc_command_t*)((u32)getThreadLocalStorage() + 0x80);
   cmd->header = id << 16;
   return cmd;
}

__attribute((always_inline))
static inline void IPCCMD_Add_Param(ipc_command_t* cmd, u32 param)
{
   cmd->params[(cmd->header >> 6) & 0x3F] = param;
   cmd->header += 1 << 6;
}

__attribute((always_inline))
static inline void IPCCMD_Add_Desc_CurProcessHandle(ipc_command_t* cmd)
{
   cmd->params[((cmd->header >> 6) & 0x3F) + (cmd->header & 0x3F)] = 0x20;
   cmd->header += 2;
}


__attribute((always_inline))
static inline void IPCCMD_Add_Desc_StaticBuffer(ipc_command_t* cmd, u32 buffer_id, void* buffer, u32 size)
{
   cmd->params[((cmd->header >> 6) & 0x3F) + (cmd->header & 0x3F)] = (size << 14) | ((buffer_id & 0xF) << 10) | 0x2;
   cmd->header ++;
   cmd->params[((cmd->header >> 6) & 0x3F) + (cmd->header & 0x3F)] = (u32)buffer;
   cmd->header ++;
}

__attribute((always_inline))
static inline void IPCCMD_Add_Desc_Buffer(ipc_command_t* cmd, void* buffer, u32 size, IPC_BufferRights rights)
{
   cmd->params[((cmd->header >> 6) & 0x3F) + (cmd->header & 0x3F)] = (size << 4) | 0x8 | rights;
   cmd->header ++;
   cmd->params[((cmd->header >> 6) & 0x3F) + (cmd->header & 0x3F)] = (u32)buffer;
   cmd->header ++;
}

__attribute((always_inline))
static inline void IPCCMD_Add_Desc_SharedHandles(ipc_command_t* cmd, u32 count, Handle* handles)
{
   int i;
   cmd->params[((cmd->header >> 6) & 0x3F) + (cmd->header & 0x3F)] = (count - 1) << 26;
   cmd->header ++;
   for (i = 0; i < count; i++)
   {
      cmd->params[((cmd->header >> 6) & 0x3F) + (cmd->header & 0x3F)] = handles[i];
      cmd->header ++;
   }
}

__attribute((always_inline))
static inline void IPCCMD_Set_StaticBuffer(ipc_command_t* cmd, u32 buffer_id, void* buffer, u32 size)
{
   cmd->static_buffer[(buffer_id << 1)] = (size << 14) | 0x2;
   cmd->static_buffer[(buffer_id << 1) + 1] = (u32)buffer;
}


__attribute((always_inline))
static inline Result IPCCMD_Send_Wait_Reply(ipc_command_t* cmd, Handle service, u32* rval0, u32* rval1)
{
   Result res = svcSendSyncRequest(service);

   if (res)
      return res;

   if(rval0)
      *rval0 = cmd->reply.val0;
   if(rval1)
      *rval1 = cmd->reply.val1;

   return cmd->reply.result;
}

__attribute((always_inline))
static inline Result IPCCMD_Send_Wait_POSIX_Reply(ipc_command_t* cmd, Handle service, u32* rval)
{
   Result res = svcSendSyncRequest(service);

   if (res)
      return res;

   if(cmd->reply.result)
      return cmd->reply.result;

   if(rval)
      *rval = cmd->reply.val1;

   return cmd->reply.val0;
}


#endif // CTR_IPC_H
