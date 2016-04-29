
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <readline/readline.h>

#include "file_list.h"
#include "common.h"
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

static const char* filename_col[FILE_TYPE_MAX] = {KYEL,NULL,KRED,KMAG,KCYN,NULL};

filelist_t* filelist_new(ctrsh_dirent* first)
{
   int i;
   filelist_t* out = malloc(sizeof(filelist_t));
   ctrsh_dirent* dir = first;
   out->size = 0;

   while (dir->entry_size)
   {
      dir = (ctrsh_dirent*)((uintptr_t)dir + dir->entry_size);
      out->size++;
   }

   out->files = malloc(out->size * sizeof(filename_t));

   dir = first;

   for (i = 0; i < out->size; i++)
   {
      int len = strlen(dir->name);
      out->files[i].name = dir->name;
      out->files[i].size = dir->file_size;
      out->files[i].mbslen = dir->mbslen;

      if(dir->is_directory)
         out->files[i].type = FILE_TYPE_DIRECTORY;
      else if ((len > 5) &&!strcasecmp(&dir->name[len - 5], ".3dsx"))
         out->files[i].type = FILE_TYPE_3DSX;
      else if ((len > 4) &&!strcasecmp(&dir->name[len - 4], ".elf"))
         out->files[i].type = FILE_TYPE_ELF;
      else if ((len > 4) &&!strcasecmp(&dir->name[len - 4], ".cia"))
         out->files[i].type = FILE_TYPE_CIA;
      else
         out->files[i].type = FILE_TYPE_NORMAL;

      dir = (ctrsh_dirent*)((uintptr_t)dir + dir->entry_size);
   }

   return out;

}

void filelist_free(filelist_t* list)
{
   free(list->files);
   free(list);
}


void filelist_sort(filelist_t* list)
{
   int i;
   bool finished = false;

   while (!finished)
   {
      finished = true;

      for (i = 0; i < list->size - 1; i++)
      {
         if (strcasecmp(list->files[i].name, list->files[i + 1].name) > 0)
         {
            filename_t tmp = list->files[i];
            list->files[i] = list->files[i + 1];
            list->files[i + 1] = tmp;
            finished = false;
         }
      }
   }
}

void filelist_sort_dir(filelist_t* list)
{
   int i;
   filename_t* old_list = list->files;
   list->files = malloc(sizeof(filename_t) * list->size);
   int pos = 0;

   for (i = 0; i < list->size; i++)
   {
      if (old_list[i].type == FILE_TYPE_DIRECTORY)
         list->files[pos++] = old_list[i];
   }

   for (i = 0; i < list->size; i++)
   {
      if (old_list[i].type != FILE_TYPE_DIRECTORY)
         list->files[pos++] = old_list[i];
   }

   free(old_list);
}

void filelist_print(filelist_t* list)
{
   int i, j;
   int term_w;
   rl_get_screen_size(NULL, &term_w);

   const int col_spacing = 2;
   const int max_colums = 20;
   int col_widths[max_colums];
   int col_entries[max_colums];

   int dir;
   int w_remaining;
   int columns = max_colums;

   do
   {
      dir = 0;
      w_remaining = term_w;

      for (i = 0; i < columns; i++)
         col_entries[i] = list->size / columns;

      for (i = 0; i < (list->size % columns); i++)
         col_entries[i]++;

      for (i = 0; i < columns; i++)
      {
         col_widths[i] = 0;

         for (j = 0; j < col_entries[i]; j++)
         {
            if (col_widths[i] < list->files[dir].mbslen)
               col_widths[i] = list->files[dir].mbslen;

            dir++;
         }

         w_remaining -= col_widths[i] + col_spacing;

         if (w_remaining < 0)
            break;
      }
   }
   while ((w_remaining < 0) && (--columns > 1));

   int k;

   for (j = 0; j < col_entries[0]; j++)
   {
      dir = 0;

      for (k = 0; k < j; k++)
         dir++;

      for (i = 0; (i < columns) && (dir < list->size); i++)
      {
         if(filename_col[list->files[dir].type])
            printf("%s%s" KNRM, filename_col[list->files[dir].type], list->files[dir].name);
         else
            printf("%s", list->files[dir].name);

         for (k = 0; k < (col_widths[i] + col_spacing - list->files[dir].mbslen); k++)
            putchar(' ');

         for (k = 0; k < col_entries[i]; k++)
            dir++;
      }

      printf("\n");
   }

}


void filelist_print_detailed(filelist_t* list)
{
   int i;

   for (i = 0; i < list->size; i++)
   {
      if(filename_col[list->files[i].type])
         printf(" ----------  ##  %s%s \n" KNRM, filename_col[list->files[i].type], list->files[i].name);
      else
         printf(" %10lli  ##  %s \n", list->files[i].size, list->files[i].name);
   }
}
