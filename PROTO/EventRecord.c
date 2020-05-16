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


//ȫ�ֱ���
u8 m_EC1;							//EC1��ֵ�������Ҫ�¼�û��д����ֵ����++���������255����һֱ�洢255 ��2009��12��15�գ�
u8 m_EC2;							//EC2��ֵ�������ͨ�¼�û��д����ֵ����++���������255����һֱ�洢255 ��2009��12��15�գ�
u8 m_ErcNum[MAX_ERC_NUM] = {0};		//ͬһ���¼�����������¼���10��
u8 m_ErcReport[EveMax] = {0};		//�����ϱ��¼���־	
char EventBuf[EveMax][528] = {0};	//��ȡ�¼��洢������


//������Ҫ�¼�������ֵ
int saveimportanceEC(u8 EC1)
{
	if(UpdateEC1(EC1) == -1)
	{
		return -1;
	}
	return 0;
}

//����һ���¼�������ֵ
int saveordinaryEC(int EC2)
{
	if(UpdateEC2(EC2) == -1)
	{
		return -1;
	}
	return 0;
}

//��ȡ��Ҫ�¼���������ֵ
int GetimportanceEC()
{
	return GetEC1();
}

//��ȡһ�������ֵ
int GetordinaryEC()
{
	return GetEC2();
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

	sTermSysInfo *pPara = GetCiSysInfo();
	
	*ValidFlag = (pPara->EvtRecoSet.CS_F9_EventRecordValidFlag[i] >> j)  & 0x01;		
	*Priority = (pPara->EvtRecoSet.CS_F9_EventRatingFlag[i] >> j)  & 0x01;
	for(i=0;i<8;i++)
	{
		printf("TermEventRecord.CS_F9_EventRecordValidFlag[%d] = %d\n",i, pPara->EvtRecoSet.CS_F9_EventRecordValidFlag[i]);
	}
	
	return 0;
}


