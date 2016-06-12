#ifndef __HC_CGI_H__
#define __HC_CGI_H__

#include <fcntl.h>
#include <pthread.h>
#include <fastcgi/fcgiapp.h>

#include "common.h"
#include "file_tool.h"

#include <types.h>
#include "cgi_local.h"

#define MAX_MSG_ID  ( 200 * 10000 )

#define HTTP_CONENT_HEAD "Content-type: text/html\r\n\r\n"


typedef int (*fun_Callback)( FCGX_Request *pstWebRequest);

extern void show_error_msg( FCGX_Request  *web_request, char *fmt, ...);

void free_web_requst( FCGX_Request  *web_request );

int web_to_cgi_cmd__get_gnss_data__parser(  FCGX_Request *pstWebRequest );


typedef struct _msg_waiter
{
	FCGX_Request  * web_request;
    struct list_head node;
    struct event timer_event;
    int    msg_id;
    int ( *response_msg_process_fun )( struct _msg_waiter *waiter, msg_head_t *head, char *data );
} msg_waiter_t;

typedef int( *response_msg_process_fun_t )( msg_waiter_t *waiter, msg_head_t *head, char *data );

int cgi_receive_response_massage( int msg_id, msg_head_t *pRspMsgHead, char **pData  );


int cgi_post_massage
(
	struct FCGX_Request  *web_request,
    int msg_type,
    char *msg_data,
    int msg_data_len,
    int *msg_id,
    struct timeval *max_wait_time,
    response_msg_process_fun_t rsp_fun
);

int cgi_send_massage( int msg_type, int *msg_id, char *msg_data, int msg_data_len );

void cmd_server_response( int sockfd, short event, void *arg );

#endif
