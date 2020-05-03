#include "file.h"
#include "Tools.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "para.h"






sTermSysInfo TermSysInfo;

sTermSysInfo *GetCiSysInfo(void)
{
    return &TermSysInfo;
}
int LocationLoad(const char* confile)
{
        if(!confile) return -1;
    
        sTermSysInfo *para = GetCiSysInfo();
        int i,j,gr;
        char ArrData[2][30];
        char *cParaInfo[] = 
        {
            "CS_F91_LONGITUDE", ArrData[0], ":",
            "CS_F91_DIMENSION", ArrData[1], ":",
        };
        memset(&para->MainStation,0,sizeof(para->MainStation));
        memset(ArrData,0,sizeof(ArrData));
        gr = sizeof(cParaInfo)/(sizeof(cParaInfo[0])*3);
        if(GetFileDataGroup(confile,cParaInfo,gr)) return -1;
        char *pTmp;
        int tmp[4];

        memset(tmp,0,sizeof(tmp));
        for(j=0;j<2;j++)
        {
            pTmp = ArrData[j];     
            if(j==0)//longitude
            {
                if(*pTmp++ == 'W') para->Location.longitude_D_BLW | 0x8;
            }
            if(j==1)//latitude
            {
                if(*pTmp++ == 'S') para->Location.latitude_D_BLW | 0x8;
            }
            for(i=0;i<4;i++)
            {
                if(*pTmp == '\0') break;
                while((*pTmp>='0')&&(*pTmp<='9'))
                {
                    tmp[i] *= 10;
                    tmp[i] += *pTmp -'0';
                    pTmp++;
                }
                pTmp++;//·Ö¸ô·û
            }
            if(j==0)
            {
                para->Location.longitude_D_BW = tmp[0]/100;
                para->Location.longitude_D_SW = tmp[0]/10;
                para->Location.longitude_D_GW = tmp[0]%10;

                para->Location.longitude_B_SW = tmp[1]/10;
                para->Location.longitude_B_GW = tmp[1]%10;

                para->Location.longitude_S_SW = tmp[2]/10;
                para->Location.longitude_S_GW = tmp[2]%10;

                para->Location.longitude_S_SFW = tmp[3]/10;
                para->Location.longitude_S_BFW = tmp[3]%10;
                
            }
            if(j==1)
            {
                para->Location.latitude_D_BW = tmp[0]/100;
                para->Location.latitude_D_SW = tmp[0]/10;
                para->Location.latitude_D_GW = tmp[0]%10;

                para->Location.latitude_B_SW = tmp[1]/10;
                para->Location.latitude_B_GW = tmp[1]%10;

                para->Location.latitude_S_SW = tmp[2]/10;
                para->Location.latitude_S_GW = tmp[2]%10;

                para->Location.latitude_S_SFW = tmp[3]/10;
                para->Location.latitude_S_BFW = tmp[3]%10;
            }
        }
        return 0;
}
int MainStationInfoLoad(const char* confile)
{
    if(!confile) return -1;

    sTermSysInfo *para = GetCiSysInfo();
    int i,gr;
    char ArrData[8][30];
    char *cParaInfo[] = 
    {
        "CS_F3_IP1",    ArrData[0],     ":",
        "CS_F3_PORT1",  ArrData[1],     ":",
        "CS_F3_IP2",    ArrData[2],     ":",
        "CS_F3_PORT2",  ArrData[3],     ":",
        "CS_F3_APN",    ArrData[4],     ":",
    };
    memset(&para->MainStation,0,sizeof(para->MainStation));
    memset(ArrData,0,sizeof(ArrData));
    gr = sizeof(cParaInfo)/(sizeof(cParaInfo[0])*3);
    if(GetFileDataGroup(confile,cParaInfo,gr)) return -1;
    para->MainStation.u_IP_main.IP = (u32)inet_addr(ArrData[0]);//Ð¡¶ËÐò´æ´¢
    para->MainStation.Port_main = (u16)atoi(ArrData[1]);
    para->MainStation.u_IP_bak.IP = (u32)inet_addr(ArrData[2]);//Ð¡¶ËÐò´æ´¢
    para->MainStation.Port_bak = (u16)atoi(ArrData[3]);
    memcpy(para->MainStation.APN,ArrData[4],sizeof(para->MainStation.APN));
    return 0;
}
int CircuitLogicAddrLoad(const char* confile)
{
    if(!confile) return -1;
    
    sTermSysInfo *para = GetCiSysInfo();
    int i,gr;
    char ArrData[2][30];
    char *cParaInfo[] = 
    {
        "AREA_CODE",        ArrData[0],    ":",
        "TERMINAL_CODE",    ArrData[1],    ":",
    };
    memset(&para->usModleAddr,0,sizeof(para->usModleAddr));
    memset(ArrData,0,sizeof(ArrData));
    gr = sizeof(cParaInfo)/(sizeof(cParaInfo)*3);
    if(GetFileDataGroup(confile,cParaInfo,gr)) return -1;
    para->usModleAddr.districtCode = atoi(ArrData[0]);
    para->usModleAddr.termAddr = atoi(ArrData[1]);
    return 0;
}
int UpCommParaLoad(const char* confile)
{
    if(!confile) return -1;
    
    sTermSysInfo *para = GetCiSysInfo();
    int i,gr;
    char ArrData[8][30];
    char *cParaInfo[] = 
    {
        "CS_F1_TRANSDELAY",             ArrData[0],             ":",
        "CS_F1_TRANSDELAYMAX",          ArrData[1],             ":",
        "CS_F1_TIMEOUT",                ArrData[2],             ":",
        "CS_F1_RETRYCNT",               ArrData[3],             ":",
        "CS_F1_FLAG_CLASS1",            ArrData[4],             ":",
        "CS_F1_FLAG_CLASS2",            ArrData[5],             ":",
        "CS_F1_FLAG_CLASS3",            ArrData[6],             ":",
        "CS_F1_PULSECYCLE",             ArrData[7],             ":",
    };
    memset(&para->UpCommuPara,0,sizeof(para->UpCommuPara));
    memset(ArrData,0,sizeof(ArrData));
    gr = sizeof(cParaInfo)/(sizeof(cParaInfo[0])*3);
    if(GetFileDataGroup(confile,cParaInfo,gr)) return -1;
    para->UpCommuPara.RTS = atoi(ArrData[0]);
    para->UpCommuPara.delay_time = atoi(ArrData[1]);
    para->UpCommuPara.u_over_time_times.st_over_time_times.over_time = atoi(ArrData[2]);
    para->UpCommuPara.u_over_time_times.st_over_time_times.over_times = atoi(ArrData[3]);
    para->UpCommuPara.confirm = (atoi(ArrData[4])&0x01)+((atoi(ArrData[5])<<1)&0x02)+((atoi(ArrData[6])<<2)&0x04);
    para->UpCommuPara.heart_beats = atoi(ArrData[7]);
    return 0;
}
int CirIpAndPortLoad(const char* confile)
{
    if(!confile) return -1;
    
    sTermSysInfo *para = GetCiSysInfo();
    int i,gr;
    char ArrData[12][30];
    char *cParaInfo[] = 
    {
        "CS_F7_IP",            ArrData[0],         ":",
        "CS_F7_MASK",          ArrData[1],         ":",
        "CS_F7_GW",            ArrData[2],         ":",
        "CS_F7_PROXYTYPE",     ArrData[3],         ":",
        "CS_F7_PROXYIP",       ArrData[4],         ":",
        "CS_F7_PROXYPOTE",     ArrData[5],         ":",
        "CS_F7_PROXYAUTH",     ArrData[6],         ":",
        "CS_F7_LEN_USER",      ArrData[7],         ":",
        "CS_F7_USER",          ArrData[8],         ":",
        "CS_F7_LEN_PWD",       ArrData[9],         ":",
        "CS_F7_PWD",           ArrData[10],        ":",
        "CS_F7_TERMPORT",      ArrData[11],        ":",
    };
    memset(&para->CiIpAndPort,0,sizeof(para->CiIpAndPort));
    memset(ArrData,0,sizeof(ArrData));
    gr = sizeof(cParaInfo)/(sizeof(cParaInfo[0])*3);
    if(GetFileDataGroup(confile,cParaInfo,gr)) return -1;

    para->CiIpAndPort.termip32u = (u32)inet_addr(ArrData[0]);
    para->CiIpAndPort.netmask32u = (u32)inet_addr(ArrData[1]);
    para->CiIpAndPort.gateway32u = (u32)inet_addr(ArrData[2]);
    para->CiIpAndPort.proxytype = atoi(ArrData[3]);
    para->CiIpAndPort.proxyserverip32u = (u32)inet_addr(ArrData[4]);
    para->CiIpAndPort.proxyserverport = atoi(ArrData[5]);
    para->CiIpAndPort.proxyserverconnectway = atoi(ArrData[6]);
    para->CiIpAndPort.user_len = atoi(ArrData[7]);
    memcpy(para->CiIpAndPort.user,ArrData[8],para->CiIpAndPort.user_len);
    para->CiIpAndPort.pw_len = atoi(ArrData[9]);
    memcpy(para->CiIpAndPort.pw,ArrData[10],para->CiIpAndPort.pw_len);
    para->CiIpAndPort.listen_port = atoi(ArrData[11]);
    return 0;
}
int UpCommWorkWaysLoad(const char* confile)
{
    if(!confile) return -1;
    
    sTermSysInfo *para = GetCiSysInfo();
    int i,gr;
    char ArrData[9][30];
    char *cParaInfo[] = 
    {
        "CS_F8_CONNSTARTMODE",  ArrData[0],    ":",
        "CS_F8_MODE",           ArrData[1],    ":",
        "CS_F8_ISUDP",          ArrData[2],    ":",
        "CS_F8_RECONNINTERVAL", ArrData[3],    ":",//s
        "CS_F8_MAXRETRYCNT",    ArrData[4],    ":",//min    
        "CS_F8_MAXIDLETIME",    ArrData[5],    ":",//min
        "CS_F8_TIMESPAN_1",     ArrData[6],    ":",
        "CS_F8_TIMESPAN_2",     ArrData[7],    ":",
        "CS_F8_TIMESPAN_3",     ArrData[8],    ":",
    };
    memset(&para->WorkWaysUpComm,0,sizeof(para->WorkWaysUpComm));
    memset(ArrData,0,sizeof(ArrData));
    gr = sizeof(cParaInfo)/(sizeof(cParaInfo[0])*3);
    if(GetFileDataGroup(confile,cParaInfo,gr)) return -1;
    para->WorkWaysUpComm.TermMode.bMode.ConnStartMode = atoi(ArrData[0]);
    para->WorkWaysUpComm.TermMode.bMode.mode = atoi(ArrData[1]);
    para->WorkWaysUpComm.TermMode.bMode.TcpUdp = atoi(ArrData[2]);
    para->WorkWaysUpComm.ReconnInterval = atoi(ArrData[3]);
    para->WorkWaysUpComm.MaxRetryCnt = atoi(ArrData[4]);
    para->WorkWaysUpComm.MaxIdleTime = atoi(ArrData[5]);
    para->WorkWaysUpComm.TimeSpan[0] = atoi(ArrData[6]);
    para->WorkWaysUpComm.TimeSpan[1] = atoi(ArrData[7]);
    para->WorkWaysUpComm.TimeSpan[2] = atoi(ArrData[8]);  
    return 0;
}
int EventParaLoad(const char* confile)
{
    if(!confile) return -1;
    
    sTermSysInfo *para = GetCiSysInfo();
    int i,gr;
    char ArrData[16][30];
    char *cParaInfo[] = 
    {
        "CS_F9_EVENTRECORDVALIDFLAG_1",      ArrData[0], ":",
        "CS_F9_EVENTRATINGFLAG_1",           ArrData[1], ":",
        
        "CS_F9_EVENTRECORDVALIDFLAG_2",      ArrData[2], ":",
        "CS_F9_EVENTRATINGFLAG_2",           ArrData[3], ":",
        
        "CS_F9_EVENTRECORDVALIDFLAG_3",      ArrData[4], ":",
        "CS_F9_EVENTRATINGFLAG_3",           ArrData[5], ":",
        
        "CS_F9_EVENTRECORDVALIDFLAG_4",      ArrData[6], ":",
        "CS_F9_EVENTRATINGFLAG_4",           ArrData[7], ":",
        
        "CS_F9_EVENTRECORDVALIDFLAG_5",      ArrData[8], ":",
        "CS_F9_EVENTRATINGFLAG_5",           ArrData[9], ":",
        
        "CS_F9_EVENTRECORDVALIDFLAG_6",      ArrData[10], ":",
        "CS_F9_EVENTRATINGFLAG_6",           ArrData[11], ":",
        
        "CS_F9_EVENTRECORDVALIDFLAG_7",      ArrData[12], ":",
        "CS_F9_EVENTRATINGFLAG_7",           ArrData[13], ":",
        
        "CS_F9_EVENTRECORDVALIDFLAG_8",      ArrData[14], ":",
        "CS_F9_EVENTRATINGFLAG_8",           ArrData[15], ":",

    };
    memset(&para->EvtRecoSet,0,sizeof(para->EvtRecoSet));
    memset(ArrData,0,sizeof(ArrData));
    gr = sizeof(cParaInfo)/(sizeof(cParaInfo[0])*3);
    if(GetFileDataGroup(confile,cParaInfo,gr)) return -1;
    for(i=0;i<gr/2;i++)
    {
        para->EvtRecoSet.CS_F9_EventRecordValidFlag[i] = atoi(ArrData[i*2]);
        para->EvtRecoSet.CS_F9_EventRatingFlag[i] = atoi(ArrData[i*2+1]);
    }
    return 0;

}
int CurLoopParaLoad(const char* confile)
{
    if(!confile) return -1;
    
    sTermSysInfo *para = GetCiSysInfo();
    int i,gr;
    char ArrData[4][30];
    char *cParaInfo[] = 
    {
        "CS_F153_PA_EN",          ArrData[0],             ":",
        "CS_F153_PB_EN",          ArrData[1],             ":",
        "CS_F153_PC_EN",          ArrData[2],             ":",
        "CS_F153_MAC",            ArrData[3],             ":",
    };
    memset(&para->CurLoopEnPara,0,sizeof(para->CurLoopEnPara));
    memset(ArrData,0,sizeof(ArrData));
    gr = sizeof(cParaInfo)/(sizeof(cParaInfo[0])*3);
    if(GetFileDataGroup(confile,cParaInfo,gr)) return -1;
    para->CurLoopEnPara.phase_a_curr = atoi(ArrData[0]);
    para->CurLoopEnPara.phase_b_curr = atoi(ArrData[1]);
    para->CurLoopEnPara.phase_c_curr = atoi(ArrData[2]);

    u32 tmp;
    tmp = (u32)inet_addr(ArrData[2]);
    memcpy(para->CurLoopEnPara.mac,(u8*)&tmp,sizeof(para->CurLoopEnPara.mac));
    return 0;    
}
int TaRateParaLoad(const char* confile)
{
    if(!confile) return -1;
    
    sTermSysInfo *para = GetCiSysInfo();
    int i,gr;
    char ArrData[5][30];
    char *cParaInfo[] = 
    {
        "CS_F154_FORMAT",            ArrData[0],             ":",
        "CS_F154_PA_RATIO",          ArrData[1],             ":",
        "CS_F154_PB_RATIO",          ArrData[2],             ":",
        "CS_F154_PC_RATIO",          ArrData[3],             ":",
        "CS_F154_MAC",               ArrData[4],             ":",
    };
    memset(&para->TARatePara,0,sizeof(para->TARatePara));
    memset(ArrData,0,sizeof(ArrData));
    gr = sizeof(cParaInfo)/(sizeof(cParaInfo[0])*3);
    if(GetFileDataGroup(confile,cParaInfo,gr)) return -1;
    para->TARatePara.format = atoi(ArrData[0]);
    para->TARatePara.phase_a_trans = atoi(ArrData[1]);
    para->TARatePara.phase_b_trans = atoi(ArrData[2]);
    para->TARatePara.phase_c_trans = atoi(ArrData[3]);
    
    u32 tmp;
    tmp = (u32)inet_addr(ArrData[4]);
    memcpy(para->TARatePara.mac,(u8*)&tmp,sizeof(para->TARatePara.mac));
    return 0;
}
int VirtualNetLoad(const char* confile)
{
    if(!confile) return -1;
    
    sTermSysInfo *para = GetCiSysInfo();
    int i,gr;
    char ArrData[2][60];
    char *cParaInfo[] = 
    {
        "CS_F16_VIRUSER",  	ArrData[0], ":",
        "CS_F16_VIRPASSWD", ArrData[1], ":",
    };
    memset(&para->VirtualNet,0,sizeof(para->VirtualNet));
    memset(ArrData,0,sizeof(ArrData));
    gr = sizeof(cParaInfo)/(sizeof(cParaInfo[0])*3);
    if(GetFileDataGroup(confile,cParaInfo,gr)) return -1;
	memcpy(para->VirtualNet.VirUser,ArrData[0],sizeof(para->VirtualNet.VirUser));
	memcpy(para->VirtualNet.VirPasswd,ArrData[1],sizeof(para->VirtualNet.VirPasswd));
    return 0;
}

