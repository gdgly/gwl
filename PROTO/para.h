#ifndef PARA_H
#define PARA_H


#include "Mrs_fw_proto376_1.h"



#define TERM698_CONF_FILE_PATH "./term698.conf"

//Ѳ�����豸ϵͳ��Ϣ
typedef struct
{
	stTermLocation 		Location;				//λ��
	stMainStationIPPort MainStationIPPort;		//��վip/port	
	MODLE_ADDR_STRU 	usModleAddr;			//�ն��߼���ַ
	stTermUpCommuPara  	UpCommuPara; 			//����ͨ�Ų���
}sCircuitSysInfo;  






























sCircuitSysInfo *GetCiSysInfo(void);
int CircuitParamInit(void);

















#endif
