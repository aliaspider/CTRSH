

TARGET = ctrsh
OBJS :=
OBJS += main.o

CFLAGS += -g -O0

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) $(LIBDIRS) $(LIBS) -Wall -o $@

clean:
	rm -f $(TARGET) $(OBJS)
