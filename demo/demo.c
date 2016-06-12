#include "common.h"
#include "demo.h"
#include"rtklib.h"

#define ID_HCNBOARD      32002
#define ID_HCNPIPE       32000
#define ID_BESTPOS       42
#define ID_SATVIS        48
#define ID_GPGSA         221
#define ID_DIREC         32003
#define ID_SAT           32001

extern int cmdfd;
extern int profd;
pthread_t tid;
pthread_mutex_t lock;
msg_head_t msg_head[5];
int sock;


int demo_get_password_fun( int sockfd, msg_head_t *pMsgHead, char *msg_data )
{
	if ( !pMsgHead )
	{
		return ERR;
	}

	if ( pMsgHead->msg_type != MSG_TYPE( CATEGORY_DEMO, OP_CODE_DEMO__GET_PASSWORD ) )
	{
		logi( "msg_type( %d, %d ) wrong\n", MSG_CATEGORY( pMsgHead->msg_type ),
                    MSG_OPCODE( pMsgHead->msg_type ) );
		return ERR;
	}

	logi( "get user( %s ) password now\n", msg_data );
	cmd_response_massage( sockfd, pMsgHead, "ok", strlen( "ok" ) );

	return OK;
}

static int state_fun( int sockfd, msg_head_t *pMsgHead, char *msg_data )
{
    int res = -1;
    char *cmd = "log oemstatus ontime 1\r\n";
#if 0
    struct tm *timer;
#endif

	if ( !pMsgHead )
	{
		return ERR;
	}

	if ( pMsgHead->msg_type != MSG_TYPE( CATEGORY_GUIDE,STATE ) )
	{
		logi( "msg_type( %d, %d ) wrong\n", MSG_CATEGORY( pMsgHead->msg_type ), 
                    MSG_OPCODE( pMsgHead->msg_type ) );
		return ERR;
	}

      if((res = write(cmdfd,cmd,strlen(cmd))) == -1){
          logi("write error");
          return ERR;
      }

      pthread_mutex_lock(&lock);
      msg_head[0] = *pMsgHead;
      sock = sockfd;
      pthread_mutex_unlock(&lock);

	return OK;
}

static int action_fun( int sockfd, msg_head_t *pMsgHead, char *msg_data )
{
    int res = -1;
    char *cmd = "log satinfo ontime 1\r\n";

	if ( !pMsgHead )
	{
		return ERR;
	}

	if ( pMsgHead->msg_type != MSG_TYPE( CATEGORY_GUIDE,ACTION) )
	{
		logi( "msg_type( %d, %d ) wrong\n", MSG_CATEGORY( pMsgHead->msg_type ), 
                    MSG_OPCODE( pMsgHead->msg_type ) );
		return ERR;
	}

      if((res = write(cmdfd,cmd,strlen(cmd))) == -1){
          logi("write error");
          return ERR;
      }

      pthread_mutex_lock(&lock);
      msg_head[1] = *pMsgHead;
      sock = sockfd;
      pthread_mutex_unlock(&lock);

	return OK;
}

static int pos_fun( int sockfd, msg_head_t *pMsgHead, char *msg_data )
{
    int res = -1;
    char *cmd = "log satinfo ontime 1\r\n";
    
	if ( !pMsgHead )
	{
		return ERR;
	}

	if ( pMsgHead->msg_type != MSG_TYPE( CATEGORY_GUIDE,POSITION) )
	{
		logi( "msg_type( %d, %d ) wrong\n", MSG_CATEGORY( pMsgHead->msg_type ), 
                    MSG_OPCODE( pMsgHead->msg_type ) );
		return ERR;
	}

      if((res = write(cmdfd,cmd,strlen(cmd))) == -1){
          logi("write error");
          return ERR;
      }

      pthread_mutex_lock(&lock);
      msg_head[2] = *pMsgHead;
      sock = sockfd;
      pthread_mutex_unlock(&lock);

	return OK;
}

