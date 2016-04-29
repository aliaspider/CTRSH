

TARGET = ctrsh
OBJS :=
OBJS += main.o
OBJS += file_list.o

CFLAGS += -g -O0 -Werror=implicit-function-declaration
LDFLAGS += -g -O0
LIBS += -lreadline
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) $(LIBDIRS) $(LIBS) -Wall -o $@

clean:
	rm -f $(TARGET) $(OBJS)
