#include"cgi.h"
#include<string.h>
#include<stdio.h>

char *config[] = {
    "setrov","setbasepos","getpos",
    "engineset","eleset","satset",
    "channelset","netset","dirset",
    "antnset","ioset"
};
int guide_msg_type2[]={
	MSG_TYPE( CATEGORY_GUIDE, STATE),
	MSG_TYPE( CATEGORY_GUIDE, ACTION),
	MSG_TYPE( CATEGORY_GUIDE, POSITION),
	MSG_TYPE( CATEGORY_GUIDE, TRACETABLE)
};

int config_msg_type[] = {
    MSG_TYPE(CATEGORY_CONFIG,SETROV),
    MSG_TYPE(CATEGORY_CONFIG,SETBASEPOS),
    MSG_TYPE(CATEGORY_CONFIG,GETPOS),
    MSG_TYPE(CATEGORY_CONFIG,ENGINESET),
    MSG_TYPE(CATEGORY_CONFIG,ELESET),
    MSG_TYPE(CATEGORY_CONFIG,SATSET),
    MSG_TYPE(CATEGORY_CONFIG,CHANNELSET),
    MSG_TYPE(CATEGORY_CONFIG,NETSET),
    MSG_TYPE(CATEGORY_CONFIG,DIRSET),
    MSG_TYPE(CATEGORY_CONFIG,ANTNSET),
    MSG_TYPE(CATEGORY_CONFIG,IOSET),
};

#if 1
static char *tstring(){

    JSON_Value *val = NULL;
    JSON_Object *object = NULL;
    char *response= NULL;

    val = json_value_init_object();
    object = json_value_get_object( val );
    if ( !object ){
        json_value_free( val );
        return NULL;
    }
    json_object_set_string(object,"ok","ok");
    
    response = json_serialize_to_string( val );
    json_value_free( val );
	return response;

}
#endif
static char *state_process2(char *data){

    return "ok";
}
char *guide2[]={
	"state",
};

char *(*(guide_fun2[]))(char *)={
	state_process2,
};

static char* setrov(URL_BUF_STRUCT *url){

    return "ok";
}

static char* setbasepos(URL_BUF_STRUCT *url){

    basepos_t *basepos;
    char *s;

    basepos = (basepos_t*)malloc(sizeof(basepos_t));
    if(!(s = get_param_value(url,"lat"))) return NULL;
    basepos->lat = atof(s);

    if(!(s = get_param_value(url,"lon"))) return NULL;
    basepos->lon= atof(s);

    if(!(s = get_param_value(url,"hgt"))) return NULL;
    basepos->hgt = atof(s);

    return (char *)basepos;
}

static char* getpos(URL_BUF_STRUCT *url){

    return NULL;
}

static char* engineset(URL_BUF_STRUCT *url){

    char *eng;
    char *s;

    eng = (char*)malloc(sizeof(char));
    if(!(s = get_param_value(url,"eng"))) return NULL;
    *eng = atoi(s);
    return (char*)eng;
}

static char* eleset(URL_BUF_STRUCT *url){

    float *ele;
    char *s;

    ele = (float*)malloc(sizeof(float));
    if(!(s = get_param_value(url,"ele"))) return NULL;
    *ele = atof(s);
    return (char*)ele;
}

static char* satset(URL_BUF_STRUCT *url){

    locksat_t *locksat;
    int i,j,num;
    char *sys,*prn,*s;

    locksat = (locksat_t*)malloc(sizeof(locksat_t));
    for(i = 0;i < 5;i++){
        sprintf(sys,"%d",i);
        if(!(s = get_param_value(url,strcat("locksys",sys)))) return NULL;
        locksat->locksys[i] = atoi(s);

        if(!(s = get_param_value(url,strcat("num",sys)))) return NULL;
        if((num = atoi(s)) > 0)
           for(j = 0;j < num;j++){
            sprintf(prn,"%d",i*5+j);
            if(!(s = get_param_value(url,strcat("lockprn",prn))))
                return NULL;
            locksat->lockprn[i][j] = atoi(s);
           }
    }

    return (char*)locksat;
}

static char* channelset(URL_BUF_STRUCT *url){

    channelset_t *channelset;
    int i;
    char *s,*sys,*fre,*chan;

    channelset = (channelset_t*)malloc(sizeof(channelset_t));
    for(i = 0;i < 5;i++){
        sprintf(sys,"%d",i);
        if(!(s = get_param_value(url,strcat("closesys",sys)))) return NULL;
        channelset->closesys[i] = atoi(s);
    }
    for(i = 0;i < 8;i++){
        sprintf(fre,"%d",i);
        if(!(s = get_param_value(url,strcat("closefre",fre)))) return NULL;
        channelset->closefre[i] = atoi(s);
    }
    for(i = 0;i < MAXPIPE;i++){
        sprintf(chan,"%d",i);
        if(!(s = get_param_value(url,strcat("channel",chan)))) return NULL;
        channelset->channel[i] = atoi(s);
    }

    return (char*)channelset;
}