static int trace_fun( int sockfd, msg_head_t *pMsgHead, char *msg_data )
{
    printf("trace_fun\n");
    int res = -1;
    char *cmd = "log satinfo ontime 1\r\n";

	if ( !pMsgHead )
	{
		return ERR;
	}

	if ( pMsgHead->msg_type != MSG_TYPE( CATEGORY_GUIDE,TRACETABLE) )
	{
		logi( "msg_type( %d, %d ) wrong\n", MSG_CATEGORY( pMsgHead->msg_type ), 
                    MSG_OPCODE( pMsgHead->msg_type ) );
		return ERR;
	}

      if((res = write(cmdfd,cmd,strlen(cmd))) == -1){
          logi("write error");
          return ERR;
      }

      pthread_mutex_lock(&lock);
      msg_head[3] = *pMsgHead;
      sock = sockfd;
      pthread_mutex_unlock(&lock);

	return OK;
}

static int setrov_fun( int sockfd, msg_head_t *pMsgHead, char *msg_data )
{
    printf("setrov\n");
    int res = -1;
    char *cmd = "fix none\r\n";

	if ( !pMsgHead )
	{
		return ERR;
	}

	if ( pMsgHead->msg_type != MSG_TYPE( CATEGORY_CONFIG,SETROV) )
	{
		logi( "msg_type( %d, %d ) wrong\n", MSG_CATEGORY( pMsgHead->msg_type ), 
                    MSG_OPCODE( pMsgHead->msg_type ) );
		return ERR;
	}

      if((res = write(cmdfd,cmd,strlen(cmd))) == -1){
          logi("write error");
          return ERR;
      }
    
//      cmd_response_massage(sockfd,pMsgHead,"ok",strlen("ok"));
#if 0
      printf("setrov successfully\n");
#endif
	return OK;
}

static int setbasepos_fun(int sockfd,msg_head_t *pMsgHead,char *msg_data){

    int res = -1;
    char cmd[1024];
    basepos_t *basepos;

    printf("set base pos\n");
    if(!msg_data || !pMsgHead) return ERR;
    if ( pMsgHead->msg_type != MSG_TYPE( CATEGORY_CONFIG,SETBASEPOS) ){
        logi( "msg_type( %d, %d ) wrong\n", MSG_CATEGORY( pMsgHead->msg_type ), 
                       MSG_OPCODE( pMsgHead->msg_type ) );
        return ERR;
    }

    basepos = (basepos_t*)msg_data;
    sprintf(cmd,"fix position %.9f %.9f %.4f\r\n",basepos->lat,basepos->lon,basepos->hgt);
    if((res = write(cmdfd,cmd,strlen(cmd))) == -1){
         logi("write error");
         return ERR;
    }

    return OK;
}

static int getpos_fun(int sockfd,msg_head_t *pMsgHead,char *msg_data){

    int res = -1;
    char *cmd = "log satinfo ontime 1\r\n";

    if(!pMsgHead) return ERR;

    if ( pMsgHead->msg_type != MSG_TYPE( CATEGORY_CONFIG,GETPOS) ){
        logi( "msg_type( %d, %d ) wrong\n", MSG_CATEGORY( pMsgHead->msg_type ), 
                       MSG_OPCODE( pMsgHead->msg_type ) );
        return ERR;
    }
    if((res = write(cmdfd,cmd,strlen(cmd))) == -1){
         logi("write error");
         return ERR;
    }

     pthread_mutex_lock(&lock);
     msg_head[4] = *pMsgHead;
     sock = sockfd;
     pthread_mutex_unlock(&lock);
    
    return OK;
}

