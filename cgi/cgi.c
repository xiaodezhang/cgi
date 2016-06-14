#include "cgi.h"
#include "hash.h"
#include"cmd_process.h"
static struct event_base *s_cgi_main_base = NULL;
static struct event cmd_listen_ev;
static struct event cmd_client_ev;

int s_cmd_fd = -1;
int sock = -1;
int s_msg_id_pool = 1;

static LIST_HEAD( PostMessageQueueHead );

void free_web_requst( FCGX_Request  *web_request )
{
    FCGX_Finish_r( web_request );
    free( web_request );
}

void show_error_msg( FCGX_Request  *web_request, char *fmt, ... )
{
    va_list ap;
    va_start( ap, fmt );
    
    FCGX_FPrintF( web_request->out, "Content-type: text/html\r\n\r\n" );
    FCGX_FPrintF( web_request->out, "<html><head><title>error</title></head><body>" );
    
    FCGX_VFPrintF( web_request->out, fmt,  ap );
    
    FCGX_FPrintF( web_request->out, "</body></html>" );
    free_web_requst( web_request );
    
    va_end( ap );
}

void process_web_request( FCGX_Request  *web_request )
{
    if ( web_request )
    {
        static char cmd_name[100]    = { 0 };
        char *stript_name = NULL;
        char *suffix      = NULL;
        Node *parser_node = NULL;
        logd("get data from web\n");
        stript_name = FCGX_GetParam( "SCRIPT_NAME", web_request->envp );
        if ( stript_name == NULL )
        {
            show_error_msg( web_request, "url not valid \n",  cmd_name );
        }
        else
        {
            if ( ( strlen( stript_name ) - 4 ) > 0 )
            {
                suffix = stript_name + ( strlen( stript_name ) - 4 );
                
                if ( suffix && ( strcmp( suffix, ".cmd" ) == 0 ) )
                {
                    /* copy strip_name to cmd_namd and skip "\" */
                    strncpy( cmd_name, ( stript_name + 1 ), ARRAY_SIZE( cmd_name ) );
                    	
                    cmd_name[ strlen( stript_name ) - 5 ] = '\0';
                    logd("cmd_name:%s\n",cmd_name);
                    parser_node = get_pos_in_hashtable( cmd_name );
                    if ( parser_node && parser_node->process_fun )
                    {
                        parser_node->process_fun( web_request );
                    }
                    else
                    {
                        show_error_msg( web_request, "command \"%s\" not recognize\n",  cmd_name );
                    }
                }
                else
                {
                    show_error_msg( web_request, "url not valid : cmd_name %s\n",  cmd_name );
                }
            }
            else
            {
				show_error_msg( web_request, "command error: %s\n", stript_name );
            }
        }
    }
    
}

struct event_base *get_cgi_main_base()
{
    return s_cgi_main_base;
}


void web_client_accept( int sockfd, short event, void *arg )
{
    int ret = 0;
    FCGX_Request *web_request;
	   //logd("get a web client\n");
    web_request = malloc( sizeof( FCGX_Request ) );
    memset( web_request, 0, sizeof( FCGX_Request ) );
    
    FCGX_InitRequest( web_request, FCGI_LISTENSOCK_FILENO, 0 );
    
    ret = FCGX_Accept_r( web_request );
    if ( ret == 0 )
    {
    			logd("get data from client\n");
        process_web_request( web_request );
    }
    else
    {
		free( web_request );
    }
}

int cgi_init( void )
{
    struct event_base *base = NULL;
    
    init_cgi_local();
    
    FCGX_Init();
    
    logi( "starting ...\n" );
    
    base = get_cgi_main_base();
    if ( base == NULL )
    {
        logi( "get_cgi_main_base ERR\n" );
        return OK;
    }
    
    event_assign( &cmd_listen_ev, base, FCGI_LISTENSOCK_FILENO, EV_READ | EV_PERSIST, web_client_accept, NULL );
    event_add( &cmd_listen_ev, NULL );
    
    return OK;
}

void unregister_cmd_clist_listen_event()
{
	if ( cmd_client_ev.ev_base )
	{
		event_del( &cmd_client_ev );
	}
}

void register_cmd_client_listen_event()
{
    struct event_base *base = NULL;

	base = get_cgi_main_base();
	if ( base == NULL )
	{
		logi( "get_cgi_main_base ERR\n" );
		return;
	}
	
	event_assign( &cmd_client_ev, base, s_cmd_fd, EV_READ | EV_PERSIST, cmd_server_response, &cmd_client_ev );
	event_add( &cmd_client_ev, NULL );
}


