#ifndef COMMON_H
#define COMMON_H

#include <3ds/types.h>
#include <3ds/services/fs.h>

#include "ctr/ctr_net.h"

#ifdef PERF_TEST
#define profiler_start()            u64 start_tick = svcGetSystemTick()
#define profiler_stop()             u64 end_tick   = svcGetSystemTick()
#define profiler_ticks()            (end_tick - start_tick)
#define profiler_ms()               (profiler_ticks() / 268123.480)
#define profiler_s()                (profiler_ticks() / 268123480.0)
#define profiler_print(X)           printf("<%s> executed in %.3fms\n", X, profiler_ms())
#define profiler_speed(X, bytes)    do{printf("%s : %i Bytes, time: %f\n", X, (int)bytes, profiler_s()); printf("speed: %.3f KB/s\n", (bytes / 1024.0) / profiler_s());} while(0)


#else
#define profiler_start()            (void)0
#define profiler_stop()             (void)0
#define profiler_print(X)           (void)0
#define profiler_speed(X, bytes)    (void)0
#endif


#endif // COMMON_H
