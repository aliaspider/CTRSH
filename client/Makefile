
TARGET = ctrsh
DEBUG = 1

OBJS :=
OBJS += main.o
OBJS += commands.o
OBJS += utils/c_str.o
OBJS += utils/file_list.o
OBJS += utils/printf_wrap.o
OBJS += utils/options.o
OBJS += serverctrl/display.o
OBJS += serverctrl/file.o
OBJS += serverctrl/misc.o
OBJS += serverctrl/recv.o
OBJS += serverctrl/send.o
OBJS += commands/cmd_exit.o
OBJS += commands/cmd_ls.o
OBJS += commands/cmd_put.o
OBJS += commands/cmd_quit.o
OBJS += commands/cmd_send.o
OBJS += commands/cmd_test.o

ifeq ($(DEBUG), 1)
   CFLAGS += -g -O0 -DDEBUG
else
   CFLAGS += -O3 -DNDEBUG
endif

LDFLAGS += $(CFLAGS)

CFLAGS += -Werror=implicit-function-declaration
CFLAGS += -I.

LIBS += -lreadline -lpthread

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) $(LIBDIRS) $(LIBS) -Wall -o $@

clean:
	rm -f $(TARGET) $(OBJS)
