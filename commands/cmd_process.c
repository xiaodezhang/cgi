#include "common.h"
#include "cmd_process.h"
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<stdlib.h>
#include<string.h>
#include<stdio.h>

#define cmdpath "/tmp/fifo_web_decode"
#define propath "/tmp/fifo_web_decode_protocol"

void cmd_client_accept( int sock, short event, void *arg );
void cmd_client_process( int sock, short event, void *arg );
int cmdfd;
int profd;

int msg_procces( int sockfd,  msg_head_t *pMsgHead, char *msg_data );

typedef struct _cmd_chain_node {
	int              msg_type;
	msg_process_fn_t msg_process_fn;
	struct _cmd_chain_node *next;
} cmd_chain_node_t;

static cmd_chain_node_t *cmd_chain = NULL;




int register_msg_process_fun( int msg_type, msg_process_fn_t msg_process_fn )
{
	cmd_chain_node_t *node      = NULL;
	cmd_chain_node_t *new_node  = NULL;
	cmd_chain_node_t *last_node = NULL;

	new_node = malloc( sizeof( cmd_chain_node_t ) );
	if ( !new_node )
	{
		return ERR;
	}

	new_node->msg_process_fn = msg_process_fn;
	new_node->msg_type = msg_type;

	if ( !cmd_chain )
	{
		cmd_chain = new_node;
		return OK;
	}


	node = cmd_chain;
	while ( node )
	{
		if ( node->msg_type == msg_type )
		{
			free( new_node );
			loge( "msg_type : %d ,%d aready registered\n", MSG_CATEGORY( msg_type ), MSG_OPCODE( msg_type ) );
			return ERR;
		}

		last_node = node;
		node = node->next;
	}

	last_node->next = new_node;


	return OK;
}

int find_msg_process_fun( int msg_type, msg_process_fn_t *msg_process_fn )
{
	cmd_chain_node_t *node = NULL;

	if ( !msg_process_fn )
	{
		return ERR;
	}

	node = cmd_chain;
	while ( node )
	{
		if ( node->msg_type == msg_type )
		{
			*msg_process_fn = node->msg_process_fn;
			return OK;
		}
		node = node->next;
	}

	return ERR;
}

static struct event	   cmd_listen_ev = {{0}};

int init_cmd_process_server( void )
{
	struct event_base *base = NULL;

	int server_fd = -1;
	int ret = 0;

	struct sockaddr_un srv_addr;

	server_fd = socket( PF_UNIX, SOCK_STREAM, 0 );
	if ( server_fd < 0 )
	{
		loge( "cannot create communication socket" );
		return ERR;
	}

	srv_addr.sun_family = AF_UNIX;
	strncpy( srv_addr.sun_path, CMD_SERVER_SOCK_PATH, sizeof( srv_addr.sun_path ) - 1 );
	unlink( CMD_SERVER_SOCK_PATH );

	ret = bind( server_fd, ( struct sockaddr * )&srv_addr, sizeof( srv_addr ) );
	if ( ret == -1 )
	{
		loge( "cannot bind server socket" );
		close( server_fd );
		unlink( CMD_SERVER_SOCK_PATH );
		return 1;
	}

	// only allow cgi to connect
	ret = listen( server_fd, 1 );
	if ( ret == -1 )
	{
		loge( "cannot listen the client connect request" );
		close( server_fd );
		unlink( CMD_SERVER_SOCK_PATH );
		return ERR;
	}


	base = get_main_base();
	event_assign( &cmd_listen_ev, base, server_fd, EV_READ | EV_PERSIST, cmd_client_accept, NULL );
	event_add( &cmd_listen_ev, NULL );

	return OK;
}

void cmd_client_accept( int sockfd, short event, void *arg )
{
	socklen_t addrlen = 0;
	int client_fd     = -1;
	struct sockaddr_un clt_addr;

	struct event_base *base = NULL;
	struct event       *cmd_client_ev = NULL;

	if	( !( event & EV_READ ) )
	{
		return;
	}

	cmd_client_ev = calloc( 1, sizeof( struct event ) );
	if ( !cmd_client_ev )
	{
		loge( "calloc event failure\n" );
		return;
	}

	client_fd = accept( sockfd, ( struct sockaddr * )&clt_addr, &addrlen );
	if ( client_fd < 0 )
	{
		loge( "accept client connect request failure\n" );
	}

	logi( "accept cmd client connect request\n" );

	base = get_main_base();
	event_assign( cmd_client_ev, base, client_fd, EV_READ | EV_PERSIST, cmd_client_process, cmd_client_ev  );
	event_add( cmd_client_ev, NULL );
}