static int setengine_fun(int sockfd,msg_head_t *pMsgHead,char *msg_data){

    int res = -1;
    char cmd[1024];
    char *postype[] ={
        "single","rtk","sbasspp","sbasppp",
        "ssrspp","ssrppp"
    };

    if(!msg_data || !pMsgHead) return ERR;
    if ( pMsgHead->msg_type != MSG_TYPE( CATEGORY_CONFIG,ENGINESET) ){
        logi( "msg_type( %d, %d ) wrong\n", MSG_CATEGORY( pMsgHead->msg_type ), 
                       MSG_OPCODE( pMsgHead->msg_type ) );
        return ERR;
    }
    sprintf(cmd,"setpvt %s\\n",postype[(int)*msg_data]);
    if((res = write(cmdfd,cmd,strlen(cmd))) == -1){
         logi("write error");
         return ERR;
    }

    return OK;
}

static int setele_fun(int sockfd,msg_head_t *pMsgHead,char *msg_data){

    int res = -1;
    char cmd[1024];
    float *data;

    if(!msg_data || !pMsgHead) return ERR;
    if ( pMsgHead->msg_type != MSG_TYPE( CATEGORY_CONFIG,ELESET) ){
        logi( "msg_type( %d, %d ) wrong\n", MSG_CATEGORY( pMsgHead->msg_type ), 
                       MSG_OPCODE( pMsgHead->msg_type ) );
        return ERR;
    }
    data = (float*)msg_data;
    sprintf(cmd,"ecutoff %f\r\n",*data);
    if((res = write(cmdfd,cmd,strlen(cmd))) == -1){
         logi("write error");
         return ERR;
    }

    return OK;
}

static int setsat_fun(int sockfd,msg_head_t *pMsgHead,char *msg_data){

    int res = -1,i,j;
    char cmd[1024],*s;
    locksat_t *locksat;
    char *sys[] = {
        "gps","gln","gal","bds","sbas"
    };

    if(!msg_data || !pMsgHead) return ERR;
    if ( pMsgHead->msg_type != MSG_TYPE( CATEGORY_CONFIG,SATSET) ){
        logi( "msg_type( %d, %d ) wrong\n", MSG_CATEGORY( pMsgHead->msg_type ), 
                       MSG_OPCODE( pMsgHead->msg_type ) );
        return ERR;
    }
    locksat = (locksat_t*)msg_data;
    for(i = 0;i < 5;i++) if(locksat->locksys[i] != 0){
        s = locksat->locksys[i] == 1 ? "lockout" : "unlockout";
        sprintf(cmd,"%s %s\r\n",s,sys[i]);
        if((res = write(cmdfd,cmd,strlen(cmd))) == -1){
             logi("write error");
             return ERR;
        }
    }
    for(j = 0;j < 5;j++){
        for(i = 0;i < MAXSAT;i++)
            if(locksat->lockprn[j][i] != 0){
                s = locksat->lockprn[j][i] == 1 ? "lockout" : "unlockout";
                sprintf(cmd,"%s %s %d\r\n",s,sys[j],i);
                if((res = write(cmdfd,cmd,strlen(cmd))) == -1){
                     logi("write error");
                     return ERR;
                }
            }
    }

    return OK;
}

static int setchannel_fun(int sockfd,msg_head_t *pMsgHead,char *msg_data){

    int res = -1,i;
    char cmd[1024],*s;
    char *fre[8] = {
        "L1","L2","L3","B1","B2","B3","GL1","GL2"
    },*sys[] = {
        "gps","gln","gal","bds","sbas"
    };
    channelset_t *channelset;
    if(!msg_data || !pMsgHead) return ERR;
    if ( pMsgHead->msg_type != MSG_TYPE( CATEGORY_CONFIG,CHANNELSET) ){
        logi( "msg_type( %d, %d ) wrong\n", MSG_CATEGORY( pMsgHead->msg_type ), 
                       MSG_OPCODE( pMsgHead->msg_type ) );
        return ERR;
    }
    channelset = (channelset_t *)msg_data;
    for(i = 0;i < 5;i++) if(channelset->closesys[i] != 0){
        s = channelset->closesys[i] == 1 ? "closesys" : "unclosesys";
        sprintf(cmd,"%s %s\r\n",s,sys[i]);
        if((res = write(cmdfd,cmd,strlen(cmd))) == -1){
             logi("write error");
             return ERR;
        }
    }
    for(i = 0;i < 8;i++)
        if(channelset->closefre[i] != 0){
            s =channelset->closefre[i] == 1 ? "close" : "unclose";
            sprintf(cmd,"%s %s\r\n",s,fre[i]);
            if((res = write(cmdfd,cmd,strlen(cmd))) == -1){
                 logi("write error");
                 return ERR;
            }
        }
    for(i = 0;i < MAXPIPE;i++)
        if(channelset->channel[i] != 0){
            s =channelset->channel[i] == 1 ? "close" : "unclose";
            sprintf(cmd,"%s %d\r\n",s,i);
            if((res = write(cmdfd,cmd,strlen(cmd))) == -1){
                 logi("write error");
                 return ERR;
            }
        }

    return OK;

}

