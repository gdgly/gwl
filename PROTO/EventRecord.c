#include "EventRecord.h"
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "queue.h"
#include "AcSample.h"
#include "file.h"
#include "para.h"
#include "ListTimer.h"
#include "Ta_Module.h"
#include <unistd.h>
#include "SQLite.h"
#include <stdio.h>


//全局变量
u8 m_EC1;							//EC1的值，如果重要事件没有写满，值就是++，如果到达255，就一直存储255 （2009年12月15日）
u8 m_EC2;							//EC2的值，如果普通事件没有写满，值就是++，如果到达255，就一直存储255 （2009年12月15日）
u8 m_ErcNum[MAX_ERC_NUM] = {0};		//同一类事件的数量，记录最多10个
u8 m_ErcReport[EveMax] = {0};		//主动上报事件标志	
char EventBuf[EveMax][528] = {0};	//获取事件存储缓冲区


//保存重要事件计数器值
int saveimportanceEC(u8 EC1)
{
	if(UpdateEC1(EC1) == -1)
	{
		return -1;
	}
	return 0;
}

//保存一般事件计数器值
int saveordinaryEC(int EC2)
{
	if(UpdateEC2(EC2) == -1)
	{
		return -1;
	}
	return 0;
}

//获取重要事件计数器的值
int GetimportanceEC()
{
	return GetEC1();
}

//获取一般计数器值
int GetordinaryEC()
{
	return GetEC2();
}


/**************************************
*函数名称：saveReportStatus
*功能描述：在文件中保存事件是否上报
*输入参数：Num：已经发生的事件序号, flag: 1为要主动上报，0已上报完成。
*输出参数： 无
*返回值：0 成功，-1 失败
***************************************/
int saveReportStatus(int Num, int flag)
{
	u8 buf[80] = {0};
	if(readfile(EVENTRECORDSTATUS, buf, sizeof(buf), "r+") != 0)
	{
		return -1;
	}
	buf[Num] = flag;
	if(writefile(EVENTRECORDSTATUS, buf, sizeof(buf), "w+") != 0)
	{
		return -1;
	}
	return 0;
}


/**************************************
*函数名称：saveEventRecordStatus
*功能描述：在文件中保存事件是否发生
*输入参数：ErcName：事件名称
*输出参数： 无
*返回值：0 成功，-1 失败
***************************************/
int saveEventRecordStatus(int ErcName)
{
	u8 buf[64] = {0};
	if(ErcName < 1 || ErcName > 64)
	{
		return -1;
	}
	if(readfile(EVENTRECORDSTATUS, buf, sizeof(buf), "r+") != 0)
	{
		return -1;
	}
	buf[ErcName - 1] = 1;
	
	if(writefile(EVENTRECORDSTATUS, buf, sizeof(buf), "w+") != 0)
	{
		return -1;
	}
	return 0;
}

//根据F9的值判断事件是一般事件还是重要事件，以及事件是否需要保存
int GetValidAndPriorityByErc(int ErcName,int *ValidFlag,int *Priority)
{
	int i,j;
	if(ErcName < 0 || ErcName > 64 || ValidFlag == NULL || Priority == NULL)
	{
		return -1;
	}
	i = (ErcName - 1)/8;
	j = (ErcName - 1)%8;

	sTermSysInfo *pPara = GetCiSysInfo();
	
	*ValidFlag = (pPara->EvtRecoSet.CS_F9_EventRecordValidFlag[i] >> j)  & 0x01;		
	*Priority = (pPara->EvtRecoSet.CS_F9_EventRatingFlag[i] >> j)  & 0x01;
	for(i=0;i<8;i++)
	{
		printf("TermEventRecord.CS_F9_EventRecordValidFlag[%d] = %d\n",i, pPara->EvtRecoSet.CS_F9_EventRecordValidFlag[i]);
	}
	
	return 0;
}


//将事件写入文件
//参数Priority：事件等级  data：事件数据           Len：数据长度
int saveEvent(int Priority,u8 ERC ,const u8* data, int Len, u32 time, int report)
{ 
	if((Priority != 1 && Priority !=0) || data == NULL || Len < 0)
	{
		return -1;
	}
	if(1 == Priority)		//写入重要事件
	{
		if(m_EC1 < EveMax)
		{
			m_EC1++;	
		}
		int i;
		char buf[1024] = {0};
		for(i=0; i< Len; i++)
		{
			sprintf(&buf[i*2], "%02x", data[i]);
		}

		InsertImportanceEvent(ERC, buf, time , report);
		saveimportanceEC(m_EC1);
	}
	else					//写入一般事件
	{
		if(m_EC2 < EveMax)
		{
			m_EC2++;	
		}
		int i;
		char buf[1024] = {0};
		for(i=0; i< Len; i++)
		{
			sprintf(&buf[i*2], "%02x", data[i]);
		}

		InsertordinaryEvent(ERC, buf, time);	
		saveordinaryEC(m_EC2);
	}
}


/**************************************
*函数名称：saveEventFile
*功能描述：保存事件记录到文件中
*输入参数：ErcName：事件名称 ， data：数据，len：数据长度  time:记录时的事件
*输出参数： 无
*返回值：0 成功，-1 失败
***************************************/
int saveEventFile(u8 ErcName,u8* data, int len, u32 time)
{
	int ValidFlag = 0;			//需不需要记录
	int Priority = 0;			//重要事件还是一般事件,重要事件需要上报

	GetValidAndPriorityByErc(ErcName, &ValidFlag, &Priority);
		
	if(ValidFlag == 0)			//不需要记录事件
	{
		printf("saveEventFile %d is disabled\n",ErcName);
		return -1;
	}

	int saveEventRes = 0;
	
	//该事件是一般事件
	if(Priority == OrdinaryEvent)
	{
		//同一种类事件数量超过10个，则删除最早的一个事件
		if (m_ErcNum[ErcName-1] >= MAX_NUM_EACH_ERC)
		{
			do
			{
				
				int old_cnt = m_ErcNum[ErcName-1];	
				DeleteordinaryEvent(ErcName);			//删除最早一个事件
				GetordinaryEvent_Num(ErcName);			//获取事件数量
				
				if (m_ErcNum[ErcName-1] >= old_cnt)		//没有减少
				{
					printf("saveEventFile event %d DelOldestEventRecord Priority %d failed\n",ErcName,Priority);
					break;
				}
			}while(m_ErcNum[ErcName-1] >= MAX_NUM_EACH_ERC);
		}
	
		//保存一般事件
		saveEventRes = saveEvent(Priority, ErcName, data, len, time, 0);

		//保存一般事件计数器
		saveordinaryEC(m_EC2);

		//删除重要事件中的该事件
		DelimpERC(ErcName);	
		
	}	
	else	//重要事件
	{
		//同一种类事件数量超过10个，则删除最早的一个事件
		if (m_ErcNum[ErcName-1] >= MAX_NUM_EACH_ERC)
		{
			do
			{
				printf("\n*********aaaaaaaaaaaaaaaaaaa\n\n");
				int old_cnt = m_ErcNum[ErcName-1];	
				DeleteImportanceEvent(ErcName);			//删除最早一个事件
				GetImportanceEvent_Num(ErcName);		//获取事件数量
				
				if (m_ErcNum[ErcName-1] >= old_cnt)		//没有减少
				{
					printf("saveEventFile event %d DelOldestEventRecord Priority %d failed\n",ErcName,Priority);
					break;
				}
				
				
			}while(m_ErcNum[ErcName-1] >= MAX_NUM_EACH_ERC);
		}

		//保存重要事件
		
		saveEventRes = saveEvent(Priority, ErcName, data, len, time, 1);

		//保存重要事件计数器
		saveimportanceEC(m_EC1);

		//删除一般事件中的该事件
		DelordERC(ErcName);	

		//更新主动上报标志位置
		//GetEventRow();	
	}
	
	printf("saveEventFile prio %d event %d saveEventRes %d\n",Priority,ErcName,saveEventRes);
	if (saveEventRes < 0)	
		return -1;
	
	//记录事件数量
	m_ErcNum[ErcName-1]++;
	
	//记录事件是否发生
	if (saveEventRecordStatus(ErcName) == 0)
	{
		return 0;
	}
	else
	{
		printf("saveEventFile %d saveEventRecordStatus failed\n",ErcName);
		return -1;
	}
}
 

//清空事件记录
void ClearAllEvent()
{
	char src[260] = {0};
	//普通
	m_EC2 = 0;
	DeleteordinaryEvent(0);
	writefile(ORDINARYFILEEC,   src, 1, "w+");

	//重要
	m_EC1 = 0;//事件状态未清除
	DeleteImportanceEvent(0);
	writefile(IMPORTANTFILEEC,   src, 1,   "w+");

	//清空事件上报状态标志位
	writefile(EVENTRECORDSTATUS, src, 80,   "w+");

	//清楚事件个数位
	memset(m_ErcNum, 0, sizeof(m_ErcNum));
	
}



