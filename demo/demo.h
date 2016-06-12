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
    unsigned char mainantnworksta;         /*�����߹������ 0������ 1 ���� */
    unsigned char mainantnrfsta;           /*��������Ƶͨ�����������ͬ�� */
    unsigned char aantnworksta;            /*�����߹���״����ͬ��         */
    unsigned char aantnrfsta;              /*�����߹���״����ͬ��         */
}antenna_t;

typedef struct{
    unsigned char acpuload;                /*armоƬcpuռ����             */
    unsigned char dcpuload;                /*dspоƬcpuռ����             */
    unsigned char astatus;                 /*armоƬ������� 0������  1������*/
    unsigned char dstatus;                 /*dspоƬ������� 0������  1������*/
#if 1
    unsigned char fstatus;                 /*fpgaоƬ���������ͬ��        */
#endif
}cpu_t;

typedef struct{
    char utctime[MAXTIMELEN];               /*     UTCʱ��       */
    antenna_t antn;                   /* ���ߺ�������Ƶͨ���������*/
    cpu_t     cpu;                   /*  оƬ�������      */
}state_t;

typedef struct{
    unsigned char satnu[5];                /* satnu[0]:GPS���������� 
                                              satnu[1]:GLONASS����������Ϊ
                                              Galileo,������SBAS  */
    unsigned char gps[MAXPRNGPS];          /* ���ٵ���GPS���ǵ�prn��   */
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


typedef struct{                              /*�ַ���             */
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
    double lat;                   /*      ����       */
    double lon;
    double hgt;
    unsigned char postype;        /*��λ����        */ 
    float difftime;               /*����ӳ�        */
    char stnid[4];                /*��վid          */
    int  gpsweek;                 /*GPS��           */
    float gpssec;                 /*GPS��           */
    float pdop;
    float hdop;
    float vdop;
    unsigned char solsatnu;          /*ʹ�ö�λ�������� */
    sateprn_t sateprn;            /*prn              */
}posstate_t;

typedef struct{
    unsigned char prn;                     /*prn                 */
    double        elev;                    /*�߶Ƚ�              */
    double        az;                      /*��λ��              */
    unsigned char cn[3];                   /*����Ƶ�������      */
}prncn_t;

typedef struct{
    unsigned char satnu[5];                /* satnu[0]:GPS���������� 
                                              satnu[1]:GLONASS����������Ϊ
                                              Galileo,������SBAS  */
    prncn_t gps[NSATGPS];                 /*GPS��������           */
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
                                           locksys[1]:GLONASS,��������Ϊ
                                           Galileo,������SBAS  */
    unsigned char lockprn[5][MAXSAT];
}locksat_t;

typedef struct{
    unsigned char closesys[5];            /*closesys[0]:GPS
                                           closesys[1]:GLONASS,��������Ϊ
                                           Galileo,������SBAS  */
    unsigned char closefre[8];            /*0-7 : "L1","L2","L3","B1","B2",
                                            "B3","GL1","GL2" */
    unsigned char channel[MAXPIPE];       /*����ͨ����        */
}channelset_t;

typedef struct{
    unsigned char type;                    /*0:��Ƶ����  1��˫Ƶ����*/
    float         baseline;                /*���߳���               */
}utab_t;

typedef struct{
    unsigned char clientbool;               /*0���ǿͻ���ģʽ 1���ͻ���ģʽ */
    unsigned char ipport[MAXIP+MAXPORT];    /*ip�Ͷ˿ں�,ex:192 168 3 100 700*/
    unsigned char datatype;                 /*0:ԭʼ���� 
                                              1:rtcm3.0 
                                              2:rtcm3.2
                                              3:nmea  */
    float datatime[10];                    /* ���ݴ���Ƶ�� 0��ʾ�����      */
}data_t;
void demo_commands_init();

#endif