static int setnet_fun(int sockfd,msg_head_t *pMsgHead,char *msg_data){

    int res = -1;
    char cmd[1024];
    net_t *net;

    if(!msg_data || !pMsgHead) return ERR;
    if(pMsgHead->msg_type != MSG_TYPE(CATEGORY_CONFIG,NETSET)){
        logi("msg_type(A%d,%d) wrong\n",MSG_CATEGORY(pMsgHead->msg_type),
                MSG_OPCODE(pMsgHead->msg_type));
        return -1;
    }
    net = (net_t*)msg_data;
    sprintf(cmd,"setip %s %s %s %s\r\n",net->ip,net->gateway,net->netmask,net->dns);
    if((res = write(cmdfd,cmd,strlen(cmd))) == -1){
         logi("write error");
         return -1;
    }
    
    return OK;
}

static int setdir_fun(int sockfd,msg_head_t *pMsgHead,char *msg_data){

    int res = -1;
    char cmd[1024],basebool;
    utab_t *utab;
    char *mode[] ={
        "snb","swb","dnb","swb"
    };

    if(!msg_data || !pMsgHead) return ERR;
    if(pMsgHead->msg_type != MSG_TYPE(CATEGORY_CONFIG,DIRSET)){
        logi("msg_type(A%d,%d) wrong\n",MSG_CATEGORY(pMsgHead->msg_type),
                MSG_OPCODE(pMsgHead->msg_type));
        return -1;
    }
    utab = (utab_t*)msg_data;
    basebool = utab->baseline == 0.0 ? 0 : 1;
    if(basebool) sprintf(cmd,"set %s %f\r\n",mode[utab->type+basebool],utab->baseline);
    else         sprintf(cmd,"set %s\r\n",mode[utab->type+basebool]);
    if((res = write(cmdfd,cmd,strlen(cmd))) == -1){
         logi("write error");
         return -1;
    }
    
    return OK;
}

static int setan_fun(int sockfd,msg_head_t *pMsgHead,char *msg_data){
    
    int res = -1;
    char cmd[1024],*s;

    if(!msg_data || !pMsgHead) return ERR;
    if(pMsgHead->msg_type != MSG_TYPE(CATEGORY_CONFIG,ANTNSET)){
        logi("msg_type(A%d,%d) wrong\n",MSG_CATEGORY(pMsgHead->msg_type),
                MSG_OPCODE(pMsgHead->msg_type));
        return -1;
    }
    
    s = *msg_data == 1 ? "antn1":"antn2";
    sprintf(cmd,"set %s\r\n",s);
    if((res = write(cmdfd,cmd,strlen(cmd))) == -1){
         logi("write error");
         return -1;
    }
    
    return OK;
}