//获取时间
void GetTime(SJGS_15 *Time)
{
	if(Time == NULL) 
		return;
	time_t t;
	struct tm ftnow;
	t = time(NULL);	
	localtime_r(&t, &ftnow);

	SJGS_15 time;
	
	
	time.FSW = ftnow.tm_min / 10;
	time.FGW = ftnow.tm_min % 10;
	
	time.SSW   = ftnow.tm_hour / 10;
	time.SGW   = ftnow.tm_hour % 10;
	
	time.RSW    = ftnow.tm_mday / 10;
	time.RGW    = ftnow.tm_mday % 10;

	ftnow.tm_mon += 1;
	time.YSW = ftnow.tm_mon / 10;
	time.YGW = ftnow.tm_mon % 10;

	ftnow.tm_year += 1900;
	time.NSW = (ftnow.tm_year%100)/10;
	time.NGW = ftnow.tm_year%10;
	memcpy(Time, &time, sizeof(time));

	printf("time.NSW = %d\n", (ftnow.tm_year%100)/10);
	printf("time.NGW = %d\n", ftnow.tm_year%10);
	
	printf("nian = %d\n",ftnow.tm_year);

	printf("TIME = %d-%d-%d-%d-%d\n\n\n", time.NSW*10+time.NGW, time.YSW*10+time.YGW, time.RSW*10+time.RGW, time.SSW*10+time.SGW, time.FSW*10+time.FGW);
	
}

//获取秒
void GetSecond(u32 *second)
{
	time_t t;
	time(&t);	
	*second = (u32)t;
}

//ERC1：数据初始化和版本变更
int CreateEvent1(stru_sjjl_ERC1       ev1)
{
	int len = 0;
	u32 second = 0;
	
	ev1.ERCcode=0x01;
	ev1.Le=0x0e;
	GetTime(&ev1.time);
	
	u8 rowdata[256];
	memset(rowdata,0,sizeof(rowdata));
	memcpy(rowdata, &ev1, sizeof(ev1));

	GetSecond(&second);
	
	len = sizeof(ev1);
	return saveEventFile(ev1.ERCcode, rowdata, len, second);
}

//ERC2：参数丢失
int CreateEvent2(stru_sjjl_ERC2      ev2)
{
	int len = 0;
	int second;
	ev2.ERCcode=0x02;	
	ev2.Le = 0x06;		
	GetTime(&ev2.time);	
	u8 rowdata[256];
	memset(rowdata,0,sizeof(rowdata));	
		
	memcpy(rowdata, &ev2, sizeof(ev2));
	GetSecond(&second);
	
	len = sizeof(ev2);
	return saveEventFile(ev2.ERCcode, rowdata, len, second);
}


//ERC3：参数变更
int CreateEvent3(stru_sjjl_ERC3 ev3)
{ 
	int len = 0;
	int second = 0;
	ev3.ERCcode=0x03;	
	ev3.Le = 0x0A;	
	GetTime(&ev3.time);
	
	char rowdata[256];
	memset(rowdata,0,sizeof(rowdata));
	
	memcpy(rowdata, &ev3, sizeof(ev3) - 4);
	memcpy(&rowdata[sizeof(ev3) - 4], ev3.uniundata, 4);
	
	GetSecond(&second);
	len = sizeof(ev3);
	
	return saveEventFile(ev3.ERCcode, rowdata, len, second);
}


//ERC14：终端停/上电
int CreateEvent14(stru_sjjl_ERC14 ev14)
{ 
	int len = 0;
	int second = 0;
	ev14.ERCcode=0x0E;	
	ev14.Le = 0x0B;			//协议李没有标志位，实际多一个标志位	
	
	char rowdata[256];
	memset(rowdata,0,sizeof(rowdata));
	
	memcpy(rowdata, &ev14, sizeof(ev14));
	GetSecond(&second);

	len = sizeof(ev14);
	return saveEventFile(ev14.ERCcode, rowdata, len, second);

}



//ERC20：消息认证错误
int CreateEvent20(stru_sjjl_ERC20 ev20)
{
	int len = 0;
	int second;
	ev20.ERCcode=0x14;	
	ev20.Le = 0x16;		
	GetTime(&ev20.time);	
	u8 rowdata[256];
	memset(rowdata,0,sizeof(rowdata));	
	
		
	memcpy(rowdata, &ev20, sizeof(ev20));
	GetSecond(&second);
	
	len = sizeof(ev20);
	return saveEventFile(ev20.ERCcode, rowdata, len, second);
}



//ERC21：巡检仪故障事件
int CreateEvent21(stru_sjjl_ERC21 ev21)
{
	int len = 0;
	int second;
	ev21.ERCcode=0x15;	
	ev21.Le = 0x06;		
	GetTime(&ev21.time);	
	u8 rowdata[256];
	memset(rowdata,0,sizeof(rowdata));	
	
		
	memcpy(rowdata, &ev21, sizeof(ev21));
	GetSecond(&second);
	
	len = sizeof(ev21);
	return saveEventFile(ev21.ERCcode, rowdata, len, second);
}


//ERC32：终端与主站通信流量超门限
int CreateEvent32(stru_sjjl_ERC32 ev32)
{
	int len = 0;
	int second;
	ev32.ERCcode=0x20;	
	ev32.Le = 0x0D;		
	GetTime(&ev32.time);	
	u8 rowdata[256];
	memset(rowdata,0,sizeof(rowdata));	
	
		
	memcpy(rowdata, &ev32, sizeof(ev32));
	GetSecond(&second);
	
	len = sizeof(ev32);
	return saveEventFile(ev32.ERCcode, rowdata, len, second);
}


//ERC34：CT 异常事件记录
int CreateEvent34(stru_sjjl_ERC34 ev34)
{
	int len = 0;
	int second;
	ev34.ERCcode=0x22;	
	ev34.Le = 0x08;		
	GetTime(&ev34.time);	
	u8 rowdata[256];
	memset(rowdata,0,sizeof(rowdata));	
		
	memcpy(rowdata, &ev34, sizeof(ev34));
	GetSecond(&second);
	
	len = sizeof(ev34);
	return saveEventFile(ev34.ERCcode, rowdata, len, second);
}


//ERC40：磁场异常事件记录
int CreateEvent40(stru_sjjl_ERC40 ev40)
{
	int len = 0;
	int second;
	ev40.ERCcode=0x28;	
	ev40.Le = 0x0F;		
	GetTime(&ev40.time);	
	u8 rowdata[256];
	memset(rowdata,0,sizeof(rowdata));	
	
		
	memcpy(rowdata, &ev40, sizeof(ev40));
	GetSecond(&second);
	
	len = sizeof(ev40);
	return saveEventFile(ev40.ERCcode, rowdata, len, second);
}


//ERC41：对时事件记录
int CreateEvent41(stru_sjjl_ERC41 ev41)
{
	int len = 0;
	int second;
	ev41.ERCcode=0x29;	
	ev41.Le = 0x0E;		
	
	u8 rowdata[256];
	memset(rowdata,0,sizeof(rowdata));	

	
	memcpy(rowdata, &ev41, sizeof(ev41));
	GetSecond(&second);

	len = sizeof(ev41);
	return saveEventFile(ev41.ERCcode, rowdata, len, second);
}




/**************************************
*函数名称：parseEvent
*功能描述：解析各事件类别与事件记录长度是否一一对应
*输入参数：Num：已经发生的事件序号, flag: 1为要主动上报，0已上报完成。
*输出参数： 无
*返回值：0 成功，-1 失败
***************************************/
/*
int parseEvent(char *RowArrContent,int rowCount)
{
	if (rowCount < 0 || rowCount > 255)
		return -1;
	if (rowCount == 0)
		return 0;
	int i=0;
	for(i=0;i<rowCount;i++)
	{
		if(RowArrContent[i].data!=NULL)
		{
			//通过ERC？来进入对应的函数 
			//第一个字节owArrContent[i].data[0]为事件类别，
			//第二个字节owArrContent[i].data[1]为事件记录长度
			switch(RowArrContent[i].data[0])
			{
				case 1:
					if (parseEve1(RowArrContent[i]) < 0)
						RowArrContent[i].datalen = 0;
					break;
				case 2:
					if (parseEve2(RowArrContent[i]) < 0)
						RowArrContent[i].datalen = 0;
					break;
				case 3:
					if (parseEve3(RowArrContent[i]) < 0)
						RowArrContent[i].datalen = 0;
					break;
				case 14:
					if (parseEve4(RowArrContent[i]) < 0)
						RowArrContent[i].datalen = 0;
					break;
				case 20:
					if (parseEve8(RowArrContent[i]) < 0)
						RowArrContent[i].datalen = 0;
					break;
				case 21:
					if (parseEve9(RowArrContent[i]) < 0)
						RowArrContent[i].datalen = 0;
					break;
				case 32:
					if (parseEve10(RowArrContent[i]) < 0)
						RowArrContent[i].datalen = 0;
					break;
				case 40:
					if (parseEve11(RowArrContent[i]) < 0)
						RowArrContent[i].datalen = 0;
					break;
				case 41:
					if (parseEve12(RowArrContent[i]) < 0)
						RowArrContent[i].datalen = 0;
					break;
				case 13:
					if (parseEve13(RowArrContent[i]) < 0)
						RowArrContent[i].datalen = 0;
					break;
				default:
					RowArrContent[i].datalen = 0;
					break;
			}
		}
		else
		{

		}
	}
	return rowCount;
}

*/


