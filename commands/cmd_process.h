#ifndef __CMD_PROCESS_H__
#define __CMD_PROCESS_H__

/**
 *   msg_head ( fixed size )| msg_data( n bytes must be multiple of 4 )
 */

typedef struct __msg_head
{
	int msg_magic; // 0x10203040
	int msg_type;
	int msg_id;    // unique identify
	int msg_len;
} msg_head_t;

typedef int (*msg_process_fn_t)(int sockfd, msg_head_t *pMsgHead, char *msg_data);

int cmd_processor_init(void);
int register_msg_process_fun(int msg_type, msg_process_fn_t msg_process_fn);
int cmd_response_massage( int sockfd, msg_head_t *ask_msg_head, void *msg_data, int msg_data_len );

#define MSG_TYPE( category, operation_code ) ( ( ( 0xFFFF & category ) << 16 ) | ( operation_code &( 0xFFFF ) ) )

#define MSG_CATEGORY( msg_type ) ( ( msg_type >> 16 ) & 0xFFFF )
#define MSG_OPCODE( msg_type )   (  msg_type & 0xFFFF )

typedef enum
{
	CATEGORY_3G,
	CATEGORY_BT,
	CATEGORY_WIFI,
	CATEGORY_GNSS,
	CATEGORY_RTK,
	CATEGORY_RADIO,
	CATEGORY_YS_CLIENT,
	CATEGORY_DISPATCH,
	CATEGORY_MISC,
	CATEGORY_COMPACTION,
	CATEGORY_CAN,
	CATEGORY_UART,
	CATEGORY_DEMO,
      CATEGORY_GUIDE,
      CATEGORY_CONFIG
} cateory_t;


// CATEGORY_GNSS commands
typedef enum
{
	OP_CODE_GNSS__OPEN,
	OP_CODE_GNSS__GET_SATELLITE_COUNT,
	OP_CODE_GNSS__GET_POSITION,
	OP_CODE_GNSS__GET_FIX_STATUS
} operation_code_gnss_t;


// CATEGORY_DEMO
typedef enum
{
	OP_CODE_DEMO__GET_PASSWORD
} operation_code_demo_t;

typedef enum{
    STATE,                           /*状态栏     */
    ACTION,                          /*活动栏     */
    POSITION,                        /*位置       */
    TRACETABLE,                 /*跟踪表      */
    GETPOS                         /*获得当前坐标*/
}operation_code_guide_t;

typedef enum{
    SETROV,                         /*设置为移动站*/
    SETBASEPOS,                     /*设置基站坐标*/
    ENGINESET,                      /*引擎设置    */
    ELESET,                         /*高度截止角设置*/
    SATSET,                         /*卫星设置    */
    CHANNELSET,                     /*通道设置    */
    NETSET,                         /*板卡网络设置*/
    DIRSET,                         /*定向设置    */
    ANTNSET,                        /*天线设置    */
    IOSET                           /*IO设置      */
}operation_code_config_t;

#endif