static int setio_fun(int sockfd,msg_head_t *pMsgHead,char *msg_data){
    
    int res = -1,i,cmdn = 0;
    char cmd[10][1024];
    data_t *data;
    char *raw[4][10] = {
        { "ranged","gpsephemb","bdsephemb","gloephemb"},
        { "rtcm1033","rtcm1005","rtcm1006","rtcm1004b", 
            "rtcm1104b","rtcm1012b" },
        { "rtcm1033","rtcm1005","rtcm1006","rtcm1074b",
            "rtcm1124b","rtcm1084b" },
        { "gpgga","gpgsv","gprmc","gpgsr","gpgsa",
            "gputa","gpgll","gpzda","gpvtg","gpgst"}
    };
#if 0
    char buff[1024]; int len;
#endif

    if(!msg_data || !pMsgHead) return ERR;
    if(pMsgHead->msg_type != MSG_TYPE(CATEGORY_CONFIG,IOSET)){
        logi("msg_type(A%d,%d) wrong\n",MSG_CATEGORY(pMsgHead->msg_type),
                MSG_OPCODE(pMsgHead->msg_type));
        return -1;
    }
    
    data = (data_t*)msg_data;
    if(data->clientbool){
        sprintf(cmd[0],"setclient %s\r\n",data->ipport);
        if((res = write(cmdfd,cmd[0],strlen(cmd[0]))) == -1){
          logi("write error");
          return -1;
        }

    }

    for(i = 0;i < 10;i++) 
        if(data->datatime[i] != 0)
           sprintf(cmd[cmdn++],"log %s ontime %f\r\n",raw[(int)data->datatype][i],
                data->datatime[i]);


   for(i = 0;i < cmdn;i++)
       if((res = write(cmdfd,cmd[i],strlen(cmd[i]))) == -1){
          logi("write error");
          return -1;
       }

#if 0
   while((len=read(profd,buff,1024)) > 0){
       buff[len] = '\0';
       printf("%s\n",buff);
   }
#endif
    return OK;
}


#if 1
static void statesend(hcnraw_t *hcnraw){

    state_t state = { { 0 } };
    gtime_t utc;
    double timer[6];

      utc=gpst2utc(hcnraw->time);
      time2epoch(utc,timer);
#if 0
      if((timer=gmtime(&utc.time))){
          sprintf(state.utctime,"%d-%d-%d-%d-%d-%d",timer->tm_year,
                  timer->tm_mon,timer->tm_mday,timer->tm_hour,timer->tm_min,
                  timer->tm_sec);
      }
#endif
      sprintf(state.utctime,"%.0f-%.0f-%.0f-%.0f-%.0f-%.0f",
              timer[0],timer[1],timer[2],timer[3],timer[4],timer[5]);
      state.antn.mainantnworksta = hcnraw->board.mainantnworksta;
      state.antn.mainantnrfsta   = hcnraw->board.mainantnrfsta;
      state.antn.aantnworksta    = hcnraw->board.aantnworksta;
      state.antn.aantnrfsta      = hcnraw->board.aantnrfsta;
      state.cpu.acpuload         = hcnraw->board.acpuload;
      state.cpu.dcpuload         = hcnraw->board.dcpuload;
      state.cpu.astatus          = hcnraw->board.astatus;
      state.cpu.dstatus          = hcnraw->board.dstatus;
#if 1
      state.cpu.fstatus          = hcnraw->board.fstatus;
#endif
      pthread_mutex_lock(&lock);
      cmd_response_massage( sock,&msg_head[0],&state,sizeof(state_t));
      pthread_mutex_unlock(&lock);

}
#endif
#if 1
static void sendboard(hcnraw_t *hcnraw){

    statesend(hcnraw);
}

static void sendpipe(hcnraw_t *hcnraw){

}

static void sendbestpos(hcnraw_t *hcnraw){

}

static void sendsatvis(hcnraw_t *hcnraw){

}

static void senddirec(hcnraw_t *hcnraw){

}

#endif
static void getpossend(hcnraw_t *hcnraw){

    basepos_t basepos = { 0 };
    double r[3],e[3];

    if(hcnraw->satpro.satnum == 0) {
        printf("demo:satnum==0\n");
        return;
    }
    r[0]=hcnraw->satpro.x;
    r[1]=hcnraw->satpro.y;
    r[2]=hcnraw->satpro.z;
    ecef2pos(r,e);
    basepos.lat = e[0]*R2D;
    basepos.lon = e[1]*R2D;
    basepos.hgt = e[2];
    basepos.ecutoff = hcnraw->satpro.ecutoff;

    pthread_mutex_lock(&lock);
    pthread_mutex_unlock(&lock);
    cmd_response_massage( sock,&msg_head[4],&basepos,sizeof(basepos_t));

}