void cmd_server_response( int sockfd, short event, void *arg )
{
    msg_head_t msg_head         = { 0 };
    char  data_buf[ 8 * 1024 ]  = { 0 };
    char *large_data_buf        = NULL;
    char *cur_data_buf          = NULL;
    int len = 0;
    int readlen = 0;
    
    if ( !arg )
    {
        return;
    }
    
    if  ( !( event & EV_READ ) )
    {
        return;
    }
    
    len = read( sockfd, &msg_head, sizeof( msg_head ) );
    if ( len <= 0 )
    {
        //event_del( arg );
        //close( sockfd );
        exit( 0 );
        
        return;
    }
    
    if ( msg_head.msg_len > ( sizeof( data_buf ) - 4 ) )
    {
        large_data_buf = malloc( msg_head.msg_len + 4 );
        memset( large_data_buf, 0, ( msg_head.msg_len + 4 ) );
        
        cur_data_buf = large_data_buf;
        if ( !large_data_buf )
        {
            loge( "malloc buf failure\n" );
        }
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
            if ( len > 0 )
            {
            	readlen -= len;
            }
            else
            {
				break;
            }
        }
    }
    /*
    msg_waiter_t *pos = NULL;
    msg_waiter_t *n   = NULL;
    
    list_for_each_entry_safe( pos, n, &PostMessageQueueHead, node )
    {
        if ( pos->msg_id == msg_head.msg_id )
        {
            if ( pos->response_msg_process_fun )
            {	            	
                pos->response_msg_process_fun( pos, &msg_head, cur_data_buf );
            }

            list_del( &( pos->node ) );
            break;
        }
    }*/
    
    if ( cur_data_buf != data_buf )
    {
        free( cur_data_buf );
    }
	/*
    if ( pos )
    {
    	free(  pos );
    }*/
}


int cmd_server_connect_init()
{
    struct sockaddr_un srv_addr;
    int ret = 0;
    
    s_cmd_fd =  socket( PF_UNIX, SOCK_STREAM, 0 );
    if ( s_cmd_fd < 0 )
    {
        loge( "cannot create communication socket" );
        
        return ERR;
    }
    
    srv_addr.sun_family = AF_UNIX;
    strcpy( srv_addr.sun_path, CMD_SERVER_SOCK_PATH );
    
    ret = connect( s_cmd_fd, ( struct sockaddr * )&srv_addr, sizeof( srv_addr ) );
    if ( ret == -1 )
    {
        perror( "cannot connect to the server" );
        close( s_cmd_fd );
        s_cmd_fd = -1;
        
        return ERR;
    }
    
    logi( "connected to command server\n" );
    
	register_cmd_client_listen_event();

 
    
    return OK;
}


int cgi_send_massage( int msg_type, int *msg_id, char *msg_data, int msg_data_len )
{
    msg_head_t msg_head = { 0 };
    int        wlen     = 0;
    int        len      = 0;
    
    if ( s_cmd_fd < 0 )
    {
        return ERR;
    }
    
    msg_head.msg_magic = 0x10203040;
    msg_head.msg_id    = s_msg_id_pool;
    msg_head.msg_type  = msg_type;
    msg_head.msg_len   = msg_data_len;
    *msg_id = msg_head.msg_id;
    s_msg_id_pool++;
    if ( s_msg_id_pool > 200 * 10000 )
    {
        s_msg_id_pool = 1;
    }
    write( s_cmd_fd, &msg_head, sizeof( msg_head_t ) );
    
    wlen = msg_data_len;
    
    while ( wlen > 0 )
    {
        len = write( s_cmd_fd, msg_data + ( msg_data_len - wlen ), wlen );
        if( len > 0 )
        {
        	wlen -= len;
        }
        else
        {	
			break;
        }	
    }
    
    return OK;
}

int send_massage( int msg_type, int *msg_id, char *msg_data, int msg_data_len )
{
    msg_head_t msg_head = { 0 };
    int        wlen     = 0;
    int        len      = 0;
    
    if ( sock < 0 )
    {
        printf("sock < 0\n");
        return ERR;
    }
    
    msg_head.msg_magic = 0x10203040;
    msg_head.msg_id    = s_msg_id_pool;
    msg_head.msg_type  = msg_type;
    msg_head.msg_len   = msg_data_len;
    *msg_id = msg_head.msg_id;
    s_msg_id_pool++;
    if ( s_msg_id_pool > 200 * 10000 )
    {
        s_msg_id_pool = 1;
    }
    write(sock, &msg_head, sizeof( msg_head_t ) );
    
    wlen = msg_data_len;
    
    while ( wlen > 0 )
    {
        len = write( sock, msg_data + ( msg_data_len - wlen ), wlen );
        if( len > 0 )
        {
        	wlen -= len;
        }
        else
        {	
			break;
        }	
    }
	printf("send data to server ok\n");
    
    return OK;
}

void onWaiterOutTime( int sockfd, short event, void *arg )
{
    msg_waiter_t *waiter = arg;
    list_del( &( waiter->node ) );

    FCGX_FPrintF( waiter->web_request->out,
              HTTP_CONENT_HEAD
              "{ rsp: \"outtime\" }" );
                  
	free_web_requst( waiter->web_request );
	free( waiter );
}

