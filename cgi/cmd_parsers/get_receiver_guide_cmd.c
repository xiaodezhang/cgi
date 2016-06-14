#include "cgi.h"

char *guide[]={
	"state",
	"action",
	"position",
	"tracetable",
      "getpos"
};
int guide_msg_type[]={
	MSG_TYPE( CATEGORY_GUIDE, STATE),
	MSG_TYPE( CATEGORY_GUIDE, ACTION),
	MSG_TYPE( CATEGORY_GUIDE, POSITION),
	MSG_TYPE( CATEGORY_GUIDE, TRACETABLE),
	MSG_TYPE( CATEGORY_GUIDE, GETPOS)
};
char *state_process(char *data);
char *action_process(char *data);
char *position_process(char *data);
char *tracetable_process(char *data);
char *getpos(char *data);

char *(*(guide_fun[]))(char *)={
	state_process,
	action_process,
	position_process,
	tracetable_process,
      getpos
};

char *state_process(char *data)
{
    JSON_Value *val = NULL;
    JSON_Object *object = NULL;
    char *response= NULL;
    logd("state_process\n");
	state_t *state = NULL;
	state = (state_t *)data;
	
	val = json_value_init_object();
    object = json_value_get_object( val );
    if ( !object ){
        json_value_free( val );
        loge( "json_value_get_object error\n" );
        return NULL;
    }
	/*
	printf("utc:%s\n",state->utctime);
	printf("mainantnworksta:%s\n",state->antn.mainantnworksta==0?"NORMAL":"INTROUBLE");
	printf("mainantnrfsta:%s\n",state->antn.mainantnrfsta==0?"NORMAL":"INTROUBLE");
	printf("aantnworksta:%s\n",state->antn.aantnworksta==0?"NORMAL":"INTROUBLE");
	printf("aantnrfsta:%s\n",state->antn.aantnrfsta==0?"NORMAL":"INTROUBLE");
	printf("acpuload:%02d\n",state->cpu.acpuload);
	printf("dcpuload:%02d\n",state->cpu.dcpuload);
	printf("astatus:%s\n",state->cpu.astatus==0?"NORMAL":"INTROUBLE");
	printf("dstatus:%s\n",state->cpu.dstatus==0?"NORMAL":"INTROUBLE");
	*/
	json_object_set_string(object,"utc",state->utctime);
	json_object_set_string(object,"mainantnworksta",state->antn.mainantnworksta==0?"NORMAL":"INTROUBLE");
	json_object_set_string(object,"mainantnrfsta",state->antn.mainantnrfsta==0?"NORMAL":"INTROUBLE");
	json_object_set_string(object,"aantnworksta",state->antn.aantnworksta==0?"NORMAL":"INTROUBLE");
	json_object_set_string(object,"aantnrfstan",state->antn.aantnrfsta==0?"NORMAL":"INTROUBLE");
	json_object_set_number(object,"acpuload",state->cpu.acpuload);
	json_object_set_number(object,"dcpuload",state->cpu.dcpuload);
	json_object_set_string(object,"astatus",state->cpu.astatus==0?"NORMAL":"INTROUBLE");
	json_object_set_string(object,"dstatus",state->cpu.dstatus==0?"NORMAL":"INTROUBLE");
    
    response = json_serialize_to_string( val );
    json_value_free( val );
	return response;
}
char *action_process(char *data)
{
    JSON_Value *val = NULL;
    JSON_Object *object = NULL;
    JSON_Array *sysarr[5];
    char *response= NULL;
    int i;  
    sateprn_t *satellite = NULL;
    satellite = (sateprn_t *)data;
    char *sys[] = {
        "gps_prn","glo_prn","gali_prn","bd_prn","sbas_prn"
    },*sysnum[] = {
        "gps_num","glo_num","gali_num","bd_num","sbas_num"
    };

    val = json_value_init_object();
    object = json_value_get_object( val );
    if ( !object ){
       json_value_free( val );
       loge( "json_value_get_object error\n" );
       return NULL;
    }
    for(i = 0;i < 5;i++){
        json_object_set_number(object,sysnum[i],satellite->satnu[i]);
        if(satellite->satnu[i] > 0){
           json_object_set_value(object,sys[i], json_value_init_array());
           sysarr[i] = json_object_get_array(object,sys[i]);
        }
#if 0
        if(satellite->satnu[i] > 0)
            for(j = 0;j < satellite->satnu[i];j++)
                json_array_append_string(sysarr[i],satellite->)
#endif
    }


    if(satellite->satnu[0] > 0)
        for(int i = 0; i < satellite->satnu[0]; i++){
              json_array_append_number(sysarr[0],satellite->gps[i]);
    }
    
    if(satellite->satnu[1] > 0) 
        for(int i = 0; i < satellite->satnu[1]; i++){
              json_array_append_number(sysarr[1],satellite->glo[i]);
        }

    if(satellite->satnu[2] > 0)
        for(int i = 0; i < satellite->satnu[2]; i++){
              json_array_append_number(sysarr[2],satellite->gali[i]);
        }
    if(satellite->satnu[3] > 0)
        for(int i = 0; i < satellite->satnu[3]; i++){
              json_array_append_number(sysarr[3],satellite->cmp[i]);

        }
    if(satellite->satnu[4] > 0)
        for(int i = 0; i < satellite->satnu[4]; i++){
              json_array_append_number(sysarr[4],satellite->sbas[i]);
    }

    response = json_serialize_to_string( val );
    json_value_free( val );
    return response;
}