#if 1
static void actionsend(hcnraw_t *hcnraw){

    int i;
    sateprn_t sateprn = { { 0 } };
    char sys[] = {
        0x01,0x04,0x20,0x20,0x01,0x04,0x20,0x01,0x04,
        0x08,0x02,0x02,0x08,0x08
    };

    //action
    if(hcnraw->satpro.satnum == 0) return;
    for(i = 0;i < 5;i++) sateprn.satnu[i] = 0;
    for(i = 0;i < hcnraw->satpro.satnum;i++){
        switch(sys[hcnraw->satpro.satprosat[i].sigtype]){
            case SYS_GPS: sateprn.gps[sateprn.satnu[0]++]=
                          hcnraw->satpro.satprosat[i].svid;
                          break;
            case SYS_GLO: sateprn.glo[sateprn.satnu[1]++]=
                          hcnraw->satpro.satprosat[i].svid;
                          break;
            case SYS_GAL:sateprn.gali[sateprn.satnu[2]++]=
                         hcnraw->satpro.satprosat[i].svid;
                          break;
            case SYS_CMP: sateprn.cmp[sateprn.satnu[3]++]=
                          hcnraw->satpro.satprosat[i].svid;
                          break;
            case SYS_SBS: sateprn.sbas[sateprn.satnu[4]++]=
                          hcnraw->satpro.satprosat[i].svid;
                          break;
        }
  #if 0
        printf("svid:%d\n",hcnraw->satpro.satprosat[i].svid);
  #endif
    }
      pthread_mutex_lock(&lock);
      cmd_response_massage( sock,&msg_head[1],&sateprn,sizeof(sateprn_t));
      pthread_mutex_unlock(&lock);
}

