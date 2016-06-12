#include <stdio.h>
#include "cgi_local.h"
#include "hash.h"


int get_password_cmd__parser( FCGX_Request *pstWebRequest );
int get_receiver_guide_cmd__parser( FCGX_Request *pstWebRequest );
int get_receiver_config_cmd__parser( FCGX_Request *pstWebRequest );


//web cmd
CmdNodeWeb web_cmd_array[ ] =
{
	// get_name command just for cgi demo, implemented in demo_cmd.c
	{ "get_receiver_guide", get_receiver_guide_cmd__parser},  // just for demo
      { "get_receiver_config",get_receiver_config_cmd__parser}

};


int load_funSet_from_web()
{
	int i = 0;

	for ( i = 0; i < ARRAY_SIZE( web_cmd_array ); i++ )
	{
		add_element_to_hashtable( web_cmd_array[i].cmd_name, web_cmd_array[i].process_fun );
			logd("get function:%s\n",web_cmd_array[i].cmd_name);
	}

	return OK;
}

int init_cgi_local()
{
	init_hashtable();

	return  load_funSet_from_web();
}


int clear_cgi_local()
{
	cleanup_hashtable();

	return OK;
}
