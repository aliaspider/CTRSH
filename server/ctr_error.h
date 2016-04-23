#ifndef CTR_ERROR_H
#define CTR_ERROR_H

#include <3ds/types.h>
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
   int id;
   const char* val;
} error_string;

extern error_string error_description_str[];
extern error_string error_summary_str [];
extern error_string error_module_str [];
extern error_string error_level_str [];

static inline const char* ctr_error_to_str(error_string* list, int id)
{
   while (list->val)
   {
      if(list->id == id)
         return list->val;
      list ++;
   }
   return "unknown";
}

typedef union
{
   struct
   {
      unsigned description : 10;
      unsigned module      : 8;
      unsigned             : 3;
      unsigned summary     : 6;
      unsigned level       : 5;
   };
   Result val;
} ctr_result_value;


s32 _net_convert_error(s32 sock_retval);

static inline void dump_result_value(Result val)
{
   if(-val < 0x80)
   {
      printf("%i(%i) : %s\n", val, _net_convert_error(val), strerror(-_net_convert_error(val)));
      return;
   }

   ctr_result_value res;
   res.val = val;
   printf("0x%08X :\n", (unsigned int)val);
//   if(val == -1)
//      return;

   printf("%-4u: %s\n", res.description, ctr_error_to_str(error_description_str, res.description));
   printf("%-4u: %s\n", res.module, ctr_error_to_str(error_module_str, res.module));
   printf("%-4u: %s\n", res.summary, ctr_error_to_str(error_summary_str, res.summary));
   printf("%-4u: %s\n\n", res.level, ctr_error_to_str(error_level_str, res.level));
}

#ifdef __cplusplus
}
#endif

#endif // CTR_ERROR_H
