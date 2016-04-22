

TARGET = ctrsh
OBJS :=
OBJS += main.o

CFLAGS += -g -O0
LDFLAGS += -g -O0
LIBS += -lreadline
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) $(LIBDIRS) $(LIBS) -Wall -o $@

clean:
	rm -f $(TARGET) $(OBJS)
