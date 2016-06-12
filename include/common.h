#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <termios.h>
#include <signal.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>


#include "types.h"
#include "project.h"
#include "utils.h"
#include "parson.h"
#include "types.h"
#include "types.h"
#include "event.h"
#include "logger.h"

#include "list.h"

#include "cmd_process.h"
#include "logger.h"


#include "event.h"
#include "demo.h"

struct event_base *get_main_base();

#endif