int cgi_post_massage
(
    struct FCGX_Request  *web_request,
    int msg_type,
    char *msg_data,
    int msg_data_len,
    int *msg_id,
    struct timeval *max_wait_time,
    response_msg_process_fun_t rsp_fun
)
{
    msg_head_t msg_head = { 0 };
    int        wlen     = 0;
    int        len      = 0;
    msg_waiter_t *waiter    = NULL;
    struct event_base *base = NULL;
    struct timeval default_wait_time = { 0 };
    
    if ( s_cmd_fd < 0 )
    {
        return ERR;
    }
    
    msg_head.msg_magic = 0x10203040;
    msg_head.msg_id    = s_msg_id_pool;
    msg_head.msg_type  = msg_type;
    msg_head.msg_len   = msg_data_len;
    *msg_id = msg_head.msg_id;
    
    s_msg_id_pool++;
    if ( s_msg_id_pool > MAX_MSG_ID )
    {
        s_msg_id_pool = 1;
    }
    
    write( s_cmd_fd, &msg_head, sizeof( msg_head_t ) );
    if ( msg_data != NULL )
    {
        wlen = msg_data_len;
        while ( wlen > 0 )
        {
            len = write( s_cmd_fd, msg_data + ( msg_data_len - wlen ), wlen );
            wlen -= len;
        }
    }
    
    waiter = malloc( sizeof( msg_waiter_t ) );
    if ( waiter == NULL )
    {
        return ERR;
    }
    
    memset( waiter, 0, sizeof( msg_waiter_t ) );
    waiter->msg_id = msg_head.msg_id;
    waiter->response_msg_process_fun = rsp_fun;
    waiter->web_request = web_request;
    
    base = get_cgi_main_base();
    if ( base == NULL )
    {
        return ERR;
    }
    
    evtimer_assign( &( waiter->timer_event ), base, onWaiterOutTime , waiter );
    list_add( &( waiter->node ), &PostMessageQueueHead );
    
    if ( max_wait_time == NULL )
    {
        default_wait_time.tv_sec  = 2 * 60;
        default_wait_time.tv_usec = 0;
        max_wait_time = &default_wait_time;
    }
    
    evtimer_add( &( waiter->timer_event ), max_wait_time );
    
    return OK;
}

int cgi_receive_response_massage( int msg_id, msg_head_t *pRspMsgHead, char **pData  )
{
    msg_head_t msg_head         = { 0 };
    char  data_buf[ 8 * 1024 ]  = { 0 };
    char *large_data_buf        = NULL;
    char *cur_data_buf          = NULL;
    int len = 0;
    int readlen = 0;

    unregister_cmd_clist_listen_event();
    
    while ( 1 )
    {
        memset( data_buf, 0, sizeof( data_buf ) );
        len = read( s_cmd_fd, &msg_head, sizeof( msg_head ) );
        if ( len < sizeof( msg_head ) )
        {
        	register_cmd_client_listen_event();
            return ERR;
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
        else
        {
            cur_data_buf = data_buf;
        }
        
        if ( msg_head.msg_len > 0 )
        {
            readlen = msg_head.msg_len;
            while ( readlen )
            {
                len = read( s_cmd_fd, cur_data_buf + ( msg_head.msg_len - readlen ), readlen );
                if( len > 0 )
                {
                	readlen -= len;
                }
                else
                {
                	loge( "read server data error\n" );
					break;
                }
            }
        }
        
        if ( msg_id == msg_head.msg_id )
        {
            *pRspMsgHead = msg_head;
            if ( msg_head.msg_len > 0 )
            {
                *pData = malloc( msg_head.msg_len + 4 );
                if ( !*pData )
                {
                    loge( "malloc for msg failure\n" );
                    
                    register_cmd_client_listen_event();
                    return ERR;
                }
                
                memset( *pData,  0, msg_head.msg_len + 4 );
                memcpy( *pData, cur_data_buf, msg_head.msg_len );
                
                if ( cur_data_buf != data_buf )
                {
                    free( cur_data_buf );
                }
            }
            else
            {
                *pData = NULL;
            }
            
            register_cmd_client_listen_event();
            
            return OK;
        }
        else
        {/*
            msg_waiter_t *pos = NULL;
            msg_waiter_t *n   = NULL;
            list_for_each_entry_safe( pos, n, &PostMessageQueueHead, node )
            {
                if ( pos->msg_id == msg_head.msg_id )
                {
                    if ( pos->response_msg_process_fun )
                    {                    	
                        pos->response_msg_process_fun( pos, &msg_head, cur_data_buf );
                    }

                    list_del( &( pos->node ) );
		            break;
                }
            }*/

		    if ( cur_data_buf != data_buf )
		    {
		        free( cur_data_buf );
		    }
/*
		    if ( pos )
		    {	
		    	free(  pos );
		    }*/
		 }
	  }

	  register_cmd_client_listen_event();
    
    return OK;
}
int main( void )
{
    logger_init( "/data/cgi.log" );
    
    s_cgi_main_base = event_base_new();
    
    cmd_server_connect_init();
    cgi_init();
    
    event_base_dispatch( s_cgi_main_base );

    return 0;
}
