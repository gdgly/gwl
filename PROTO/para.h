#ifndef PARA_H
#define PARA_H


#include "Mrs_fw_proto376_1.h"



#define TERM698_CONF_FILE_PATH "./term698.conf"

//巡检仪设备系统信息
typedef struct
{
	stTermLocation 		Location;				//位置
	stMainStationIPPort MainStationIPPort;		//主站ip/port	
	MODLE_ADDR_STRU 	usModleAddr;			//终端逻辑地址
	stTermUpCommuPara  	UpCommuPara; 			//上行通信参数
}sCircuitSysInfo;  






























sCircuitSysInfo *GetCiSysInfo(void);
int CircuitParamInit(void);

















#endif
