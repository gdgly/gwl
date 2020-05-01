#ifndef _FILE_H_
#define _FILE_H_
#include <stdio.h>
#include "Sgd_types.h"
#include "Mrs_fw_proto376_1.h"



//�¼�����ļ�
//#define ORDINARYFILEDATA 	"ordinaryEvent.dat"				//�¼����������ļ������������ݿ�
#define ORDINARYFILEEC 		"ordinaryEC.dat"
//#define IMPORTANTFILEDATA	"ImportanceEvent.dat"			//�¼����������ļ������������ݿ�
#define IMPORTANTFILEEC 	"importanceEC.dat"
#define EVENTRECORDSTATUS 	"EventRecordStatus.dat" 	


#define EveMax 				80			//�¼��洢���������
#define MAX_ERC_NUM 		64
#define MAX_NUM_EACH_ERC 	10			//ÿ���¼����洢10��

#define OrdinaryEvent		0			//��ʾһ���¼�
#define ImportantEvent		1			//��ʾ��Ҫ�¼�


//Afn 0x0e 3�����ݻ�ȡ
struct EventHead
{
	u8 ErcName;
	u8 ErcLen;			//������ͷ
};


//����������
struct  EventRange			
{
	u8 Pm;
	u8 Pn;
};


int GetValidAndPriorityByErc(int ErcName,int *ValidFlag,int *Priority);





#endif
