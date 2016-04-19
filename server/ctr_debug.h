#ifndef _CTR_DEBUG_H__
#define _CTR_DEBUG_H__

#include <stdio.h>
#include <errno.h>
#include "ctr_error.h"

#ifdef __cplusplus
extern "C" {
#endif
void wait_for_input(void);
void dump_result_value(Result val);
#ifdef __cplusplus
}
#endif

#define DEBUG_HOLD() do{printf("%s@%s:%d.\n",__FUNCTION__, __FILE__, __LINE__);fflush(stdout);wait_for_input();}while(0)
#define DEBUG_LINE() do{printf("%s:%d.\n",__FUNCTION__, __LINE__);fflush(stdout);}while(0)
#define DEBUG_STR(X) printf( "%s: %s\n", #X, (char*)(X))
#define DEBUG_VAR(X) printf( "%-20s: 0x%08X\n", #X, (u32)(X))
#define DEBUG_ADDR(X) printf( "*(%-17s): 0x%08X\n", #X, *(unsigned*)(X))
#define DEBUG_BOOL(X) printf( "%-20s: %s\n", #X, X?"true":"false")
#define DEBUG_INT(X) printf( "%-20s: %10i\n", #X, (s32)(X))
#define DEBUG_VAR64(X) printf( #X"\r\t\t\t\t : 0x%016llX\n", (u64)(X))
#define DEBUG_ERROR(X) do{Result res_ = X; if(res_){printf("error@%s (%s:%d).\n",__FUNCTION__, __FILE__, __LINE__);dump_result_value(res_);}}while(0)
#define DEBUG_CERROR(X) do{int res_ = (X); if(res_ < 0){printf("error %i @%s (%s:%d).\n%s\n", res_, __FUNCTION__, __FILE__, __LINE__,strerror(errno));}}while(0)
#define PRINTFPOS(X,Y) "\x1b["#X";"#Y"H"
#define PRINTFPOS_STR(X,Y) "\x1b["X";"Y"H"

#endif //_CTR_DEBUG_H__

