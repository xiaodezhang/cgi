TARGET=compaction


CC_PRE = arm-none-linux-gnueabi-
CC = $(CC_PRE)gcc

CFLAGS= -g -Wall -Werror -O0 -std=gnu99 \
       -I./ \
       -I./include \
       -I./oem6 \
       -I./demo \
       -I./parson\
       -I./utils \
       -I./commands
	


LDFLAGS=-L./lib -lrt -lm -Wl,-Bstatic -levent_core -levent_extra -Wl,-Bdynamic \
        -lpthread -lz

# -lbluetooth
#bluetooth/bluetooth.c

SRCS= ./parson/parson.c \
      ./utils/logger.c \
      ./utils/utils.c \
      ./utils/stopwatch.c \
	./oem6/rtkcmn.c \
	./oem6/rcvraw.c \
	./oem6/novatel.c \
      ./demo/demo.c \
      ./commands/cmd_process.c \
      ./main.c

all:
	$(CC) -o $(TARGET)  $(SRCS) $(CFLAGS) $(LDFLAGS) 
	$(MAKE) -C cgi

clean:
	rm  -rf $(TARGET)
	$(MAKE) -C cgi clean
	