//status:1登录，2退出，3心跳
int sendLinkPacktoMainStaion(int stauts, unsigned char *ucDatagram, int *length)
{
	stFrame13761Head *pAckFrame = (stFrame13761Head *)ucDatagram;
	stDataDI data_di;
	u16 pos = 0;
	u8 month;
	stTime sttime;
	
	//创建应答帧
	//head1
	pAckFrame->header1 = FRAME_START_13761; 											// 0x68
	//len
	pAckFrame->len.len_un.len_s.m_pid = PROTOCOL_13761_VALID;   						// 标识规约2
	pAckFrame->len.len_un.len_s.m_len = sizeof(stDataDI) + PROTOCOL_13761_CTR_SEQ_LEN;	//12
	
	if(stauts==3)
		pAckFrame->len.len_un.len_s.m_len += 6;
	
	pAckFrame->len1.len_un.len_s.m_pid = pAckFrame->len.len_un.len_s.m_pid;
	pAckFrame->len1.len_un.len_s.m_len = pAckFrame->len.len_un.len_s.m_len;
	
	//head2
	pAckFrame->header2 = FRAME_START_13761; 											// 0x68
	//ctrl
	pAckFrame->control.control_un.resp_s.dir = FRAME_13761_DIR_UP;						//1
	pAckFrame->control.control_un.resp_s.prm = FRAME_13761_PRM_START_STATION;
	pAckFrame->control.control_un.resp_s.rsv = 0;
	pAckFrame->control.control_un.resp_s.acd = 0;										//无事件访问
	pAckFrame->control.control_un.resp_s.code = LINK_CTRL_UP_RES_NODATA;
	
	//addr
	memcpy((uint8_t*)&(pAckFrame->addr),(uint8_t*)&GetCiSysInfo()->usModleAddr,sizeof(MODLE_ADDR_STRU));
	memset(&pAckFrame->addr.mainStn_TermGrp_addr,0,sizeof(unFrame13761M_Ma_unType));//终端启动发送帧
	
	//usr data
	pAckFrame->userdata.afn = AFN_02_LINK_TEST;
	pAckFrame->userdata.appseq.seq = 0;				//帧序号为0
	pAckFrame->userdata.appseq.con = 1;				//需要应答
	pAckFrame->userdata.appseq.fin = 1;				
	pAckFrame->userdata.appseq.fir = 1;				//单帧
	pAckFrame->userdata.appseq.tpv = 0;				//没有时间标签

	//DA,DT
	pos=0;
	setPn(0, &data_di.DA);
	setFn(stauts, &data_di.DT);
	memcpy(&(pAckFrame->userdata.data[0]), (uint8_t*)&(data_di), sizeof(stDataDI));
	pos += sizeof(stDataDI);// 4

	//3为心跳帧
	if(stauts == 3)
	{
		time_t t;
		struct tm ftnow;
		t = time(NULL);	
		localtime_r(&t, &ftnow);
	
		intToBcd(ftnow.tm_year, &sttime.year, 1);
		intToBcd(ftnow.tm_mon,  &month, 1);
		sttime.month=month;		
		intToBcd(ftnow.tm_mday, &sttime.day, 1);
			
		if(ftnow.tm_wday == 0)
		{
			sttime.week= 7;
		}
		else
		{
			sttime.week = ftnow.tm_wday;
		}
		
		intToBcd(ftnow.tm_hour,&sttime.hour,1);
		intToBcd(ftnow.tm_min,&sttime.min,1); 
		intToBcd(ftnow.tm_sec,&sttime.sec,1);

			//获取当前时钟
		memcpy(&(pAckFrame->userdata.data[0])+pos, (u8 *)&sttime, sizeof(stTermTime));
		pos+=6;
	}
	
	pAckFrame->userdata.data[pos++] = GDW376_1_CalSum((uint8_t *)&(pAckFrame->control), pAckFrame->len.len_un.len_s.m_len);
	pAckFrame->userdata.data[pos++] = FRAME_END_13761;
	*length = sizeof(stFrame13761Head)+pos;
	
	//len =sizeof(stFrame13761Head)+pos;
	
	//链路维护帧无需重发
	return 0;							//Protocl13761SendData2(ACKbuf, len);
}


//主动上报组帧函数
s32 report13761UPtoMainStaion(uint8_t afn, uint8_t *ACKdata, uint16_t ack_data_len, int fd)
{
	uint8_t ACKbuf[PROTOCOL_13761_BUF_SIZE] = {0};
	uint16_t pos = 0;
	static u8 pfc=0;
	StDataFormat16 DataFormat16={0};
	stFrame13761Head *pAckFrame =NULL;

	Eventdata *Event  = (Eventdata *)ACKdata;

	//帧序号为静态变量
	//static u8 seq = 0;
	
	if ( (ack_data_len == 0) || (ACKdata == NULL))
	{
		return 0;
	}

	time_t t;
	struct tm ftnow;
	t = time(NULL);	
	localtime_r(&t, &ftnow);

	intToBcd(ftnow.tm_mday, &DataFormat16.day, 1);
	intToBcd(ftnow.tm_hour, &DataFormat16.hour, 1);
	intToBcd(ftnow.tm_min, &DataFormat16.minute, 1);
	intToBcd(ftnow.tm_sec, &DataFormat16.second, 1);

	//uint16_t ack_len = 0;
	pAckFrame = (stFrame13761Head *)ACKbuf;

	//创建应答帧
	pAckFrame->header1 = FRAME_START_13761;
	pAckFrame->header2 = FRAME_START_13761;
	//seq++;
	memcpy((uint8_t*)&(pAckFrame->addr),(uint8_t*)&GetCiSysInfo()->usModleAddr,sizeof(MODLE_ADDR_STRU));
	memset(&pAckFrame->addr.mainStn_TermGrp_addr,0,sizeof(unFrame13761M_Ma_unType));//终端启动发送帧
	//pAckFrame->userdata.appseq.seq = seq&0xf;										//帧序号	
	
	pAckFrame->userdata.appseq.seq = Event->seq& 0xf;								//帧序号	
	pAckFrame->userdata.appseq.tpv = 1;												//主动上报有时间标签										
	pAckFrame->userdata.appseq.fir = 1;
	pAckFrame->userdata.appseq.fin = 1;
	pAckFrame->userdata.appseq.con = 1;
	pAckFrame->userdata.afn = afn;
	
	pAckFrame->control.control_un.resp_s.dir = FRAME_13761_DIR_UP;
	pAckFrame->control.control_un.resp_s.prm = FRAME_13761_PRM_START_STATION;
	pAckFrame->control.control_un.resp_s.rsv = 0;
	pAckFrame->control.control_un.resp_s.acd = 1;									//有事件标志
	pAckFrame->control.control_un.resp_s.code = LINK_CTRL_UP_RES_USERDATA;
	
	pos = 0;
	memcpy(&(pAckFrame->userdata.data[pos]), Event->data, ack_data_len);
	pos += ack_data_len;
	
	if (pAckFrame->control.control_un.resp_s.acd == 1)	//附加信息域事件计数器 EC
	{
		AppAuxEC Ec = {0};
		//AppAuxEC;
		pAckFrame->userdata.data[pos++]=Ec.AppAuxEC1;
		pAckFrame->userdata.data[pos++]=Ec.AppAuxEC2;
	}
	
	if (pAckFrame->userdata.appseq.tpv ==1)				//附加信息域时间标签 Tp
	{
		AppAuxTp tp={0};
		
		tp.AppAuxTpPFC=pfc++;							//启动帧帧序号计数器PFC加1
		memcpy(&tp.AppAuxTpDate, &DataFormat16, sizeof(StDataFormat16));
		memcpy(&(pAckFrame->userdata.data[pos]), (uint8_t*)(&tp), sizeof(AppAuxTp));
		pos += sizeof(AppAuxTp);
	}
	
	pAckFrame->len.len_un.len_s.m_len = pos + PROTOCOL_13761_CTR_SEQ_LEN;
	pAckFrame->len.len_un.len_s.m_pid = PROTOCOL_13761_VALID;
	pAckFrame->len1.len_un.len_s.m_len = pAckFrame->len.len_un.len_s.m_len;
	pAckFrame->len1.len_un.len_s.m_pid = pAckFrame->len.len_un.len_s.m_pid;
	
	pAckFrame->userdata.data[pos++] = GDW376_1_CalSum((uint8_t *)&(pAckFrame->control), pAckFrame->len.len_un.len_s.m_len);
	
	pAckFrame->userdata.data[pos] = FRAME_END_13761;
	
	pos = pAckFrame->len.len_un.len_s.m_len + PROTOCOL_13761_HEAD_LEN + PROTOCOL_13761_TAIL_LEN;
	if (pos > PROTOCOL_13761_BUF_SIZE )
	{
		return 0;
	}
	return Protocl13761SendData2(ACKbuf, pos, fd);
}