void cmd_client_process( int sockfd, short event, void *arg )
{
	msg_head_t msg_head         = { 0 };
	char  data_buf[ 8 * 1024 ]  = { 0 };
	char *large_data_buf        = NULL;
	char *cur_data_buf          = NULL;
	int len = 0;
	int readlen = 0;
	int ret = 0;

	if ( !arg )
	{
		return;
	}

	if	( !( event & EV_READ ) )
	{
		return;
	}

	len = read( sockfd, &msg_head, sizeof( msg_head ) );
	if ( len <= 0 )
	{
		event_del( arg );
		close( sockfd );

		return;
	}

	if ( msg_head.msg_len >  sizeof( data_buf ) )
	{
		large_data_buf = malloc( msg_head.msg_len );
		memset( large_data_buf, 0, msg_head.msg_len );

		cur_data_buf = large_data_buf;
		if ( !large_data_buf )
		{
			loge( "malloc buf failure\n" );
		}
	}
	else if ( msg_head.msg_len > 0 )
	{
		cur_data_buf = data_buf;
	}
	else
	{
		cur_data_buf = data_buf;
	}

	if ( msg_head.msg_len > 0 )
	{
		readlen = msg_head.msg_len;

		while ( readlen > 0 )
		{
			len = read( sockfd, cur_data_buf + ( msg_head.msg_len - readlen ), readlen );
			if( len > 0 )
			{
				readlen -= len;
			}
			else
			{
				break;
			}
		}
	}

	//loge( "processing msg_type( %d, %d ) \n", MSG_CATEGORY( msg_head.msg_type ), MSG_OPCODE( msg_head.msg_type ) );

	ret = msg_procces( sockfd, &msg_head, cur_data_buf );
	if ( ret == ERR )
	{
		loge( "process msg_type( %d, %d ) failure\n", MSG_CATEGORY( msg_head.msg_type ), MSG_OPCODE( msg_head.msg_type ) );
	}

	if ( cur_data_buf != data_buf )
	{
		free( cur_data_buf );
	}
}

int msg_procces( int sockfd, msg_head_t *pMsgHead, char *msg_data )
{
	int ret = ERR;
	msg_process_fn_t msg_process_fn = NULL;


	if ( !pMsgHead )
	{
		return ERR;
	}


	ret = find_msg_process_fun( pMsgHead->msg_type, &msg_process_fn );
	if ( ret == OK )
	{
		if ( msg_process_fn )
		{
			return msg_process_fn( sockfd, pMsgHead, msg_data );
		}
	}

	return ERR;
}

int echo_msg_proc_fun( int sockfd, msg_head_t *pMsgHead, char *msg_data )
{
	if ( !pMsgHead )
	{
		return ERR;
	}

	if ( pMsgHead->msg_type == 1 )
	{
		logi( "data: %s\n", msg_data );
	}


	return OK;
}


int cmd_response_massage( int sockfd, msg_head_t *ask_msg_head, void *msg_data, int msg_data_len )
{
    msg_head_t msg_head = { 0 };
    int        wlen     = 0;
    int        len      = 0;
    unsigned  char *data = msg_data;
    
    if ( sockfd <= 0 )
    {
        return ERR;
    }
    msg_head.msg_magic = 0x10203040;
    msg_head.msg_id    = ask_msg_head->msg_id;
    msg_head.msg_type  = ask_msg_head->msg_type;
    msg_head.msg_len   = msg_data_len;
    
    write( sockfd, &msg_head, sizeof( msg_head_t ) );
    if ( msg_data_len > 0 )
    {
	    wlen = msg_data_len;
	    while ( wlen > 0 )
	    {
	        len = write( sockfd, data + ( msg_data_len - wlen ), wlen );
	        if ( len >  0)
	        {
	        	wlen -= len;
	        }
	        else
	        {
	        	close( sockfd );
				break;
	        }	        
	    }
    }
    
    return OK;
}

void commands_init( void )
{

    if((cmdfd = open(cmdpath,O_WRONLY)) == -1){
        logi("open error");
        return;
    }
    if((profd = open(propath,O_RDONLY)) == -1){
        logi("open error");
        return;
    }
	demo_commands_init();
}


int cmd_processor_init( void )
{
	init_cmd_process_server();

	commands_init();

	logi( "cmd processor start ...\n" );

	return OK;
}