char *position_process(char *data)
{
    JSON_Value *val = NULL;
    JSON_Object *object = NULL;
    JSON_Array *sysarr[5];
    char *response= NULL;
    char *type[] ={
        "SINGLE","RTK","SBASSPP","SBASPPP",
        "SSRSPP","SSRPPP"
    },*sysprn[] = {
        "gps_prn","glo_prn","gali_prn",
        "bd_prn","sbas_prn"
    },*sysnum[] = {
        "gps_num","glo_num","gali_num","bd_num","sbas_num"
    };
    int i;

    posstate_t *posstate= NULL;
    posstate = (posstate_t *)data;
    val = json_value_init_object();
    object = json_value_get_object( val );
    if ( !object ){
        json_value_free( val );
        loge( "json_value_get_object error\n" );
        return NULL;
    }
    

    json_object_set_number(object,"lat",posstate->lat);
    json_object_set_number(object,"lon",posstate->lon);
    json_object_set_number(object,"high",posstate->hgt);
    json_object_set_string(object,"postype",type[posstate->postype]);
    json_object_set_number(object,"difftime",posstate->difftime);
    json_object_set_string(object,"baseid",posstate->stnid);
    json_object_set_number(object,"gpsweek",posstate->gpsweek);
    json_object_set_number(object,"gpssec",posstate->gpssec);
    json_object_set_number(object,"pdop",posstate->pdop);
    json_object_set_number(object,"hdop",posstate->hdop);
    json_object_set_number(object,"vdop",posstate->vdop);
    json_object_set_number(object,"solution",posstate->solsatnu);

    json_object_set_number(object,"gps_num",posstate->sateprn.satnu[0]);
    json_object_set_number(object,"glo_num",posstate->sateprn.satnu[1]);
    json_object_set_number(object,"gali_num",posstate->sateprn.satnu[2]);
    json_object_set_number(object,"bd_num",posstate->sateprn.satnu[3]);
    json_object_set_number(object,"sbas_num",posstate->sateprn.satnu[4]);

    for(i = 0;i < 5;i++){
        json_object_set_number(object,sysnum[i],posstate->sateprn.satnu[i]);
        if(posstate->sateprn.satnu[i] > 0)
        json_object_set_value(object,sysprn[i],json_value_init_array());
        sysarr[i]= json_object_get_array(object,sysprn[i]);
    }

    if(posstate->sateprn.satnu[0] > 0)
        for(int i = 0; i < posstate->sateprn.satnu[0]; i++){
              json_array_append_number(sysarr[0],posstate->sateprn.gps[i]);
    }
    
    if(posstate->sateprn.satnu[1] > 0) 
        for(int i = 0; i < posstate->sateprn.satnu[1]; i++){
              json_array_append_number(sysarr[1],posstate->sateprn.glo[i]);
        }

    if(posstate->sateprn.satnu[2] > 0)
        for(int i = 0; i < posstate->sateprn.satnu[2]; i++){
              json_array_append_number(sysarr[2],posstate->sateprn.gali[i]);
        }
    if(posstate->sateprn.satnu[3] > 0)
        for(int i = 0; i < posstate->sateprn.satnu[3]; i++){
              json_array_append_number(sysarr[3],posstate->sateprn.cmp[i]);

        }
    if(posstate->sateprn.satnu[4] > 0)
        for(int i = 0; i < posstate->sateprn.satnu[4]; i++){
              json_array_append_number(sysarr[4],posstate->sateprn.sbas[i]);
    }

    response = json_serialize_to_string( val );
    json_value_free( val );
	return response;
}
char *tracetable_process(char *data)
{
    JSON_Value *val = NULL;
    JSON_Object *object = NULL;
    JSON_Array *gps_array= NULL;
    JSON_Array *glo_array= NULL;
    JSON_Array *gali_array= NULL;
    JSON_Array *bd_array= NULL;
    JSON_Array *sbas_array= NULL;
    char *response= NULL;

    JSON_Value *val_loop = NULL;
    JSON_Object *object_loop = NULL;

	tracetable_t *tracetable = NULL;
	tracetable = (tracetable_t *)data;

	val = json_value_init_object();
    object = json_value_get_object( val );
    if ( !object ){
        json_value_free( val );
        loge( "json_value_get_object error\n" );
        return NULL;
    }
	
	json_object_set_number(object,"gps_num",tracetable->satnu[0]);
	json_object_set_number(object,"glo_num",tracetable->satnu[1]);
	json_object_set_number(object,"gali_num",tracetable->satnu[2]);
	json_object_set_number(object,"bd_num",tracetable->satnu[3]);
	json_object_set_number(object,"sbas_num",tracetable->satnu[4]);

	json_object_set_value(object, "gps_sat", json_value_init_array());
	json_object_set_value(object, "glo_sat", json_value_init_array());
	json_object_set_value(object, "gali_sat", json_value_init_array());
	json_object_set_value(object, "bd_sat", json_value_init_array());
	json_object_set_value(object, "sbas_sat", json_value_init_array());

	gps_array = json_object_get_array(object, "gps_sat");
	glo_array = json_object_get_array(object, "glo_sat");
	gali_array = json_object_get_array(object, "gali_sat");
	bd_array = json_object_get_array(object, "bd_sat");
	sbas_array = json_object_get_array(object, "sbas_sat");

	for(int i = 0; i < tracetable->satnu[0]; i++){
		val_loop = json_value_init_object();
		object_loop = json_value_get_object(val_loop);
		
		json_object_set_number(object_loop,"prn",tracetable->gps[i].prn);
		json_object_set_number(object_loop,"elevation",tracetable->gps[i].elev);
		json_object_set_number(object_loop,"azimuth",tracetable->gps[i].az);
		json_object_set_number(object_loop,"L1",tracetable->gps[i].cn[0]);
		json_object_set_number(object_loop,"L2",tracetable->gps[i].cn[1]);
		json_object_set_number(object_loop,"L5",tracetable->gps[i].cn[2]);
		json_array_append_value(gps_array,val_loop);
	}
	for(int i = 0; i < tracetable->satnu[1]; i++){
		val_loop = json_value_init_object();
		object_loop = json_value_get_object(val_loop);

		json_object_set_number(object_loop,"prn",tracetable->glo[i].prn);
		json_object_set_number(object_loop,"elevation",tracetable->glo[i].elev);
		json_object_set_number(object_loop,"azimuth",tracetable->glo[i].az);
		json_object_set_number(object_loop,"L1",tracetable->glo[i].cn[0]);
		json_object_set_number(object_loop,"L2",tracetable->glo[i].cn[1]);
		json_object_set_number(object_loop,"L5",tracetable->glo[i].cn[2]);
		json_array_append_value(glo_array,val_loop);
	}
	for(int i = 0; i < tracetable->satnu[2]; i++){
		val_loop = json_value_init_object();
		object_loop = json_value_get_object(val_loop);

		json_object_set_number(object_loop,"prn",tracetable->gali[i].prn);
		json_object_set_number(object_loop,"elevation",tracetable->gali[i].elev);
		json_object_set_number(object_loop,"azimuth",tracetable->gali[i].az);
		json_object_set_number(object_loop,"L1",tracetable->gali[i].cn[0]);
		json_object_set_number(object_loop,"L2",tracetable->gali[i].cn[1]);
		json_object_set_number(object_loop,"L5",tracetable->gali[i].cn[2]);
		json_array_append_value(gali_array,val_loop);
	}
	for(int i = 0; i < tracetable->satnu[3]; i++){
		val_loop = json_value_init_object();
		object_loop = json_value_get_object(val_loop);

		json_object_set_number(object_loop,"prn",tracetable->cmp[i].prn);
		json_object_set_number(object_loop,"elevation",tracetable->cmp[i].elev);
		json_object_set_number(object_loop,"azimuth",tracetable->cmp[i].az);
		json_object_set_number(object_loop,"L1",tracetable->cmp[i].cn[0]);
		json_object_set_number(object_loop,"L2",tracetable->cmp[i].cn[1]);
		json_object_set_number(object_loop,"L5",tracetable->cmp[i].cn[2]);
		json_array_append_value(bd_array,val_loop);
	}
	
	for(int i = 0; i < tracetable->satnu[4]; i++){
		val_loop = json_value_init_object();
		object_loop = json_value_get_object(val_loop);

		json_object_set_number(object_loop,"prn",tracetable->sbas[i].prn);
		json_object_set_number(object_loop,"elevation",tracetable->sbas[i].elev);
		json_object_set_number(object_loop,"azimuth",tracetable->sbas[i].az);
		json_object_set_number(object_loop,"L1",tracetable->sbas[i].cn[0]);
		json_object_set_number(object_loop,"L2",tracetable->sbas[i].cn[1]);
		json_object_set_number(object_loop,"L5",tracetable->sbas[i].cn[2]);
		json_array_append_value(sbas_array,val_loop);
	}

    response = json_serialize_to_string( val );
    json_value_free( val );
	return response;
}
char *getpos(char *data)
{
    JSON_Value *val = NULL;
    JSON_Object *object = NULL;
    char *response= NULL;
    logd("getpos\n");
    basepos_t *basepos;

    basepos = (basepos_t*)data;
    val = json_value_init_object();
    object = json_value_get_object( val );
    if ( !object ){
        json_value_free( val );
        loge( "json_value_get_object error\n" );
        return NULL;
    }
    json_object_set_number(object,"hgt",basepos->hgt);
    json_object_set_number(object,"hgt",basepos->lat);
    json_object_set_number(object,"hgt",basepos->lon);
    response = json_serialize_to_string( val );

    json_value_free( val );
    return response;
}

