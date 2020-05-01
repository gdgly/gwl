#include "file.h"
#include "Tools.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "local.h"

sCircuitSysInfo CircuitSysInfo;

char *LocationLabal[] = 
{
    "CS_F91_LONGITUDE",
    "CS_F91_DIMENSION",
};

char *MainStaInfoLabal[] = 
{
    "CS_F3_IP1",
    "CS_F3_PORT1",
    "CS_F3_IP2",
    "CS_F3_PORT2",
    "CS_F3_APN",
};

char *LogicAddrLabal[] = 
{
    "AREA_CODE",
    "TERMINAL_CODE",
};

char *MainStaInfoLabal[] = 
{
    "CS_F3_IP1",
    "CS_F3_PORT1",
    "CS_F3_IP2",
    "CS_F3_PORT2",
    "CS_F3_APN",
};
char *UpCommLabal[] = 
{
    "CS_F3_IP1",
    "CS_F3_PORT1",
    "CS_F3_IP2",
    "CS_F3_PORT2",
    "CS_F3_APN",
};




sCircuitSysInfo *GetCiSysInfo(void)
{
    return &CircuitSysInfo;
}
static int CircuitParaDefault(FILE *fp)
{//default data to variable and file
    sCircuitSysInfo *para = GetCiSysInfo();
    return 0;
}
static int LocationLoad(char *FileBuf)
{
    return 0;
}
static int MainStationInfoLoad(char *FileBuf)
{
    return 0;
}
static int CircuitLogicAddrLoad(char *FileBuf)
{
    return 0;
}
static int UpCommParaLoad(char *FileBuf)
{
    return 0;
}

static int CircuitParamInit(void)
{
    int FileSize,malsize,ret = 0;
    FILE *fp;
    char *pFileBuf;


    fp = fopen(TERM698_CONF_FILE_PATH,"r");
    if(!fp)
    {
        fp = fopen(TERM698_CONF_FILE_PATH,"w+");
        if(!fp)
        {
            ret = -1;
            printf("[para_int]new file failled!\n");
            return ret;
        }
        if(flock(fileno(fp),LOCK_EX | LOCK_NB))
        {
            ret = -1;
            printf("[para_int]file is busy!!\n");
            return ret;
        }
        ret = CircuitParaDefault(fp); 
        flock(fileno(fp),LOCK_UN);
        fclose(fp);
        return ret;
    }
    do
    {
        fseek(fp,0,SEEK_END);
        FileSize = ftell(fp);  
        malsize = FileSize+10;
        pFileBuf = malloc(malsize);
        if(!pFileBuf)
        {
            ret = -1;
            printf("[para_int]malloc failed!\n");
            break;
        }
        memset(pFileBuf,0,malsize);
        ret = fread(pFileBuf,1,FileSize,fp);
        if(ret<=0) 
        {
            ret = -1;
            printf("para int read file failled!\n");
            break;
        }
        fclose(fp);
        ret = LocationLoad(pFileBuf);
        if(ret)
        {
            break;
        }
        ret = MainStationInfoLoad(pFileBuf);
        if(ret)
        {
            break;
        }
        ret = CircuitLogicAddrLoad(pFileBuf);
        if(ret)
        {
            break;
        }
        ret = UpCommParaLoad(pFileBuf);
        if(ret)
        {
            break;
        }
    }while(0);
    if(pFileBuf)
    {
        free(pFileBuf);
    }
    return ret;
//    stTermUpCommuPara TermUpCommuPara;
//    memset(&TermUpCommuPara, 0, sizeof(TermUpCommuPara));
//    getTermUpCommuPara(&TermUpCommuPara);
//    //上行通信参数
//    memcpy((u8 *)&gCircuitSystemInfo.UpCommuPara, (u8 *)&TermUpCommuPara, sizeof(stTermUpCommuPara));
}





















































