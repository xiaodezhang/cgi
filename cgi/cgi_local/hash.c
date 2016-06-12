#include <stdio.h>
#include "hash.h"

static Node *hashtable[SIZE] = { 0 };

int init_hashtable()
{
    
    memset( hashtable, 0x0, ARRAY_SIZE( hashtable ) );
    
    return OK;
}

unsigned int cal_pos_in_hashtable( char *s )
{
    unsigned int h = 0;
    
    while ( *s )
    {
        h = *s + h * 31;
        s++;
    }
    
    return ( h % SIZE );
}

Node *get_pos_in_hashtable( char *s )
{
    unsigned int  pos = cal_pos_in_hashtable( s );
    Node      *p   = hashtable[pos];
    
    while ( p != NULL )
    {
        if ( !strcmp( p->cmd_name, s ) )
        {
            return p;
        }
        
        p = p->next;
    }
    
    return NULL;
    
}

int add_element_to_hashtable( char *cmd_name, fun_Callback   process_fun )
{
    int    res    = 0;
    unsigned int   n_pos  = 0;
    Node       *p_node = NULL;
    
    if ( strlen( cmd_name ) <= 0 )
    {
        res = -2;
    }
    
    if ( ( p_node = get_pos_in_hashtable( cmd_name ) ) == NULL )
    {
        n_pos = cal_pos_in_hashtable( cmd_name );
        p_node = ( Node * )malloc( sizeof( Node ) );
        if ( p_node == NULL )
        {
            return -1;
        }
        
        memcpy( p_node->cmd_name, cmd_name, strlen( cmd_name ) ); //forword insert method
        p_node->process_fun = process_fun;
        p_node->next = hashtable[n_pos];
        hashtable[n_pos] = p_node;
        
        return 1;
        
    }
    else
    {
        return 0;
    }
}

int show_hashtable()
{
    int i;
    Node *t;
    
    for ( i = 0; i < SIZE; i++ )
    {
        if ( hashtable[i] == NULL )
        {
            printf( "()" );
        }
        else
        {
            t = hashtable[i];
            printf( "(" );
            for ( ; t != NULL; t = t->next )
            {
                printf( "(%s) ", t->cmd_name );
            }
            printf( ")\n" );
        }
    }
    
    return 1;
}

int cleanup_hashtable()
{
    int i;
    Node *p, *t;
    
    for ( i = 0; i < SIZE; i++ )
    {
        p = hashtable[i];
        while ( p != NULL )
        {
            t = p->next;
            free( p->cmd_name );
            free( p );
        }
    }
    return 0;
}