/*===============================================================
*函数名称：send_event
*功能描述：完成事件上报报文组帧，入队
*输入参数：无
*输出参数：无
*返回值：无
===============================================================*/
static u8 Seq = 0;					//事件上报帧序号
void send_event(int fd)
{
	u8 pAckBuf[1024] = {0};
	//是否允许主动上报		  
	//05f29允许终端主动上报
	//05f37禁止终端主动上报			

	//是否有数据需要上报
	GetEventRow();

	//构建上报报文
	EventsBlockHead head = {0};
	head.DA[0] = 0x00;
	head.DA[1] = 0x00;
	head.DT[0] = 0x01;
	head.DT[1] = 0x00;
	head.EC1 = m_EC1;
	head.EC2 = m_EC2;
	head.Pm = 0;
	head.Pn = 1;

	memcpy(pAckBuf, &head, sizeof(head));

	
	

	//检查上报标志位
	int i;
	for(i=0; i<80; i++)
	{
		int sum = sizeof(head);
		//有需要可以上报
		if(m_ErcReport[i] == 1)
		{
			//从数据库获取重要事件	
			printf("***shangbao*** = %d\n",i);
			int ret = GetImportanceEvent(i,i+1);
			if (ret < 0)		//失败
			{
				
			}

			int m = 0;
			int len = strlen(EventBuf[0]);
			printf("EventBuf[0] = %s\n\n",EventBuf[0]);
			
			for(m=0; m<len/2; m++)
			{
				//sscanf(&EventBuf[0][m*2], "%02x", (int *)&pAckBuf[m + sum]);
				int ret;
				sscanf(&EventBuf[0][m*2], "%02x", &ret);
				pAckBuf[m + sum] = ret;	
			}	

			sum = sum + len/2;
			
			//将报文放入发送队列	
			Eventdata Event = {0};
			memcpy(Event.data, pAckBuf, sum);
			Event.report_count = 0;
			Event.report_sucesss_flag = 0;	
			Event.datalen = sum;
			Event.seq = Seq++;							//帧序号
			
			if(sequeue_input(queue, Event) == 0)		//入队成功
			{
				//将上报标志位清0
				UpdateEventRow(i);	
			}	
		}	
	}

	//发送事件上报报文
	for(i=0; i<queue->count; i++)
	{
		Eventdata popdata = {0};
		sequeue_output(queue, &popdata);
		
		if(popdata.report_sucesss_flag == 0 && popdata.report_count < 3)
		{
			//发送给主站
			report13761UPtoMainStaion(0x0E, (u8 *)&popdata, popdata.datalen, fd);	
			
			//重新入队
			popdata.report_count++;
			sequeue_input(queue, popdata);	
		}			
	}	
}


//事件记录数据初始化
int EventDataInit()
{
	//删除数据库文件
	int ret = remove(FILENAME);
	if(ret == -1)
	{
		printf("Event Date Init fail\n");
		return -1;
	}
		
	//没有表则进行创建
	ret = create_table();
	if(ret == -1)
	{
		printf("Event Date Init fail\n");
		return -1;
	}		
	return 0;
}

//事件记录初始化
void EventInit()
{
	//没有表则进行创建
	create_table();

	//获取一般事件计数器
	m_EC1 = GetimportanceEC();

	//获取重要事件计数器
	m_EC2 = GetordinaryEC();

	//获取每个ERC记录个数
	int i = 0;
	for(i=0; i<MAX_ERC_NUM; i++)
	{
		GetImportanceEvent_Num(i+1);				//获取重要事件数量
		int Importantnum = m_ErcNum[i];
		
		GetordinaryEvent_Num(i+1);				   //获取一般事件数量
		int Ordinarynum =  m_ErcNum[i];
		m_ErcNum[i] = (Importantnum > Ordinarynum ? Importantnum : Ordinarynum);
	}

}



/***********************流量统计*******************************/
long		  comm_read_flow_;			//累计主站下发流量
long		  comm_write_flow_; 		//累计上传字节流量
long		  comm_curr_month_flow_;	//当月流量；					总的流量
long		  comm_last_day_flow_;		//当日流量；					上一天的流量
int 		  current_day_; 			//当前日
int 		  current_month_;			//当前月份
int 		  current_year_;			//当前年份
int  flow_event_reported_; 	//抑制频繁报告流量超限。1：可以创建事件，0：不创建事件

//int 		  f36_ytxllmx = 1024;//月流量门限

long 		  to_read_;
long		  to_write_;


//流量统计数据初始化
int FlowDataInit()
{	
	FILE *fd = NULL;
	fd = fopen(flow_path_,"w+");
	if(fd ==  NULL)
	{
		printf("Flow Data Init fail\n");
		return -1;
	}
	fclose(fd);
	return 0;
}

/***********************获取流量部分**************************************/
//处理流量，判断是否产生事件
void dealFlow()
{
	long R_test = get_comm_read_flow_without_lock();				//获取主站下发流量，并将下发流量清0
	long W_test = get_comm_write_flow_without_lock();				//获取终端上传流量，并将上传流量清0
    int res_r = set_comm_read_flow(R_test);							//下发流量增加
	int res_w = set_comm_write_flow(W_test);						//上传流量增加

    if(res_r == 4 || res_w == 4)//产生流量超限事件
	{
		stru_sjjl_ERC32 evt;
		u32  fluxlimit  = get_csf36_Month_Commflux_threshold();		//获取流量门限
		u32  flux       = get_month_flows();						//获取当前流量

		memcpy(evt.CommFluxLimit,&fluxlimit,4);			//流量门限
		memcpy(evt.CommFlux,&flux,4);					//当月流量	
		CreateEvent32(evt);								//创建事件记录
	}
}


long bytes_to_write()
{
	long temp = 0;
	temp = to_write_;
	to_write_ = 0;
	return to_write_;
}


long bytes_to_read()
{
	long temp = 0;
	temp = to_read_;
	to_read_ = 0;
	return temp;
}


//获取主站下发流量，并将下发流量青0
long get_comm_read_flow_without_lock()
{
	return bytes_to_read();
}


//获取终端上传流量，并将上传流量清0
long get_comm_write_flow_without_lock()
{
	return bytes_to_write();
}


//下发流量增加
int set_comm_read_flow(long value)
{
	int res   =  0;  
	comm_read_flow_ = comm_read_flow_ + value;

	//添加到当月流量
	res = set_month_flows_(value);  //如果res = 4的时候，表示要事件
	return res;
}


//上传流量增加
int set_comm_write_flow(long value)
{
	int res   =  0;
	comm_write_flow_ = comm_write_flow_ + value;

	//添加到当月流量
	res = set_month_flows_(value);		 //如果res = 4的时候，表示要事件

	return res;
}

//当月的流量(包括下发流量和上传流量)
int set_month_flows_(long value)
{
	int res = 0;
	comm_curr_month_flow_  += value;
	 //判断是否超流量，流量门限大于0的情况下
 	if(!flow_event_reported_ && GetCiSysInfo()->FluxLimitor > 0  && comm_curr_month_flow_ > GetCiSysInfo()->FluxLimitor)
	{
		res  = 4;
		flow_event_reported_ = 1;
	}
	return res;
}

////获取流量门限
long get_csf36_Month_Commflux_threshold()
{
   return GetCiSysInfo()->FluxLimitor;
}

//获取当前流量
long get_month_flows()
{
	return comm_curr_month_flow_;
}

//日总流量
long get_curr_day_flow()				//CTermParameters::
{
	long all_flow =  comm_write_flow_ + comm_read_flow_; 		//总流量
	
	return all_flow - comm_last_day_flow_;		//总流量-当日流量
}
/***********************获取流量部分end**************************************/


/*************************保存参数部分***************************************/
//加载，用于初始化
int load_flow_without_lock()
{
   long temp = 0;		
   if (getconfparam(flow_path_, "COMM_READ_FLOW", &temp, ':')<0)
	   saveconfparam(flow_path_, "COMM_READ_FLOW", temp, ':');
   comm_read_flow_	= temp;
 
   temp  = 0;
	if (getconfparam(flow_path_, "COMM_WRITE_FLOW", &temp, ':')<0)
	   saveconfparam(flow_path_, "COMM_WRITE_FLOW", temp, ':');
	comm_write_flow_  = temp;

	//初始当月流量
	load_comm_flow_month_without_lock();

	return 0;
}


//保存流量到文件	
int save_comm_flow(int check_new_day)
{
	if (save_comm_flow_month_without_lock() < 0)
		return -1;
	if (check_new_day && is_day_accross_without_lock())		//检查新的一天，如果跨越了日期
	{
		load_comm_flow_month_without_lock();
		save_comm_flow_month_without_lock();				//保存流量，保存日流量、月流量、上传流量、下发流量
	}
	return 0;
}



