#ifndef __UTILTS_H__
#define __UTILTS_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <signal.h>
#include <stdarg.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/time.h>
#include <sys/shm.h>
#include <pthread.h>
#include <sys/stat.h>
#include <stopwatch.h>


extern int check_nic( char *nic_name );
extern int find_pid_by_name(char *ProcName, int *foundpid);
extern int find_pid_by_name_zc( const char* ProcName, int* foundpid, int maxcount);
extern char * base64_encode( const unsigned char * bindata, char * base64, int binlength );
extern int zccompress(const char *data, const int len, char *out, unsigned long *outlen);
#endif
