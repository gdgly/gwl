#ifndef PARA_H
#define PARA_H


#include "Mrs_fw_proto376_1.h"




//巡检仪设备系统信息
typedef struct
{
	stTermLocation Location;				//位置
	stMainStationIPPort MainStation;		//主站ip/port	
	MODLE_ADDR_STRU usModleAddr;			//终端逻辑地址
	stTermUpCommuPara UpCommuPara; 			//上行通信参数
	DATA_AFN04_F7 CiIpAndPort;//巡检仪ip和端口
	sTermWorkWaysUpComm WorkWaysUpComm;//上行通信工作方式
	TermEventRecordSet EvtRecoSet;
    current_circuit_param_stru CurLoopEnPara;
    current_transformer_rate_stru TARatePara;
	sVirtualNet VirtualNet;
	u32 FluxLimitor;//流量限额。
	TermMessageConParameterSet MessageCon;  //消息认证参数
}__attribute__((packed))sTermSysInfo;  






























sTermSysInfo *GetCiSysInfo(void);
int LocationLoad(const char* confile);
int MainStationInfoLoad(const char* confile);
int CircuitLogicAddrLoad(const char* confile);
int UpCommParaLoad(const char* confile);
int CirIpAndPortLoad(const char* confile);
int UpCommWorkWaysLoad(const char* confile);
int EventParaLoad(const char* confile);
int CurLoopParaLoad(const char* confile);
int TaRateParaLoad(const char* confile);
int TermUpFluxLimitLoad(const char* confile);

int TermParaPowerUp(const char* confile);

int TermParaDefault(void);
int TermParaDefButMainSta(void);

int ConsCheInfoLoad(const char* confile);
















#endif
