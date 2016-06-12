#include"rtklib.h"
static int findp(char** pf,int key){
    char *pb;
    if((pb = strchr(*pf,key)) < 0) return -1;
    *pb = '\0';
    *pf = pb+1;
    printf("\npf:%s",*pf);
    return 1;
}
static void test(){
    char data[] ="$GPGSA,M,3,17,02,30,04,05,10,09,06,31,12,,,1.2,0.8,0.9*2B";
    char *p = data,*pb;
    int i,j = 0,chesum,prn[12];
    float pdop,hdop,vdop;

#if 0
    p = data;
    pb = strchr(p,',');
    *pb = '\0';
#endif
    if(findp(&p,',') > 0)
    printf("\ngps:%s",p);
#if 0
    p = pb+1;
    pb = strchr(p,',');
    *pb = '\0';
    printf("\nM:%s",p);
    
    p = strchr(p+1,',');
    *p = '\0';
    printf("%s",p);
#endif
#if 0
    getp = strtok(p,",");
    if((i = strcmp("$GPGSA",getp)) != 0) return -1;
    getp = strtok(NULL,",");
    printf("\nM:%s",getp);
    getp = strtok(NULL,",");
    printf("\node:%s",getp);
    printf("\nprn:");
    for(i = 0;i < 12;i++)
        if((getp = strtok(NULL,",")) != ""){
            printf("%s   ",getp);
            prn[j++] = atoi(getp);
        }

    getp = strtok(NULL,",");
    printf("\npdop:",getp);
    pdop = atof(strtok(NULL,","));
    hdop = atof(strtok(NULL,","));
    vdop = atof(strtok(NULL,"*"));
    chesum = atoi(p);

    printf("\npdop:%f   hdop:%f  vdop:%f",pdop,hdop,vdop);
    printf("%d  ",chesum);
    getp = strtok(p,",");
    printf("\ngpgsa: %s",getp);
    printf("\ni:%d",i);
#endif

}
static int decode0(char *filename){

    raw_t raw; int i;
    FILE* file;

    if(!(file = fopen(filename,"r"))) return -1;
    if(!(init_raw(&raw))) return -1;

    while(fgets(raw.buff,MAXRAWLEN,file)){
        decode_gpgsa(&raw);
#if 0
        printf("\nprn:");
        for(i = 0;i < PRNNUMBER;i++) printf("%d  ",raw.gpgsa.prn[i]);
#endif
#if 0
        printf("\npdop:%f  hdop:%f  vdop:%f",raw.gpgsa.pdop,raw.gpgsa.hdop,
                raw.gpgsa.vdop);
#endif

#if 0
        fputs(raw.buff,stdout);
#endif
    }
    
    fclose(file);
    free_raw(&raw);
    return 0;
}
static int decode1(char *filename){

    raw_t raw;int i,sum,data,inputsum;
    FILE * file;

    if(!(file = fopen(filename,"rb"))) return -1;
    if(!(init_raw(&raw))) return -1;

    while((inputsum = input_oem4f(&raw,file)) > 0) ;
#if 0
    while(1){
       if((data=fgetc(file)) != EOF){
         if((sum = fread(raw.buff,sizeof(char),3,file)) == 3){
            if(sync_oem4(raw.buff,()))
         }
       }
    }
#endif
    fclose(file); free_raw(&raw);
    return 0;
}
extern int decode(char *filename,int type){

    return type ? decode0(filename) : decode1(filename);
}
void tinputoem4(){

#if 0
    char *filename = "gpgsa.dat";
    decode(filename,1);
#endif
#if 1
#if 0
    char *filename = "bestposb.dat";
#endif
    char *filename  = "satvisb.dat";
    decode(filename,0);
#endif
}
int main(){
    tinputoem4();
    return 0;
}
