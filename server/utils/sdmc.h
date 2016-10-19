#ifndef SDMC_H
#define SDMC_H

#include <3ds/services/fs.h>

extern FS_Archive sdmc_archive;

void sdmc_init();

void sdmc_exit();

#endif // SDMC_H
