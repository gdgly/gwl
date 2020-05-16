#ifndef _TA_MODULE_H_
#define _TA_MODULE_H_

#include "Sgd_types.h"


#define MAC_SIZE 					4
#define TA_CURENT_OFFSET			22

#define CIRCUIT_NORMAL				0//正常状态
#define CIRCUIT_SHORTCUT			1//短路
#define CIRCUIT_BREAK				2//开路
#define CIRCUIT_SEC_SEPARATE		4//二次分流
#define CIRCUIT_FONT_SHORTCUT		8//一次短路


typedef struct 
{
    u8  IVcode:1;   //1时， ESAM 使用外部 IV；“0”时，ESAM 使用内部 IV。
    u8  DirFlag:1;  //“1” 时， 数据由主站下发；当下传标识为“0” 时， 数据由终端下发
    u8  EncryFlag:1;//“1”时， 645 帧中数据区的 DATA 为密文；当密文标识为“0”时， 645 帧中数据区的 DATA 为明文
    u8  Reserve:5;
}__attribute__((packed))sTAG_FormatCode;

typedef struct 
{
    sTAG_FormatCode FormatTag;
    u8  Phase_A_ststus;
    u8  Phase_B_ststus;
    u8  Phase_C_ststus;
    u8  Mac[MAC_SIZE];
}__attribute__((packed))sFormat_TA_Status;




sFormat_TA_Status GetTaStatus(void);
int TaModuleInit(void);
void AppCloseTaPort(void);


#endif
