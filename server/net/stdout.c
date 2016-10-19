#include <sys/iosupport.h>
#include "ctr/ctr_debug.h"
#include "ctr/ctr_net.h"
#include "common.h"



ssize_t con_write(struct _reent *r,int fd,const char *ptr, size_t len) ;
ssize_t net_write(struct _reent *r, int fd, const char *ptr, size_t len) {

   if (ctrsh.server.stdout_soc)
   {
      DEBUG_ERROR(ctrnet_send(ctrsh.server.stdout_soc, (void*)ptr, len, 0, &ctrsh.server.stdout_addr));
   }

   return con_write(r, fd, ptr, len);
}

static const devoptab_t devoptab_net = {
	"netprint",
	0,
	NULL,
	NULL,
	net_write,
	NULL,
	NULL,
	NULL
};

void netprint_init(void)
{
   ctrsh.server.stdout_soc = 0;
   devoptab_list[STD_OUT] = &devoptab_net;
   devoptab_list[STD_ERR] = &devoptab_net;

}
