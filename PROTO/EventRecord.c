#include "EventRecord.h"
#include<time.h>
#include<string.h>
#include<stdlib.h>

//ȫ�ֱ���
u8 m_EC1;							//EC1��ֵ�������Ҫ�¼�û��д����ֵ����++���������255����һֱ�洢255 ��2009��12��15�գ�
u8 m_EC2;							//EC2��ֵ�������ͨ�¼�û��д����ֵ����++���������255����һֱ�洢255 ��2009��12��15�գ�
u8 m_ErcNum[MAX_ERC_NUM] = {0};		//ͬһ���¼�����������¼���10��
u8 m_ErcReport[EveMax] = {0};		//�����ϱ��¼���־	


//������Ҫ�¼�������ֵ
int saveimportanceEC(u8 EC)
{
	if(writefile(IMPORTANTFILEEC, &EC, 1, "w+") != 0)
	{
		return -1;
	}
	return 0;
}

//����һ���¼�������ֵ
int saveordinaryEC(int EC)
{
	if(writefile(ORDINARYFILEEC, &EC, 1, "w+") != 0)
	{
		return -1;
	}
	return 0;
}

//��ȡ��Ҫ�¼���������ֵ
int GetEC1()
{
	return 0;
}

//��ȡһ�������ֵ
int GetEC2()
{
	return 0;
}


/**************************************
*�������ƣ�saveReportStatus
*�������������ļ��б����¼��Ƿ��ϱ�
*���������Num���Ѿ��������¼����, flag: 1ΪҪ�����ϱ���0���ϱ���ɡ�
*��������� ��
*����ֵ��0 �ɹ���-1 ʧ��
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
*�������ƣ�GetReportStatus
*������������ȡ��Ҫ�¼������ϱ���״̬
*����������¼���ʶ����8���ֽڣ���ʾ64���¼�
*��������� ��
*����ֵ��0 �ɹ���-1 ʧ��
***************************************/
int GetReportStatus(u8 *flag)
{
	
}


/**************************************
*�������ƣ�saveEventRecordStatus
*�������������ļ��б����¼��Ƿ���
*���������ErcName���¼�����
*��������� ��
*����ֵ��0 �ɹ���-1 ʧ��
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
*�������ƣ�GetEventStatus
*�������������ļ��б����¼��Ƿ���
*����������¼���ʶ����8���ֽڣ���ʾ64���¼�
*��������� ��
*����ֵ��0 �ɹ���-1 ʧ��
***************************************/
int GetEventStatus(u8 *flag)
{

}

extern TermEventRecordSet TermEventRecord;

//����F9��ֵ�ж��¼���һ���¼�������Ҫ�¼����Լ��¼��Ƿ���Ҫ����
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

