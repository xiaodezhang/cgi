#include "stopwatch.h"

void stopwatch_start(  StopWatchPtr sw )
{
    gettimeofday( &sw->start_tm, NULL );
}

void stopwatch_stop(  StopWatchPtr sw )
{
    unsigned int gap = 0;
    
    gettimeofday( &sw->end_tm, NULL );
    gap = ( sw->end_tm.tv_sec - sw->start_tm.tv_sec ) * 1000 * 1000;
    gap = ( gap + sw->end_tm.tv_usec ) - sw->start_tm.tv_usec;
    sw->gap = gap;
}

void stopwatch_reset(  StopWatchPtr sw )
{
    memset( sw, 0, sizeof( StopWatch ) );
}

void stopwatch_show(  StopWatchPtr sw )
{
    fprintf( stderr,    "stopwatch start <%lu(s), %lu(us)>\n"
			            "stopwatch stop  <%lu(s), %lu(us)>\n"
			            "stopwatch eclipse : %f(s) | %f(ms) | %lu(us)\n",
			            sw->start_tm.tv_sec, sw->start_tm.tv_usec,
			            sw->end_tm.tv_sec, sw->end_tm.tv_usec,
			            ( sw->gap * 1.0e-6 ), ( sw->gap * 1.0e-3 ), ( unsigned long )sw->gap );
}



