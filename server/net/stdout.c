#include <3ds/types.h>
#include <3ds/services/apt.h>
#include <sys/iosupport.h>
#include "ctr/ctr_debug.h"
#include "ctr/ctr_net.h"
#include "common.h"

static const devoptab_t* old_stdout;
static const devoptab_t* old_stderr;

ssize_t net_stdout_write(struct _reent *r, int fd, const char *ptr, size_t len) {

   DEBUG_ERROR(ctrnet_send(ctrsh.server.stdout_soc, (void*)ptr, len, 0, &ctrsh.server.stdout_addr));

   return old_stdout->write_r(r, fd, ptr, len);
}

ssize_t net_stderr_write(struct _reent *r, int fd, const char *ptr, size_t len) {

   DEBUG_ERROR(ctrnet_send(ctrsh.server.stdout_soc, (void*)ptr, len, 0, &ctrsh.server.stdout_addr));

   return old_stderr->write_r(r, fd, ptr, len);
}

static const devoptab_t net_stdout = {
   "netstdout",
   0,
   NULL,
   NULL,
   net_stdout_write,
   NULL,
   NULL,
   NULL
};

static const devoptab_t net_stderr = {
	"netstderr",
	0,
	NULL,
	NULL,
	net_stderr_write,
	NULL,
	NULL,
	NULL
};


void netprint_init(void)
{
   do
   {
      Result ret = ctrnet_accept(ctrsh.server.socket, &ctrsh.server.stdout_soc, &ctrsh.server.stdout_addr);
      DEBUG_ERROR(ret);
      if (!ret)
         break;
   }
   while (aptMainLoop());

   old_stdout = devoptab_list[STD_OUT];
   old_stderr = devoptab_list[STD_ERR];
   devoptab_list[STD_OUT] = &net_stdout;
   devoptab_list[STD_ERR] = &net_stderr;
}

void netprint_deinit(void)
{
   devoptab_list[STD_OUT] = old_stdout;
   devoptab_list[STD_ERR] = old_stderr;
   ctrnet_close(ctrsh.server.stdout_soc);
}
