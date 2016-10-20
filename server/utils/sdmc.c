#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <sys/cdefs.h>
#include <3ds/services/fs.h>

static FS_Archive sdmc_archive;

void sdmc_init()
{
   FSUSER_OpenArchive(&sdmc_archive, ARCHIVE_SDMC, fsMakePath(PATH_ASCII, ""));
}


void sdmc_exit()
{
   FSUSER_CloseArchive(sdmc_archive);
}


Result sdmc_open_dir(Handle* out, const char* path)
{
   const wchar_t* path_w = L"/";
   wchar_t* buffer = NULL;
   _Static_assert(sizeof(wchar_t) == 2, "wchar_t");

   if (path && *path)
   {
      int len = strlen(path) + 1;
      buffer = malloc(len * sizeof(wchar_t));
      mbstowcs(buffer, path, len);
      path_w = buffer;
   }

   FS_Path dirpath;
   dirpath.type = PATH_UTF16;
   dirpath.size = (wcslen(path_w) + 1) * sizeof(wchar_t);
   dirpath.data = path_w;

   Result ret = FSUSER_OpenDirectory(out, sdmc_archive, dirpath);
   free(buffer);
   return ret;

}