//保存流量-月
int save_comm_flow_month_without_lock()
{
	int res   =  -1;
   	char c_temp[64];
	//月流量
	sprintf(c_temp,"flow_%02d_%d",current_month_, current_year_); 		//年和月

    res =  saveconfparam(flow_path_, c_temp, comm_curr_month_flow_, ':');		//保存当月流量
    //日流量
	sprintf(c_temp,"flow_%02d_%02d_%d", current_day_, current_month_, current_year_); 	//年月日
	
	res =  saveconfparam(flow_path_, c_temp, comm_read_flow_ + comm_write_flow_, ':'); 			//保存日流量
	
	save_comm_read_flow();			//保存下发流量
	save_comm_write_flow();			//保存上传流量
	return res;
}


//判断是否是跨日期
int is_day_accross_without_lock()
{
 	struct tm ftnow;
	time_t t;
	t = time(NULL);	
	localtime_r(&t, &ftnow);

	ftnow.tm_year = ftnow.tm_year + 1900;
	ftnow.tm_mon = ftnow.tm_mon + 1;
	
	if(ftnow.tm_year != current_year_ || ftnow.tm_mon != current_month_ || ftnow.tm_mday != current_day_)
	{
		return 1;
	}
	return 0;
}


void load_comm_flow_month_without_lock()			//加载流量
{
	//当前月的流量清0	
	comm_curr_month_flow_ = 0;	

	//当前月流量为读写流量之和
	comm_last_day_flow_ = comm_write_flow_ + comm_read_flow_;
	
	struct tm ftnow;
	time_t t;
	t = time(NULL);	
	localtime_r(&t, &ftnow);

	ftnow.tm_year = ftnow.tm_year + 1900;
	ftnow.tm_mon = ftnow.tm_mon + 1;
	
   	char c_temp[64];
	sprintf(c_temp,"flow_%02d_%d", ftnow.tm_mon, ftnow.tm_year);

	current_day_   = ftnow.tm_mday;
	current_month_ = ftnow.tm_mon;
	current_year_  = ftnow.tm_year;

	//新的一月，开始上报
	flow_event_reported_ = 0;									
	
	long temp  = 0;
	//不能够获取到当前月，就是新的一月
   	if (getconfparam(flow_path_, c_temp, &temp, ':')<0)	
   	{
   		saveconfparam(flow_path_, c_temp, temp, ':');
   	}

	//更新月流量，是新的一月就清零
	comm_curr_month_flow_ = temp;	

	//
	t = time(NULL) - 24*3600;			
	localtime_r(&t, &ftnow);
	ftnow.tm_year = ftnow.tm_year + 1900;
	ftnow.tm_mon = ftnow.tm_mon + 1;
	sprintf(c_temp,"flow_%02d_%02d_%d", ftnow.tm_mday, ftnow.tm_mon, ftnow.tm_year);
	
	temp = comm_write_flow_ + comm_read_flow_;

	//不能够获取当天的，就是新的一天
	if( getconfparam(flow_path_, c_temp, &temp, ':') <0 )
	{
		saveconfparam(flow_path_, c_temp, temp, ':');
	}  

	//更新当日流量
	comm_last_day_flow_ = temp;
}



//保存主站下发字节流量到文件
int save_comm_read_flow()
{
   int res   =  -1;
   res =  saveconfparam(flow_path_, "COMM_READ_FLOW", comm_read_flow_,  ':');
   return res;

}

//保存上报主站流量到文件
int save_comm_write_flow()
{
   int res   =  -1;
   res =  saveconfparam(flow_path_,"COMM_WRITE_FLOW",comm_write_flow_, ':');			//保存参数，路径、符号、值
   return res;
}


//保存参数到文件
int saveconfparam(const char* confile, const char*name, long value, char delimiter)
{
	if (NULL == confile || NULL == name)
		return -1;
	
	char value_buf[64] = {0};
	value_buf[sizeof(value_buf) - 1] = 0;
	
	snprintf(value_buf, sizeof(value_buf) - 1, "%ld", value);
	
	int ret = saveConfPara(confile, name, value_buf, ':');
	return ret;
}


//从文件获取参数
int getconfparam(const char* confile,const char*name, long * value, char delimiter)
{
	char value_buf[64];

	int ret = getConfPara(confile, name, value_buf, sizeof(value_buf), delimiter);
	
	if (ret < 0)
		return ret;

	char* end = NULL;
	long tmpval = strtol(value_buf, &end, 0);
	
	if (end && *end) 			//(end && *end && !isspace(*end))
		return -1;
	
	*value = tmpval;

	return 0;
}

/*********************************保存参数部分end****************************************/
#if 0
//取得所有下发流量
long get_comm_read_flow()			//CTermParameters::
{
	return comm_read_flow_ ;
}

//取得所有上报流量
long get_comm_write_flow()			//CTermParameters::
{
	return comm_write_flow_;
}

//总流量()
long get_comm_flow()				//CTermParameters::
{
	long all_flow =  comm_write_flow_ + comm_read_flow_;
	return all_flow;
}

#endif











/********************************参数丢失事件***********************************/
//参数丢失事件是否发生
int CheckParaLostEvent(int Fn)				//int Fn
{
	stru_sjjl_ERC2 ev2;
	memset(&ev2, 0, sizeof(ev2));
	if (Fn == 1 || Fn == 3 || Fn == 7 || Fn == 8 || Fn == 9 || Fn == 10 || Fn == 16 || Fn == 89 || Fn == 91 ||  Fn == 153 || Fn == 154)
	{
		//终端参数丢失事件 
		ev2.LoseTerminal = 1;
		ev2.LosePot = 0;
		CreateEvent2(ev2);
	}
	else
	{
		return -1;
	}

/*	
	//监测到终端参数文件丢失时，产生事件记录
	if(access(TERM3761_PARA_CONF,F_OK)==-1)			//TERM3761_PARA_CONF
	{
		stru_sjjl_ERC2 ev2;
		memset(&ev2,0,sizeof(ev2));
		ev2.LoseTerminal = 1;  //终端参数丢失
		
		//记录事件
		CreateEvent2(ev2);
	}
*/
	return 0;
}


/***********************参数初始化和版本变更*********************/
#if 0
/*======================================================
*函数名称：IsTermVersionChange
*功能描述：终端版本是否变更
*输入参数：无
*输出参数：无
*返 回 值：0 成功，-1 失败
*======================================================*/
int IsTermVersionChange()
{ 
	int  i = 0, ret = -1;
	char buf[8] = {0}; 
	char path[1024] = {0}; 
	
	u8 NewTermSoftwareVersion[4], OldTermSoftwareVersion[4];

	//版本路径
	snprintf(path, sizeof(path)-1, "%s", "terminal_version.conf");

	memset(buf, 0x0, sizeof(buf));  
	memset(NewTermSoftwareVersion, '0', 4);
	memset(OldTermSoftwareVersion, '0', 4);
	
	//读新的终端软件版本号 
	if (getConfPara(path, "TermSoftwareVersion", buf, sizeof(buf), ':') >= 0)
	{
		for (i=0;i<4;i++)
		{
			NewTermSoftwareVersion[i] = buf[i];
		} 
		//旧软件版本号
		if (getConfPara(path, "TraceSoftwareVersion", buf, sizeof(buf),':') >= 0)
		{
			for (i=0;i<4;i++)
			{
				OldTermSoftwareVersion[i] = buf[i];
			} 
		}
		else
		{ 
			saveConfPara(path, "TraceSoftwareVersion", buf, ':');
		}
		if ((NewTermSoftwareVersion[0] != OldTermSoftwareVersion[0]) ||
			(NewTermSoftwareVersion[1] != OldTermSoftwareVersion[1]) ||
			(NewTermSoftwareVersion[2] != OldTermSoftwareVersion[2]) ||
			(NewTermSoftwareVersion[3] != OldTermSoftwareVersion[3]))
		{
			//产生版本变更事件
			stru_sjjl_ERC1 event1;
			memset(&event1, 0, sizeof(stru_sjjl_ERC1));
			event1.VER = 1;
			memcpy(event1.VerNew, NewTermSoftwareVersion, 4);
			memcpy(event1.VerOld, OldTermSoftwareVersion, 4);
			CreateEvent1(event1);
			 
			for (i=0;i<4;i++)
			{
				buf[i] = NewTermSoftwareVersion[i];
			}
			saveConfPara(path, "TraceSoftwareVersion", buf, ':');

			ret = 0;
		}	
	}
	return ret;
}
#endif

