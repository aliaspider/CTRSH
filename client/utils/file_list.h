#ifndef FILE_LIST_H
#define FILE_LIST_H

#include <stdint.h>

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
   filename_t* files;
}filelist_t;


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <readline/readline.h>

#include "file_list.h"
#include "../server/server_cmd.h"

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
#define KGRY  "\x1B[90m"
#define KLRD  "\x1B[91m"
#define KLGR  "\x1B[92m"
#define KLYL  "\x1B[93m"
#define KLBL  "\x1B[94m"
#define KLPR  "\x1B[95m"
#define KTRQ  "\x1B[96m"


filelist_t* filelist_new(ctrsh_dirent* first);
void filelist_free(filelist_t* list);
void filelist_sort(filelist_t* list);
void filelist_sort_dir(filelist_t* list);
void filelist_print(filelist_t* list);
void filelist_print_detailed(filelist_t* list);
#endif // FILE_LIST_H
