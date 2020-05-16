#ifndef ARCHIVES_H
#define ARCHIVES_H


#include "Sgd_types.h"
#include "list.h"

#define ADDR_LEN			6
#define MAX_EQUIP_NUM		2040//3761 protol define

#define ASC_ROW_ID		"rowid"
#define ASC_SEQ			"EquipSquenceNo"


typedef struct
{
	//u16 EquipNum;
	u16 EquipSquenceNo;
	u16 CirrentNo;
	u8 ComRateAndPort;
	u8 ProtolType;
	u8 Addr[ADDR_LEN];
	u8 TariffNum;
	u8 DispIntDeciNum;
	u8 CollectorAddr[ADDR_LEN];
	u8 UserInfo;
}__attribute__((packed))sArchivesDataUnit;




typedef struct
{
	int DbId;
	sArchivesDataUnit ArchivesDataUnit;
}sArchSaveStruct;



typedef struct
{
	sList ArchList;
	sArchivesDataUnit ArchivesDataUnit;
}sArchListStruct;

typedef struct
{
	u8 name[30];
	u8 type[30];
}sTableItem;

typedef struct 
{
	u16 EquipNum;
	sList ArchList;
}sArchPara;

typedef struct
{
	u16 seq;
	void *Data;
}sArchRequire;


sArchPara *GetArchPara(void);
int CreateArchTable(void);
int InsertDataStruct(sArchivesDataUnit *ArchDb);
int ReadArchFromDb(sArchivesDataUnit *ArchDb,u8 *MasterKey,int seq);
int ArchSearchData(u16 seq,sArchivesDataUnit *data);
int ArchDataReset(void);
int ArchivesInit(void);














#endif