//更改配置文件中的软件版本号并产生终端版本变更事件
int WriteVersionFileAndCreateEvent(u8 *newVersion)
{
	if(newVersion == NULL)
		return -1;
	char buf[64] = {0};
	int i = 0;
	
	stru_sjjl_ERC1 event1;
	event1.VER = 1;
	
	//获取原版本号
	if (getConfPara(TERM_SOFT_VERSION, "TermSoftwareVersion", buf, sizeof(buf), ':') >= 0 )
	{
		for (i=0;i<4;i++)
		{
			event1.VerOld[i] = buf[i];
		} 
	}

	//记下原版本号
	if (saveConfPara(TERM_SOFT_VERSION, "OldSoftwareVersion", buf, ':') < 0)
	{
		return -1;
	}
	//写入新版本号
	snprintf(buf, 5, "%c%c%c%c", newVersion[0]/*-0x30*/, newVersion[1]/*-0x30*/, newVersion[2]/*-0x30*/, newVersion[3]/*-0x30*/);
	if (saveConfPara(TERM_SOFT_VERSION, "TermSoftwareVersion", buf, ':') < 0)		//写入错误
	{
		return -1;
	}
	else
	{
		memcpy(event1.VerNew, newVersion, 4);
		CreateEvent1(event1);
		return 0;
	}
}



/*======================================================
*函数名称：GetMagnetic
*功能描述：获取磁场大小
*输入参数：无
*输出参数：无
*返 回 值：0 成功，-1 失败
*======================================================*/
int GetMagnetic(char *pathname,char *buf,int len,int offset)
{
	/*

	int fd;
	int ret;

	fd = open(pathname,O_RDWR);
	if (fd < 0)
	{
		printf("open %s dev failed.\n",pathname);
		return -1;
	}

	int addr = 1;
	unsigned char wrbuf[2];
	memset(wrbuf,0,2);//read from EEPROM offset 0
	if (offset)
	{

		wrbuf[0] = (unsigned char)offset;
	}
	
	if (ioctl(fd,I2C_SLAVE,addr) < 0)
	{
		perror("read set slave addr:");
		close(fd);
		return -1;
	}
	ret = write(fd,wrbuf,E2PROMADDRLEN);
	
	if (ret != E2PROMADDRLEN)
	{
		perror("send read cmd failed:");
		fprintf(stderr,"at offset 0x%x\n",offset);
		close(fd);
		return -1;
	}


	ret = read(fd,buf,len);
	if (ret != len)
	{
		perror("read failed:");
		fprintf(stderr,"ret %d\n",ret);
		ret = -1;
	}
	close(fd);

	return ret;
	*/
}


extern tstMCurDataStruct ACdate;;
/*======================================================
*函数名称：GetACdate
*功能描述：获取交采数据电压电流
*输入参数：无
*输出参数：无
*返 回 值：0 成功，-1 失败
*======================================================*/
int GetACdate()
{
	if(com_645_2007_SendFrame(GET_JC_DATA, NULL) == -1)
		return -1;
	return 0;
}


//电流存储数据初始化
int ACdatecurrentInit()
{
	FILE *fd = NULL;
	fd = fopen(ACdate_path,"w+");
	if(fd == NULL)
	{
		printf("Current Data Init fail\n");
		return -1;
	}
	fclose(fd);
	return 0;
}


//存储电流值，从整点开始每隔15分钟记录一次
void SaveACdatecurrent()
{
	if(GetACdate() == -1)
	{
		//return;
	}
	
	char date[32] = {0};
	time_t t;
	struct tm ftnow;
	t = time(NULL);	
	localtime_r(&t, &ftnow);

	ftnow.tm_mon += 1;
	ftnow.tm_year += 1900;
	sprintf(date, "%d-%d-%d %d-%d", ftnow.tm_year, ftnow.tm_mon, ftnow.tm_mday, ftnow.tm_hour, ftnow.tm_min);

	char current[64] = {0};
	sprintf(current,"%10d %10d %10d",ACdate.I[0], ACdate.I[1], ACdate.I[2]);
	saveConfPara(ACdate_path, date, current, ':');
}


/***************************停上电事件************************/
//前提：交采正常工作(交采接线正确，交采模块测量准确)，电源模块正常工作，电源模块工作范围覆盖参数F98电压范围
//电压低于125V，则产生事件 
int check_power_off()
{
	//读取交采数据
	//GetACdate();
	
	//发生停电事件
	if(1)//if(ACdate.U[0] <  125 && ACdate.U[1] < 125 && ACdate.U[2] < 125)	
	{
		stru_sjjl_ERC14 ev14;
		memset(&ev14, 0, sizeof(ev14));
		ev14.EventEnable = 0x1;
		ev14.EventNormal = 0x1;
		
		//获取停电时间
		GetTime(&ev14.offtime);
		u8 buf[5] = {0xEE, 0xEE, 0xEE, 0xEE, 0xEE};
		memcpy(&ev14.ontime, buf, 5);
		
		//记录事件
		CreateEvent14(ev14);

		//立即上报
		//send_event(0);
	}
	return 0;
}

int check_power_on()
{
	//读取交采数据
	//GetACdate();
	
	//发生上电事件
	if(1)//(ACdate.U[0] >  125 || ACdate.U[1] > 125 || ACdate.U[2] < 125)	
	{	
		u8 buf[128] = {0};
		stru_sjjl_ERC14 ev14 = {0};
		//检测停电事件
		int ERC = 14;		
		int Event_cont = GetErcEvent(ERC);
		int len = strlen(EventBuf[Event_cont-1]);
		int i;
		for(i=0; i<len/2; i++)
		{
			//sscanf(&EventBuf[Event_cont-1][i*2], "%02x", (int *)&buf[i]);
			int ret;
			sscanf(&EventBuf[Event_cont-1][i*2], "%02x", &ret);
			buf[i] = ret;		
		}
		memcpy(&ev14, buf, sizeof(ev14));
		if(ev14.ontime.FGW == 0xE && ev14.ontime.FSW == 0xE &&
		   ev14.ontime.SGW == 0xE && ev14.ontime.SSW == 0xE &&
		   ev14.ontime.RGW == 0xE && ev14.ontime.RSW == 0xE &&
		   ev14.ontime.YGW == 0xE && ev14.ontime.YSW == 0xE &&
		   ev14.ontime.NGW == 0xE && ev14.ontime.NSW == 0xE)
		{
			//创建上电事件，更新停电事件

			//获取上电时间
			GetTime(&ev14.ontime);

			u8 *Event = (u8 *)&ev14;		
			char buf[1024] = {0};			
			for(i=0; i<sizeof(ev14); i++)
			{
				sprintf(&buf[i*2], "%02x", Event[i]);
			}	
			int second = 0;
			GetSecond(&second);
			
			//更新事件
			UpdatePower(buf, second);
		}
	}
	return 0;
}