//���¼�д���ļ�
//����Priority���¼��ȼ�  data���¼�����           Len�����ݳ���
int saveEvent(int Priority,int ERC ,const u8* data,int Len, int time, int report)
{ 
	if((Priority != 1 && Priority !=0) || data == NULL || Len < 0)
	{
		return -1;
	}
	if(1 == Priority)		//д����Ҫ�¼�
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
	else					//д��һ���¼�
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
*�������ƣ�saveEventFile
*���������������¼���¼���ļ���
*���������ErcName���¼����� �� data�����ݣ�len�����ݳ���  time:��¼ʱ���¼�
*��������� ��
*����ֵ��0 �ɹ���-1 ʧ��
***************************************/
int saveEventFile(int ErcName,u8* data,long len, int time)
{
	int ValidFlag = 0;			//�費��Ҫ��¼
	int Priority = 0;			//��Ҫ�¼�����һ���¼�,��Ҫ�¼���Ҫ�ϱ�

	GetValidAndPriorityByErc(ErcName, &ValidFlag, &Priority);
		
	if(ValidFlag == 0)			//����Ҫ��¼�¼�
	{
		printf("saveEventFile %d is disabled\n",ErcName);
		//return -1;
	}

	int saveEventRes = 0;
	
	//���¼���һ���¼�
	if(Priority == OrdinaryEvent)
	{
		//ͬһ�����¼���������10������ɾ�������һ���¼�
		if (m_ErcNum[ErcName-1] >= MAX_NUM_EACH_ERC)
		{
			do
			{
				int old_cnt = m_ErcNum[ErcName-1];	
				DeleteordinaryEvent(ErcName);			//ɾ������һ���¼�
				GetordinaryEvent_Num(ErcName);			//��ȡ�¼�����
				
				if (m_ErcNum[ErcName-1] >= old_cnt)		//û�м���
				{
					printf("saveEventFile event %d DelOldestEventRecord Priority %d failed\n",ErcName,Priority);
					break;
				}
			}while(m_ErcNum[ErcName-1] >= MAX_NUM_EACH_ERC);
		}
	
		//����һ���¼�
		saveEventRes = saveEvent(Priority, ErcName, data, len, time, 0);

		//ɾ����Ҫ�¼��еĸ��¼�
		DeleteImportanceEvent(ErcName);	
		
	}	
	else	//��Ҫ�¼�
	{
		//ͬһ�����¼���������10������ɾ�������һ���¼�
		if (m_ErcNum[ErcName-1] >= MAX_NUM_EACH_ERC)
		{
			do
			{
				int old_cnt = m_ErcNum[ErcName-1];	
				DeleteImportanceEvent(ErcName);			//ɾ������һ���¼�
				GetImportanceEvent_Num(ErcName);		//��ȡ�¼�����
				
				if (m_ErcNum[ErcName-1] >= old_cnt)		//û�м���
				{
					printf("saveEventFile event %d DelOldestEventRecord Priority %d failed\n",ErcName,Priority);
					break;
				}
				
				
			}while(m_ErcNum[ErcName-1] >= MAX_NUM_EACH_ERC);
		}

		//������Ҫ�¼�
		saveEventRes = saveEvent(Priority, ErcName, data, len, time, 1);

		//ɾ��һ���¼��еĸ��¼�
		DeleteImportanceEvent(ErcName);	

		//���������ϱ���־λ��
		GetEventRow();	
	}
	
	printf("saveEventFile prio %d event %d saveEventRes %d\n",Priority,ErcName,saveEventRes);
	if (saveEventRes < 0)	
		return -1;
	
	//��¼�¼�����
	m_ErcNum[ErcName-1]++;
	
	//��¼�¼��Ƿ���
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
	//��ͨ
	m_EC2 = 0;
	DeleteordinaryEvent(0);
	writefile(ORDINARYFILEEC,   src, 1, "w+");

	//��Ҫ
	m_EC1 = 0;//�¼�״̬δ���
	DeleteImportanceEvent(0);
	writefile(IMPORTANTFILEEC,   src, 1,   "w+");

	//����¼��ϱ�״̬��־λ
	writefile(EVENTRECORDSTATUS, src, 80,   "w+");

	//����¼�����λ
	memset(m_ErcNum, 0, sizeof(m_ErcNum));
	
}



//��ȡʱ��
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

//��ȡ��
void GetSecond(int *second)
{
	time_t t;
	time(&t);	
	*second = (int)t;
}

//ERC1�����ݳ�ʼ���Ͱ汾���
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

//ERC2��������ʧ
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


//ERC3���������
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


//ERC14���ն�ͣ/�ϵ�
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



//ERC20����Ϣ��֤����
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



//ERC21��Ѳ���ǹ����¼�
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


//ERC32���ն�����վͨ������������
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


//ERC34��CT �쳣�¼���¼
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


//ERC40���ų��쳣�¼���¼
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


//ERC41����ʱ�¼���¼
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
*�������ƣ�parseEvent
*�����������������¼�������¼���¼�����Ƿ�һһ��Ӧ
*���������Num���Ѿ��������¼����, flag: 1ΪҪ�����ϱ���0���ϱ���ɡ�
*��������� ��
*����ֵ��0 �ɹ���-1 ʧ��
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
			//ͨ��ERC���������Ӧ�ĺ��� 
			//��һ���ֽ�owArrContent[i].data[0]Ϊ�¼����
			//�ڶ����ֽ�owArrContent[i].data[1]Ϊ�¼���¼����
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







