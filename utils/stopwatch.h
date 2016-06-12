#ifndef __HC_STOPWATCH_H__
#define __HC_STOPWATCH_H__

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdint.h>
#include <sys/time.h>


typedef struct stopwatch
{
    struct timeval start_tm;
    struct timeval end_tm;
    unsigned int   gap; /* uint -> micro second */
} StopWatch, *StopWatchPtr;

extern void stopwatch_start(  StopWatchPtr sw );
extern void stopwatch_stop(  StopWatchPtr sw );
extern void stopwatch_reset(  StopWatchPtr sw );
extern void stopwatch_show(  StopWatchPtr sw );


#endif