static char* netset(URL_BUF_STRUCT *url){

    net_t *net;
    char *s;

    net = (net_t*)malloc(sizeof(net_t));
    if(!(s = get_param_value(url,"ip"))) return NULL;
    strcpy((char*)net->ip,s);
    if(!(s = get_param_value(url,"gateway"))) return NULL;
    strcpy((char*)net->gateway,s);
    if(!(s = get_param_value(url,"netmask"))) return NULL;
    strcpy((char*)net->netmask,s);
    if(!(s = get_param_value(url,"dns"))) return NULL;
    strcpy((char*)net->dns,s);

    return (char*)net;
}

static char* dirset(URL_BUF_STRUCT *url){

    utab_t *utab;
    char *s;

    utab = (utab_t*)malloc(sizeof(utab_t));
    if(!(s = get_param_value(url,"type"))) return NULL;
    utab->type = atoi(s);
    if(!(s = get_param_value(url,"baseline"))) return NULL;
    utab->baseline = atof(s);

    return (char*)utab;
}

static char* antnset(URL_BUF_STRUCT *url){

    char *antn,*s;

    antn = (char*)malloc(sizeof(char));
    if(!(s = get_param_value(url,"antn"))) return NULL;
    *antn = atoi(s);

    return antn;
}

static char* ioset(URL_BUF_STRUCT *url){

    data_t *data;
    int i;
    char *s,*time;

    data = (data_t*)malloc(sizeof(data_t));
    if(!(s = get_param_value(url,"clientbool"))) return NULL;
    data->clientbool = atoi(s);
    if(!(s = get_param_value(url,"ipport"))) return NULL;
    strcpy((char*)data->ipport,s);
    if(!(s = get_param_value(url,"datatype"))) return NULL;
    data->datatype = atoi(s);
    for(i = 0;i < 10;i++){
        sprintf(time,"%d",i);
        if(!(s = get_param_value(url,strcat("datatype",time)))) return NULL;
        data->datatime[i] = atof(s);
    }

    return (char*)data;
}

char *(*(config_fun[]))(URL_BUF_STRUCT* url) ={
    setrov,setbasepos,getpos,
    engineset,eleset,satset,
    channelset,netset,dirset,
    antnset,ioset
};

extern int get_receiver_config_cmd__parser(FCGX_Request *pstWebRequest){

    char *pstrDat          =  NULL;
    char *query_string     =  NULL;
    char *sendstring;
    URL_BUF_STRUCT url_buf = { { 0 } };

    int msg_id = 0;
    msg_head_t stRspMsgHead;
    char * pData = NULL;
    char *restring;
    
    if ( NULL == pstWebRequest  )
    {
        return ERR;
    }
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
    pstrDat = get_param_value( &url_buf, "config" );
    if ( !pstrDat )
    {
        FCGX_FPrintF( pstWebRequest->out,
                      HTTP_CONENT_HEAD
                      "{ rsp: \"parameter error\"}" );
                      
        free_web_requst( pstWebRequest );
        
        return ERR;
    }
	
    logd("get config=%s\n",pstrDat);
#if 0
    char *(*pconfig_fun)(URL_BUF_STRUCT*) = NULL;
#endif
    logd("sizeof:%d\n",sizeof(config)/sizeof(config[0]));
    for(int i = 0;i < sizeof(config)/sizeof(config[0]);i++)
    {
          if(strcmp(pstrDat,config[i]) == 0)//find the string
          {
#if 0
                pconfig_fun = config_fun[i];
                sendstring = pconfig_fun(&url_buf);
#endif
                sendstring = "sendstring";
                if(sendstring){
                    logd("before send\n");
                   cgi_send_massage(config_msg_type[i],&msg_id,sendstring, strlen( sendstring) );
                   logd("before receive\n");
                   cgi_receive_response_massage( msg_id, &stRspMsgHead, &pData );
                   logd("receive\n");
                   
#if 0
                   logd("%s\n",pData);
                   free(sendstring);
#endif
                }
          }
    }
#if 1
    restring = tstring();
    logd("restring:%s\n",restring);
   FCGX_FPrintF( pstWebRequest->out,
                  HTTP_CONENT_HEAD
                  "%s",restring );
   free(restring);
#endif
    free_web_requst( pstWebRequest );
    if ( pData )
    {
          free( pData );
    }

    return OK;
}

