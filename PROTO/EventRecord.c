#include "EventRecord.h"
#include<time.h>
#include<string.h>
#include<stdlib.h>

//全局变量
u8 m_EC1;							//EC1的值，如果重要事件没有写满，值就是++，如果到达255，就一直存储255 （2009年12月15日）
u8 m_EC2;							//EC2的值，如果普通事件没有写满，值就是++，如果到达255，就一直存储255 （2009年12月15日）
u8 m_ErcNum[MAX_ERC_NUM] = {0};		//同一类事件的数量，记录最多10个
u8 m_ErcReport[EveMax] = {0};		//主动上报事件标志	


//保存重要事件计数器值
int saveimportanceEC(u8 EC)
{
	if(writefile(IMPORTANTFILEEC, &EC, 1, "w+") != 0)
	{
		return -1;
	}
	return 0;
}

//保存一般事件计数器值
int saveordinaryEC(int EC)
{
	if(writefile(ORDINARYFILEEC, &EC, 1, "w+") != 0)
	{
		return -1;
	}
	return 0;
}

//获取重要事件计数器的值
int GetEC1()
{
	return 0;
}

//获取一般计数器值
int GetEC2()
{
	return 0;
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
*函数名称：GetReportStatus
*功能描述：获取重要事件主动上报的状态
*输入参数：事件标识，共8个字节，表示64个事件
*输出参数： 无
*返回值：0 成功，-1 失败
***************************************/
int GetReportStatus(u8 *flag)
{
	
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


/**************************************
*函数名称：GetEventStatus
*功能描述：在文件中保存事件是否发生
*输入参数：事件标识，共8个字节，表示64个事件
*输出参数： 无
*返回值：0 成功，-1 失败
***************************************/
int GetEventStatus(u8 *flag)
{

}

extern TermEventRecordSet TermEventRecord;

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
	*ValidFlag = (TermEventRecord.CS_F9_EventRecordValidFlag[i] >> j)  & 0x01;		
	*Priority = (TermEventRecord.CS_F9_EventRatingFlag[i] >> j)  & 0x01;
	return 0;
}

//将事件写入文件
//参数Priority：事件等级  data：事件数据           Len：数据长度
int saveEvent(int Priority,int ERC ,const u8* data,int Len, int time, int report)
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
		printf("buf = %s\n",buf);
		
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
		printf("buf = %s\n",buf);
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
int saveEventFile(int ErcName,u8* data,long len, int time)
{
	int ValidFlag = 0;			//需不需要记录
	int Priority = 0;			//重要事件还是一般事件,重要事件需要上报

	GetValidAndPriorityByErc(ErcName, &ValidFlag, &Priority);
		
	if(ValidFlag == 0)			//不需要记录事件
	{
		printf("saveEventFile %d is disabled\n",ErcName);
		//return -1;
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

		//删除重要事件中的该事件
		DeleteImportanceEvent(ErcName);	
		
	}	
	else	//重要事件
	{
		//同一种类事件数量超过10个，则删除最早的一个事件
		if (m_ErcNum[ErcName-1] >= MAX_NUM_EACH_ERC)
		{
			do
			{
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

		//删除一般事件中的该事件
		DeleteImportanceEvent(ErcName);	

		//更新主动上报标志位置
		GetEventRow();	
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
	time_t t;
	struct tm ftnow;
	t = time(NULL);	
	localtime_r(&t, &ftnow);

	SJGS_15 time;
	
	
	time.FSW = ftnow.tm_min / 10;
	time.FSW = ftnow.tm_min % 10;
	
	time.SSW   = ftnow.tm_hour / 10;
	time.SGW   = ftnow.tm_hour % 10;
	
	time.RSW    = ftnow.tm_mday / 10;
	time.RGW    = ftnow.tm_mday % 10;

	ftnow.tm_mon += 1;
	time.YSW = ftnow.tm_mon / 10;
	time.YGW = ftnow.tm_mon % 10;

	ftnow.tm_year += 1900;
	time.NSW = ftnow.tm_year%100 - ftnow.tm_year%10;
	time.NGW = ftnow.tm_year%10;
	memcpy(Time, &time, sizeof(time));
	
}

//获取秒
void GetSecond(int *second)
{
	time_t t;
	time(&t);	
	*second = (int)t;
}

//ERC1：数据初始化和版本变更
int CreateEvent1(stru_sjjl_ERC1       ev1)
{
	int len = 0;
	int second = 0;
	
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
int CreateEvent3(stru_sjjl_ERC2 ev3)
{ 
	int len = 0;
	int second = 0;
	ev3.ERCcode=0x03;	
	ev3.Le = 0x0A;	
	
	char rowdata[256];
	memset(rowdata,0,sizeof(rowdata));
	
	memcpy(rowdata, &ev3, sizeof(ev3));
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
	ev14.Le = 0x0A;	
	
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
	ev34.Le = 0x03;		
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







