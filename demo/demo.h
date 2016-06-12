#ifndef __DEMO_H__
#define __DEMO_H__
#include <time.h>
#include"rtklib.h"

#define SIGLE       0
#define RTK         1
#define SBASSPP     2
#define SBASPPP     3
#define SSRSPP      4
#define SSRPPP      5

#define MAXIP   50
#define MAXPORT 10
#define MAXTIMELEN 1024


typedef struct{
    unsigned char mainantnworksta;         /*主天线工作情况 0：正常 1 故障 */
    unsigned char mainantnrfsta;           /*主天线射频通道工作情况，同上 */
    unsigned char aantnworksta;            /*副天线工作状况，同上         */
    unsigned char aantnrfsta;              /*副天线工作状况，同上         */
}antenna_t;

typedef struct{
    unsigned char acpuload;                /*arm芯片cpu占用率             */
    unsigned char dcpuload;                /*dsp芯片cpu占用率             */
    unsigned char astatus;                 /*arm芯片工作情况 0：正常  1：故障*/
    unsigned char dstatus;                 /*dsp芯片工作情况 0：正常  1：故障*/
#if 1
    unsigned char fstatus;                 /*fpga芯片工作情况，同上        */
#endif
}cpu_t;

typedef struct{
    char utctime[MAXTIMELEN];               /*     UTC时间       */
    antenna_t antn;                   /* 天线和天线射频通道工作情况*/
    cpu_t     cpu;                   /*  芯片工作情况      */
}state_t;

typedef struct{
    unsigned char satnu[5];                /* satnu[0]:GPS卫星数量， 
                                              satnu[1]:GLONASS，以下依次为
                                              Galileo,北斗，SBAS  */
    unsigned char gps[MAXPRNGPS];          /* 跟踪到的GPS卫星的prn号   */
    unsigned char glo[MAXPRNGLO];
    unsigned char gali[MAXPRNGAL];
    unsigned char cmp[MAXPRNCMP];
    unsigned char sbas[MAXPRNSBS];
}sateprn_t;

typedef struct{
    unsigned char sys;                       /* GPS || BDS || GLONASS   */
    unsigned char obstype;                   /*L1,L2,L5 || B1,B2,B3 || L1,L2*/
    unsigned char state;                     /*0 : close 1: open         */
}channel_t;


typedef struct{                              /*字符串             */
    unsigned char ip[MAXIP];      
    unsigned char gateway[MAXIP];
    unsigned char netmask[MAXIP];
    unsigned char dns[MAXIP];
}net_t;

#if 0
typedef struct{
    unsigned char type;
    float         baseline;
}direc_t;
#endif

typedef struct{
    double lat;                   /*      经度       */
    double lon;
    double hgt;
    unsigned char postype;        /*定位类型        */ 
    float difftime;               /*差分延迟        */
    char stnid[4];                /*基站id          */
    int  gpsweek;                 /*GPS周           */
    float gpssec;                 /*GPS秒           */
    float pdop;
    float hdop;
    float vdop;
    unsigned char solsatnu;          /*使用定位的卫星数 */
    sateprn_t sateprn;            /*prn              */
}posstate_t;

typedef struct{
    unsigned char prn;                     /*prn                 */
    double        elev;                    /*高度角              */
    double        az;                      /*方位角              */
    unsigned char cn[3];                   /*各个频点载噪比      */
}prncn_t;

typedef struct{
    unsigned char satnu[5];                /* satnu[0]:GPS卫星数量， 
                                              satnu[1]:GLONASS，以下依次为
                                              Galileo,北斗，SBAS  */
    prncn_t gps[NSATGPS];                 /*GPS卫星属性           */
    prncn_t glo[NSATGLO];
    prncn_t gali[NSATGAL];
    prncn_t cmp[NSATCMP];
    prncn_t sbas[NSATSBS];
}tracetable_t;

typedef struct{
    double lat;
    double lon;
    double hgt;
    float ecutoff;
}basepos_t;


typedef struct{
    unsigned char locksys[5];            /*locksys[0]:GPS
                                           locksys[1]:GLONASS,以下依次为
                                           Galileo,北斗，SBAS  */
    unsigned char lockprn[5][MAXSAT];
}locksat_t;

typedef struct{
    unsigned char closesys[5];            /*closesys[0]:GPS
                                           closesys[1]:GLONASS,以下依次为
                                           Galileo,北斗，SBAS  */
    unsigned char closefre[8];            /*0-7 : "L1","L2","L3","B1","B2",
                                            "B3","GL1","GL2" */
    unsigned char channel[MAXPIPE];       /*具体通道号        */
}channelset_t;

typedef struct{
    unsigned char type;                    /*0:单频定向  1：双频定向*/
    float         baseline;                /*基线长度               */
}utab_t;

typedef struct{
    unsigned char clientbool;               /*0：非客户端模式 1：客户端模式 */
    unsigned char ipport[MAXIP+MAXPORT];    /*ip和端口号,ex:192 168 3 100 700*/
    unsigned char datatype;                 /*0:原始数据 
                                              1:rtcm3.0 
                                              2:rtcm3.2
                                              3:nmea  */
    float datatime[10];                    /* 数据传输频率 0表示不输出      */
}data_t;
void demo_commands_init();

#endif