/*======================================================
*函数名称：CheckTAEvent
*功能描述：检测TA模块的四种事件
*输入参数：无
*输出参数：无
*返 回 值：0 成功，-1 失败
*======================================================*/
static sFormat_TA_Status TaStatusBack;
int CheckTAEvent()
{
	sFormat_TA_Status TAStatus = GetTaStatus();

	stru_sjjl_ERC34 ev34;
	
	printf("************1111111***********\n\n");
	//检测A、B、c相二次回路短路
	memset(&ev34, 0, sizeof(ev34));
	if(TaStatusBack.Phase_A_ststus != TAStatus.Phase_A_ststus)
	{
		if(TAStatus.Phase_A_ststus&0X01 == 1)
		{
			ev34.A = 1;
			ev34.extype = 0x01;  //01二次侧短路
		}
	}
	if(TaStatusBack.Phase_B_ststus != TAStatus.Phase_B_ststus)
	{
		if(TAStatus.Phase_B_ststus&0X01 == 1)
		{
			ev34.B = 1;
			ev34.extype = 0x01;  //01二次侧短路
		}
	}
	if(TaStatusBack.Phase_C_ststus != TAStatus.Phase_C_ststus)
	{
		if(TAStatus.Phase_C_ststus&0X01 == 1)
		{
			ev34.C = 1;
			ev34.extype = 0x01;  //01二次侧短路
		}
	}
	//发生了，记录事件
	if(ev34.A == 1 || ev34.B == 1 || ev34.C == 1)
	{
		ev34.ss = 1;			//起止标志
		ev34.pn = 1;			//测量点号
		CreateEvent34(ev34);	//记录事件	
		printf("************222222***********\n\n");
	}
		
	
	//检测A、B、C相CT开路
	memset(&ev34, 0, sizeof(ev34));
	if(TaStatusBack.Phase_A_ststus != TAStatus.Phase_A_ststus)
	{
		if(TAStatus.Phase_A_ststus&0X02 == 1)
		{
			ev34.A = 1;
			ev34.extype = 0x02; //02开路
		}
	}
	if(TaStatusBack.Phase_B_ststus != TAStatus.Phase_B_ststus)
	{
		if(TAStatus.Phase_B_ststus&0X02 == 1)
		{
			ev34.B = 1;
			ev34.extype = 0x02; //02开路
		}
	}
	if(TaStatusBack.Phase_C_ststus != TAStatus.Phase_C_ststus)
	{
		if(TAStatus.Phase_C_ststus&0X02 == 1)
		{
			ev34.C = 1;
			ev34.extype = 0x02; //02开路
		}
	}
	//发生了，记录事件
	if(ev34.A == 1 || ev34.B == 1 || ev34.C == 1)
	{
		ev34.ss = 1;			//起止标志
		ev34.pn = 1;			//测量点号
		CreateEvent34(ev34);	//记录事件	
	}


	//检测A、B、C相CT二次串入半导体
	memset(&ev34, 0, sizeof(ev34));
	if(TaStatusBack.Phase_A_ststus != TAStatus.Phase_A_ststus)
	{
		if(TAStatus.Phase_A_ststus&0X04 == 1)
		{
			ev34.A = 1;
			ev34.extype = 0x06; //06回路串接整流设备
		}
	}
	if(TaStatusBack.Phase_B_ststus != TAStatus.Phase_B_ststus)
	{
		if(TAStatus.Phase_B_ststus&0X04 == 1)
		{
			ev34.B = 1;
			ev34.extype = 0x06; //06回路串接整流设备
		}
	}
	if(TaStatusBack.Phase_C_ststus != TAStatus.Phase_C_ststus)
	{
		if(TAStatus.Phase_C_ststus&0X04 == 1)
		{
			ev34.C = 1;
			ev34.extype = 0x06; //06回路串接整流设备
		}
	}
	//发生了，记录事件
	if(ev34.A == 1 || ev34.B == 1 || ev34.C == 1)
	{
		ev34.ss = 1;			//起止标志
		ev34.pn = 1;			//测量点号
		CreateEvent34(ev34);	//记录事件	
		printf("************3333***********\n\n");
	}


	//检测A、B、C相CT恢复正常
	memset(&ev34, 0, sizeof(ev34));
	if(TaStatusBack.Phase_A_ststus != TAStatus.Phase_A_ststus)
	{
		if(TAStatus.Phase_A_ststus == 0x00)
		{
			ev34.A = 1;
			ev34.extype = 0x00; //00恢复正常
		}
	}
	if(TaStatusBack.Phase_B_ststus != TAStatus.Phase_B_ststus)
	{
		if(TAStatus.Phase_B_ststus == 0x00)
		{
			ev34.B = 1;
			ev34.extype = 0x00; //00恢复正常
		}
	}
	if(TaStatusBack.Phase_C_ststus != TAStatus.Phase_C_ststus)
	{
		if(TAStatus.Phase_C_ststus == 0x00)
		{
			ev34.C = 1;
			ev34.extype = 0x00; //00恢复正常
		}
	}
	//发生了，记录事件
	if(ev34.A == 1 || ev34.B == 1 || ev34.C == 1)
	{
		ev34.ss = 0;			//起止标志
		ev34.pn = 1;			//测量点号
		CreateEvent34(ev34);	//记录事件	
		printf("************4444***********\n\n");
	}


	//检测一次分流事件，变比小于 500:5，ABC 三相二次电流等于 0 时产生事件记录 
	memset(&ev34, 0, sizeof(ev34));
	if(TaStatusBack.Phase_A_ststus != TAStatus.Phase_A_ststus)
	{
		if(TAStatus.Phase_A_ststus&0X08 == 1)
		{
			ev34.A = 1;
			ev34.extype = 0x03; //一次分流
		}
	}
	if(TaStatusBack.Phase_B_ststus != TAStatus.Phase_B_ststus)
	{
		if(TAStatus.Phase_B_ststus&0X08 == 0x08)
		{
			ev34.B = 1;
			ev34.extype = 0x03; //一次分流
		}
	}
	if(TaStatusBack.Phase_C_ststus != TAStatus.Phase_C_ststus)
	{
		if(TAStatus.Phase_C_ststus&0X08 == 1)
		{
			ev34.C = 1;
			ev34.extype = 0x03; //一次分流
		}
	}
	//发生了，记录事件
	if(ev34.A == 1 || ev34.B == 1 || ev34.C == 1)
	{
		ev34.ss = 1;			//起止标志
		ev34.pn = 1;			//测量点号
		CreateEvent34(ev34);	//记录事件	
		printf("************5555***********\n\n");
	}
		
	return 0;
}





//函数功能：读巡检仪内部文件获取信号强度值，并转换为信号品质值返回给Process_F15 by:wrh
int readgprssignal()
{
    FILE *fd;
    char gprsbuf[20]={0};
    int signal=0;
	int i=0;
    char *pos = NULL;
	int SignalStrengthArray[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 };
	int SignalArray[] = {-113, -111, -109, -107, -105, -103, -101, -99, -97, -95, -93, -91, -89, -87, -85, -83, -81, -79, -77, -75, -73, -71, -69, -67, -65, -63, -61, -59, -57, -55, -53, -51};
	fd = fopen(GPRS_SIGNAL_PATH,"r");
	printf("file open success!\n");								//文件打开成功
	//LOG_ERROR((LM_ERROR,ACE_TEXT("file open success!\n")));	//文件打开成功
    if(NULL == fd)    //文件不存在
    {
    printf("file doesn't exit!\n");//文件不存在
        return 0;
    }
    else
    {
        fread(gprsbuf,sizeof(gprsbuf),1,fd);
	    printf("file read success!\n");								//读文件成功
	    printf("%s\n",gprsbuf);										//查看读取字符串的内容
	    //LOG_ERROR((LM_ERROR,ACE_TEXT("file read success!\n")));	//读文件成功
        fclose(fd);
	    fd=NULL;
	    printf("file close success!\n");							//关闭文件成功
    }

    if(pos = strstr(gprsbuf,"rssi="))
    {
        printf("get rssi success!pos=%s\n",pos);				//获取信号强度成功
        pos += 5;
		printf("pos+5=%s\n",pos);								//查看pos+5的值
        signal = strtol(pos,NULL,10);
		printf("string to long success!\n");					//转换为整型成功
		printf("signal=%d.\n",signal);							//转换为整型成功
       for (i = 0; i < sizeof(SignalStrengthArray) / sizeof(SignalStrengthArray[0]); i++)
	    {
		if (signal ==SignalStrengthArray[i])
		    {
		printf("signal match Arraymember success!SignalStrengthArray=%d,SignalArray=%d\n",SignalStrengthArray[i],SignalArray[i]);//信号在数组里匹配到对应的元素
		    //SignalArray[i]-=800;								//信号品质小于0，将百分位的最高位赋值为1
		printf("SignalArray=%d\n",SignalArray[i]);
		    return SignalArray[i];
		printf("return signal data success!\n");				//返回信号数据成功
		    }
		else 
			continue;
        }
    }
    return 0;
}


/*
//F15：终端网络信号品质 by:wrh			集中器使用的是请求一类数据的F15，扩展的协议
int Process_F15(int FN,AutoRef<Process_698info>&P_698_info)
 {
    void *fenetgy=NULL;//fenetgy初始化为空
    int a,b,c,d,e;

//块长
    if(FN==15)
      {
       printf("enter F15 success!\n");
	   P_698_info->replyblocklen698=sizeof(StDataFormat5);//获取数据格式05的内存大小，作为应答数据单元数据块长度
       printf("F15 get replyblocklen698 success!\n");
      }
    else
	   return -1;
//创建698应答帧
if(P_698_info->CheckWhetherNew698ReplyFrame(P_698_info->frm698->getEC1(),
P_698_info->frm698->getEC2(),P_698_info->replyblocklen698 + sizeof(DataId698),
false,P_698_info->is_last_di)<0)
   {
    //error
    MY_ACE_ERROR((LM_ERROR,ACE_TEXT("698 realtime read reply new frame faile fn %d pn %d.\n ,"),FN,P_698_info->MPno));
    return -1;
   }  
//添加到当前
if (NULL != P_698_info->newReplymb)
   {	
   Protocol_698_Frame::appendNewMB(P_698_info->firstReplymb,P_698_info->
curReplymb,P_698_info->newReplymb,P_698_info->spmsg698);
   P_698_info->curReplymb = P_698_info->newReplymb;
   P_698_info->newReplymb = NULL;
   }
ACE_ASSERT(P_698_info->replycurpos698 > 0);
P_698_info->replydi = (DataId698*)(P_698_info->replyFrame.data() + P_698_info
->replycurpos698);
P_698_info->replydi->setSingleFN(FN);
P_698_info->replydi->setSinglePN(0);
P_698_info->replycurpos698 += sizeof(DataId698);

if(FN==15)
	{
		fenetgy = (StDataFormat5*)(P_698_info->replyFrame.data() + P_698_info->
replycurpos698);
		ACE_OS::memset(fenetgy,NULLBCD,P_698_info->replyblocklen698);
		  a=abs((readgprssignal())%10);
		  b=abs(((readgprssignal())*10)%10);
		  c=abs(((readgprssignal())/100)%10);
		  d=abs(((readgprssignal())/10)%10);
		  e=readgprssignal()>0? 0:1;
		  ((StDataFormat5*)fenetgy)->BCDgewei=a;
		  ((StDataFormat5*)fenetgy)->BCDshifenwei=b;
		  ((StDataFormat5*)fenetgy)->BCDbaiwei=c;
		  ((StDataFormat5*)fenetgy)->BCDshiwei=d;
		  ((StDataFormat5*)fenetgy)->S=e;
		  printf("shifenwei gewei shiwei baiwei S is %d %d %d %d %d\n",b,a,d,c,e); 
		  printf("F15 get signal data success!\n");//获取信号数据成功
	}   
	else
		return -1;
	P_698_info->replycurpos698 += P_698_info->replyblocklen698;//replycurpos698应答帧待填写位置

	
	P_698_info->replyblocklen698 += sizeof(DataId698); //DataId698数据单元标识定义
		return 0;
}
*/