#endif
#if 1
static void possend(hcnraw_t *hcnraw){

    int i;
    double r[3],e[3];
    posstate_t posstate = { 0 };
    char sys[] = {
        0x01,0x04,0x20,0x20,0x01,0x04,0x20,0x01,0x04,
        0x08,0x02,0x02,0x08,0x08
    };

      if(hcnraw->satpro.solsatnum <= 0) return;
      r[0]=hcnraw->satpro.x;
      r[1]=hcnraw->satpro.y;
      r[2]=hcnraw->satpro.z;
      ecef2pos(r,e);
      posstate.lat = e[0]*R2D;
      posstate.lon = e[1]*R2D;
      posstate.hgt = e[2];
      posstate.postype = hcnraw->satpro.pvtstatus;
      posstate.difftime= hcnraw->satpro.age;
      for(i = 0;i < 4;i++) posstate.stnid[i] = hcnraw->satpro.stnid[i];
      posstate.gpssec = (float)time2gpst(hcnraw->time,&posstate.gpsweek);
      posstate.pdop = hcnraw->satpro.pdop;
      posstate.hdop = hcnraw->satpro.hdop;
      posstate.vdop = hcnraw->satpro.vdop;
      posstate.solsatnu= hcnraw->satpro.solsatnum;
      for(i = 0;i < 5;i++) posstate.sateprn.satnu[i] = 0;
      for(i = 0;i < hcnraw->satpro.satnum;i++){
          if(hcnraw->satpro.satprosat[i].satstatus != 0) continue;
          switch(sys[hcnraw->satpro.satprosat[i].sigtype]){
              case SYS_GPS: posstate.sateprn.gps[posstate.sateprn.satnu[0]++]= 
                            hcnraw->satpro.satprosat[i].svid;
                            break;
              case SYS_GLO: posstate.sateprn.glo[posstate.sateprn.satnu[1]++]=
                            hcnraw->satpro.satprosat[i].svid;
                            break;
              case SYS_GAL:posstate.sateprn.gali[posstate.sateprn.satnu[2]++]=
                            hcnraw->satpro.satprosat[i].svid;
                            break;
              case SYS_CMP: posstate.sateprn.cmp[posstate.sateprn.satnu[3]++]=
                            hcnraw->satpro.satprosat[i].svid;
                            break;
              case SYS_SBS: posstate.sateprn.sbas[posstate.sateprn.satnu[4]++]=
                            hcnraw->satpro.satprosat[i].svid;
                            break;
          }

      }
      pthread_mutex_lock(&lock);
      cmd_response_massage( sock,&msg_head[2],&posstate,sizeof(posstate_t));
      pthread_mutex_unlock(&lock);

}
#endif
#if 1
static void tracesend(hcnraw_t *hcnraw){

    int i,j;
    tracetable_t tracetable = { { 0 } };
    char sys[] = {
        0x01,0x04,0x20,0x20,0x01,0x04,0x20,0x01,0x04,
        0x08,0x02,0x02,0x08,0x08
    };

    if(hcnraw->satpro.satnum <= 0) return;
    
    for(i = 0;i < 5;i++) tracetable.satnu[i] = 0;
    for(i = 0;i < hcnraw->satpro.satnum;i++){
        switch(sys[hcnraw->satpro.satprosat[i].sigtype]){
            case SYS_GPS: tracetable.gps[tracetable.satnu[0]].prn =
                              hcnraw->satpro.satprosat[i].svid;
                          tracetable.gps[tracetable.satnu[0]].elev =
                              hcnraw->satpro.satprosat[i].elevation;
                          tracetable.gps[tracetable.satnu[0]].az=
                              hcnraw->satpro.satprosat[i].azimuth;

                          for(j = 0;j < 3;j++)
                             tracetable.gps[tracetable.satnu[0]].cn[j]=
                                 hcnraw->satpro.satprosat[i].cn[j];
                          tracetable.satnu[0]++;
                          break;

            case SYS_GLO: tracetable.glo[tracetable.satnu[1]].prn =
                              hcnraw->satpro.satprosat[i].svid;
                          tracetable.glo[tracetable.satnu[1]].elev =
                              hcnraw->satpro.satprosat[i].elevation;
                          tracetable.glo[tracetable.satnu[1]].az=
                              hcnraw->satpro.satprosat[i].azimuth;

                          for(j = 0;j < 3;j++)
                             tracetable.glo[tracetable.satnu[1]].cn[j]=
                                 hcnraw->satpro.satprosat[i].cn[j];
                          tracetable.satnu[1]++;
                          break;

            case SYS_GAL: tracetable.gali[tracetable.satnu[2]].prn =
                              hcnraw->satpro.satprosat[i].svid;
                          tracetable.gali[tracetable.satnu[2]].elev =
                              hcnraw->satpro.satprosat[i].elevation;
                          tracetable.gali[tracetable.satnu[2]].az=
                              hcnraw->satpro.satprosat[i].azimuth;

                          for(j = 0;j < 3;j++)
                             tracetable.gali[tracetable.satnu[2]].cn[j]=
                                 hcnraw->satpro.satprosat[i].cn[j];
                          tracetable.satnu[2]++;
                          break;

            case SYS_CMP: tracetable.cmp[tracetable.satnu[3]].prn =
                             hcnraw->satpro.satprosat[i].svid;
                          tracetable.cmp[tracetable.satnu[3]].elev =
                              hcnraw->satpro.satprosat[i].elevation;
                          tracetable.cmp[tracetable.satnu[3]].az=
                              hcnraw->satpro.satprosat[i].azimuth;

                          for(j = 0;j < 3;j++)
                             tracetable.cmp[tracetable.satnu[3]].cn[j]=
                                 hcnraw->satpro.satprosat[i].cn[j];
                          tracetable.satnu[3]++;
                          break;

            case SYS_SBS: tracetable.sbas[tracetable.satnu[4]].prn =
                              hcnraw->satpro.satprosat[i].svid;
                          tracetable.sbas[tracetable.satnu[4]].elev =
                              hcnraw->satpro.satprosat[i].elevation;
                          tracetable.sbas[tracetable.satnu[4]].az=
                              hcnraw->satpro.satprosat[i].azimuth;

                          for(j = 0;j < 3;j++)
                             tracetable.sbas[tracetable.satnu[4]].cn[j]=
                                 hcnraw->satpro.satprosat[i].cn[j];
                          tracetable.satnu[4]++;
                          break;

        }
    }
    pthread_mutex_lock(&lock);
    cmd_response_massage( sock,&msg_head[3],&tracetable,sizeof(tracetable_t));
    pthread_mutex_unlock(&lock);
}
#endif
static void sendsat(hcnraw_t *hcnraw){

    getpossend(hcnraw);
    actionsend(hcnraw);
    possend(hcnraw);
    tracesend(hcnraw);
}


