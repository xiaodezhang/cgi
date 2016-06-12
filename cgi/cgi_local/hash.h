#ifndef __HC_CGI_HASH_H__
#define __HC_CGI_HASH_H__


#include <string.h>
#include <stdlib.h>
#include "cgi.h"


#define SIZE 400


typedef struct
{
    char cmd_name[50];
    fun_Callback   process_fun;
} CmdNodeWeb, CmdNodeWebPtr;


typedef struct _Node
{
    char            cmd_name[50];
    fun_Callback    process_fun;
    struct  _Node  *next;
} Node, NodePtr;


//static Node* hashtable[SIZE];

int    init_hashtable();
unsigned int   cal_pos_in_hashtable( char *s );
Node       *get_pos_in_hashtable( char *s );
int    add_element_to_hashtable( char *cmd_name, fun_Callback   process_fun );
int    show_hashtable();
int    cleanup_hashtable();

#endif
