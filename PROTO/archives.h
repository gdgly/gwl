#ifndef ARCHIVES_H
#define ARCHIVES_H


#include "Sgd_types.h"

#define ADDR_LEN			6
#define MAX_EQUIP_NUM		2040//3761 protol define



typedef struct
{
	u16 EquipNum;
	u16 EquipSquenceNo;
	u16 CirrentNo;
	u8 ComRateAndPort;
	u8 ProtolType;
	u8 Addr[ADDR_LEN];
	u8 TariffNum;
	u8 DispIntDeciNum;
	u8 CollectorAddr[ADDR_LEN];
	u8 UserInfo;
}sArchivesDataUnit;



































#endif
