#include "common.h"
#include <string.h>


struct event_base *s_base = NULL;


void devices_init()
{

}

struct event_base *get_main_base()
{
	return s_base;
}


int main( int argc, char *argv[] )
{

	logger_init( NULL );

	logi( "compaction application startup ...\n" );

	s_base = event_base_new();

	cmd_processor_init();
	devices_init();

	event_base_dispatch( s_base );



	return 0;
}