//===========================================================================
int CircuitParaLoad(const char* confile)
{
    int ret = 0;
    
    ret += LocationLoad(confile);    
    ret += MainStationInfoLoad(confile);
    ret += CircuitLogicAddrLoad(confile);
    ret += UpCommParaLoad(confile);
    ret += CirIpAndPortLoad(confile);
    ret += UpCommWorkWaysLoad(confile);
    ret += EventParaLoad(confile);
    ret += CurLoopParaLoad(confile);
    ret += TaRateParaLoad(confile);
    return ret;
}
int TermParaPowerUp(const char* confile)
{
    if(strcmp(TERM3761_PARA_CONF,confile)!=0)
    {
		printf("[para_debug]conf file error!!\n");
		return -1;
	}
	if(access(confile,F_OK) != 0)
	{
		printf("[para_debug]loss of conf file!!\n");
		TermParaDefault();
		return -1;
	}
    return CircuitParaLoad(confile);
}


int TermParaDefault(void)
{
	if(access(TERM3761_PARA_CONF_INIT,F_OK)==-1)
	{
		printf("[para_debug]loss of default conf file!!\n");
		return -1;
	}
	char tmp[200];

	memset(tmp,0,sizeof(tmp));
	snprintf(tmp, sizeof(tmp), "sudo cp -f %s %s\n",TERM3761_PARA_CONF_INIT,TERM3761_PARA_CONF);
//	printf("sys cmd:%s\n",tmp);
	system(tmp);
	return CircuitParaLoad(TERM3761_PARA_CONF_INIT);
}
int TermParaDefButMainSta(void)
{
	if(access(TERM3761_PARA_CONF_INIT,F_OK)==-1)
	{
		printf("[para_debug]loss of default conf file!!\n");
		return -1;
	}		
    char val[5][30];
    char *pData[] = 
    {
        "CS_F3_IP1",            val[0],             ":",
        "CS_F3_PORT1",          val[1],             ":",
        "CS_F3_IP2",            val[2],             ":",
        "CS_F3_PORT2",          val[3],             ":",
        "CS_F3_APN",            val[4],             ":",
    };
	if(0!=GetFileDataGroup(TERM3761_PARA_CONF,pData,sizeof(pData)/(sizeof(pData[0])*3)))
	{
		printf("[para_debug]loss of main station para!!\n");
		return -1;
	}	
	TermParaDefault();
	SaveConfMultiGroup(TERM3761_PARA_CONF,pData,sizeof(pData)/(sizeof(pData[0])*3));	
}

















































