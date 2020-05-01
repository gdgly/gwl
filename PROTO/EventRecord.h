#ifndef _FILE_H_
#define _FILE_H_
#include <stdio.h>
#include "Sgd_types.h"
#include "Mrs_fw_proto376_1.h"



//事件相关文件
//#define ORDINARYFILEDATA 	"ordinaryEvent.dat"				//事件不保存在文件，保存在数据库
#define ORDINARYFILEEC 		"ordinaryEC.dat"
//#define IMPORTANTFILEDATA	"ImportanceEvent.dat"			//事件不保存在文件，保存在数据库
#define IMPORTANTFILEEC 	"importanceEC.dat"
#define EVENTRECORDSTATUS 	"EventRecordStatus.dat" 	


#define EveMax 				80			//事件存储做大的数据
#define MAX_ERC_NUM 		64
#define MAX_NUM_EACH_ERC 	10			//每类事件最多存储10条

#define OrdinaryEvent		0			//表示一般事件
#define ImportantEvent		1			//表示重要事件


//Afn 0x0e 3类数据获取
struct EventHead
{
	u8 ErcName;
	u8 ErcLen;			//不包括头
};


//命令数据体
struct  EventRange			
{
	u8 Pm;
	u8 Pn;
};


int GetValidAndPriorityByErc(int ErcName,int *ValidFlag,int *Priority);





#endif
