#ifndef _EVENTRECORD_H_
#define _EVENTRECORD_H_

#include <stdio.h>
#include "Sgd_types.h"
#include "Mrs_fw_proto376_1.h"

//事件相关文件
//#define ORDINARYFILEDATA 	"ordinaryEvent.dat"				//事件不保存在文件，保存在数据库
#define ORDINARYFILEEC 		"ordinaryEC.dat"
//#define IMPORTANTFILEDATA	"ImportanceEvent.dat"			//事件不保存在文件，保存在数据库
#define IMPORTANTFILEEC 	"importanceEC.dat"
#define EVENTRECORDSTATUS 	"EventRecordStatus.dat" 	

#define flow_path_	 "/mnt/conf/Comm_flow.conf"


#define EveMax 				80			//事件存储做大的数据
#define MAX_ERC_NUM 		64
#define MAX_NUM_EACH_ERC 	10			//每类事件最多存储10条

#define OrdinaryEvent		0			//表示一般事件
#define ImportantEvent		1			//表示重要事件



int GetValidAndPriorityByErc(int ErcName,int *ValidFlag,int *Priority);
int saveEventFile(u8 ErcName,u8* data, int len, u32 time);


int CreateEvent1(stru_sjjl_ERC1     ev1);
int CreateEvent2(stru_sjjl_ERC2 ev2);
int CreateEvent3(stru_sjjl_ERC3 ev3);
int CreateEvent14(stru_sjjl_ERC14 ev14);
int CreateEvent20(stru_sjjl_ERC20 ev20);
int CreateEvent21(stru_sjjl_ERC21 ev21);
int CreateEvent32(stru_sjjl_ERC32 ev32);
int CreateEvent34(stru_sjjl_ERC34 ev34);
int CreateEvent40(stru_sjjl_ERC40 ev40);
int CreateEvent41(stru_sjjl_ERC41 ev41);

int sendLinkPacktoMainStaion(int stauts, unsigned char *ucDatagram, int *length);
s32 report13761UPtoMainStaion(uint8_t afn, uint8_t *ACKdata, uint16_t ack_data_len, int fd);
void send_event(int fd);

void EventInit();




long get_comm_read_flow_without_lock();
long get_comm_write_flow_without_lock();
long get_csf36_Month_Commflux_threshold();

long get_month_flows();

int saveconfparam(const char* confile, const char*name, long value, char delimiter);
int getconfparam(const char* confile,const char*name, long * value, char delimiter);

void load_comm_flow_month_without_lock();




#endif