#if 1
static void *send2cgi(void *arg){

    int res = -1,type;
    hcnraw_t hcnraw = { { 0 } };

    while((res = input_hcnoem4p(&hcnraw,profd,&type)) > 0){
        switch(type){
            case ID_HCNBOARD: sendboard(&hcnraw);   break;
            case ID_HCNPIPE:  sendpipe(&hcnraw);    break;
            case ID_BESTPOS:  sendbestpos(&hcnraw); break;
            case ID_SATVIS:   sendsatvis(&hcnraw);  break;
            case ID_DIREC:    senddirec(&hcnraw);   break;
            case ID_SAT:      sendsat(&hcnraw);     break;
        }
    }
    return ((void*)0);
}

#endif
void demo_commands_init()
{

#if 1
    if(pthread_create(&tid,NULL,send2cgi,NULL) != 0){
        logi("create thread error!\n");
        exit(1);
    }
    pthread_mutex_init(&lock,NULL);
#endif

    register_msg_process_fun( MSG_TYPE( CATEGORY_DEMO, OP_CODE_DEMO__GET_PASSWORD ), 
            demo_get_password_fun  );
    register_msg_process_fun( MSG_TYPE( CATEGORY_GUIDE, STATE), 
            state_fun);
    register_msg_process_fun( MSG_TYPE( CATEGORY_GUIDE, ACTION), 
            action_fun);
    register_msg_process_fun( MSG_TYPE( CATEGORY_GUIDE, POSITION), 
            pos_fun);
    register_msg_process_fun( MSG_TYPE( CATEGORY_GUIDE, TRACETABLE), 
            trace_fun);
    register_msg_process_fun( MSG_TYPE( CATEGORY_CONFIG, SETROV), 
            setrov_fun);
    register_msg_process_fun( MSG_TYPE( CATEGORY_CONFIG, SETBASEPOS), 
            setbasepos_fun);
    register_msg_process_fun( MSG_TYPE( CATEGORY_CONFIG, GETPOS), 
            getpos_fun);
    register_msg_process_fun( MSG_TYPE( CATEGORY_CONFIG, ENGINESET), 
            setengine_fun);
    register_msg_process_fun( MSG_TYPE( CATEGORY_CONFIG,ELESET), 
            setele_fun);
    register_msg_process_fun( MSG_TYPE( CATEGORY_CONFIG,SATSET), 
            setsat_fun);
    register_msg_process_fun( MSG_TYPE( CATEGORY_CONFIG,CHANNELSET), 
            setchannel_fun);
    register_msg_process_fun( MSG_TYPE( CATEGORY_CONFIG,NETSET), 
            setnet_fun);
    register_msg_process_fun( MSG_TYPE( CATEGORY_CONFIG,DIRSET), 
            setdir_fun);
    register_msg_process_fun( MSG_TYPE( CATEGORY_CONFIG,ANTNSET), 
            setan_fun);
    register_msg_process_fun( MSG_TYPE( CATEGORY_CONFIG,IOSET), 
            setio_fun);
}

