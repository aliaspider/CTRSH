#include <3ds/services/fs.h>

FS_Archive sdmc_archive;

void sdmc_init()
{
   FSUSER_OpenArchive(&sdmc_archive, ARCHIVE_SDMC, fsMakePath(PATH_ASCII,""));

}


void sdmc_exit()
{
   FSUSER_CloseArchive(sdmc_archive);
}