//���¼�д���ļ�
//����Priority���¼��ȼ�  data���¼�����           Len�����ݳ���
int saveEvent(int Priority,u8 ERC ,const u8* data, int Len, u32 time, int report)
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
int saveEventFile(u8 ErcName,u8* data, int len, u32 time)
{
	int ValidFlag = 0;			//�費��Ҫ��¼
	int Priority = 0;			//��Ҫ�¼�����һ���¼�,��Ҫ�¼���Ҫ�ϱ�

	GetValidAndPriorityByErc(ErcName, &ValidFlag, &Priority);
		
	if(ValidFlag == 0)			//����Ҫ��¼�¼�
	{
		printf("saveEventFile %d is disabled\n",ErcName);
		return -1;
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

		//����һ���¼�������
		saveordinaryEC(m_EC2);

		//ɾ����Ҫ�¼��еĸ��¼�
		DelimpERC(ErcName);	
		
	}	
	else	//��Ҫ�¼�
	{
		//ͬһ�����¼���������10������ɾ�������һ���¼�
		if (m_ErcNum[ErcName-1] >= MAX_NUM_EACH_ERC)
		{
			do
			{
				printf("\n*********aaaaaaaaaaaaaaaaaaa\n\n");
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

		//������Ҫ�¼�������
		saveimportanceEC(m_EC1);

		//ɾ��һ���¼��еĸ��¼�
		DelordERC(ErcName);	

		//���������ϱ���־λ��
		//GetEventRow();	
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
 

//����¼���¼
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

//��ȡ��
void GetSecond(u32 *second)
{
	time_t t;
	time(&t);	
	*second = (u32)t;
}

//ERC1�����ݳ�ʼ���Ͱ汾���
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


//ERC14���ն�ͣ/�ϵ�
int CreateEvent14(stru_sjjl_ERC14 ev14)
{ 
	int len = 0;
	int second = 0;
	ev14.ERCcode=0x0E;	
	ev14.Le = 0x0B;			//Э����û�б�־λ��ʵ�ʶ�һ����־λ	
	
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
	ev34.Le = 0x08;		
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


//status:1��¼��2�˳���3����
int sendLinkPacktoMainStaion(int stauts, unsigned char *ucDatagram, int *length)
{
	stFrame13761Head *pAckFrame = (stFrame13761Head *)ucDatagram;
	stDataDI data_di;
	u16 pos = 0;
	u8 month;
	stTime sttime;
	
	//����Ӧ��֡
	//head1
	pAckFrame->header1 = FRAME_START_13761; 											// 0x68
	//len
	pAckFrame->len.len_un.len_s.m_pid = PROTOCOL_13761_VALID;   						// ��ʶ��Լ2
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
	pAckFrame->control.control_un.resp_s.acd = 0;										//���¼�����
	pAckFrame->control.control_un.resp_s.code = LINK_CTRL_UP_RES_NODATA;
	
	//addr
	memcpy((uint8_t*)&(pAckFrame->addr),(uint8_t*)&GetCiSysInfo()->usModleAddr,sizeof(MODLE_ADDR_STRU));
	memset(&pAckFrame->addr.mainStn_TermGrp_addr,0,sizeof(unFrame13761M_Ma_unType));//�ն���������֡
	
	//usr data
	pAckFrame->userdata.afn = AFN_02_LINK_TEST;
	pAckFrame->userdata.appseq.seq = 0;				//֡���Ϊ0
	pAckFrame->userdata.appseq.con = 1;				//��ҪӦ��
	pAckFrame->userdata.appseq.fin = 1;				
	pAckFrame->userdata.appseq.fir = 1;				//��֡
	pAckFrame->userdata.appseq.tpv = 0;				//û��ʱ���ǩ

	//DA,DT
	pos=0;
	setPn(0, &data_di.DA);
	setFn(stauts, &data_di.DT);
	memcpy(&(pAckFrame->userdata.data[0]), (uint8_t*)&(data_di), sizeof(stDataDI));
	pos += sizeof(stDataDI);// 4

	//3Ϊ����֡
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

			//��ȡ��ǰʱ��
		memcpy(&(pAckFrame->userdata.data[0])+pos, (u8 *)&sttime, sizeof(stTermTime));
		pos+=6;
	}
	
	pAckFrame->userdata.data[pos++] = GDW376_1_CalSum((uint8_t *)&(pAckFrame->control), pAckFrame->len.len_un.len_s.m_len);
	pAckFrame->userdata.data[pos++] = FRAME_END_13761;
	*length = sizeof(stFrame13761Head)+pos;
	
	//len =sizeof(stFrame13761Head)+pos;
	
	//��·ά��֡�����ط�
	return 0;							//Protocl13761SendData2(ACKbuf, len);
}


//�����ϱ���֡����
s32 report13761UPtoMainStaion(uint8_t afn, uint8_t *ACKdata, uint16_t ack_data_len, int fd)
{
	uint8_t ACKbuf[PROTOCOL_13761_BUF_SIZE] = {0};
	uint16_t pos = 0;
	static u8 pfc=0;
	StDataFormat16 DataFormat16={0};
	stFrame13761Head *pAckFrame =NULL;

	Eventdata *Event  = (Eventdata *)ACKdata;

	//֡���Ϊ��̬����
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

	//����Ӧ��֡
	pAckFrame->header1 = FRAME_START_13761;
	pAckFrame->header2 = FRAME_START_13761;
	//seq++;
	memcpy((uint8_t*)&(pAckFrame->addr),(uint8_t*)&GetCiSysInfo()->usModleAddr,sizeof(MODLE_ADDR_STRU));
	memset(&pAckFrame->addr.mainStn_TermGrp_addr,0,sizeof(unFrame13761M_Ma_unType));//�ն���������֡
	//pAckFrame->userdata.appseq.seq = seq&0xf;										//֡���	
	
	pAckFrame->userdata.appseq.seq = Event->seq& 0xf;								//֡���	
	pAckFrame->userdata.appseq.tpv = 1;												//�����ϱ���ʱ���ǩ										
	pAckFrame->userdata.appseq.fir = 1;
	pAckFrame->userdata.appseq.fin = 1;
	pAckFrame->userdata.appseq.con = 1;
	pAckFrame->userdata.afn = afn;
	
	pAckFrame->control.control_un.resp_s.dir = FRAME_13761_DIR_UP;
	pAckFrame->control.control_un.resp_s.prm = FRAME_13761_PRM_START_STATION;
	pAckFrame->control.control_un.resp_s.rsv = 0;
	pAckFrame->control.control_un.resp_s.acd = 1;									//���¼���־
	pAckFrame->control.control_un.resp_s.code = LINK_CTRL_UP_RES_USERDATA;
	
	pos = 0;
	memcpy(&(pAckFrame->userdata.data[pos]), Event->data, ack_data_len);
	pos += ack_data_len;
	
	if (pAckFrame->control.control_un.resp_s.acd == 1)	//������Ϣ���¼������� EC
	{
		AppAuxEC Ec = {0};
		//AppAuxEC;
		pAckFrame->userdata.data[pos++]=Ec.AppAuxEC1;
		pAckFrame->userdata.data[pos++]=Ec.AppAuxEC2;
	}
	
	if (pAckFrame->userdata.appseq.tpv ==1)				//������Ϣ��ʱ���ǩ Tp
	{
		AppAuxTp tp={0};
		
		tp.AppAuxTpPFC=pfc++;							//����֡֡��ż�����PFC��1
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
*�������ƣ�send_event
*��������������¼��ϱ�������֡�����
*�����������
*�����������
*����ֵ����
===============================================================*/
static u8 Seq = 0;					//�¼��ϱ�֡���
void send_event(int fd)
{
	u8 pAckBuf[1024] = {0};
	//�Ƿ����������ϱ�		  
	//05f29�����ն������ϱ�
	//05f37��ֹ�ն������ϱ�			

	//�Ƿ���������Ҫ�ϱ�
	GetEventRow();

	//�����ϱ�����
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

	
	

	//����ϱ���־λ
	int i;
	for(i=0; i<80; i++)
	{
		int sum = sizeof(head);
		//����Ҫ�����ϱ�
		if(m_ErcReport[i] == 1)
		{
			//�����ݿ��ȡ��Ҫ�¼�	
			printf("***shangbao*** = %d\n",i);
			int ret = GetImportanceEvent(i,i+1);
			if (ret < 0)		//ʧ��
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
			
			//�����ķ��뷢�Ͷ���	
			Eventdata Event = {0};
			memcpy(Event.data, pAckBuf, sum);
			Event.report_count = 0;
			Event.report_sucesss_flag = 0;	
			Event.datalen = sum;
			Event.seq = Seq++;							//֡���
			
			if(sequeue_input(queue, Event) == 0)		//��ӳɹ�
			{
				//���ϱ���־λ��0
				UpdateEventRow(i);	
			}	
		}	
	}

	//�����¼��ϱ�����
	for(i=0; i<queue->count; i++)
	{
		Eventdata popdata = {0};
		sequeue_output(queue, &popdata);
		
		if(popdata.report_sucesss_flag == 0 && popdata.report_count < 3)
		{
			//���͸���վ
			report13761UPtoMainStaion(0x0E, (u8 *)&popdata, popdata.datalen, fd);	
			
			//�������
			popdata.report_count++;
			sequeue_input(queue, popdata);	
		}			
	}	
}


//�¼���¼���ݳ�ʼ��
int EventDataInit()
{
	//ɾ�����ݿ��ļ�
	int ret = remove(FILENAME);
	if(ret == -1)
	{
		printf("Event Date Init fail\n");
		return -1;
	}
		
	//û�б�����д���
	ret = create_table();
	if(ret == -1)
	{
		printf("Event Date Init fail\n");
		return -1;
	}		
	return 0;
}

//�¼���¼��ʼ��
void EventInit()
{
	//û�б�����д���
	create_table();

	//��ȡһ���¼�������
	m_EC1 = GetimportanceEC();

	//��ȡ��Ҫ�¼�������
	m_EC2 = GetordinaryEC();

	//��ȡÿ��ERC��¼����
	int i = 0;
	for(i=0; i<MAX_ERC_NUM; i++)
	{
		GetImportanceEvent_Num(i+1);				//��ȡ��Ҫ�¼�����
		int Importantnum = m_ErcNum[i];
		
		GetordinaryEvent_Num(i+1);				   //��ȡһ���¼�����
		int Ordinarynum =  m_ErcNum[i];
		m_ErcNum[i] = (Importantnum > Ordinarynum ? Importantnum : Ordinarynum);
	}

}



/***********************����ͳ��*******************************/
long		  comm_read_flow_;			//�ۼ���վ�·�����
long		  comm_write_flow_; 		//�ۼ��ϴ��ֽ�����
long		  comm_curr_month_flow_;	//����������					�ܵ�����
long		  comm_last_day_flow_;		//����������					��һ�������
int 		  current_day_; 			//��ǰ��
int 		  current_month_;			//��ǰ�·�
int 		  current_year_;			//��ǰ���
int  flow_event_reported_; 	//����Ƶ�������������ޡ�1�����Դ����¼���0���������¼�

//int 		  f36_ytxllmx = 1024;//����������

long 		  to_read_;
long		  to_write_;


//����ͳ�����ݳ�ʼ��
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

/***********************��ȡ��������**************************************/
//�����������ж��Ƿ�����¼�
void dealFlow()
{
	long R_test = get_comm_read_flow_without_lock();				//��ȡ��վ�·������������·�������0
	long W_test = get_comm_write_flow_without_lock();				//��ȡ�ն��ϴ������������ϴ�������0
    int res_r = set_comm_read_flow(R_test);							//�·���������
	int res_w = set_comm_write_flow(W_test);						//�ϴ���������

    if(res_r == 4 || res_w == 4)//�������������¼�
	{
		stru_sjjl_ERC32 evt;
		u32  fluxlimit  = get_csf36_Month_Commflux_threshold();		//��ȡ��������
		u32  flux       = get_month_flows();						//��ȡ��ǰ����

		memcpy(evt.CommFluxLimit,&fluxlimit,4);			//��������
		memcpy(evt.CommFlux,&flux,4);					//��������	
		CreateEvent32(evt);								//�����¼���¼
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


//��ȡ��վ�·������������·�������0
long get_comm_read_flow_without_lock()
{
	return bytes_to_read();
}


//��ȡ�ն��ϴ������������ϴ�������0
long get_comm_write_flow_without_lock()
{
	return bytes_to_write();
}


//�·���������
int set_comm_read_flow(long value)
{
	int res   =  0;  
	comm_read_flow_ = comm_read_flow_ + value;

	//��ӵ���������
	res = set_month_flows_(value);  //���res = 4��ʱ�򣬱�ʾҪ�¼�
	return res;
}


//�ϴ���������
int set_comm_write_flow(long value)
{
	int res   =  0;
	comm_write_flow_ = comm_write_flow_ + value;

	//��ӵ���������
	res = set_month_flows_(value);		 //���res = 4��ʱ�򣬱�ʾҪ�¼�

	return res;
}

//���µ�����(�����·��������ϴ�����)
int set_month_flows_(long value)
{
	int res = 0;
	comm_curr_month_flow_  += value;
	 //�ж��Ƿ��������������޴���0�������
 	if(!flow_event_reported_ && GetCiSysInfo()->FluxLimitor > 0  && comm_curr_month_flow_ > GetCiSysInfo()->FluxLimitor)
	{
		res  = 4;
		flow_event_reported_ = 1;
	}
	return res;
}

////��ȡ��������
long get_csf36_Month_Commflux_threshold()
{
   return GetCiSysInfo()->FluxLimitor;
}

//��ȡ��ǰ����
long get_month_flows()
{
	return comm_curr_month_flow_;
}

//��������
long get_curr_day_flow()				//CTermParameters::
{
	long all_flow =  comm_write_flow_ + comm_read_flow_; 		//������
	
	return all_flow - comm_last_day_flow_;		//������-��������
}
/***********************��ȡ��������end**************************************/


/*************************�����������***************************************/
//���أ����ڳ�ʼ��
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

	//��ʼ��������
	load_comm_flow_month_without_lock();

	return 0;
}


//�����������ļ�	
int save_comm_flow(int check_new_day)
{
	if (save_comm_flow_month_without_lock() < 0)
		return -1;
	if (check_new_day && is_day_accross_without_lock())		//����µ�һ�죬�����Խ������
	{
		load_comm_flow_month_without_lock();
		save_comm_flow_month_without_lock();				//�������������������������������ϴ��������·�����
	}
	return 0;
}



//��������-��
int save_comm_flow_month_without_lock()
{
	int res   =  -1;
   	char c_temp[64];
	//������
	sprintf(c_temp,"flow_%02d_%d",current_month_, current_year_); 		//�����

    res =  saveconfparam(flow_path_, c_temp, comm_curr_month_flow_, ':');		//���浱������
    //������
	sprintf(c_temp,"flow_%02d_%02d_%d", current_day_, current_month_, current_year_); 	//������
	
	res =  saveconfparam(flow_path_, c_temp, comm_read_flow_ + comm_write_flow_, ':'); 			//����������
	
	save_comm_read_flow();			//�����·�����
	save_comm_write_flow();			//�����ϴ�����
	return res;
}


//�ж��Ƿ��ǿ�����
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


void load_comm_flow_month_without_lock()			//��������
{
	//��ǰ�µ�������0	
	comm_curr_month_flow_ = 0;	

	//��ǰ������Ϊ��д����֮��
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

	//�µ�һ�£���ʼ�ϱ�
	flow_event_reported_ = 0;									
	
	long temp  = 0;
	//���ܹ���ȡ����ǰ�£������µ�һ��
   	if (getconfparam(flow_path_, c_temp, &temp, ':')<0)	
   	{
   		saveconfparam(flow_path_, c_temp, temp, ':');
   	}

	//���������������µ�һ�¾�����
	comm_curr_month_flow_ = temp;	

	//
	t = time(NULL) - 24*3600;			
	localtime_r(&t, &ftnow);
	ftnow.tm_year = ftnow.tm_year + 1900;
	ftnow.tm_mon = ftnow.tm_mon + 1;
	sprintf(c_temp,"flow_%02d_%02d_%d", ftnow.tm_mday, ftnow.tm_mon, ftnow.tm_year);
	
	temp = comm_write_flow_ + comm_read_flow_;

	//���ܹ���ȡ����ģ������µ�һ��
	if( getconfparam(flow_path_, c_temp, &temp, ':') <0 )
	{
		saveconfparam(flow_path_, c_temp, temp, ':');
	}  

	//���µ�������
	comm_last_day_flow_ = temp;
}



//������վ�·��ֽ��������ļ�
int save_comm_read_flow()
{
   int res   =  -1;
   res =  saveconfparam(flow_path_, "COMM_READ_FLOW", comm_read_flow_,  ':');
   return res;

}

//�����ϱ���վ�������ļ�
int save_comm_write_flow()
{
   int res   =  -1;
   res =  saveconfparam(flow_path_,"COMM_WRITE_FLOW",comm_write_flow_, ':');			//���������·�������š�ֵ
   return res;
}


//����������ļ�
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


//���ļ���ȡ����
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

/*********************************�����������end****************************************/
#if 0
//ȡ�������·�����
long get_comm_read_flow()			//CTermParameters::
{
	return comm_read_flow_ ;
}

//ȡ�������ϱ�����
long get_comm_write_flow()			//CTermParameters::
{
	return comm_write_flow_;
}

//������()
long get_comm_flow()				//CTermParameters::
{
	long all_flow =  comm_write_flow_ + comm_read_flow_;
	return all_flow;
}

#endif











/********************************������ʧ�¼�***********************************/
//������ʧ�¼��Ƿ���
int CheckParaLostEvent(int Fn)				//int Fn
{
	stru_sjjl_ERC2 ev2;
	memset(&ev2, 0, sizeof(ev2));
	if (Fn == 1 || Fn == 3 || Fn == 7 || Fn == 8 || Fn == 9 || Fn == 10 || Fn == 16 || Fn == 89 || Fn == 91 ||  Fn == 153 || Fn == 154)
	{
		//�ն˲�����ʧ�¼� 
		ev2.LoseTerminal = 1;
		ev2.LosePot = 0;
		CreateEvent2(ev2);
	}
	else
	{
		return -1;
	}

/*	
	//��⵽�ն˲����ļ���ʧʱ�������¼���¼
	if(access(TERM3761_PARA_CONF,F_OK)==-1)			//TERM3761_PARA_CONF
	{
		stru_sjjl_ERC2 ev2;
		memset(&ev2,0,sizeof(ev2));
		ev2.LoseTerminal = 1;  //�ն˲�����ʧ
		
		//��¼�¼�
		CreateEvent2(ev2);
	}
*/
	return 0;
}


/***********************������ʼ���Ͱ汾���*********************/
#if 0
/*======================================================
*�������ƣ�IsTermVersionChange
*�����������ն˰汾�Ƿ���
*�����������
*�����������
*�� �� ֵ��0 �ɹ���-1 ʧ��
*======================================================*/
int IsTermVersionChange()
{ 
	int  i = 0, ret = -1;
	char buf[8] = {0}; 
	char path[1024] = {0}; 
	
	u8 NewTermSoftwareVersion[4], OldTermSoftwareVersion[4];

	//�汾·��
	snprintf(path, sizeof(path)-1, "%s", "terminal_version.conf");

	memset(buf, 0x0, sizeof(buf));  
	memset(NewTermSoftwareVersion, '0', 4);
	memset(OldTermSoftwareVersion, '0', 4);
	
	//���µ��ն�����汾�� 
	if (getConfPara(path, "TermSoftwareVersion", buf, sizeof(buf), ':') >= 0)
	{
		for (i=0;i<4;i++)
		{
			NewTermSoftwareVersion[i] = buf[i];
		} 
		//������汾��
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
			//�����汾����¼�
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

//���������ļ��е�����汾�Ų������ն˰汾����¼�
int WriteVersionFileAndCreateEvent(u8 *newVersion)
{
	if(newVersion == NULL)
		return -1;
	char buf[64] = {0};
	int i = 0;
	
	stru_sjjl_ERC1 event1;
	event1.VER = 1;
	
	//��ȡԭ�汾��
	if (getConfPara(TERM_SOFT_VERSION, "TermSoftwareVersion", buf, sizeof(buf), ':') >= 0 )
	{
		for (i=0;i<4;i++)
		{
			event1.VerOld[i] = buf[i];
		} 
	}

	//����ԭ�汾��
	if (saveConfPara(TERM_SOFT_VERSION, "OldSoftwareVersion", buf, ':') < 0)
	{
		return -1;
	}
	//д���°汾��
	snprintf(buf, 5, "%c%c%c%c", newVersion[0]/*-0x30*/, newVersion[1]/*-0x30*/, newVersion[2]/*-0x30*/, newVersion[3]/*-0x30*/);
	if (saveConfPara(TERM_SOFT_VERSION, "TermSoftwareVersion", buf, ':') < 0)		//д�����
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
*�������ƣ�GetMagnetic
*������������ȡ�ų���С
*�����������
*�����������
*�� �� ֵ��0 �ɹ���-1 ʧ��
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
*�������ƣ�GetACdate
*������������ȡ�������ݵ�ѹ����
*�����������
*�����������
*�� �� ֵ��0 �ɹ���-1 ʧ��
*======================================================*/
int GetACdate()
{
	if(com_645_2007_SendFrame(GET_JC_DATA, NULL) == -1)
		return -1;
	return 0;
}


//�����洢���ݳ�ʼ��
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


//�洢����ֵ�������㿪ʼÿ��15���Ӽ�¼һ��
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


/***************************ͣ�ϵ��¼�************************/
//ǰ�᣺������������(���ɽ�����ȷ������ģ�����׼ȷ)����Դģ��������������Դģ�鹤����Χ���ǲ���F98��ѹ��Χ
//��ѹ����125V��������¼� 
int check_power_off()
{
	//��ȡ��������
	//GetACdate();
	
	//����ͣ���¼�
	if(1)//if(ACdate.U[0] <  125 && ACdate.U[1] < 125 && ACdate.U[2] < 125)	
	{
		stru_sjjl_ERC14 ev14;
		memset(&ev14, 0, sizeof(ev14));
		ev14.EventEnable = 0x1;
		ev14.EventNormal = 0x1;
		
		//��ȡͣ��ʱ��
		GetTime(&ev14.offtime);
		u8 buf[5] = {0xEE, 0xEE, 0xEE, 0xEE, 0xEE};
		memcpy(&ev14.ontime, buf, 5);
		
		//��¼�¼�
		CreateEvent14(ev14);

		//�����ϱ�
		//send_event(0);
	}
	return 0;
}

int check_power_on()
{
	//��ȡ��������
	//GetACdate();
	
	//�����ϵ��¼�
	if(1)//(ACdate.U[0] >  125 || ACdate.U[1] > 125 || ACdate.U[2] < 125)	
	{	
		u8 buf[128] = {0};
		stru_sjjl_ERC14 ev14 = {0};
		//���ͣ���¼�
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
			//�����ϵ��¼�������ͣ���¼�

			//��ȡ�ϵ�ʱ��
			GetTime(&ev14.ontime);

			u8 *Event = (u8 *)&ev14;		
			char buf[1024] = {0};			
			for(i=0; i<sizeof(ev14); i++)
			{
				sprintf(&buf[i*2], "%02x", Event[i]);
			}	
			int second = 0;
			GetSecond(&second);
			
			//�����¼�
			UpdatePower(buf, second);
		}
	}
	return 0;
}



/*======================================================
*�������ƣ�CheckTAEvent
*�������������TAģ��������¼�
*�����������
*�����������
*�� �� ֵ��0 �ɹ���-1 ʧ��
*======================================================*/
static sFormat_TA_Status TaStatusBack;
int CheckTAEvent()
{
	sFormat_TA_Status TAStatus = GetTaStatus();

	stru_sjjl_ERC34 ev34;
	
	printf("************1111111***********\n\n");
	//���A��B��c����λ�·��·
	memset(&ev34, 0, sizeof(ev34));
	if(TaStatusBack.Phase_A_ststus != TAStatus.Phase_A_ststus)
	{
		if(TAStatus.Phase_A_ststus&0X01 == 1)
		{
			ev34.A = 1;
			ev34.extype = 0x01;  //01���β��·
		}
	}
	if(TaStatusBack.Phase_B_ststus != TAStatus.Phase_B_ststus)
	{
		if(TAStatus.Phase_B_ststus&0X01 == 1)
		{
			ev34.B = 1;
			ev34.extype = 0x01;  //01���β��·
		}
	}
	if(TaStatusBack.Phase_C_ststus != TAStatus.Phase_C_ststus)
	{
		if(TAStatus.Phase_C_ststus&0X01 == 1)
		{
			ev34.C = 1;
			ev34.extype = 0x01;  //01���β��·
		}
	}
	//�����ˣ���¼�¼�
	if(ev34.A == 1 || ev34.B == 1 || ev34.C == 1)
	{
		ev34.ss = 1;			//��ֹ��־
		ev34.pn = 1;			//�������
		CreateEvent34(ev34);	//��¼�¼�	
		printf("************222222***********\n\n");
	}
		
	
	//���A��B��C��CT��·
	memset(&ev34, 0, sizeof(ev34));
	if(TaStatusBack.Phase_A_ststus != TAStatus.Phase_A_ststus)
	{
		if(TAStatus.Phase_A_ststus&0X02 == 1)
		{
			ev34.A = 1;
			ev34.extype = 0x02; //02��·
		}
	}
	if(TaStatusBack.Phase_B_ststus != TAStatus.Phase_B_ststus)
	{
		if(TAStatus.Phase_B_ststus&0X02 == 1)
		{
			ev34.B = 1;
			ev34.extype = 0x02; //02��·
		}
	}
	if(TaStatusBack.Phase_C_ststus != TAStatus.Phase_C_ststus)
	{
		if(TAStatus.Phase_C_ststus&0X02 == 1)
		{
			ev34.C = 1;
			ev34.extype = 0x02; //02��·
		}
	}
	//�����ˣ���¼�¼�
	if(ev34.A == 1 || ev34.B == 1 || ev34.C == 1)
	{
		ev34.ss = 1;			//��ֹ��־
		ev34.pn = 1;			//�������
		CreateEvent34(ev34);	//��¼�¼�	
	}


	//���A��B��C��CT���δ���뵼��
	memset(&ev34, 0, sizeof(ev34));
	if(TaStatusBack.Phase_A_ststus != TAStatus.Phase_A_ststus)
	{
		if(TAStatus.Phase_A_ststus&0X04 == 1)
		{
			ev34.A = 1;
			ev34.extype = 0x06; //06��·���������豸
		}
	}
	if(TaStatusBack.Phase_B_ststus != TAStatus.Phase_B_ststus)
	{
		if(TAStatus.Phase_B_ststus&0X04 == 1)
		{
			ev34.B = 1;
			ev34.extype = 0x06; //06��·���������豸
		}
	}
	if(TaStatusBack.Phase_C_ststus != TAStatus.Phase_C_ststus)
	{
		if(TAStatus.Phase_C_ststus&0X04 == 1)
		{
			ev34.C = 1;
			ev34.extype = 0x06; //06��·���������豸
		}
	}
	//�����ˣ���¼�¼�
	if(ev34.A == 1 || ev34.B == 1 || ev34.C == 1)
	{
		ev34.ss = 1;			//��ֹ��־
		ev34.pn = 1;			//�������
		CreateEvent34(ev34);	//��¼�¼�	
		printf("************3333***********\n\n");
	}


	//���A��B��C��CT�ָ�����
	memset(&ev34, 0, sizeof(ev34));
	if(TaStatusBack.Phase_A_ststus != TAStatus.Phase_A_ststus)
	{
		if(TAStatus.Phase_A_ststus == 0x00)
		{
			ev34.A = 1;
			ev34.extype = 0x00; //00�ָ�����
		}
	}
	if(TaStatusBack.Phase_B_ststus != TAStatus.Phase_B_ststus)
	{
		if(TAStatus.Phase_B_ststus == 0x00)
		{
			ev34.B = 1;
			ev34.extype = 0x00; //00�ָ�����
		}
	}
	if(TaStatusBack.Phase_C_ststus != TAStatus.Phase_C_ststus)
	{
		if(TAStatus.Phase_C_ststus == 0x00)
		{
			ev34.C = 1;
			ev34.extype = 0x00; //00�ָ�����
		}
	}
	//�����ˣ���¼�¼�
	if(ev34.A == 1 || ev34.B == 1 || ev34.C == 1)
	{
		ev34.ss = 0;			//��ֹ��־
		ev34.pn = 1;			//�������
		CreateEvent34(ev34);	//��¼�¼�	
		printf("************4444***********\n\n");
	}


	//���һ�η����¼������С�� 500:5��ABC ������ε������� 0 ʱ�����¼���¼ 
	memset(&ev34, 0, sizeof(ev34));
	if(TaStatusBack.Phase_A_ststus != TAStatus.Phase_A_ststus)
	{
		if(TAStatus.Phase_A_ststus&0X08 == 1)
		{
			ev34.A = 1;
			ev34.extype = 0x03; //һ�η���
		}
	}
	if(TaStatusBack.Phase_B_ststus != TAStatus.Phase_B_ststus)
	{
		if(TAStatus.Phase_B_ststus&0X08 == 0x08)
		{
			ev34.B = 1;
			ev34.extype = 0x03; //һ�η���
		}
	}
	if(TaStatusBack.Phase_C_ststus != TAStatus.Phase_C_ststus)
	{
		if(TAStatus.Phase_C_ststus&0X08 == 1)
		{
			ev34.C = 1;
			ev34.extype = 0x03; //һ�η���
		}
	}
	//�����ˣ���¼�¼�
	if(ev34.A == 1 || ev34.B == 1 || ev34.C == 1)
	{
		ev34.ss = 1;			//��ֹ��־
		ev34.pn = 1;			//�������
		CreateEvent34(ev34);	//��¼�¼�	
		printf("************5555***********\n\n");
	}
		
	return 0;
}





//�������ܣ���Ѳ�����ڲ��ļ���ȡ�ź�ǿ��ֵ����ת��Ϊ�ź�Ʒ��ֵ���ظ�Process_F15 by:wrh
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
	printf("file open success!\n");								//�ļ��򿪳ɹ�
	//LOG_ERROR((LM_ERROR,ACE_TEXT("file open success!\n")));	//�ļ��򿪳ɹ�
    if(NULL == fd)    //�ļ�������
    {
    printf("file doesn't exit!\n");//�ļ�������
        return 0;
    }
    else
    {
        fread(gprsbuf,sizeof(gprsbuf),1,fd);
	    printf("file read success!\n");								//���ļ��ɹ�
	    printf("%s\n",gprsbuf);										//�鿴��ȡ�ַ���������
	    //LOG_ERROR((LM_ERROR,ACE_TEXT("file read success!\n")));	//���ļ��ɹ�
        fclose(fd);
	    fd=NULL;
	    printf("file close success!\n");							//�ر��ļ��ɹ�
    }

    if(pos = strstr(gprsbuf,"rssi="))
    {
        printf("get rssi success!pos=%s\n",pos);				//��ȡ�ź�ǿ�ȳɹ�
        pos += 5;
		printf("pos+5=%s\n",pos);								//�鿴pos+5��ֵ
        signal = strtol(pos,NULL,10);
		printf("string to long success!\n");					//ת��Ϊ���ͳɹ�
		printf("signal=%d.\n",signal);							//ת��Ϊ���ͳɹ�
       for (i = 0; i < sizeof(SignalStrengthArray) / sizeof(SignalStrengthArray[0]); i++)
	    {
		if (signal ==SignalStrengthArray[i])
		    {
		printf("signal match Arraymember success!SignalStrengthArray=%d,SignalArray=%d\n",SignalStrengthArray[i],SignalArray[i]);//�ź���������ƥ�䵽��Ӧ��Ԫ��
		    //SignalArray[i]-=800;								//�ź�Ʒ��С��0�����ٷ�λ�����λ��ֵΪ1
		printf("SignalArray=%d\n",SignalArray[i]);
		    return SignalArray[i];
		printf("return signal data success!\n");				//�����ź����ݳɹ�
		    }
		else 
			continue;
        }
    }
    return 0;
}


/*
//F15���ն������ź�Ʒ�� by:wrh			������ʹ�õ�������һ�����ݵ�F15����չ��Э��
int Process_F15(int FN,AutoRef<Process_698info>&P_698_info)
 {
    void *fenetgy=NULL;//fenetgy��ʼ��Ϊ��
    int a,b,c,d,e;

//�鳤
    if(FN==15)
      {
       printf("enter F15 success!\n");
	   P_698_info->replyblocklen698=sizeof(StDataFormat5);//��ȡ���ݸ�ʽ05���ڴ��С����ΪӦ�����ݵ�Ԫ���ݿ鳤��
       printf("F15 get replyblocklen698 success!\n");
      }
    else
	   return -1;
//����698Ӧ��֡
if(P_698_info->CheckWhetherNew698ReplyFrame(P_698_info->frm698->getEC1(),
P_698_info->frm698->getEC2(),P_698_info->replyblocklen698 + sizeof(DataId698),
false,P_698_info->is_last_di)<0)
   {
    //error
    MY_ACE_ERROR((LM_ERROR,ACE_TEXT("698 realtime read reply new frame faile fn %d pn %d.\n ,"),FN,P_698_info->MPno));
    return -1;
   }  
//��ӵ���ǰ
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
		  printf("F15 get signal data success!\n");//��ȡ�ź����ݳɹ�
	}   
	else
		return -1;
	P_698_info->replycurpos698 += P_698_info->replyblocklen698;//replycurpos698Ӧ��֡����дλ��

	
	P_698_info->replyblocklen698 += sizeof(DataId698); //DataId698���ݵ�Ԫ��ʶ����
		return 0;
}
*/




//���ļ���ȡ�ն˰汾��Ϣ
int GetTermVersionInfo(stTermVerInfo *termVerInfo)
{ 
	char buf[1024] = {0};
	int i = 0;

	memset(buf, 0x0, sizeof(buf));
	memset(termVerInfo, 0, sizeof(stTermVerInfo));
	
	//���̴��� 
	if(getConfPara(TERM_SOFT_VERSION, "ManufacturerCode", buf, sizeof(buf), ':') >= 0)
	{
		for (i=0;i<4;i++)
		{
			termVerInfo->factry_code[i] = buf[i];
		} 
	} 
	
	//�豸��� 
	if(getConfPara(TERM_SOFT_VERSION, "DeviceSerialNo", buf, sizeof(buf), ':') >= 0)
	{
		for (i=0;i<8;i++)
		{
			termVerInfo->term_number[i] = buf[i];
		} 
	} 

	//�ն�����汾�� 
	if (getConfPara(TERM_SOFT_VERSION, "TermSoftwareVersion", buf, sizeof(buf), ':') >= 0)
	{
		for (i=0;i<4;i++)
		{
			termVerInfo->soft_ver[i] = buf[i];
		} 
	}
	//�ն������������
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
	
	//�ն�����������Ϣ�� 
	if (getConfPara(TERM_SOFT_VERSION, "TermConfCapacityInfoCode", buf, sizeof(buf), ':') >= 0)
	{
		for (i=0;i<11;i++)
		{
			termVerInfo->capacity_info[i] = buf[i];
		}
	}
	//�ն�ͨ��Э��汾��
	if (getConfPara(TERM_SOFT_VERSION, "TermCommProtocolVersion", buf, sizeof(buf), ':') >= 0)
	{
		for (i=0;i<4;i++)
		{
			termVerInfo->Protocol_version[i] = buf[i];
		}
	}
	//�ն�Ӳ���汾�� 
	if (getConfPara(TERM_SOFT_VERSION, "TermHardwareVersion", buf, sizeof(buf), ':') >= 0)
	{
		for (i=0;i<4;i++)
		{
			termVerInfo->hardware_ver[i] = buf[i];
		}
	}
	//�ն�Ӳ���������� 
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




//���ļ���ȡԶ��ģ��汾��Ϣ
int GetRemoteComModalVerInfo(RemoteVerInfo *remoteModalVerInfo)
{ 
	char buf[1024] = {0};
	int i = 0;

	memset(buf, 0x0, sizeof(buf));
	memset(remoteModalVerInfo, 0, sizeof(RemoteVerInfo));
	
	//���̴��� 
	if (getConfPara(REMOTE_MODULE_CONF_PATH, "ManufacturerCode", buf, sizeof(buf), ':') >= 0)
	{
		for (i=0;i<4;i++)
		{
			remoteModalVerInfo->factry_code[i] = buf[i];
		} 
	} 
	
	//ģ���ͺ� 
	if (getConfPara(REMOTE_MODULE_CONF_PATH, "ModuleNo", buf, sizeof(buf), ':') >= 0)
	{
		for (i=0;i<8;i++)
		{
			remoteModalVerInfo->module_type[i] = buf[i];
		} 
	} 

	//����汾��  
	if (getConfPara(REMOTE_MODULE_CONF_PATH, "ModuleSoftwareVersion", buf, sizeof(buf), ':') >= 0)
	{
		for (i=0;i<4;i++)
		{
			remoteModalVerInfo->soft_ver[i] = buf[i];
		} 
	}
	
	//�����������
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


	//Ӳ���汾�� 
	if (getConfPara(REMOTE_MODULE_CONF_PATH, "TermHardwareVersion", buf, sizeof(buf), ':') >= 0)
	{
		for (i=0;i<4;i++)
		{
			remoteModalVerInfo->hardware_ver[i] = buf[i];
		}
	}
	
	//Ӳ����������
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

	//SIM��ICCID
	if (getConfPara(REMOTE_MODULE_CONF_PATH, "SIMCardICCID", buf, sizeof(buf), ':') >= 0)
	{
		for (i=0;i<20;i++)
		{
			remoteModalVerInfo->ICCID[i] = buf[i];
		}
	}
	return 0;
}



//����¼��Ķ�ʱ���� �ų��¼���ͣ�ϵ��¼��������������¼���TA�¼���������ʧ�������¼������ض��ļ��
static void EvevtCallBack(void * data)
{
	//�����������¼�
	//��������
	dealFlow();
	
	//�洢����
	save_comm_flow(1);

	//ͣ���¼�
	//check_power_off();

	//TA�¼�
	//CheckTAEvent();
	
	//�ų��¼�


	//������ʧ
	//CheckParaLostEvent();
}
int check_evevt_time()
{
	add_timer(2, 6, EvevtCallBack, NULL, NULL);

	//���������ϵ��¼����
	check_power_on();	
	return 0;
}


static void SendEvevtCallBack(void * data)
{
	send_event(0);		//��Ҫ�ļ�������
}
//�����ϱ��Ķ�ʱ��
int send_event_time()
{
	add_timer(2, 10, SendEvevtCallBack, NULL, NULL);
}


