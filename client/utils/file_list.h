#ifndef FILE_LIST_H
#define FILE_LIST_H

#include <stdint.h>
#include "../server/server_cmd.h"

typedef enum
{
   FILE_TYPE_DIRECTORY,
   FILE_TYPE_NORMAL,
   FILE_TYPE_3DSX,
   FILE_TYPE_ELF,
   FILE_TYPE_CIA,
   FILE_TYPE_MAX,
}file_type_enum;

typedef struct
{
   char* name;
   uint64_t size;
   int mbslen;
   file_type_enum type;
}filename_t;

typedef struct
{
   int size;
   void* buffer;
   filename_t* files;
}filelist_t;

filelist_t* filelist_new(ctrsh_dirent* first);
void filelist_free(filelist_t* list);
void filelist_sort(filelist_t* list);
void filelist_sort_dir(filelist_t* list);
void filelist_print(filelist_t* list);
void filelist_print_detailed(filelist_t* list);
#endif // FILE_LIST_H
