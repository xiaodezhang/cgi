#ifndef __HC_CGI_TOOL_H__
#define __HC_CGI_TOOL_H__

#include <config.h>
#include "utils/utils.h"
#include "util.h"
#include <sys/time.h>
#include <string.h>


int array_length_get(int array_name)
{
	return sizeof(array_name) / sizeof(array_name[0]);
}


int get_index_for_dataFormat(char* p)
{
    if(0 == strcmp(p, "close"))
    {
    	return DAT_FRQ_OFF;
    }
	else if(0 == strcmp(p, "auto"))
	{
		return DAT_FRQ_AUTO;
	}
	else if(0 == strcmp(p, "10HZ"))
	{
	    return DAT_FRQ_10HZ;
	}
	else if(0 == strcmp(p, "5HZ"))
	{
	    return DAT_FRQ_5HZ;
	}
	else if(0 == strcmp(p, "2HZ"))
	{
		return DAT_FRQ_2HZ;
	}
	else if(0 == strcmp(p, "1s"))
	{
		return  DAT_FRQ_1HZ;
	}
	else if(0 == strcmp(p, "2s"))
	{
	    return DAT_FRQ_2S;
	}
	else if(0 == strcmp(p, "5s"))
	{
		return DAT_FRQ_5S;
	}
	else if(0 == strcmp(p, "10s"))
	{
		return DAT_FRQ_10S;
	}
	else if(0 == strcmp(p, "15s"))
	{
		return DAT_FRQ_15S;
	}
	else if(0 == strcmp(p, "30s"))
	{
		return DAT_FRQ_30S;
	}
	else if(0 == strcmp(p, "1min"))
	{
		return DAT_FRQ_1M;
	}
	else if(0 == strcmp(p, "5min"))
	{
		return DAT_FRQ_5M;
	}
	else if(0 == strcmp(p, "10min"))
	{
		return DAT_FRQ_10M;
	}
    return 0;
}


//get standard io index by offVlaue
int get_local_io_index(char command[MAX_CMD_COUNT][MAX_DETAIL_LENGTH], int offVlaue)
{
    int result = -1;
	char numCh[5] = {0};
	char* p = command[1];

	p = p + offVlaue;
	strcpy(numCh, p);
	result = atoi(numCh);

	return result;
}


/////////////////////////////////////////////////
//convert local index to standard index
//type:   0:tcp_client  1:tcp_server  2:com  3:ntrip_client 4:ntrip_server 5:ntrip_caster
//is not end, and later continue....
///////////////////////////////////////////////////
int convert_index_to_standndex(int n, int type)
{
    int result = -1;
	if(0 == type)
	{
		switch(n)
		{
			case 1:
				result = TCP_CLIENT_1;
				break;
			case 2:
				result = TCP_CLIENT_2;
				break;
			case 3:
				result = TCP_CLIENT_3;
				break;
			case 4:
				result = TCP_CLIENT_4;
				break;
			case 5:
				result = TCP_CLIENT_5;
				break;
			
			default:;
		}
	}
	else if(1 == type)
	{
		switch(n)
		{
			case 1:
				result = TCP_SERVER_1;
				break;
			case 2:
				result = TCP_SERVER_2;
				break;
			default:;
		}
	}
	else if(2 == type)
	{
		switch(n)
		{
			case 2:
				result = COM_PORT_ID__COM2;
				break;
			case 3:
				result = COM_PORT_ID__COM3;
				break;
			default:;
		}
	}
	else if(3 == type)
	{
		switch(n)
		{
		    case 1:
				result = NTRIP_CLIENT_1;
				break;
			case 2:
				result = NTRIP_CLIENT_2;
				break;
			case 3:
				result = NTRIP_CLIENT_2;
				break;
			default:;
		}
	}
	else if(4 == type)
	{
		switch(n)
		{
		    case 1:
				result = NTRIP_SERVER_1;
				break;
			case 2:
				result = NTRIP_SERVER_2;
				break;
			case 3:
				result = NTRIP_SERVER_3;
				break;
			default:;
		}
	}
	else if(5 == type)
	{
		switch(n)
		{
		    case 1:
				result = NTRIP_CASTER_1;
				break;
			case 2:
				result = NTRIP_CASTER_2;
				break;
			case 3:
				result = NTRIP_CASTER_3;
				break;
			default:;
		}
	}
	

	return result;
	
}


#endif



