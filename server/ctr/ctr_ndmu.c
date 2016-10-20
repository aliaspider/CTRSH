
#include "ctr_ipc.h"


extern Handle __ndmu_servhandle;
typedef enum
{
   DAEMON_CEC   = 0,
   DAEMON_BOSS  = 1,
   DAEMON_NIM   = 2,
   DAEMON_FRIENDS = 3
} ndm_Daemon;
typedef enum
{
   DAEMON_MASK_CEC  =      1 << DAEMON_CEC,
   DAEMON_MASK_BOSS =      1 << DAEMON_BOSS,
   DAEMON_MASK_NIM  =      1 << DAEMON_NIM,
   DAEMON_MASK_FRIENDS =   1 << DAEMON_FRIENDS
} ndm_DaemonMask;

Result ndmuSuspendDaemons(ndm_DaemonMask mask)
{
   ipc_command_t* command =  IPCCMD_New(0x6);
   IPCCMD_Add_Param(command, mask);
   return IPCCMD_Send_Wait_Reply(command, __ndmu_servhandle, NULL, NULL);
}

Result ndmuResumeDaemons(ndm_DaemonMask mask)
{
   ipc_command_t* command =  IPCCMD_New(0x6);
   IPCCMD_Add_Param(command, mask);
   return IPCCMD_Send_Wait_Reply(command, __ndmu_servhandle, NULL, NULL);
}
Result ndmuSuspendScheduler(bool Async)
{
   ipc_command_t* command =  IPCCMD_New(0x8);
   IPCCMD_Add_Param(command, Async);
   return IPCCMD_Send_Wait_Reply(command, __ndmu_servhandle, NULL, NULL);
}

Result ndmuResumeScheduler(void)
{
   return IPCCMD_Send_Wait_Reply(IPCCMD_New(0x9), __ndmu_servhandle, NULL, NULL);
}
