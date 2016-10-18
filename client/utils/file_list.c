
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <readline/readline.h>

#include "file_list.h"
#include "common.h"
#include "../server/server_cmd.h"

static const char* filename_col[FILE_TYPE_MAX] = {KBLD KBLU, KNRM, KBLD KYEL, KMAG, KBLD KLRD};

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
   const int col_spacing = 2;
   const int max_colums = 20;

   int col_widths[max_colums];
   int col_entries[max_colums];

   int i, j, k, id, term_w, w_remaining, columns;

   rl_get_screen_size(NULL, &term_w);
   columns = max_colums;

   do
   {
      id = 0;
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
            if (col_widths[i] < list->files[id].mbslen)
               col_widths[i] = list->files[id].mbslen;

            id++;
         }

         w_remaining -= col_widths[i] + col_spacing;

         if (w_remaining < 0)
            break;
      }
   }
   while ((w_remaining < 0) && (--columns > 1));


   for (j = 0; j < col_entries[0]; j++, id = j)
   {
      for (i = 0; (i < columns) && (id < list->size); i++, id += col_entries[i])
      {
         rl_printf("%s%s" KNRM, filename_col[list->files[id].type], list->files[id].name);

         for (k = 0; k < (col_widths[i] + col_spacing - list->files[id].mbslen); k++)
            putchar(' ');
      }

      rl_printf("\n");
   }

}


void filelist_print_detailed(filelist_t* list)
{
   int i;

   for (i = 0; i < list->size; i++)
   {
      if(list->files[i].type == FILE_TYPE_DIRECTORY)
         rl_printf(" ----------  %s%s \n" KNRM, filename_col[list->files[i].type], list->files[i].name);
      else
         rl_printf(" %10lli  %s%s \n" KNRM, list->files[i].size, filename_col[list->files[i].type], list->files[i].name);
   }
}
