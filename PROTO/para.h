#ifndef PARA_H
#define PARA_H


#include "Mrs_fw_proto376_1.h"




//Ѳ�����豸ϵͳ��Ϣ
typedef struct
{
	stTermLocation Location;				//λ��
	stMainStationIPPort MainStation;		//��վip/port	
	MODLE_ADDR_STRU usModleAddr;			//�ն��߼���ַ
	stTermUpCommuPara UpCommuPara; 			//����ͨ�Ų���
	DATA_AFN04_F7 CiIpAndPort;//Ѳ����ip�Ͷ˿�
	sTermWorkWaysUpComm WorkWaysUpComm;//����ͨ�Ź�����ʽ
	TermEventRecordSet EvtRecoSet;
    current_circuit_param_stru CurLoopEnPara;
    current_transformer_rate_stru TARatePara;
	sVirtualNet VirtualNet;
	u32 FluxLimitor;//�����޶
	TermMessageConParameterSet MessageCon;  //��Ϣ��֤����
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
