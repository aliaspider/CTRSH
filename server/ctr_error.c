#include <stdio.h>
#include <string.h>
#include "ctr_error.h"

s32 _net_convert_error(s32 sock_retval);

typedef struct
{
   int id;
   const char* val;
} error_string;

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

static const error_string error_description_str[] =
{
   {0     , "Success"},
   {2     , "Invalid memory permissions (kernel)"},
   {4     , "Invalid ticket version (AM)"},
   {5     , "Invalid string length. This error is returned when service name length is greater than 8 or zero. (srv)"},
   {6     , "Access denied. This error is returned when you request a service that you don't have access to. (srv)"},
   {7     , "String size does not match string contents. This error is returned when service name contains an unexpected null byte. (srv)"},
   {8     , "Camera already in use/busy (qtm)."},
   {10    , "Not enough memory (os)"},
   {26    , "Session closed by remote (os)"},
   {37    , "Invalid NCCH? (AM)"},
   {39    , "Invalid title version (AM)"},
   {43    , "Database doesn't exist/failed to open (AM)"},
   {44    , "Trying to uninstall system-app (AM)"},
   {47    , "Invalid command header (OS)"},
   {101   , "Archive not mounted/mount-point not found (fs)"},
   {105   , "Request timed out (http)"},
   {106   , "Invalid signature/CIA? (AM)"},
   {120   , "Title/object not found? (fs)"},
   {141   , "Gamecard not inserted? (fs)"},
   {230   , "Invalid open-flags / permissions? (fs)"},
   {271   , "Invalid configuration (mvd)."},
   {391   , "NCCH hash-check failed? (fs)"},
   {392   , "RSA/AES-MAC verification failed? (fs)"},
   {393   , "Invalid database? (AM)"},
   {395   , "RomFS/Savedata hash-check failed? (fs)"},
   {630   , "Command not allowed / missing permissions? (fs)"},
   {702   , "Invalid path? (fs)"},
   {761   , "Incorrect read-size for ExeFS? (fs)"},
   {1000  , "Invalid selection"},
   {1001  , "Too large"},
   {1002  , "Not authorized"},
   {1003  , "Already done"},
   {1004  , "Invalid size"},
   {1005  , "Invalid enum value"},
   {1006  , "Invalid combination"},
   {1007  , "No data"},
   {1008  , "Busy"},
   {1009  , "Misaligned address"},
   {1010  , "Misaligned size"},
   {1011  , "Out of memory"},
   {1012  , "Not implemented"},
   {1013  , "Invalid address"},
   {1014  , "Invalid pointer"},
   {1015  , "Invalid handle"},
   {1016  , "Not initialized"},
   {1017  , "Already initialized"},
   {1018  , "Not found"},
   {1019  , "Cancel requested"},
   {1020  , "Already exists"},
   {1021  , "Out of range"},
   {1022  , "Timeout"},
   {1023  , "Invalid result value"},
   {0, NULL}
};

static const error_string error_summary_str [] =
{
   {0,    "Success"},
   {1,    "Nothing happened"},
   {2,    "Would block"},
   {3,    "Out of resource"},
   {4,    "Not found"},
   {5,    "Invalid state"},
   {6,    "Not supported"},
   {7,    "Invalid argument"},
   {8,    "Wrong argument"},
   {9,    "Canceled"},
   {10,   "Status changed"},
   {11,   "Internal"},
   {63,   "Invalid result value"},
   {0, NULL}
};

static const error_string error_module_str [] =
{
   {0     , "Common"},
   {1     , "Kernel"},
   {2     , "Util"},
   {3     , "File server"},
   {4     , "Loader server"},
   {5     , "TCB"},
   {6     , "OS"},
   {7     , "DBG"},
   {8     , "DMNT"},
   {9     , "PDN"},
   {10    , "GX"},
   {11    , "I2C"},
   {12    , "GPIO"},
   {13    , "DD"},
   {14    , "CODEC"},
   {15    , "SPI"},
   {16    , "PXI"},
   {17    , "FS"},
   {18    , "DI"},
   {19    , "HID"},
   {20    , "CAM"},
   {21    , "PI"},
   {22    , "PM"},
   {23    , "PM_LOW"},
   {24    , "FSI"},
   {25    , "SRV"},
   {26    , "NDM"},
   {27    , "NWM"},
   {28    , "SOC"},
   {29    , "LDR"},
   {30    , "ACC"},
   {31    , "RomFS"},
   {32    , "AM"},
   {33    , "HIO"},
   {34    , "Updater"},
   {35    , "MIC"},
   {36    , "FND"},
   {37    , "MP"},
   {38    , "MPWL"},
   {39    , "AC"},
   {40    , "HTTP"},
   {41    , "DSP"},
   {42    , "SND"},
   {43    , "DLP"},
   {44    , "HIO_LOW"},
   {45    , "CSND"},
   {46    , "SSL"},
   {47    , "AM_LOW"},
   {48    , "NEX"},
   {49    , "Friends"},
   {50    , "RDT"},
   {51    , "Applet"},
   {52    , "NIM"},
   {53    , "PTM"},
   {54    , "MIDI"},
   {55    , "MC"},
   {56    , "SWC"},
   {57    , "FatFS"},
   {58    , "NGC"},
   {59    , "CARD"},
   {60    , "CARDNOR"},
   {61    , "SDMC"},
   {62    , "BOSS"},
   {63    , "DBM"},
   {64    , "Config"},
   {65    , "PS"},
   {66    , "CEC"},
   {67    , "IR"},
   {68    , "UDS"},
   {69    , "PL"},
   {70    , "CUP"},
   {71    , "Gyroscope"},
   {72    , "MCU"},
   {73    , "NS"},
   {74    , "News"},
   {75    , "RO"},
   {76    , "GD"},
   {77    , "Card SPI"},
   {78    , "EC"},
   {79    , "Web Browser"},
   {80    , "Test"},
   {81    , "ENC"},
   {82    , "PIA"},
   {83    , "ACT"},
   {84    , "VCTL"},
   {85    , "OLV"},
   {86    , "NEIA"},
   {87    , "NPNS"},
   {90    , "AVD"},
   {91    , "L2B"},
   {92    , "MVD"},
   {93    , "NFC"},
   {94    , "UART"},
   {95    , "SPM"},
   {96    , "QTM"},
   {97    , "NFP (amiibo)"},
   {254   , "Application"},
   {255   , "Invalid result value"},
   {0, NULL}
};

static const error_string error_level_str [] =
{
   {0 , "Success"},
   {1 , "Info"},
   {25, "Status"},
   {26, "Temporary"},
   {27, "Permanent"},
   {28, "Usage"},
   {29, "Reinitialize"},
   {30, "Reset"},
   {31, "Fatal"},
   {0, NULL}
};

static const char* ctr_error_to_str(const error_string* list, int id)
{
   while (list->val)
   {
      if(list->id == id)
         return list->val;
      list ++;
   }
   return "unknown";
}

void dump_result_value(Result val)
{
   if(-val < 0x80)
   {
      printf("%li(%li) : %s\n", val, _net_convert_error(val), strerror(-_net_convert_error(val)));
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

