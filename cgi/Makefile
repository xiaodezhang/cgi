

TARGET=N72.fcgi

CC_PRE = arm-none-linux-gnueabi-
CC = $(CC_PRE)gcc


SRCS=cgi.c \
	  cgi_local/cgi_local.c \
	  cgi_local/url_parser.c \
	  cgi_local/hash.c \
	  cgi_local/file_tool.c  \
	  cmd_parsers/get_receiver_guide_cmd.c \
	  cmd_parsers/get_receiver_config_cmd.c \


SRCS+= ../utils/logger.c \
       ../parson/parson.c 
		   

CFLAGS= -g -Wall -Werror -O0 -std=gnu99 \
        -I./ \
	-Icgi_local \
	-Icmd_parsers  \

CFLAGS += -I../include \
          -I../uart \
          -I../demo \
          -I../parson\
          -I../utils \
          -I../commands 
	


LDFLAGS = -L../lib \
         -lfcgi \
         -lpthread \
         -lrt  \
         -levent_core \
         -levent_extra


all:
	$(CC) -o $(TARGET)  $(SRCS) $(CFLAGS) $(LDFLAGS) 

clean:
		rm  -rf $(TARGET)


