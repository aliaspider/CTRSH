
TARGET = ctrsh

OBJS :=
OBJS += main.o
OBJS += commands.o
OBJS += soc_stdout.o
OBJS += commands/cmd_exit.o
OBJS += commands/cmd_ls.o
OBJS += commands/cmd_put.o
OBJS += commands/cmd_quit.o
OBJS += commands/cmd_send.o
OBJS += utils/file_list.o
OBJS += utils/printf_wrap.o
OBJS += utils/options.o

CFLAGS += -g -O0 -Werror=implicit-function-declaration
CFLAGS += -I.
LDFLAGS += -g -O0
LIBS += -lreadline -lpthread
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) $(LIBDIRS) $(LIBS) -Wall -o $@

clean:
	rm -f $(TARGET) $(OBJS)