//从文件读取终端版本信息
int GetTermVersionInfo(stTermVerInfo *termVerInfo)
{ 
	char buf[1024] = {0};
	int i = 0;

	memset(buf, 0x0, sizeof(buf));
	memset(termVerInfo, 0, sizeof(stTermVerInfo));
	
	//厂商代号 
	if(getConfPara(TERM_SOFT_VERSION, "ManufacturerCode", buf, sizeof(buf), ':') >= 0)
	{
		for (i=0;i<4;i++)
		{
			termVerInfo->factry_code[i] = buf[i];
		} 
	} 
	
	//设备编号 
	if(getConfPara(TERM_SOFT_VERSION, "DeviceSerialNo", buf, sizeof(buf), ':') >= 0)
	{
		for (i=0;i<8;i++)
		{
			termVerInfo->term_number[i] = buf[i];
		} 
	} 

	//终端软件版本号 
	if (getConfPara(TERM_SOFT_VERSION, "TermSoftwareVersion", buf, sizeof(buf), ':') >= 0)
	{
		for (i=0;i<4;i++)
		{
			termVerInfo->soft_ver[i] = buf[i];
		} 
	}
	//终端软件发布日期
	if (getConfPara(TERM_SOFT_VERSION, "TermSoftwareReleaseDate", buf, sizeof(buf), ':') >= 0)
	{
		int temp[3] = {0};
		printf("buf = %s\n",buf);
		sscanf(&buf[0], "%02d", &temp[2]);
		sscanf(&buf[2], "%02d", &temp[1]);
		sscanf(&buf[4], "%02d", &temp[0]);

		printf("temp[0] = %d\n",temp[0]);
		printf("temp[1] = %d\n",temp[1]);
		printf("temp[2] = %d\n",temp[2]);


		u8 ret[3] = {0};
		intToBcd(temp[0], &ret[0], 1);
		intToBcd(temp[1], &ret[1], 1);
		intToBcd(temp[2], &ret[2], 1);
		memcpy(&termVerInfo->release_time, ret, 3);
	}
	
	//终端配置容量信息码 
	if (getConfPara(TERM_SOFT_VERSION, "TermConfCapacityInfoCode", buf, sizeof(buf), ':') >= 0)
	{
		for (i=0;i<11;i++)
		{
			termVerInfo->capacity_info[i] = buf[i];
		}
	}
	//终端通信协议版本号
	if (getConfPara(TERM_SOFT_VERSION, "TermCommProtocolVersion", buf, sizeof(buf), ':') >= 0)
	{
		for (i=0;i<4;i++)
		{
			termVerInfo->Protocol_version[i] = buf[i];
		}
	}
	//终端硬件版本号 
	if (getConfPara(TERM_SOFT_VERSION, "TermHardwareVersion", buf, sizeof(buf), ':') >= 0)
	{
		for (i=0;i<4;i++)
		{
			termVerInfo->hardware_ver[i] = buf[i];
		}
	}
	//终端硬件发布日期 
	if (getConfPara(TERM_SOFT_VERSION, "TermHardwareReleaseDate", buf, sizeof(buf), ':') >= 0)
	{
		int temp[3] = {0};
		printf("buf = %s\n",buf);
		sscanf(&buf[0], "%02d", &temp[2]);
		sscanf(&buf[2], "%02d", &temp[1]);
		sscanf(&buf[4], "%02d", &temp[0]);

		printf("temp[0] = %d\n",temp[0]);
		printf("temp[1] = %d\n",temp[1]);
		printf("temp[2] = %d\n",temp[2]);


		u8 ret[3] = {0};
		intToBcd(temp[0], &ret[0], 1);
		intToBcd(temp[1], &ret[1], 1);
		intToBcd(temp[2], &ret[2], 1);
		memcpy(&termVerInfo->hardware_release_time, ret, 3);
	} 

	return 0;
}




//从文件读取远程模块版本信息
int GetRemoteComModalVerInfo(RemoteVerInfo *remoteModalVerInfo)
{ 
	char buf[1024] = {0};
	int i = 0;

	memset(buf, 0x0, sizeof(buf));
	memset(remoteModalVerInfo, 0, sizeof(RemoteVerInfo));
	
	//厂商代号 
	if (getConfPara(REMOTE_MODULE_CONF_PATH, "ManufacturerCode", buf, sizeof(buf), ':') >= 0)
	{
		for (i=0;i<4;i++)
		{
			remoteModalVerInfo->factry_code[i] = buf[i];
		} 
	} 
	
	//模块型号 
	if (getConfPara(REMOTE_MODULE_CONF_PATH, "ModuleNo", buf, sizeof(buf), ':') >= 0)
	{
		for (i=0;i<8;i++)
		{
			remoteModalVerInfo->module_type[i] = buf[i];
		} 
	} 

	//软件版本号  
	if (getConfPara(REMOTE_MODULE_CONF_PATH, "ModuleSoftwareVersion", buf, sizeof(buf), ':') >= 0)
	{
		for (i=0;i<4;i++)
		{
			remoteModalVerInfo->soft_ver[i] = buf[i];
		} 
	}
	
	//软件发布日期
	if (getConfPara(REMOTE_MODULE_CONF_PATH, "ModuleSoftwareReleaseDate", buf, sizeof(buf), ':') >= 0)
	{
		int temp[3] = {0};
		printf("buf = %s\n",buf);
		sscanf(&buf[0], "%02d", &temp[2]);
		sscanf(&buf[2], "%02d", &temp[1]);
		sscanf(&buf[4], "%02d", &temp[0]);

		printf("temp[0] = %d\n",temp[0]);
		printf("temp[1] = %d\n",temp[1]);
		printf("temp[2] = %d\n",temp[2]);


		u8 ret[3] = {0};
		intToBcd(temp[0], &ret[0], 1);
		intToBcd(temp[1], &ret[1], 1);
		intToBcd(temp[2], &ret[2], 1);
		memcpy(&remoteModalVerInfo->release_time, ret, 3);
		
	}


	//硬件版本号 
	if (getConfPara(REMOTE_MODULE_CONF_PATH, "TermHardwareVersion", buf, sizeof(buf), ':') >= 0)
	{
		for (i=0;i<4;i++)
		{
			remoteModalVerInfo->hardware_ver[i] = buf[i];
		}
	}
	
	//硬件发布日期
	if (getConfPara(REMOTE_MODULE_CONF_PATH, "ModuleHardwareReleaseDate", buf, sizeof(buf), ':') >= 0)
	{
		int temp[3] = {0};
		printf("buf = %s\n",buf);
		sscanf(&buf[0], "%02d", &temp[2]);
		sscanf(&buf[2], "%02d", &temp[1]);
		sscanf(&buf[4], "%02d", &temp[0]);

		printf("temp[0] = %d\n",temp[0]);
		printf("temp[1] = %d\n",temp[1]);
		printf("temp[2] = %d\n",temp[2]);


		u8 ret[3] = {0};
		intToBcd(temp[0], &ret[0], 1);
		intToBcd(temp[1], &ret[1], 1);
		intToBcd(temp[2], &ret[2], 1);
		
		memcpy(&remoteModalVerInfo->hardware_release_time, ret, 3);
	} 

	//SIM卡ICCID
	if (getConfPara(REMOTE_MODULE_CONF_PATH, "SIMCardICCID", buf, sizeof(buf), ':') >= 0)
	{
		for (i=0;i<20;i++)
		{
			remoteModalVerInfo->ICCID[i] = buf[i];
		}
	}
	return 0;
}



//检测事件的定时器， 磁场事件、停上电事件、流量超门限事件、TA事件、参数丢失，其他事件都有特定的检测
static void EvevtCallBack(void * data)
{
	//流量超门限事件
	//处理流量
	dealFlow();
	
	//存储流量
	save_comm_flow(1);

	//停电事件
	//check_power_off();

	//TA事件
	//CheckTAEvent();
	
	//磁场事件


	//参数丢失
	//CheckParaLostEvent();
}
int check_evevt_time()
{
	add_timer(2, 6, EvevtCallBack, NULL, NULL);

	//程序启动上电事件检测
	check_power_on();	
	return 0;
}


static void SendEvevtCallBack(void * data)
{
	send_event(0);		//需要文件描述符
}
//主动上报的定时器
int send_event_time()
{
	add_timer(2, 10, SendEvevtCallBack, NULL, NULL);
}