int get_receiver_guide_cmd__parser( FCGX_Request *pstWebRequest )
{
    char *pstrDat          =  NULL;
    char *query_string     =  NULL;
    URL_BUF_STRUCT url_buf = { { 0 } };

    char *response_string = NULL;
    
    int msg_id = 0;
	msg_head_t stRspMsgHead;
	char * pData = NULL;
    
    if ( NULL == pstWebRequest  )
    {
        return ERR;
    }
      logd("I get here\n");
    query_string = FCGX_GetParam( "QUERY_STRING", pstWebRequest->envp );
    if ( query_string == NULL )
    {
        FCGX_FPrintF( pstWebRequest->out,
                      HTTP_CONENT_HEAD
                      "{ rsp: \"no query string\"}" );
                      
        free_web_requst( pstWebRequest );
        
        return ERR;
    }
    
    url_parse( &url_buf, query_string );
	
    pstrDat = get_param_value( &url_buf, "guide" );
    if ( !pstrDat )
    {
        FCGX_FPrintF( pstWebRequest->out,
                      HTTP_CONENT_HEAD
                      "{ rsp: \"parameter error\"}" );
                      
        free_web_requst( pstWebRequest );
        
        return ERR;
    }
	
    logd("get guide=%s\n",pstrDat);
    	//while(1);
	char *(*pguide_fun)(char *) = NULL;
	logd("sizeof:%d\n",sizeof(guide)/sizeof(guide[0]));
	for(int i = 0;i < sizeof(guide)/sizeof(guide[0]);i++)
	{
		if(strcmp(pstrDat,guide[i]) == 0)//find the string
		{
			logd("find string:%s\n",pstrDat);
                  logd("send\n");
			cgi_send_massage(guide_msg_type[i],&msg_id, pstrDat, strlen( pstrDat ) );
                  logd("receive\n");
			cgi_receive_response_massage( msg_id, &stRspMsgHead, &pData );
                  logd("guide_fun\n");
			pguide_fun = guide_fun[i];
			if(pguide_fun){
                      logd("response_string\n");
				response_string = pguide_fun(pData);
                  }
			else{
				loge("get guide_fun err\n");
                  }
		}
	}
	
    
      logd("fprintf\n");
    FCGX_FPrintF( pstWebRequest->out,
                  HTTP_CONENT_HEAD
                  "%s", response_string );
                  
    free( response_string );
    free_web_requst( pstWebRequest );


	if ( pData )
	{
		free( pData );
	}

    return OK;
}

