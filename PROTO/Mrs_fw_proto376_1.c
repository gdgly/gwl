#include "Mrs_fw_proto376_1.h"
#include "file.h"
#include "Tools.h"
#include "appdata.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "queue.h"
#include "local_fun.h"
#include "ListTimer.h"
#include "para.h"
#include "GetTerminalESAMData.h"
#include "MESamComMgr.h"
#include <time.h>




CIRCUIT_SYSTEM_INFO_STRU gChargeSystemInfo={0};			//�豸��Ϣ�򣬰����߼���ַ
TermEventRecordSet TermEventRecord = {0};				//F9�ն��¼���¼��������
stUpflag Upflag={0};									//��¼�����ظ���־��ȫ������


/***************************************
  * @brief   376.1Э�����У���
  * @param [in] pBuf��������ָ��
  * @param [in] len������������
  * @retval  u8��У���
  *************************************/
u8 GDW376_1_CalSum(u8* pBuf, u16 len)
{
	register u16 i;
	register u8 Sum = 0;
	
	if(pBuf == NULL)
	{
		return 0;
	}
	
	for(i = 0; i < len; i++)
	{
		Sum += *pBuf++;
	}
	return(Sum);
}

/**************************************
  * @brief   ���376.1��Լ����֡
  * @param [in] pBuf��������ָ��
  * @param [in] len�����ݳ���
  * @retval  u8*���޺Ϸ�֡�򷵻�NULL
  *************************************/
u8* GDW376_1_CheckOutFrm2(u8* pBuf, u16 len)
{
	u16 i;
	u8* p;
	

	if(pBuf == NULL)
		return(NULL);
	if(len < 6)
		return(NULL);
	for(i = 0; i < len - 6; i++)
	{
		p = pBuf + i;
		if(*p != 0x68)
			continue;
		memcpy((u8*)&len,p+1,2);
		len>>=2;
		if(((len) < (1+ 5 + 1 + 1 + 1 + 1)) \
		   || (len > (FRAME_MAX_SIZE - 8)))
			continue;
		if(*(p+len+1+2+2+1+1+1-1) != 0x16)
			continue;
		if(*(p+len+1+2+2+1+1+1-2) == GDW376_1_CalSum(p + 6, len))
			return(p);
	}
	return(NULL);
}

/*****************************************************************************************
��������:   isRight13761Frame(uint8_t *pBuf, uint16_t *pBufSize)
����˵��:   1376.1֡�жϣ���ȡ����������Ч��֡,��Ч֡�洢�ڻ�����[0]��ʼ
�������:	
�������:	��Ч����֡����
����˵��: 
*****************************************************************************************/
uint16_t isRight13761Frame(uint8_t *pBuf, uint16_t* len)
{
	stFrame13761Head *pFrame;
	uint8_t checkSum;
	uint16_t frameSize = 0;
	uint16_t i = 0;
	uint16_t data_len = *len;
	
	if ((pBuf == NULL) || (data_len == 0))
	{
		return 0;
	}
	if (data_len > PROTOCOL_13761_BUF_SIZE) //1200
	{
		data_len = PROTOCOL_13761_BUF_SIZE;
	}
	for (i = 0; i < data_len; ++i)
	{
		pFrame = (stFrame13761Head *)&pBuf[i];
		if(pFrame->header1 != FRAME_START_13761)    //0x68
		{
			continue;
		}
		if (data_len - i < PROTOCOL_13761_MIN_FRAME_LEN)	//����̫�� 20
		{
			break;
		}
		if (pFrame->header2 != FRAME_START_13761)
		{
			continue;
		}
		if(pFrame->len.len_un.value != pFrame->len1.len_un.value)
		{
			continue;
		}
		if(pFrame->len.len_un.len_s.m_pid != PROTOCOL_13761_VALID)  // 2
		{
			continue;
		}
		if(pFrame->len.len_un.len_s.m_len > PROTOCOL_13761_MAX__DATA_LEN)
		{
			continue;
		}

		frameSize = pFrame->len.len_un.len_s.m_len + PROTOCOL_13761_CTR_SEQ_LEN;
		checkSum = CheckSum13761((uint8_t *)&(pFrame->control), pFrame->len.len_un.len_s.m_len);
		if(checkSum != pBuf[i + frameSize - sizeof(stFrame13761Tail)])
		{
 			continue;
		}
		if(pBuf[i + frameSize - 1] != FRAME_END_13761)
		{
 			continue;
		}
		break;		//�ҵ����ʵ�֡
	}
	
	if (i > 0)	//֡ǰ��
	{
		data_len -= i;
		memmove( pBuf, &pBuf[ i ], data_len );
	}
	*len = data_len;
		
	return frameSize;
}

/*****************************************************************************************
��������:      CheckSum13761
����˵��:      ����checksum ֵ
�������:
�������:
����˵��:
*****************************************************************************************/
uint8_t CheckSum13761(uint8_t *pBuf, uint16_t size)
{
	uint8_t chkSum = 0;
	
	while(size--)
	{
		chkSum += *pBuf;
		pBuf++;
	}
	return chkSum;
}

/*****************************************************************************************
��������:   getFrame13761Tail
����˵��:   ��ȡ����֡֡β
�������:	pBuf, len
�������:	
����ֵ��	β������ָ��
����˵��:	
*****************************************************************************************/
stFrame13761Tail* getFrame13761Tail(uint8_t *pBuf, uint16_t len)
{
	stFrame13761Head *pFrame = (stFrame13761Head *)pBuf;
	if ((pBuf == NULL) || (len == 0))
	{
		return NULL;
	}
	
	return (stFrame13761Tail*)&pBuf[(PROTOCOL_13761_HEAD_LEN + pFrame->len.len_un.len_s.m_len )];
}

/*****************************************************************************************
��������:   getCmdDataEnd
����˵��:   ��ȡ����֡Ӧ�ò�����β�����ݣ�������ڸ�����Ϣ��
			�򷵻ظ�����Ϣ��ʼ�Ĳ��֣�û���򷵻�֡β����
�������:	pBuf, len
�������:	
����ֵ��	β������ָ��
����˵��:	
*****************************************************************************************/
uint8_t* getCmdDataEnd(uint8_t *pBuf, uint16_t len)
{
	stFrame13761Head *pFrame = (stFrame13761Head *)pBuf;
	uint8_t* data_end = NULL;
	if ((pBuf == NULL) || (len == 0))
	{
		return NULL;
	}


	
	if (pFrame->control.control_un.act_s.prm == FRAME_13761_PRM_ACK_STATION)	//������֡
	{
		return NULL;
	}
	if (pFrame->control.control_un.act_s.dir != FRAME_13761_DIR_UP)
	{
		data_end = (uint8_t*)getAppAuxPwdDown(pBuf, len);
		if (!data_end)
			data_end = (uint8_t*)getAppAuxDownTp(pBuf, len);
	}
	else
	{
		data_end = (uint8_t*)getAppAuxUpEC(pBuf, len);
		if (!data_end)
			data_end = (uint8_t*)getAppAuxUpTp(pBuf, len);
	}
	if (!data_end)
	{
		data_end = (uint8_t*)getFrame13761Tail(pBuf, len);
	}
	
	return data_end;
}

/*****************************************************************************************
��������:   isAuxWithPwd
����˵��:   �ж��Ƿ����PWD��
�������:	AFN
�������:	
����ֵ��	0-�����ڣ�1-����
����˵��:	
*****************************************************************************************/
uint8_t isAuxWithPwd(uint8_t AFN)
{
	uint8_t ret = 0;
	switch(AFN)
	{
//		case AFN_00_CONFIRM:			//00H	ȷ�ϨM����
		case AFN_01_RESET:				//01H	��λ
//		case AFN_02_LINK_TEST:			//02H	��·�ӿڼ��
//		case AFN_03_RELAYSTATION:		//03H	�м�վ����
		case AFN_04_SETUP_PARM:			//04H	���ò���
		case AFN_05_CONTROL:			//05H	��������
		case AFN_06_AUTHENTICATION:		//06H	�����֤����ԿЭ��
//		case AFN_08_LEVEL_CHECK:		//08H	�������ն������ϱ�
//		case AFN_09_TMCONFIG:			//09H	�����ն�����
//		case AFN_0A_INQUIRY_PARM:		//0AH	��ѯ����
//		case AFN_0B_TASK_DATA:			//0BH	������������
//		case AFN_0C_REALTIME_DATA:		//0CH	����1�����ݣ�ʵʱ���ݣ�
//		case AFN_0D_HISTORY_DATA:		//0DH	����2�����ݣ���ʷ���ݣ�
//		case AFN_0E_EVENT_DATA:			//0EH	����3�����ݣ��¼����ݣ�
		case AFN_0F_FILE_TRANSFER:		//0FH	�ļ�����
		case AFN_10_DATA_TRANSMIT:		//10H	����ת��
			ret = 1;
			break;
		default:
			ret = 0;
			break;
	}
	return ret;
}

/*****************************************************************************************
��������:   getAppAuxPwdDown
����˵��:   ��ȡ���루���У�
�������:	pBuf, len
�������:	
����ֵ��	PWD����ָ��
����˵��:	
*****************************************************************************************/
AppAuxPwd* getAppAuxPwdDown(uint8_t *pBuf, uint16_t len)
{
	stFrame13761Head *pFrame = (stFrame13761Head *)pBuf;
	if ((pBuf == NULL) || (len == 0))
	{
		return NULL;
	}

	
	if (pFrame->control.control_un.act_s.prm == FRAME_13761_PRM_ACK_STATION ||
		pFrame->userdata.afn == AFN_00_CONFIRM ||
		pFrame->control.control_un.act_s.dir == FRAME_13761_DIR_UP
	    )
	{
		return NULL;
	}	
	if (isAuxWithPwd(pFrame->userdata.afn) == 0)	//������PWD
	{
		return NULL;
	}
	if (pFrame->userdata.appseq.tpv == 0)
	{
		return (AppAuxPwd*)(&pFrame->control + pFrame->len.len_un.len_s.m_len - sizeof(AppAuxPwd));
	}
	else if(pFrame->len.len_un.len_s.m_len - sizeof(stFrame13761CtrlType) - sizeof(stFrame13761AddrType) < sizeof(AppAuxPwd))
	{
		return NULL;
	}
	else
	{
		return (AppAuxPwd*)(&pFrame->control + pFrame->len.len_un.len_s.m_len - sizeof(AppAuxPwd)- sizeof(AppAuxTp));//- sizeof(AppAuxTp) add by phh
	}	
}



/*****************************************************************************************
��������:   getAppAuxDownTp
����˵��:   ��ȡʱ���ǩ�����У�
�������:	pBuf, len
�������:	
����ֵ��	Tp����ָ��
����˵��:	
*****************************************************************************************/
AppAuxTp* getAppAuxDownTp(uint8_t *pBuf, uint16_t len)
{
	stFrame13761Head *pFrame = (stFrame13761Head *)pBuf;
	if ((pBuf == NULL) || (len == 0))
	{
		return NULL;
	}

	
	if (pFrame->control.control_un.act_s.dir == FRAME_13761_DIR_UP)
	{
		return NULL;
	}
	if (pFrame->userdata.appseq.tpv == 0)
	{
		return NULL;
	}
	if (pFrame->control.control_un.act_s.prm == FRAME_13761_PRM_ACK_STATION)
	{
		if (pFrame->len.len_un.len_s.m_len - sizeof(stFrame13761CtrlType) - sizeof(stFrame13761AddrType) < sizeof(AppAuxTp))
		{
			return NULL;
		}
	}
	else
	{
		if (isAuxWithPwd(pFrame->userdata.afn) == 0)	//������PWD
		{
			if (pFrame->len.len_un.len_s.m_len - sizeof(stFrame13761CtrlType) - sizeof(stFrame13761AddrType) < sizeof(AppAuxTp) + sizeof(stDataDI))
			{
				return NULL;
			}
		}
		else
		{
			if (pFrame->len.len_un.len_s.m_len - sizeof(stFrame13761CtrlType) - sizeof(stFrame13761AddrType) < sizeof(AppAuxDown) + sizeof(stDataDI))
			{
				return NULL;
			}
		}
	}
	return (AppAuxTp*)(&pFrame->control + pFrame->len.len_un.len_s.m_len - sizeof(AppAuxTp));	
}



/*****************************************************************************************
��������:   getAppAuxUpEC
����˵��:   ��ȡ�¼������������У�
�������:	pBuf, len
�������:	
����ֵ��	EC����ָ��
����˵��:	
*****************************************************************************************/
AppAuxEC* getAppAuxUpEC(uint8_t *pBuf, uint16_t len)
{
	stFrame13761Head *pFrame = (stFrame13761Head *)pBuf;
	if ((pBuf == NULL) || (len == 0))
	{
		return NULL;
	}
	
	
	if (pFrame->control.control_un.act_s.dir != FRAME_13761_DIR_UP ||
	    pFrame->control.control_un.act_s.fcb == 0 ||
	    (pFrame->len.len_un.len_s.m_len - sizeof(stFrame13761CtrlType) - sizeof(stFrame13761AddrType) < sizeof(AppAuxEC) + sizeof(stDataDI))
	   )
	{
		return NULL;
	}
	if (pFrame->userdata.appseq.tpv == 0)	 //ֻ��ת������û��AppSeq,����EC/Tp	//֡������SEQ
	{
		return (AppAuxEC*)(&pFrame->control + pFrame->len.len_un.len_s.m_len - sizeof(AppAuxEC));
	}
	else if (pFrame->len.len_un.len_s.m_len - sizeof(stFrame13761CtrlType) - sizeof(stFrame13761AddrType) < sizeof(AppAuxUp))
		return NULL;
	else
		return (AppAuxEC*)(&pFrame->control + pFrame->len.len_un.len_s.m_len - sizeof(AppAuxUp));
}


/*****************************************************************************************
��������:   getAppAuxUpTp
����˵��:   ��ȡʱ���ǩ�����У�
�������:	pBuf, len
�������:	
����ֵ��	Tp����ָ��
����˵��:	
*****************************************************************************************/
AppAuxTp* getAppAuxUpTp(uint8_t *pBuf, uint16_t len)
{
	stFrame13761Head *pFrame = (stFrame13761Head *)pBuf;
	if ((pBuf == NULL) || (len == 0))
	{
		return NULL;
	}	
	
	
	if (pFrame->control.control_un.act_s.dir != FRAME_13761_DIR_UP ||
	    pFrame->userdata.appseq.tpv == 0		//ֻ��ת������û��AppSeq,����EC/Tp
	   )
	{
		return NULL;
	}
	if (pFrame->control.control_un.act_s.fcb)
	{
		if (pFrame->len.len_un.len_s.m_len - sizeof(stFrame13761CtrlType) - sizeof(stFrame13761AddrType) < sizeof(AppAuxUp)+ sizeof(stDataDI))
			return NULL;
	}
	else
	{
		if (pFrame->len.len_un.len_s.m_len - sizeof(stFrame13761CtrlType) - sizeof(stFrame13761AddrType) < sizeof(AppAuxTp)+ sizeof(stDataDI))
			return NULL;
	}
	return (AppAuxTp*)(&pFrame->control + pFrame->len.len_un.len_s.m_len - sizeof(AppAuxTp));
}


#define TERMUPCOMMUPARA    "./config/TermUpCommuPara.conf"
int getTermUpCommuPara(stTermUpCommuPara *TermUpCommuPara)
{
	char TERMUPPARA[256];
	char *pos = NULL;
	FILE *fd;
    
	memset(TERMUPPARA, 0, sizeof(TERMUPPARA));
	fd = fopen(TERMUPCOMMUPARA,"r");    
	if(fd<=0)    //�ļ������ڣ���ʼ��
	{
		fd = fopen(TERMUPCOMMUPARA,"w+");
		if(fd<=0)
		{
			printf("Creat TermUpCommuPara.conf failed\n");
			//return baudrate;
			return -1;
		}   
		TermUpCommuPara->RTS = 35;
		TermUpCommuPara->delay_time = 0;
		TermUpCommuPara->u_over_time_times.over_time_times = 0;
		TermUpCommuPara->confirm = 0; 
		TermUpCommuPara->heart_beats =2;
		sprintf(TERMUPPARA, "RTS:%d,DELAY:%d,over:%d,confirm:%d,heart:%d", TermUpCommuPara->RTS, TermUpCommuPara->delay_time, TermUpCommuPara->u_over_time_times.over_time_times, TermUpCommuPara->confirm, TermUpCommuPara->heart_beats);
		fwrite((char *)&TERMUPPARA, strlen(TERMUPPARA), 1,  fd);
		printf("Creat and Init ./conf/TermUpCommuPara.conf success\n");
		fclose(fd);
	}
	else
    {
		fread(TERMUPPARA,sizeof(TERMUPPARA)-1,1,fd);
		fclose(fd);
		printf("Open ./config/TermUpCommuPara.conf ok\n");
		
		if(NULL != (pos=strstr((const char *)TERMUPPARA,"RTS:")))  //�ҵ���ֵ����ȡ
		{
			pos += 4;
			TermUpCommuPara->RTS = strtol(pos,NULL,10);
		}

		if(NULL != (pos=strstr((const char *)TERMUPPARA,"DELAY:")))  //�ҵ���ֵ����ȡ
		{
			pos += 6;
			TermUpCommuPara->delay_time = strtol(pos,NULL,10);
		}

		if(NULL != (pos=strstr((const char *)TERMUPPARA,"over:")))  //�ҵ���ֵ����ȡ
		{
			pos += 5;
			TermUpCommuPara->u_over_time_times.over_time_times = strtol(pos,NULL,10);
		}
		
		if(NULL != (pos=strstr((const char *)TERMUPPARA,"confirm:")))  //�ҵ���ֵ����ȡ
		{
			pos += 8;
			TermUpCommuPara->confirm = strtol(pos,NULL,10);
		}

		if(NULL != (pos=strstr((const char *)TERMUPPARA,"heart:")))  //�ҵ���ֵ����ȡ
		{
			pos += 6;
			TermUpCommuPara->heart_beats = strtol(pos,NULL,10);
		}		
	}
    return 0;
}

int SetTermUpCommuPara(stTermUpCommuPara *TermUpCommuPara)
{
	unlink(TERMUPCOMMUPARA);    //./config/TermUpCommuPara.conf  ɾ���ļ�
	char TERMUPPARA[256];
	FILE *fd;

	memset(TERMUPPARA, 0, sizeof(TERMUPPARA));
	sprintf(TERMUPPARA, "RTS:%d,DELAY:%d,over:%d,confirm:%d,heart:%d", TermUpCommuPara->RTS, TermUpCommuPara->delay_time, TermUpCommuPara->u_over_time_times.over_time_times, TermUpCommuPara->confirm, TermUpCommuPara->heart_beats);
	fd = fopen(TERMUPCOMMUPARA,"a+");
	if(fd<=0)
	{
		return -1;
	}
	fwrite(TERMUPPARA,1,strlen(TERMUPPARA),fd);
	fclose(fd);
    return 0;
}



//�յ�ȷ�ϡ����ϱ���
static uint8_t AFN_00H(uint8_t *pBuf, uint16_t len)
{
	stFrame13761Head *pFrame;
	uint8_t nRet = 0;
	uint8_t Fn = 0;
	uint16_t Pn = 0;
	int i = 0;

	stDataDI* di=NULL;
	stError *Err=NULL;
	u8 heatAck=0;

	
	if ((pBuf == NULL) || (len == 0))
	{
		return 0;
	}
	pFrame = (stFrame13761Head *)pBuf;

	di = (stDataDI*)(pFrame->userdata.data);
	getFn(di->DT, &Fn);
	getPn(di->DA, &Pn);

	if(Fn == 0x01)
	{
		printf("quanbuqueren\n\n");
	}

	//F03�����ݵ�Ԫ��ʶȷ�ϡ�����
	if(Fn == 0x03)
	{
		Err=(stError*)&di[1];

		//��·�ӿڼ��ȷ��֡
		if(Err->afn==0x02)
		{		
			//02 f1(��¼)��ȷ��֡
			if(Err->Di[0]==0x00&&Err->Di[1]==0x00&&Err->Di[2]==0x01&&Err->Di[3]==0x00)
			{
				Upflag.signAck=1;		//��¼�ظ���־��1
			}
			
			//02 f3(����)��ȷ��֡
			if(Err->Di[0]==0x00&&Err->Di[1]==0x00&&Err->Di[2]==0x04&&Err->Di[3]==0x00)
			{
				Upflag.heatAck=1;		//�����ظ���־��1
			}
		}

		//�����ϱ�֡
		if(Err->afn == 0x0E)
		{ 
			//��Ҫ�¼��ϱ�ȷ��֡
			if((Err->Di[0]==0x00&&Err->Di[1]==0x00&&Err->Di[2]==0x01&&Err->Di[3]==0x00))
			{
				for(i = 0; i<queue->count; i++)
				{
					//�յ���֡�ͷ���ȥ��֡������
					Eventdata Event = {0};
					sequeue_output(queue, &Event);

					//�ҵ���
					if(pFrame->userdata.appseq.seq == Event.seq && Event.report_sucesss_flag == 0)
					{
						Event.report_sucesss_flag = 1;
					}
					else
					{
						//�������
						sequeue_input(queue, Event);
					}
					
				}
			}
		}	
	}
	return nRet;
}

//hardware init
static uint8_t GDW376_1_AFN01H_01H(uint8_t* pBuf, uint16_t* len)
{
	return 1;
}
//software init
static uint8_t GDW376_1_AFN01H_02H(uint8_t* pBuf, uint16_t* len)
{
	return 1;
}
//goback factory
static uint8_t GDW376_1_AFN01H_03H(uint8_t* pBuf, uint16_t* len)
{
	int ret = 0;
	ret = TermParaDefault();
	return ret? 0:1;
}
//goback factory without mainstation para
static uint8_t GDW376_1_AFN01H_04H(uint8_t* pBuf, uint16_t* len)
{
	int ret = 0;
	ret = TermParaDefButMainSta();
	return ret? 0:1;
}
/**
  * @brief   ��ʼ��
  * @param   None
  * @retval  None
  */
static uint8_t AFN_01H(uint8_t *pBuf, uint16_t len, int fd)
{
	stFrame13761Head *pFrame;
	uint8_t nRet = 0;
	uint8_t* dataend = NULL;
	uint8_t Fn = 0;
	uint16_t Pn = 0;
	uint8_t fail_cnt = 0;
	uint8_t fail = 0;
	uint8_t di_cnt = 0;
	uint16_t di_data_len = 0;
	uint8_t di_arry[5*MAX_13761_DI_NUM];
	uint8_t di_arry_len = 0;
	stDataDI* di=NULL;
	uint8_t type =0;
	if ((pBuf == NULL) || (len == 0))
	{
		return 0;
	}
	pFrame = (stFrame13761Head *)pBuf;
	if (pFrame->userdata.afn != AFN_01_RESET)
	{
		return 0;
	}
	
	dataend = getCmdDataEnd(pBuf, len);
	di = (stDataDI*)(pFrame->userdata.data);
	printf("[3761Debug]reset term\n");
	while((uint8_t*)&di[1] <= dataend)
	{
		fail = 0;
		di_data_len = 0;
		memcpy(&di_arry[5*di_cnt], (uint8_t*)&di[0], sizeof(stDataDI));
		di_arry[5*di_cnt + sizeof(stDataDI)] = ACK_ERR_OK;
		di_cnt++;
		nRet = getFn(di->DT, &Fn);
		if (nRet == 0)
		{
			fail = 1;
		}
		nRet = getPn(di->DA, &Pn);
		if (nRet == 0)
		{
			fail = 1;
		}
		if (Pn != 0)	//��������Pn == 0
		{
			fail = 1;
		}
		if (fail == 0)
		{
			switch(Fn)
			{
				case 1: //Ӳ����ʼ��
					nRet = GDW376_1_AFN01H_01H((uint8_t*)&di[1], &di_data_len);
					break;
				case 2: //�����ʼ��
					nRet = GDW376_1_AFN01H_02H((uint8_t*)&di[1], &di_data_len);
					break;
				case 3: //�ָ���������
					printf("[3761Debug]reset term to factory default\n");
					nRet = GDW376_1_AFN01H_03H((uint8_t*)&di[1], &di_data_len);
					break;
				case 4: //����������ϵͳ��վͨ���йصģ���������������ʼ��
					nRet = GDW376_1_AFN01H_04H((uint8_t*)&di[1], &di_data_len);
					break;
				default:
					nRet = 0;
					break;
			}
		}
		
		if ((nRet == 0) || (fail > 0))
		{
			fail_cnt++;
			di_arry[5*(di_cnt-1) + sizeof(stDataDI)] = ACK_ERR_OT;
			break;			//����DI�����ж�
		}
		di = (stDataDI*)((uint8_t*)&di[1] + di_data_len);
	}
	di_arry_len = 5 * di_cnt;
	
	//�ж��Ƿ��������������ǣ��洢

	
	//Ӧ��-���ݳɹ���Ӧ��
	type = ACK_PART;//���ֳɹ�
	if (fail_cnt == di_cnt) //Ӧ��ȫ����
	{
		type = ACK_ALL_ERR;
	}
	else if (fail_cnt == 0) //Ӧ��ȫȷ��
	{
		type = ACK_ALL_OK;
	}
	else
	{
		type = ACK_PART;
	}
	nRet = sendAcktoMainStation(type, pBuf, len, di_arry, di_arry_len,fd);
	
	return nRet;
}

//afn =04 f1
/*****************************************************************************************
��������:   SetParamF1
����˵��:   F1�ն�����ͨ�Ų�������
�������:	pBuf:���ݵ�Ԫ
�������:	len�����ݵ�Ԫ����
����ֵ��	0-ʧ�ܣ�1-�ɹ�
����˵��: 
*****************************************************************************************/
//#pragma optimize=none

static uint8_t GDW376_1_AFN04H_01H(uint8_t* pBuf, uint16_t* len)
{
    if(!pBuf||!len) return 0;

    stTermUpCommuPara* pPara = (stTermUpCommuPara *)pBuf;
	
	if (!pPara) return 0;

    int ret = 0;
    int i;
    char val[8][30];
    char *pData[] = 
    {
        "CS_F1_TRANSDELAY",             val[0],             ":",
        "CS_F1_TRANSDELAYMAX",          val[1],             ":",
        "CS_F1_TIMEOUT",                val[2],             ":",
        "CS_F1_RETRYCNT",               val[3],             ":",
        "CS_F1_FLAG_CLASS1",            val[4],             ":",
        "CS_F1_FLAG_CLASS2",            val[5],             ":",
        "CS_F1_FLAG_CLASS3",            val[6],             ":",
        "CS_F1_PULSECYCLE",             val[7],             ":",
    };
    i = 0;
    ret += itoa(pPara->RTS,val[i++],10);
    ret += itoa(pPara->delay_time,val[i++],10);
    ret += itoa(pPara->u_over_time_times.st_over_time_times.over_time,val[i++],10);
    ret += itoa(pPara->u_over_time_times.st_over_time_times.over_times,val[i++],10);
    ret += itoa((pPara->confirm&0x01)? 1:0,val[i++],10);
    ret += itoa((pPara->confirm&0x02)? 1:0,val[i++],10);
    ret += itoa((pPara->confirm&0x04)? 1:0,val[i++],10);
    ret += itoa(pPara->heart_beats,val[i++],10);    
    
    if(ret) return 0;
    ret = SaveConfMultiGroup(TERM3761_PARA_CONF,pData,i);
    *len = sizeof(stTermUpCommuPara);
    UpCommParaLoad(TERM3761_PARA_CONF);
	
	return (ret==0)? 1:0;    
}

//afn =04 f3
//#pragma optimize=none



//F3��վ����
static uint8_t GDW376_1_AFN04H_03H(uint8_t* pBuf, uint16_t* len)
{
	if(!pBuf||!len) return 0;
	stMainStationIPPort* pPara = (stMainStationIPPort *)pBuf;
	
	if (!pPara) return 0;
	
    int ret = 0;
    int i;
    char val[5][30];
    char *pData[] = 
    {
        "CS_F3_IP1",            val[0],             ":",
        "CS_F3_PORT1",          val[1],             ":",
        "CS_F3_IP2",            val[2],             ":",
        "CS_F3_PORT2",          val[3],             ":",
        "CS_F3_APN",            val[4],             ":",
    };
    i = 0;
    snprintf(val[i++],30,"%d.%d.%d.%d",pPara->u_IP_main.st_IP.IP_s1,pPara->u_IP_main.st_IP.IP_s2,pPara->u_IP_main.st_IP.IP_s3,pPara->u_IP_main.st_IP.IP_s4);
    ret += itoa(pPara->Port_main,val[i++],10);
    snprintf(val[i++],30,"%d.%d.%d.%d",pPara->u_IP_bak.st_IP.IP_s1,pPara->u_IP_bak.st_IP.IP_s2,pPara->u_IP_bak.st_IP.IP_s3,pPara->u_IP_bak.st_IP.IP_s4);
    printf("ip bak:%d.%d.%d.%d\n",pPara->u_IP_bak.st_IP.IP_s1,pPara->u_IP_bak.st_IP.IP_s2,pPara->u_IP_bak.st_IP.IP_s3,pPara->u_IP_bak.st_IP.IP_s4);
    ret += itoa(pPara->Port_bak,val[i++],10);
    snprintf(val[i++],30,"%s",pPara->APN);      
    if(ret) return 0;
    ret = SaveConfMultiGroup(TERM3761_PARA_CONF,pData,i);
    *len = sizeof(stMainStationIPPort);
    MainStationInfoLoad(TERM3761_PARA_CONF);   
    
	return (ret==0)? 1:0;    
}


//F7�ն�IP�Ͷ˿ں�
static uint8_t GDW376_1_AFN04H_07H(uint8_t* pBuf, uint16_t* len)
{
	if(!pBuf||!len) return 0;

    DATA_AFN04_F7* pPara = (DATA_AFN04_F7 *)pBuf;
	
	if (!pPara) return 0;

    
    int ret = 0;
    int i;
    char val[13][30];
    char *pData[] = 
    {
        "CS_F7_IP",            val[0],             ":",
        "CS_F7_MASK",          val[1],             ":",
        "CS_F7_GW",            val[2],             ":",
        "CS_F7_PROXYTYPE",     val[3],             ":",
        "CS_F7_PROXYIP",       val[4],             ":",
        "CS_F7_PROXYPOTE",     val[5],             ":",
        "CS_F7_PROXYAUTH",     val[6],             ":",
        "CS_F7_LEN_USER",      val[7],             ":",
        "CS_F7_USER",          val[8],             ":",
        "CS_F7_LEN_PWD",       val[9],             ":",
        "CS_F7_PWD",           val[10],            ":",
        "CS_F7_TERMPORT",      val[11],            ":",
    };
    i = 0;
    snprintf(val[i++],30,"%d.%d.%d.%d",pPara->termip[0],pPara->termip[1],pPara->termip[2],pPara->termip[3]);
    snprintf(val[i++],30,"%d.%d.%d.%d",pPara->netmask[0],pPara->netmask[1],pPara->netmask[2],pPara->netmask[3]);
    snprintf(val[i++],30,"%d.%d.%d.%d",pPara->gateway[0],pPara->gateway[1],pPara->gateway[2],pPara->gateway[3]);
    ret += itoa(pPara->proxytype,val[i++],10);
    snprintf(val[i++],30,"%d.%d.%d.%d",pPara->proxyserverip[0],pPara->proxyserverip[1],pPara->proxyserverip[2],pPara->proxyserverip[3]);
    ret += itoa(pPara->proxyserverport,val[i++],10);
    ret += itoa(pPara->proxyserverconnectway,val[i++],10);
    ret += itoa(pPara->user_len,val[i++],10);
    if(pPara->user_len)
    {
        snprintf(val[i++],20,"%s",pPara->user);
    }
    ret += itoa(pPara->pw_len,val[i++],10);
    if(pPara->pw_len)
    {
        snprintf(val[i++],20,"%s",pPara->pw);
    }    
    if(ret) return 0;
    ret = SaveConfMultiGroup(TERM3761_PARA_CONF,pData,i);
    *len = sizeof(DATA_AFN04_F7);
	CirIpAndPortLoad(TERM3761_PARA_CONF);
	add_timer(5, 0, LocalSetTermIpInfo, NULL, NULL);
	return (ret==0)? 1:0;  
}
static uint8_t GDW376_1_AFN04H_08H(uint8_t* pBuf, uint16_t* len)
{
    if (!pBuf||!len) return 0;
    sTermWorkWaysUpComm* pPara = (sTermWorkWaysUpComm *)pBuf;
	
	if(!pPara) return 0;

    int ret = 0;
    int i;
    char val[9][30];
    
    char *pData[] = 
    {
        "CS_F8_CONNSTARTMODE",  val[0], ":",
        "CS_F8_MODE",           val[1], ":",
        "CS_F8_ISUDP",          val[2], ":",
        "CS_F8_RECONNINTERVAL", val[3], ":",
        "CS_F8_MAXRETRYCNT",    val[4], ":",    
        "CS_F8_MAXIDLETIME",    val[5], ":",
        "CS_F8_TIMESPAN_1",     val[6], ":",
        "CS_F8_TIMESPAN_2",     val[7], ":",
        "CS_F8_TIMESPAN_3",     val[8], ":",
    };

    i=0;
    ret += itoa(pPara->TermMode.bMode.ConnStartMode,val[i++],10);
    ret += itoa(pPara->TermMode.bMode.mode,val[i++],10);
    ret += itoa(pPara->TermMode.bMode.TcpUdp,val[i++],10);
    ret += itoa(pPara->ReconnInterval,val[i++],10);
    ret += itoa(pPara->MaxRetryCnt,val[i++],10);
    ret += itoa(pPara->MaxIdleTime,val[i++],10);
    ret += itoa(pPara->TimeSpan[0],val[i++],10);
    ret += itoa(pPara->TimeSpan[1],val[i++],10);
    ret += itoa(pPara->TimeSpan[2],val[i++],10);
    if(ret) return 0;
    ret = SaveConfMultiGroup(TERM3761_PARA_CONF,pData,i);
	*len = sizeof(sTermWorkWaysUpComm);
    UpCommWorkWaysLoad(TERM3761_PARA_CONF);
	return (ret==0)? 1:0;     
}


//F9�ն��¼���¼��������
static uint8_t GDW376_1_AFN04H_09H(uint8_t* pBuf, uint16_t* len)
{
    if (!pBuf||!len) return 0;
    TermEventRecordSet* pPara = (TermEventRecordSet *)pBuf;
	
	if(!pPara) return 0;

    int ret = 0;
    int i;
    char val[16][30];
    
    char *pData[] = 
    {
        "CS_F9_EVENTRECORDVALIDFLAG_1",      val[0], ":",
        "CS_F9_EVENTRATINGFLAG_1",           val[1], ":",
        
        "CS_F9_EVENTRECORDVALIDFLAG_2",      val[2], ":",
        "CS_F9_EVENTRATINGFLAG_2",           val[3], ":",
        
        "CS_F9_EVENTRECORDVALIDFLAG_3",      val[4], ":",
        "CS_F9_EVENTRATINGFLAG_3",           val[5], ":",
        
        "CS_F9_EVENTRECORDVALIDFLAG_4",      val[6], ":",
        "CS_F9_EVENTRATINGFLAG_4",           val[7], ":",
        
        "CS_F9_EVENTRECORDVALIDFLAG_5",      val[8], ":",
        "CS_F9_EVENTRATINGFLAG_5",           val[9], ":",
        
        "CS_F9_EVENTRECORDVALIDFLAG_6",      val[10], ":",
        "CS_F9_EVENTRATINGFLAG_6",           val[11], ":",
        
        "CS_F9_EVENTRECORDVALIDFLAG_7",      val[12], ":",
        "CS_F9_EVENTRATINGFLAG_7",           val[13], ":",
        
        "CS_F9_EVENTRECORDVALIDFLAG_8",      val[14], ":",
        "CS_F9_EVENTRATINGFLAG_8",           val[15], ":",
    };
    int gr = sizeof(pData)/(sizeof(pData[0])*3);
    for(i=0;i<(gr/2);i++)
    {
        if(i>sizeof(pPara->CS_F9_EventRecordValidFlag)) 
        {
            ret = -1;
            break;
        }
        ret += itoa(pPara->CS_F9_EventRecordValidFlag[i],val[2*i],10);
        ret += itoa(pPara->CS_F9_EventRatingFlag[i],val[2*i+1],10);
    }
    if(ret) return 0;
    ret = SaveConfMultiGroup(TERM3761_PARA_CONF,pData,gr);
	*len = sizeof(TermEventRecordSet);
    EventParaLoad(TERM3761_PARA_CONF);
	return (ret==0)? 1:0;     
}
static uint8_t GDW376_1_AFN04H_16H(uint8_t* pBuf, uint16_t* len)
{
    if (!pBuf||!len) return 0;
    sVirtualNet* pPara = (sVirtualNet *)pBuf;
	
	if(!pPara) return 0;

    int ret = 0;
    int i;
    char val[2][60];
    
    char *pData[] = 
    {
        "CS_F16_VIRUSER",  	val[0], ":",
        "CS_F16_VIRPASSWD", val[1], ":",
    };

	memcpy(val[0],pPara->VirUser,sizeof(pPara->VirUser));
	memcpy(val[1],pPara->VirPasswd,sizeof(pPara->VirPasswd));
    ret = SaveConfMultiGroup(TERM3761_PARA_CONF,pData,sizeof(pData)/(sizeof(pData[0])*3));
	*len = sizeof(sVirtualNet);
    VirtualNetLoad(TERM3761_PARA_CONF);
	return (ret==0)? 1:0;     
}






//F89�ն��߼���ַ(�������롢�ն��߼���ַ)
static uint8_t GDW376_1_AFN04H_89H(uint8_t* pBuf, uint16_t* len)
{
    
    if (!pBuf||!len) return 0;
    MODLE_ADDR_STRU* pPara = (MODLE_ADDR_STRU *)pBuf;
	
	if(!pPara) return 0;

    int ret = 0;
    int i;
    char val[2][30];
    
    char *pData[] = 
    {
        "CS_F89_AREA_CODE",        val[0],    ":",
        "CS_F89_TERMINAL_CODE",    val[1],    ":",
    };

    i=0;
    ret += itoa(pPara->districtCode,val[i++],10);
    ret += itoa(pPara->termAddr,val[i++],10);
    if(ret||(i>9)) return 0;
    ret = SaveConfMultiGroup(TERM3761_PARA_CONF,pData,i);
	*len = sizeof(MODLE_ADDR_STRU);
    CircuitLogicAddrLoad(TERM3761_PARA_CONF);
	return (ret==0)? 1:0;     
}



//F91�ն˵���λ��
static uint8_t GDW376_1_AFN04H_91H(uint8_t* pBuf, uint16_t* len)
{
    if (!pBuf||!len) return 0;
    stTermLocation* pPara = (stTermLocation *)pBuf;
	
	if(!pPara) return 0;

    int ret = 0;
    int i;
    char val[2][30];
    
    char *pData[] = 
    {
        "CS_F91_LONGITUDE", val[0], ":",
        "CS_F91_DIMENSION", val[1], ":",
    };

    i=0;
    int degree,min,sec,SecDec;

    degree = pPara->longitude_D_BW*100 + pPara->longitude_D_SW*10 + pPara->longitude_D_GW;
    min = pPara->longitude_B_SW*10 + pPara->longitude_B_GW;
    sec = pPara->longitude_S_SW*10 + pPara->longitude_S_GW;
    SecDec = pPara->longitude_S_SFW*10 + pPara->longitude_S_BFW;    
    if(pPara->longitude_D_BLW&0x8)
    {
        if(SecDec)
            snprintf(val[i++],30,"W%d-%d\'%d.%d\"",degree,min,sec,SecDec);
        else
            snprintf(val[i++],30,"W%d-%d\'%d\"",degree,min,sec);
    }
    else
    {
        if(SecDec)
            snprintf(val[i++],30,"E%d-%d\'%d.%d\"",degree,min,sec,SecDec);
        else
            snprintf(val[i++],30,"E%d-%d\'%d\"",degree,min,sec);

    }
    degree = pPara->latitude_D_BW*100 + pPara->latitude_D_SW*10 + pPara->latitude_D_GW;
    min = pPara->latitude_B_SW*10 + pPara->latitude_B_GW;
    sec = pPara->latitude_S_SW*10 + pPara->latitude_S_GW;
    SecDec = pPara->latitude_S_SFW*10 + pPara->latitude_S_BFW;        
    if(pPara->latitude_D_BLW&0x8)
    {
        if(SecDec)
            snprintf(val[i++],30,"S%d-%d\'%d.%d\"",degree,min,sec,SecDec);
        else
            snprintf(val[i++],30,"S%d-%d\'%d\"",degree,min,sec);
    }
    else
    {
        if(SecDec)
            snprintf(val[i++],30,"N%d-%d\'%d.%d\"",degree,min,sec,SecDec);
        else
            snprintf(val[i++],30,"N%d-%d\'%d\"",degree,min,sec);
    }
        
    ret = SaveConfMultiGroup(TERM3761_PARA_CONF,pData,i);
	*len = sizeof(stTermLocation);
    LocationLoad(TERM3761_PARA_CONF);
	return (ret==0)? 1:0;  
}






//���õ�����·ʹ�ܲ���
static uint8_t GDW376_1_AFN04H_153H(uint8_t* pBuf, uint16_t* len)
{
    if(!pBuf||!len) return 0;

    current_circuit_param_stru* pPara = (current_circuit_param_stru *)pBuf;
	
	if (!pPara) return 0;

    int ret = 0;
    int i;
    char val[4][30];
    char *pData[] = 
    {
        "CS_F153_PA_EN",          val[0],             ":",
        "CS_F153_PB_EN",          val[1],             ":",
        "CS_F153_PC_EN",          val[2],             ":",
        "CS_F153_MAC",            val[3],             ":",
    };
    if(pPara->phase_a_curr<2)
    {
        ret += itoa(pPara->phase_a_curr,val[i++],10);
    }
    if(pPara->phase_b_curr<2)
    {
        ret += itoa(pPara->phase_b_curr,val[i++],10);
    }
    if(pPara->phase_c_curr<2)
    {
        ret += itoa(pPara->phase_c_curr,val[i++],10);
    }
    snprintf(val[i++],30,"%d.%d.%d.%d",pPara->mac[3],pPara->mac[2],pPara->mac[1],pPara->mac[0]);
    if(ret || (i>4)) return 0;
    ret = SaveConfMultiGroup(TERM3761_PARA_CONF,pData,i);
    *len = sizeof(current_circuit_param_stru);
	ret += CurLoopParaLoad(TERM3761_PARA_CONF);
	return (ret==0)? 1:0;    
}


//����TA��Ȳ���
static uint8_t GDW376_1_AFN04H_154H(uint8_t* pBuf, uint16_t* len)
{
    if(!pBuf||!len) return 0;

    current_transformer_rate_stru* pPara = (current_transformer_rate_stru *)pBuf;
	
	if (!pPara) return 0;

    int ret = 0;
    int i;
    char val[5][30];
    char *pData[] = 
    {
        "CS_F154_FORMAT",            val[0],             ":",
        "CS_F154_PA_RATIO",          val[1],             ":",
        "CS_F154_PB_RATIO",          val[2],             ":",
        "CS_F154_PC_RATIO",          val[3],             ":",
        "CS_F154_MAC",               val[4],             ":",
    };
    i=0;
    ret += itoa(pPara->format,val[i++],10);
    ret += itoa(pPara->phase_a_trans,val[i++],10);
    ret += itoa(pPara->phase_b_trans,val[i++],10);
    ret += itoa(pPara->phase_c_trans,val[i++],10);
    snprintf(val[i++],30,"%d.%d.%d.%d",pPara->mac[3],pPara->mac[2],pPara->mac[1],pPara->mac[0]);    
    if(ret || (i>4)) return 0;
    ret = SaveConfMultiGroup(TERM3761_PARA_CONF,pData,i);
    *len = sizeof(current_circuit_param_stru);
	ret += TaRateParaLoad(TERM3761_PARA_CONF);
	return (ret==0)? 1:0;

}


//���ò���	//LINE2075
static uint8_t AFN_04H(uint8_t *pBuf, uint16_t len, int fd)
{
	stFrame13761Head *pFrame;
	uint8_t nRet = 0;	
	uint8_t Fn = 0;
	uint16_t Pn = 0;
	uint8_t fail_cnt = 0;
	uint8_t fail = 0;
	uint8_t di_cnt = 0;
	uint16_t di_data_len = 0;
	uint8_t di_arry[5*MAX_13761_DI_NUM];
	uint8_t di_arry_len = 0;
	uint8_t* dataend = NULL;
	stDataDI* di = NULL;
	uint8_t type = 0;
	if ((pBuf == NULL) || (len == 0))
	{
		return 0;
	}
	pFrame = (stFrame13761Head *)pBuf;
	if (pFrame->userdata.afn != AFN_04_SETUP_PARM)
	{
		return 0;
	}
	
	dataend = getCmdDataEnd(pBuf, len);
	di = (stDataDI*)(pFrame->userdata.data);//DA1+DA2+DT1+DT2

	while((uint8_t*)&di[1] <= dataend)
	{
		fail = 0;
		di_data_len = 0;
		memcpy(&di_arry[5*di_cnt], (uint8_t*)&di[0], sizeof(stDataDI));
		di_arry[5*di_cnt + sizeof(stDataDI)] = ACK_ERR_OK;
		di_cnt++;
		nRet = getFn(di->DT, &Fn);
		if (nRet == 0)
		{
			fail = 1;
		}
		nRet = getPn(di->DA, &Pn);
		if (nRet == 0)
		{
			fail = 1;
		}
		if (Pn != 0)	//��������Pn == 0
		{
			fail = 1;
		}
		if (fail == 0)
		{
			switch(Fn)
			{
				case 1:		//F1�ն�����ͨ�Ų�������
					nRet = GDW376_1_AFN04H_01H((uint8_t*)&di[1], &di_data_len);
					break;
				case 3:		//F3��վIP��ַ�Ͷ˿�
					nRet = GDW376_1_AFN04H_03H((uint8_t*)&di[1], &di_data_len);
					break;
				case 7:		//�ն� IP ��ַ�Ͷ˿�
					nRet = GDW376_1_AFN04H_07H((uint8_t*)&di[1], &di_data_len);		
					break;
				case 8:
					nRet = GDW376_1_AFN04H_08H((uint8_t*)&di[1], &di_data_len);
					break;
				case 9:		//F9�ն��¼���¼��������
					nRet = GDW376_1_AFN04H_09H((uint8_t*)&di[1], &di_data_len);
					break;
				case 16:		//F9�ն��¼���¼��������
					nRet = GDW376_1_AFN04H_16H((uint8_t*)&di[1], &di_data_len);
					break;
				case 89:	//�����ն��߼���ַ
					nRet = GDW376_1_AFN04H_89H((uint8_t*)&di[1], &di_data_len);
					break;
				case 91:	//�����ն˵���λ��
					nRet = GDW376_1_AFN04H_91H((uint8_t*)&di[1], &di_data_len);
					break;
				case 153:  //������·ʹ�ܲ���-x
					nRet = GDW376_1_AFN04H_153H((uint8_t*)&di[1], &di_data_len);
					break;
				case 154:  //TA��Ȳ���-x
					nRet = GDW376_1_AFN04H_154H((uint8_t*)&di[1], &di_data_len);
					break;
				default:
					nRet = 0;
					break;
			}
			
			if(nRet != 0)
			{
				//��������¼���¼
				if (Fn == 1 || Fn == 3 || Fn == 7 || Fn == 8 || Fn == 9 || Fn == 89 || Fn == 153 || Fn == 154) 
				{ 
					//��¼�¼�	
					stru_sjjl_ERC3 event3;
					
					u8 buf[4] = {0};
					buf[0] = 0x00;
					buf[1] = 0x00;
					
					stDT DT = {0};
					setFn(Fn, &DT);
					buf[2] = DT.DT1;
					buf[3] = DT.DT2;
					
					event3.uniundata = buf;
					event3.MstAddr = 0;					//����վ��ַ?
					CreateEvent3(event3) >= 0;
				}
			}
		}
		
		if ((nRet == 0) || (fail > 0))
		{
			fail_cnt++;
			di_arry[5*(di_cnt-1) + sizeof(stDataDI)] = ACK_ERR_OT;
			break;			//����DI�����ж�
		}
		di = (stDataDI*)((uint8_t*)&di[1] + di_data_len);
	}
	di_arry_len = 5 * di_cnt;
	
	//�ж��Ƿ��������������ǣ��洢


	//Ӧ��-���ݳɹ���Ӧ��			
	if (fail_cnt == di_cnt)		//Ӧ��ȫ����
	{
		type = ACK_ALL_ERR;
	}
	else if (fail_cnt == 0)		//Ӧ��ȫȷ��
	{
		type = ACK_ALL_OK;
	}
	else
	{
		type = ACK_PART;		//���ֳɹ�
	}
	nRet = sendAcktoMainStation(type, pBuf, len, di_arry, di_arry_len, fd);		//����Ӧ�𣬺���Ҫ
	
	return nRet;
}

//F1�����ն˰汾��Ϣ
static uint8_t GDW376_1_AFN09H_01H(uint8_t* pBuf, uint16_t* len, uint8_t* pAckBuf, uint16_t* ack_pos)
{
	if ((pBuf == NULL)||(len == NULL) || (pAckBuf == NULL) || (ack_pos == NULL))
	{
		return 0;
	}
	
	//*ack_pos += sizeof();
	return 1;
}


//F9����Զ��ͨ��ģ����Ϣ
static uint8_t GDW376_1_AFN09H_09H(uint8_t* pBuf, uint16_t* len, uint8_t* pAckBuf, uint16_t* ack_pos)
{
	if ((pBuf == NULL)||(len == NULL) || (pAckBuf == NULL) || (ack_pos == NULL))
	{
		return 0;
	}
	
	//*ack_pos += sizeof(stMainStationIPPort);
	return 1;
}


//�����ն�������Ϣ
static uint8_t AFN_09H(uint8_t *pBuf, uint16_t len, int fd)
{
	stFrame13761Head *pFrame;
	uint8_t nRet = 0;	
	uint8_t ACKbuf[PROTOCOL_13761_BUF_SIZE];
	uint16_t ack_data_len = 0;
	uint8_t Fn = 0;
	uint16_t Pn = 0;
	uint8_t fail_cnt = 0;
	uint8_t fail = 0;
	uint8_t di_cnt = 0;
	uint16_t di_data_len = 0;
	uint8_t* dataend = NULL;
	stDataDI* di = NULL;
	if ((pBuf == NULL) || (len == 0))
	{
		return 0;
	}
	pFrame = (stFrame13761Head *)pBuf;
	if (pFrame->userdata.afn != AFN_09_TMCONFIG)		//AFN09
	{
		return 0;
	}
	
	dataend = getCmdDataEnd(pBuf, len);
	di = (stDataDI*)(pFrame->userdata.data);

	while((uint8_t*)&di[1] <= dataend)
	{
		fail = 0;
		di_data_len = 0;
		di_cnt++;
		nRet = getFn(di->DT, &Fn);
		if (nRet == 0)
		{
			fail = 1;
		}
		nRet = getPn(di->DA, &Pn);
		if (nRet == 0)
		{
			fail = 1;
		}
		if (Pn != 0)	//������ѯPn == 0
		{
			//fail = 1;
		}
		if (fail == 0)
		{
			stDataDI* pDI = (stDataDI*)&ACKbuf[ack_data_len];
			setFn(Fn, &pDI->DT);
			setPn(Pn, &pDI->DA);
			ack_data_len += sizeof(stDataDI);
			switch(Fn)
			{
				case 1: 	//F1�ն˰汾��Ϣ
					nRet = GDW376_1_AFN09H_01H((uint8_t*)&di[1], &di_data_len, ACKbuf, &ack_data_len);
					break;
				case 9: 	//F9Զ��ͨ��ģ��汾��Ϣ
					nRet = GDW376_1_AFN09H_09H((uint8_t*)&di[1], &di_data_len, ACKbuf, &ack_data_len);
					break;
				default:
					nRet = 0;
					break;
			}
			if (nRet == 0)
			{
				ack_data_len -= sizeof(stDataDI);
			}
		}
		if ((nRet == 0) || (fail > 0))
		{
			fail_cnt++;
			
			break;			//����DI�����ж�
		}
		di = (stDataDI*)((uint8_t*)&di[1] + di_data_len);

		
	}
	
	//Ӧ��
	if (fail_cnt > 0)	//�д���Ӧ�����
	{
		nRet = sendAcktoMainStation(ACK_ALL_ERR, pBuf, len, ACKbuf, 0, fd);
	}
	else	//����Ӧ��
	{
		nRet = sendAckFrametoMainStation(LINK_CTRL_UP_RES_USERDATA, pBuf, len, ACKbuf, ack_data_len, fd);
		
	}
	
	return nRet;
}


static uint8_t GDW376_1_AFN0AH_01H(uint8_t* pBuf, uint16_t* len, uint8_t* pAckBuf, uint16_t* ack_pos)
{
    if(!pBuf||!len||!pAckBuf||!ack_pos) return 0;
    
    sTermSysInfo *pPara = GetCiSysInfo();
    memcpy(pAckBuf,(uint8_t*)&pPara->UpCommuPara,sizeof(pPara->UpCommuPara));
    *ack_pos += sizeof(pPara->UpCommuPara);
    return 1;
}

//F3��վIP���˿ڡ�APN��ѯ
static uint8_t GDW376_1_AFN0AH_03H(uint8_t* pBuf, uint16_t* len, uint8_t* pAckBuf, uint16_t* ack_pos)
{
    if(!pBuf||!len||!pAckBuf||!ack_pos) return 0;
    
    sTermSysInfo *pPara = GetCiSysInfo();
    memcpy(pAckBuf,(uint8_t*)&pPara->MainStation,sizeof(pPara->MainStation));
    *ack_pos += sizeof(pPara->MainStation);
    return 1;

}

//F7�ն�IP�Ͷ˿ںŲ�ѯ
static uint8_t GDW376_1_AFN0AH_07H(uint8_t* pBuf, uint16_t* len, uint8_t* pAckBuf, uint16_t* ack_pos)
{
    if(!pBuf||!len||!pAckBuf||!ack_pos) return 0;
    
    sTermSysInfo *pPara = GetCiSysInfo();
    memcpy(pAckBuf,(uint8_t*)&pPara->CiIpAndPort,sizeof(pPara->CiIpAndPort));
    *ack_pos += sizeof(pPara->CiIpAndPort);
    return 1;

}


//F9�ն��¼���¼���ò�ѯ
static uint8_t GDW376_1_AFN0AH_09H(uint8_t* pBuf, uint16_t* len, uint8_t* pAckBuf, uint16_t* ack_pos)
{
    if(!pBuf||!len||!pAckBuf||!ack_pos) return 0;
    
    sTermSysInfo *pPara = GetCiSysInfo();
    memcpy(pAckBuf,(uint8_t*)&pPara->EvtRecoSet,sizeof(pPara->EvtRecoSet));
    *ack_pos += sizeof(pPara->EvtRecoSet);
    return 1;

}
static uint8_t GDW376_1_AFN0AH_16H(uint8_t* pBuf, uint16_t* len, uint8_t* pAckBuf, uint16_t* ack_pos)
{
    if(!pBuf||!len||!pAckBuf||!ack_pos) return 0;
    
    sTermSysInfo *pPara = GetCiSysInfo();
    memcpy(pAckBuf,(uint8_t*)&pPara->VirtualNet,sizeof(pPara->VirtualNet));
    *ack_pos += sizeof(pPara->VirtualNet);
    return 1;
}

//F89�ն��߼���ַ��ѯ
static uint8_t GDW376_1_AFN0AH_89H(uint8_t* pBuf, uint16_t* len, uint8_t* pAckBuf, uint16_t* ack_pos)
{
    if(!pBuf||!len||!pAckBuf||!ack_pos) return 0;
    
    sTermSysInfo *pPara = GetCiSysInfo();
	printf("terminal addr:\n");
	printhexdata(&pPara->usModleAddr,sizeof(pPara->usModleAddr));
    memcpy(pAckBuf,(uint8_t*)&pPara->usModleAddr,sizeof(pPara->usModleAddr));
    *ack_pos += sizeof(pPara->usModleAddr);
    return 1;

}


//F91�ն˵���λ����Ϣ
static uint8_t GDW376_1_AFN0AH_91H(uint8_t* pBuf, uint16_t* len, uint8_t* pAckBuf, uint16_t* ack_pos)
{
    if(!pBuf||!len||!pAckBuf||!ack_pos) return 0;
    
    sTermSysInfo *pPara = GetCiSysInfo();
    memcpy(pAckBuf,(uint8_t*)&pPara->Location,sizeof(pPara->Location));
    *ack_pos += sizeof(pPara->Location);
    return 1;

}

////������·ʹ�ܲ���-x
static uint8_t GDW376_1_AFN0AH_153H(uint8_t* pBuf, uint16_t* len, uint8_t* pAckBuf, uint16_t* ack_pos)
{
    if(!pBuf||!len||!pAckBuf||!ack_pos) return 0;
    
    sTermSysInfo *pPara = GetCiSysInfo();
    memcpy(pAckBuf,(uint8_t*)&pPara->CurLoopEnPara,sizeof(pPara->CurLoopEnPara));
    *ack_pos += sizeof(pPara->CurLoopEnPara);
    return 1;

}

//F154//TA��Ȳ���-x
static uint8_t GDW376_1_AFN0AH_154H(uint8_t* pBuf, uint16_t* len, uint8_t* pAckBuf, uint16_t* ack_pos)
{
    if(!pBuf||!len||!pAckBuf||!ack_pos) return 0;
    
    sTermSysInfo *pPara = GetCiSysInfo();
    memcpy(pAckBuf,(uint8_t*)&pPara->TARatePara,sizeof(pPara->TARatePara));
    *ack_pos += sizeof(pPara->TARatePara);
    return 1;

}


//��ѯ����
static uint8_t AFN_0AH(uint8_t *pBuf, uint16_t len, int fd)
{
	stFrame13761Head *pFrame;
	uint8_t nRet = 0;	
	uint8_t ACKbuf[PROTOCOL_13761_BUF_SIZE];
	uint16_t ack_data_len = 0;
	uint8_t Fn = 0;
	uint16_t Pn = 0;
	uint8_t fail_cnt = 0;
	uint8_t fail = 0;
	uint8_t di_cnt = 0;
	uint16_t di_data_len = 0;
	uint8_t* dataend = NULL;
	stDataDI* di = NULL;
	if ((pBuf == NULL) || (len == 0))
	{
		return 0;
	}
	pFrame = (stFrame13761Head *)pBuf;
	if (pFrame->userdata.afn != AFN_0A_INQUIRY_PARM)
	{
		return 0;
	}
	
	dataend = getCmdDataEnd(pBuf, len);
	di = (stDataDI*)(pFrame->userdata.data);
	while((uint8_t*)&di[1] <= dataend)
	{
		fail = 0;
		di_data_len = 0;
		di_cnt++;
		printf("[AFN_0A]DT1=0x%x\nDT2=0x%x\n",di->DT.DT1,di->DT.DT2);
		nRet = getFn(di->DT, &Fn);
		if (nRet == 0)
		{
			fail = 1;
		}
		printf("[AFN_0A]Fn=F%d\n",Fn);
		nRet = getPn(di->DA, &Pn);
		if (nRet == 0)
		{
			fail = 1;
		}
		if (Pn != 0)	//������ѯPn == 0
		{
			//fail = 1;
		}
		if (fail == 0)
		{
			stDataDI* pDI = (stDataDI*)&ACKbuf[ack_data_len];
			setFn(Fn, &pDI->DT);
			setPn(Pn, &pDI->DA);
			ack_data_len += sizeof(stDataDI);
			switch(Fn)
			{
				case 1: 	//F1�ն�����ͨ�Ų�����ѯ
					nRet = GDW376_1_AFN0AH_01H((uint8_t*)&di[1], &di_data_len, ACKbuf, &ack_data_len);
					break;
//				case 2: 	//F2�ն˵���λ����Ϣ
//					//nRet = GDW376_1_AFN0AH_02H((uint8_t*)&di[1], &di_data_len, ACKbuf, &ack_data_len);
//					break;
				case 3: 	//F3��վIP���˿ڡ�APN
					nRet = GDW376_1_AFN0AH_03H((uint8_t*)&di[1], &di_data_len, ACKbuf, &ack_data_len);
					break;
				case 7: 	//F7�ն�ip�Ͷ˿ں�
					nRet = GDW376_1_AFN0AH_07H((uint8_t*)&di[1], &di_data_len, ACKbuf, &ack_data_len);
					break;
				case 9: 	//F9�ն��¼���¼���ò�ѯ				//F9����ͨ��ģ��汾��ѯ
					nRet = GDW376_1_AFN0AH_09H((uint8_t*)&di[1], &di_data_len, ACKbuf, &ack_data_len);
					break;
				case 16: 	
					nRet = GDW376_1_AFN0AH_16H((uint8_t*)&di[1], &di_data_len, ACKbuf, &ack_data_len);
					break;
				case 89:	//F89�ն��߼���ַ��ѯ
					nRet = GDW376_1_AFN0AH_89H((uint8_t*)&di[1], &di_data_len, ACKbuf, &ack_data_len);
				case 91:	//F91�ն˵���λ�ò�ѯ
					nRet = GDW376_1_AFN0AH_91H((uint8_t*)&di[1], &di_data_len, ACKbuf, &ack_data_len);
					break;	
				case 153: //������·ʹ�ܲ���-x
					nRet = GDW376_1_AFN0AH_153H((uint8_t*)&di[1], &di_data_len, ACKbuf, &ack_data_len);
					break;
				case 154: //TA��Ȳ���-x
					nRet = GDW376_1_AFN0AH_154H((uint8_t*)&di[1], &di_data_len, ACKbuf, &ack_data_len);
					break;
				default:
					nRet = 0;
					break;
			}
			if (nRet == 0)
			{
				ack_data_len -= sizeof(stDataDI);
			}
		}
		if ((nRet == 0) || (fail > 0))
		{
			fail_cnt++;
			
			break;			//����DI�����ж�
		}
		di = (stDataDI*)((uint8_t*)&di[1] + di_data_len);

		
	}
	
	//Ӧ��
	if (fail_cnt > 0)	//�д���Ӧ�����
	{
		nRet = sendAcktoMainStation(ACK_ALL_ERR, pBuf, len, ACKbuf, 0, fd);
	}
	else	//����Ӧ��
	{
		nRet = sendAckFrametoMainStation(LINK_CTRL_UP_RES_USERDATA, pBuf, len, ACKbuf, ack_data_len, fd);
		
	}
	
	return nRet;
}


static uint8_t GDW376_1_AFN05H_031H(uint16_t Pn,uint8_t* pBuf, uint16_t* len)
{
	stTime* pPara = (stTime *)pBuf;
	u8 nyear = pPara->year;
	u8 mon   = pPara->month;
	u8 day   = pPara->day;
	u8 hour  = pPara->hour;
	u8 min   = pPara->min;
	u8 sec   = pPara->sec;
	
	nyear = BCD2INT(nyear);
	mon   = BCD2INT(mon);
	day   = BCD2INT(day);
	hour  = BCD2INT(hour);
	min   = BCD2INT(min);
	sec   = BCD2INT(sec);

	//����ʱ��
//	RTC_Set(nyear,mon,day,hour,min,sec);     
//  BKP_WriteBackupRegister(BKP_DR1, 0x1016);	//��ִ�еĺ󱸼Ĵ�����д���û���������
//  RTC_Get();//����ʱ��

	//��ʱ�ɹ������ɶ�ʱ�¼�
	if (1)
	{
		stru_sjjl_ERC41 ev41 = {0};
		ev41.mp[0] = 0x00;
		ev41.mp[1] = 0x00; 

		u8 month = 0;
		time_t t;
		struct tm ftnow;
		t = time(NULL);	
		localtime_r(&t, &ftnow);

		ftnow.tm_mon += 1;
		ftnow.tm_year += 1900;

		intToBcd(ftnow.tm_year, &ev41.TimeBeforeAdjust.year, 1);
		intToBcd(ftnow.tm_mon,  &month, 1);
		ev41.TimeBeforeAdjust.month = month;		
		intToBcd(ftnow.tm_mday, &ev41.TimeBeforeAdjust.day, 1);
			
		if(ftnow.tm_wday == 0)
		{
			ev41.TimeBeforeAdjust.week= 7;
		}
		else
		{
			ev41.TimeBeforeAdjust.week = ftnow.tm_wday;
		}
		
		intToBcd(ftnow.tm_hour,&ev41.TimeBeforeAdjust.hour,1);
		intToBcd(ftnow.tm_min,&ev41.TimeBeforeAdjust.min,1); 
		intToBcd(ftnow.tm_sec,&ev41.TimeBeforeAdjust.sec,1);

		memcpy(&ev41.TimeAfterAdjust, pPara, sizeof(SJGS_01));
		
		CreateEvent41(ev41);
	}
		
	* len=sizeof(stTime);
	return 1;
}

static uint8_t AFN_05H(uint8_t *pBuf, uint16_t len, int fd)
{
stFrame13761Head *pFrame;
	uint8_t nRet = 0;	
	uint8_t Fn = 0;
	uint16_t Pn = 0;
	uint8_t fail_cnt = 0;
	uint8_t fail = 0;
	uint8_t di_cnt = 0;
	uint16_t di_data_len = 0;
	uint8_t di_arry[5*MAX_13761_DI_NUM];
	uint8_t di_arry_len = 0;
	uint8_t* dataend = NULL;
	stDataDI* di = NULL;
	uint8_t type = ACK_PART;//���ֳɹ�
	u16 i=0;
	if ((pBuf == NULL) || (len == 0))
	{
		return 0;
	}
	
	pFrame = (stFrame13761Head *)pBuf;
	if (pFrame->userdata.afn != AFN_05_CONTROL)
	{
		return 0;
	}
	
	dataend = getCmdDataEnd(pBuf, len);
	di = (stDataDI*)(pFrame->userdata.data);

	while((uint8_t*)&di[1] <= dataend)
	{
		fail = 0;
		di_data_len = 0;
		memcpy(&di_arry[5*di_cnt], (uint8_t*)&di[0], sizeof(stDataDI));
		di_arry[5*di_cnt + sizeof(stDataDI)] = ACK_ERR_OK;
		di_cnt++;
		nRet = getFn(di->DT, &Fn);
		if (nRet == 0)
		{
			fail = 1;
		}
		nRet = getPn(di->DA, &Pn);
		if (nRet == 0)
		{
			fail = 1;
		}
		if (Pn != 0)
		{
			//fail = 1;
		}
		if (fail == 0)
		{
			switch(Fn)
			{
				case 1: 
	
					break;
				case 2: 
					break;

					break;
				case 27: //�����ն�����վͨ��
					//nRet = GDW376_1_AFN05H_027H((uint8_t*)&di[1], &di_data_len);
					break;
				case 29: //�����ն������ϱ�
					//nRet = GDW376_1_AFN05H_029H((uint8_t*)&di[1], &di_data_len);
					
					break;
				case 31: //��ʱ����
					nRet = GDW376_1_AFN05H_031H(Pn,(uint8_t*)&di[1], &di_data_len);
					break;
				case 32: //������Ϣ
					//nRet = GDW376_1_AFN05H_032H((uint8_t*)&di[1], &di_data_len);
					
					break;
				case 35: //��ֹ�ն�����վͨ��
					//nRet = GDW376_1_AFN05H_035H((uint8_t*)&di[1], &di_data_len);
					break;
				case 37: //��ֹ�ն������ϱ�
					//nRet = GDW376_1_AFN05H_037H((uint8_t*)&di[1], &di_data_len);
					
					break;
				case 38: //�����ն�������վ
					//nRet = GDW376_1_AFN05H_038H((uint8_t*)&di[1], &di_data_len);
					break;
				case 39: //�����ն˶Ͽ�����
					//nRet = GDW376_1_AFN05H_039H((uint8_t*)&di[1], &di_data_len);
					break;
				default:
					nRet = 0;
					break;
			}
		}
		if ((nRet == 0) || (fail > 0))
		{
			fail_cnt++;
			di_arry[5*(di_cnt-1) + sizeof(stDataDI)] = ACK_ERR_OT;
			break;			//����DI�����ж�
		}
		di = (stDataDI*)((uint8_t*)&di[1] + di_data_len);
	}
	di_arry_len = 5 * di_cnt;
	
	//�ж��Ƿ��������������ǣ��洢

	
	//Ӧ��-���ݳɹ���Ӧ��
	if (fail_cnt == di_cnt) //Ӧ��ȫ����
	{
		type = ACK_ALL_ERR;
	}
	else if (fail_cnt == 0) //Ӧ��ȫȷ��
	{
		type = ACK_PART;
	}

	for(i=0;i<di_cnt;i++)
	{
		
		//stDataDI* di = NULL;
		di=(stDataDI*)&di_arry[i*5];
		if(di->DT.DT2==0x00)
		{
			//05 F1,F2,F3
			if(di->DT.DT1==0x04||di->DT.DT1==0x02||di->DT.DT1==0x01)
			{

				//if(di_cnt == 0x01)
				{
					return 0;
				}

			}
		}			
	}
	nRet = sendAcktoMainStation(type, pBuf, len, di_arry, di_arry_len, fd);	
	return nRet;
}


static uint8_t GDW376_1_AFN06H_100H(uint8_t* pBuf, uint16_t* len, uint8_t* pAckBuf, uint16_t* ack_pos)
{
	int ret = 0;
	
	if(!pAckBuf||!ack_pos) return 0;
	
	ret = MainStaGetTermCertiInfo(pBuf,8,pAckBuf,ack_pos);
	return (ret? 0:1);

}
static uint8_t GDW376_1_AFN06H_101H(uint8_t* pBuf, uint16_t* len, uint8_t* pAckBuf, uint16_t* ack_pos)
{
	int ret = 0;
	
	if(!pAckBuf||!ack_pos) return 0;
	
	ret = GetTerminalSecurityAuthenInfo(pBuf,8,pAckBuf,60,ack_pos);
	return (ret? 0:1);
}
static uint8_t GDW376_1_AFN06H_102H(uint8_t* pBuf, uint16_t* len, uint8_t* pAckBuf, uint16_t* ack_pos)
{
	int ret = 0;
	int fd=-1;
	int n_sefile=0;
	unsigned char EsamId[8];


	fd = OpenComPort(fd ,"/dev/ttyS9");
	if(-1==fd) 
	{
		printf("[06F102]open uart failed\n");
		return -1;
	}
	n_sefile = 36*pBuf[0];
	if(0==TerminalCtAuthenticate(fd,EsamId))
	{
		ret = TAModuleKeyUpdate(fd,pBuf[0],&pBuf[1],n_sefile,pBuf+n_sefile+1,EsamId);
		
	}
	else
	{
		printf("authenticate failled\n");
		ret = -1;
	}
	return (ret? 0:1);
}

static uint8_t AFN_06H(uint8_t *pBuf, uint16_t len, int fd)
{
	stFrame13761Head *pFrame;
	uint8_t nRet = 0;	
	uint8_t ACKbuf[PROTOCOL_13761_BUF_SIZE];
	uint16_t ack_data_len = 0;
	uint8_t Fn = 0;
	uint16_t Pn = 0;
	uint8_t fail_cnt = 0;
	uint8_t fail = 0;
	uint8_t di_cnt = 0;
	uint16_t di_data_len = 0;
	uint8_t* dataend = NULL;
	stDataDI* di = NULL;
	if ((pBuf == NULL) || (len == 0))
	{
		return 0;
	}
	pFrame = (stFrame13761Head *)pBuf;
	if (pFrame->userdata.afn != AFN_0A_INQUIRY_PARM)
	{
		return 0;
	}
	
	dataend = getCmdDataEnd(pBuf, len);
	di = (stDataDI*)(pFrame->userdata.data);

	while((uint8_t*)&di[1] <= dataend)
	{
		fail = 0;
		di_data_len = 0;
		di_cnt++;
		nRet = getFn(di->DT, &Fn);
		if (nRet == 0)
		{
			fail = 1;
		}
		nRet = getPn(di->DA, &Pn);
		if (nRet == 0)
		{
			fail = 1;
		}
		if (Pn != 0)	//������ѯPn == 0
		{
			//fail = 1;
		}
		if (fail == 0)
		{
			stDataDI* pDI = (stDataDI*)&ACKbuf[ack_data_len];
			setFn(Fn, &pDI->DT);
			setPn(Pn, &pDI->DA);
			ack_data_len += sizeof(stDataDI);
			switch(Fn)
			{
				case 100: //��ȡ�ն˰�ȫ��֤��Ϣ
					nRet = GDW376_1_AFN06H_100H((uint8_t*)&di[1], &di_data_len, ACKbuf, &ack_data_len);
					break;
				case 101: //��ȡTAר��ģ�鰲ȫ��Ϣ
					nRet = GDW376_1_AFN06H_101H((uint8_t*)&di[1], &di_data_len, ACKbuf, &ack_data_len);
					break;
				case 102: //ģ��ESAM ��Կ����
					nRet = GDW376_1_AFN06H_102H((uint8_t*)&di[1], &di_data_len, ACKbuf, &ack_data_len);
					break;				
				default:
					nRet = 0;
					break;
			}
			if (nRet == 0)
			{
				ack_data_len -= sizeof(stDataDI);
			}
		}
		if ((nRet == 0) || (fail > 0))
		{
			fail_cnt++;
			
			break;			//����DI�����ж�
		}
		di = (stDataDI*)((uint8_t*)&di[1] + di_data_len);

		
	}
	
	//Ӧ��
	if (fail_cnt > 0)	//�д���Ӧ�����
	{
		nRet = sendAcktoMainStation(ACK_ALL_ERR, pBuf, len, ACKbuf, 0, fd);
	}
	else	//����Ӧ��
	{
		nRet = sendAckFrametoMainStation(LINK_CTRL_UP_RES_USERDATA, pBuf, len, ACKbuf, ack_data_len, fd);
		
	}
	
	return nRet;
}


static uint8_t GDW376_1_AFN0CH_50H(uint8_t* pBuf, uint16_t* len, uint8_t* pAckBuf, uint16_t* ack_pos)
{
    
}

//��ѯ����
static uint8_t AFN_0CH(uint8_t *pBuf, uint16_t len, int fd)
{
	stFrame13761Head *pFrame;
	uint8_t nRet = 0;	
	uint8_t ACKbuf[PROTOCOL_13761_BUF_SIZE];
	uint16_t ack_data_len = 0;
	uint8_t Fn = 0;
	uint16_t Pn = 0;
	uint8_t fail_cnt = 0;
	uint8_t fail = 0;
	uint8_t di_cnt = 0;
	uint16_t di_data_len = 0;
	uint8_t* dataend = NULL;
	stDataDI* di = NULL;
	if ((pBuf == NULL) || (len == 0))
	{
		return 0;
	}
	pFrame = (stFrame13761Head *)pBuf;
	if (pFrame->userdata.afn != AFN_0A_INQUIRY_PARM)
	{
		return 0;
	}
	
	dataend = getCmdDataEnd(pBuf, len);
	di = (stDataDI*)(pFrame->userdata.data);

	while((uint8_t*)&di[1] <= dataend)
	{
		fail = 0;
		di_data_len = 0;
		di_cnt++;
		nRet = getFn(di->DT, &Fn);
		if (nRet == 0)
		{
			fail = 1;
		}
		nRet = getPn(di->DA, &Pn);
		if (nRet == 0)
		{
			fail = 1;
		}
		if (Pn != 0)	//������ѯPn == 0
		{
			//fail = 1;
		}
		if (fail == 0)
		{
			stDataDI* pDI = (stDataDI*)&ACKbuf[ack_data_len];
			setFn(Fn, &pDI->DT);
			setPn(Pn, &pDI->DA);
			ack_data_len += sizeof(stDataDI);
			switch(Fn)
			{
				case 50: 	//����һ������
					nRet = GDW376_1_AFN0AH_01H((uint8_t*)&di[1], &di_data_len, ACKbuf, &ack_data_len);
					break;
				default:
					nRet = 0;
					break;
			}
			if (nRet == 0)
			{
				ack_data_len -= sizeof(stDataDI);
			}
		}
		if ((nRet == 0) || (fail > 0))
		{
			fail_cnt++;
			
			break;			//����DI�����ж�
		}
		di = (stDataDI*)((uint8_t*)&di[1] + di_data_len);

		
	}
	
	//Ӧ��
	if (fail_cnt > 0)	//�д���Ӧ�����
	{
		nRet = sendAcktoMainStation(ACK_ALL_ERR, pBuf, len, ACKbuf, 0, fd);
	}
	else	//����Ӧ��
	{
		nRet = sendAckFrametoMainStation(LINK_CTRL_UP_RES_USERDATA, pBuf, len, ACKbuf, ack_data_len, fd);
		
	}
	
	return nRet;
}

/*****************************************************************************************
��������:   sendAckFrametoMainStation
����˵��:   ����Ӧ��֡����վ
�������:	function:�����룬frame:�յ���֡��frame_len��֡��
            ACKdata:Ӧ������ݣ�ack_data_len�����ݳ���
�������:	0-ʧ�ܣ�1-�ɹ�
����˵��: 
*****************************************************************************************/
uint8_t sendAFN0EtoMainStation(uint8_t function, uint8_t *frame, uint16_t frame_len, uint8_t *ACKdata, uint16_t ack_data_len, int fd)
{
	uint8_t ACKbuf[PROTOCOL_13761_BUF_SIZE];		//���1200
	uint16_t pos = 0;
	stFrame13761Head *pFrame = NULL;
	stFrame13761Head *pAckFrame = NULL;
	if ((frame == NULL) || (frame_len == 0) || (ACKdata == NULL))
	{
		return 0;
	}
	
	//uint16_t ack_len = 0;
	pFrame = (stFrame13761Head *)frame;
	pAckFrame = (stFrame13761Head *)ACKbuf;
	//����Ӧ��֡
	pAckFrame->header1 = FRAME_START_13761;
	pAckFrame->header2 = FRAME_START_13761;
	if ((pFrame->addr.termAddr == 0xFFFF) && (pFrame->addr.mainStn_TermGrp_addr.ma_s.group == 1))	//�㲥֡
	{
		return 1;
	}
	memcpy((uint8_t*)&(pAckFrame->addr),(uint8_t*)&GetCiSysInfo()->usModleAddr,sizeof(MODLE_ADDR_STRU));
	pAckFrame->addr.mainStn_TermGrp_addr.value = pFrame->addr.mainStn_TermGrp_addr.value;
	/*
	pAckFrame->userdata.appseq.seq = pFrame->userdata.appseq.seq;		//����֡���
	pAckFrame->userdata.appseq.tpv = 1;									//�����¼���ǩ
	pAckFrame->userdata.appseq.fir = 1;									//��һ֡
	pAckFrame->userdata.appseq.fin = 0;			//1;					//���һ֡
	pAckFrame->userdata.appseq.con = 0;									//�費��Ҫȷ��
	pAckFrame->userdata.afn = pFrame->userdata.afn;						//
	*/

	pAckFrame->userdata.appseq.seq = pFrame->userdata.appseq.seq;		//����֡���
	pAckFrame->userdata.appseq.tpv = pFrame->userdata.appseq.tpv; 		//�����¼���ǩ
	pAckFrame->userdata.appseq.fir = pFrame->userdata.appseq.fir; 		//��һ֡
	pAckFrame->userdata.appseq.fin = pFrame->userdata.appseq.fin; 		//���һ֡
	pAckFrame->userdata.appseq.con = pFrame->userdata.appseq.con; 		//�費��Ҫȷ��
	pAckFrame->userdata.afn = pFrame->userdata.afn; 					

	
	pAckFrame->control.control_un.resp_s.dir = FRAME_13761_DIR_UP;
	pAckFrame->control.control_un.resp_s.prm = FRAME_13761_PRM_ACK_STATION;
	pAckFrame->control.control_un.resp_s.rsv = 0;
	pAckFrame->control.control_un.resp_s.acd = 1;
	pAckFrame->control.control_un.resp_s.code = function;		//pAckFrame->control.control_un.resp_s.code = function;
	
	pos = 0;
	memcpy(&(pAckFrame->userdata.data[pos]), ACKdata, ack_data_len);
	pos += ack_data_len;
	
	if (pAckFrame->control.control_un.resp_s.acd == 1)	//�����¼�
	{
		AppAuxEC Ec = {0};
		//AppAuxEC;
		pAckFrame->userdata.data[pos++]=Ec.AppAuxEC1;
		pAckFrame->userdata.data[pos++]=Ec.AppAuxEC2;
	}
	
	if (pAckFrame->userdata.appseq.tpv ==1)	//ʱ���ǩ
	{
		//AppAuxTp
		//memcpy(&(pAckFrame->userdata.data[pos]), (uint8_t*)(pFrame->userdata.data + pFrame->len.len_un.len_s.m_len - 
		//PROTOCOL_13761_CTR_SEQ_LEN - sizeof(AppAuxTp)), sizeof(AppAuxTp));
		
		memcpy(&(pAckFrame->userdata.data[pos]), (uint8_t*)(&pFrame->control + pFrame->len.len_un.len_s.m_len - sizeof(AppAuxTp)), sizeof(AppAuxTp));
		pos += sizeof(AppAuxTp);
	}

	
	pAckFrame->len.len_un.len_s.m_len = pos + PROTOCOL_13761_CTR_SEQ_LEN;
	pAckFrame->len.len_un.len_s.m_pid = PROTOCOL_13761_VALID;
	pAckFrame->len1.len_un.len_s.m_len = pAckFrame->len.len_un.len_s.m_len;
	pAckFrame->len1.len_un.len_s.m_pid = pAckFrame->len.len_un.len_s.m_pid;
	
	pAckFrame->userdata.data[pos++] = GDW376_1_CalSum((uint8_t *)&(pAckFrame->control), pAckFrame->len.len_un.len_s.m_len);
	
	pAckFrame->userdata.data[pos] = FRAME_END_13761;
	
	pos = pAckFrame->len.len_un.len_s.m_len + PROTOCOL_13761_HEAD_LEN + PROTOCOL_13761_TAIL_LEN;
	if (pos > PROTOCOL_13761_BUF_SIZE)
	{
		return 0;
	}
	
	return Protocl13761SendData2(ACKbuf, pos, fd);						//��ѯ������������վ-x
}

extern u8 m_EC1;
extern u8 m_EC2;
extern u8 EventBuf[80][528];
//������Ҫ�¼�
static uint8_t GDW376_1_AFN0EH_01H(uint8_t* pBuf, uint16_t* len, uint8_t *frame, uint16_t frame_len, int fd)
{
	if ((pBuf == NULL)||(len == NULL) || (frame == NULL))	// || (ack_pos == NULL))
	{
		return 0;
	}
	uint8_t pAckBuf[PROTOCOL_13761_BUF_SIZE] = {0};
	stFrame13761Head *pAckFrame = NULL;
	DATA_AFN0E_F1 *Point = (DATA_AFN0E_F1 *)pBuf;
	u8 m_Pm = Point->Pm;
	u8 m_Pn = Point->Pn;
	
	//�����ݿ��ȡ��Ҫ�¼�	
	int Event_sum = GetImportanceEvent(m_Pm, m_Pn);
	if (Event_sum < 0)//ʧ��
	{
		
	}

	//�������֡ͷ��
	EventsBlockHead head = {0};
	head.DA[0] = 0x00;
	head.DA[1] = 0x00;
	head.DT[0] = 0x01;
	head.DT[1] = 0x00;
	head.EC1 = m_EC1;
	head.EC2 = m_EC2;
	head.Pm = m_Pm;
	head.Pn = m_Pn;

	int sumlen = 0;
	memcpy(&pAckBuf[sumlen], &head, sizeof(head));

	sumlen += sizeof(head);
	

	pAckFrame = (stFrame13761Head *)frame;
	int FreamSeq = 0;
	
	int i;
	for(i=0; i<Event_sum; i++)
	{
		printf("EventBuf[%d] = %s\n",i,EventBuf[i]);

		
		//�������¼�������¼���¼�����Ƿ��Ӧ
		//parseEvent(EventBuf,  );
		
		int len = strlen(EventBuf[i]);
		
		if((sumlen+len/2)	 > 1200)		//���÷�֡
		{
			if(FreamSeq == 0)				//��һ֡
			{		
				//pAckFrame->userdata.appseq.seq = pFrame->userdata.appseq.seq;		//����֡���
				pAckFrame->userdata.appseq.tpv = 1;									//�����¼���ǩ
				pAckFrame->userdata.appseq.fir = 1;									//��һ֡
				pAckFrame->userdata.appseq.fin = 0;									//���һ֡				10��֡��һ֡
				pAckFrame->userdata.appseq.con = 0;									//�費��Ҫȷ��
				//pAckFrame->userdata.afn = pFrame->userdata.afn;				    
				 sendAFN0EtoMainStation(LINK_CTRL_UP_RES_USERDATA, (u8 *)pAckFrame, frame_len, pAckBuf, sumlen, fd);
				FreamSeq++;
				sumlen = sizeof(head);
			}
			else						   //�м�֡
			{
				FreamSeq++;
				pAckFrame->userdata.appseq.seq = pAckFrame->userdata.appseq.seq + 1; 
				pAckFrame->userdata.appseq.tpv = 1;										//�����¼���ǩ
				pAckFrame->userdata.appseq.fir = 0;										//��һ֡
				pAckFrame->userdata.appseq.fin = 0;										//���һ֡				00��֡�м�֡
				sendAFN0EtoMainStation(LINK_CTRL_UP_RES_USERDATA, (u8 *)pAckFrame, frame_len, pAckBuf, sumlen, fd);
				sumlen = sizeof(head);
			}
		}
		
		int m;
		for(m=0; m<len/2; m++)
		{
			sscanf(&EventBuf[i][m*2], "%02x", (int *)&pAckBuf[m+sumlen]);

		}	
		sumlen = sumlen + len/2;
	}

	if(FreamSeq > 0)		//��֡���һ֡
	{
		FreamSeq++;
		pAckFrame->userdata.appseq.seq = pAckFrame->userdata.appseq.seq + 1; 
		pAckFrame->userdata.appseq.tpv = 1;										//�����¼���ǩ
		pAckFrame->userdata.appseq.fir = 0;										//��һ֡
		pAckFrame->userdata.appseq.fin = 1;										//���һ֡				00��֡�м�֡
	}
	else					//��֡
	{
		//pAckFrame->userdata.appseq.seq = pAckFrame->userdata.appseq.seq + 1; 
		pAckFrame->userdata.appseq.tpv = 1;										//�����¼���ǩ
		pAckFrame->userdata.appseq.fir = 1;										//��һ֡
		pAckFrame->userdata.appseq.fin = 1;										//���һ֡				00��֡�м�֡
	}
	
	 sendAFN0EtoMainStation(LINK_CTRL_UP_RES_USERDATA, (u8 *)pAckFrame, frame_len, pAckBuf, sumlen, fd);

	//*ack_pos += sumlen;
	
	return 1;
}


//����һ���¼�
static uint8_t GDW376_1_AFN0EH_02H(uint8_t* pBuf, uint16_t* len, uint8_t *frame, uint16_t frame_len, int fd)
{
	if ((pBuf == NULL)||(len == NULL) || (frame == NULL))	// || (ack_pos == NULL))
	{
		return 0;
	}
	uint8_t pAckBuf[PROTOCOL_13761_BUF_SIZE] = {0};
	stFrame13761Head *pAckFrame = NULL;
	DATA_AFN0E_F1 *Point = (DATA_AFN0E_F1 *)pBuf;
	u8 m_Pm = Point->Pm;
	u8 m_Pn = Point->Pn;
	
	//�����ݿ��ȡһ���¼�	
	int Event_sum = GetordinaryEvent(m_Pm, m_Pn);
	
	if (Event_sum < 0)//ʧ��
	{
		
	}

	//�������֡ͷ��
	EventsBlockHead head = {0};
	head.DA[0] = 0x00;
	head.DA[1] = 0x00;
	head.DT[0] = 0x02;
	head.DT[1] = 0x00;
	head.EC1 = m_EC1;
	head.EC2 = m_EC2;
	head.Pm = m_Pm;
	head.Pn = m_Pn;

	int sumlen = 0;
	memcpy(&pAckBuf[sumlen], &head, sizeof(head));

	sumlen += sizeof(head);


	pAckFrame = (stFrame13761Head *)frame;
	int FreamSeq = 0;
	
	int i;
	for(i=0; i<Event_sum; i++)
	{
		printf("EventBuf[%d] = %s\n",i,EventBuf[i]);

		
		//�������¼�������¼���¼�����Ƿ��Ӧ
		//parseEvent(EventBuf,  );
		
		int len = strlen(EventBuf[i]);
		
		if((sumlen+len/2)	 > 35)		//���÷�֡
		{
			if(FreamSeq == 0)				//��һ֡
			{		
				//pAckFrame->userdata.appseq.seq = pFrame->userdata.appseq.seq;		//����֡���
				pAckFrame->userdata.appseq.tpv = 1;									//�����¼���ǩ
				pAckFrame->userdata.appseq.fir = 1;									//��һ֡
				pAckFrame->userdata.appseq.fin = 0;									//���һ֡				10��֡��һ֡
				pAckFrame->userdata.appseq.con = 0;									//�費��Ҫȷ��
				//pAckFrame->userdata.afn = pFrame->userdata.afn;				    
				 sendAFN0EtoMainStation(LINK_CTRL_UP_RES_USERDATA, (u8 *)pAckFrame, frame_len, pAckBuf, sumlen, fd);
				FreamSeq++;
				sumlen = sizeof(head);
			}
			else						   //�м�֡
			{
				FreamSeq++;
				pAckFrame->userdata.appseq.seq = pAckFrame->userdata.appseq.seq + 1; 
				pAckFrame->userdata.appseq.tpv = 1;										//�����¼���ǩ
				pAckFrame->userdata.appseq.fir = 0;										//��һ֡
				pAckFrame->userdata.appseq.fin = 0;										//���һ֡				00��֡�м�֡
				sendAFN0EtoMainStation(LINK_CTRL_UP_RES_USERDATA, (u8 *)pAckFrame, frame_len, pAckBuf, sumlen, fd);
				sumlen = sizeof(head);
			}
		}
		
		int m;
		for(m=0; m<len/2; m++)
		{
			sscanf(&EventBuf[i][m*2], "%02x", (int *)&pAckBuf[m+sumlen]);

		}	
		sumlen = sumlen + len/2;
	}

	if(FreamSeq > 0)		//��֡���һ֡
	{
		FreamSeq++;
		pAckFrame->userdata.appseq.seq = pAckFrame->userdata.appseq.seq + 1; 
		pAckFrame->userdata.appseq.tpv = 1;										//�����¼���ǩ
		pAckFrame->userdata.appseq.fir = 0;										//��һ֡
		pAckFrame->userdata.appseq.fin = 1;										//���һ֡				00��֡�м�֡
	}
	else					//��֡
	{
		//pAckFrame->userdata.appseq.seq = pAckFrame->userdata.appseq.seq + 1; 
		pAckFrame->userdata.appseq.tpv = 1;										//�����¼���ǩ
		pAckFrame->userdata.appseq.fir = 1;										//��һ֡
		pAckFrame->userdata.appseq.fin = 1;										//���һ֡				00��֡�м�֡
	}
	
	 sendAFN0EtoMainStation(LINK_CTRL_UP_RES_USERDATA, (u8 *)pAckFrame, frame_len, pAckBuf, sumlen, fd);

	//*ack_pos += sumlen;
	return 1;
}


//���¼�����ERC�����¼�
static uint8_t GDW376_1_AFN0EH_03H(uint8_t* pBuf, uint16_t* len, uint8_t *frame, uint16_t frame_len, int fd)
{
	if ((pBuf == NULL)||(len == NULL) || (frame == NULL))	// || (ack_pos == NULL))
	{
		return 0;
	}
	uint8_t pAckBuf[PROTOCOL_13761_BUF_SIZE] = {0};
	stFrame13761Head *pAckFrame = NULL;
	DATA_AFN0E_F3 *Point = (DATA_AFN0E_F3 *)pBuf;
	u8 ERC = Point->ERC;

	//����ERC�����ݿ��л�ȡ�¼�	
	int Event_sum = GetErcEvent(Point->ERC);

	printf("Event_sum = %d\n",Event_sum);
	if (Event_sum < 0)		//��ȡʧ��	
	{
		
	}

	//�������֡ͷ��
	EventReqReply head;	
	head.DA[0] = 0x00;
	head.DA[1] = 0x00;
	head.DT[0] = 0x04;
	head.DT[1] = 0x00;

	
	if(Point->Num < Event_sum)
		Event_sum = Point->Num;

	head.EventNum = Event_sum;

	int sumlen = 0;

	memcpy(&pAckBuf[sumlen], &head, sizeof(head));

	sumlen += sizeof(head);
	
	pAckFrame = (stFrame13761Head *)frame;
	int FreamSeq = 0;
	
	int i;
	for(i=0; i<Event_sum; i++)
	{
		printf("EventBuf[%d] = %s\n",i,EventBuf[i]);

		
		//�������¼�������¼���¼�����Ƿ��Ӧ
		//parseEvent(EventBuf,  );
		
		int len = strlen(EventBuf[i]);
		
		if((sumlen+len/2)	 > 1200)		//���÷�֡
		{
			if(FreamSeq == 0)				//��һ֡
			{		
				//pAckFrame->userdata.appseq.seq = pFrame->userdata.appseq.seq;		//����֡���
				pAckFrame->userdata.appseq.tpv = 1;									//�����¼���ǩ
				pAckFrame->userdata.appseq.fir = 1;									//��һ֡
				pAckFrame->userdata.appseq.fin = 0;									//���һ֡				10��֡��һ֡
				pAckFrame->userdata.appseq.con = 0;									//�費��Ҫȷ��
				//pAckFrame->userdata.afn = pFrame->userdata.afn;				    
				 sendAFN0EtoMainStation(LINK_CTRL_UP_RES_USERDATA, (u8 *)pAckFrame, frame_len, pAckBuf, sumlen, fd);
				FreamSeq++;
				sumlen = sizeof(head);
			}
			else						   //�м�֡
			{
				FreamSeq++;
				pAckFrame->userdata.appseq.seq = pAckFrame->userdata.appseq.seq + 1; 
				pAckFrame->userdata.appseq.tpv = 1;										//�����¼���ǩ
				pAckFrame->userdata.appseq.fir = 0;										//��һ֡
				pAckFrame->userdata.appseq.fin = 0;										//���һ֡				00��֡�м�֡
				sendAFN0EtoMainStation(LINK_CTRL_UP_RES_USERDATA, (u8 *)pAckFrame, frame_len, pAckBuf, sumlen, fd);
				sumlen = sizeof(head);
			}
		}
		
		int m;
		for(m=0; m<len/2; m++)
		{
			sscanf(&EventBuf[i][m*2], "%02x", (int *)&pAckBuf[m+sumlen]);

		}	
		sumlen = sumlen + len/2;
	}

	if(FreamSeq > 0)		//��֡���һ֡
	{
		FreamSeq++;
		pAckFrame->userdata.appseq.seq = pAckFrame->userdata.appseq.seq + 1; 
		pAckFrame->userdata.appseq.tpv = 1;										//�����¼���ǩ
		pAckFrame->userdata.appseq.fir = 0;										//��һ֡
		pAckFrame->userdata.appseq.fin = 1;										//���һ֡				00��֡�м�֡
	}
	else					//��֡
	{
		//pAckFrame->userdata.appseq.seq = pAckFrame->userdata.appseq.seq + 1; 
		pAckFrame->userdata.appseq.tpv = 1;										//�����¼���ǩ
		pAckFrame->userdata.appseq.fir = 1;										//��һ֡
		pAckFrame->userdata.appseq.fin = 1;										//���һ֡				00��֡�м�֡
	}
	
	 sendAFN0EtoMainStation(LINK_CTRL_UP_RES_USERDATA, (u8 *)pAckFrame, frame_len, pAckBuf, sumlen, fd);

	//*ack_pos += sumlen;

	return 1;
}



//�����¼�����
uint8_t AFN_0EH(uint8_t *pBuf, uint16_t len, int fd)
{
	stFrame13761Head *pFrame;
	uint8_t nRet = 0;	
	uint8_t ACKbuf[PROTOCOL_13761_BUF_SIZE];
	uint16_t ack_data_len = 0;
	uint8_t Fn = 0;
	uint16_t Pn = 0;
	uint8_t fail_cnt = 0;
	uint8_t fail = 0;
	uint8_t di_cnt = 0;
	uint16_t di_data_len = 0;
	uint8_t* dataend = NULL;
	stDataDI* di = NULL;
	if ((pBuf == NULL) || (len == 0))
	{
		return 0;
	}
	pFrame = (stFrame13761Head *)pBuf;
	if (pFrame->userdata.afn != AFN_0E_EVENT_DATA)
	{
		return 0;
	}
	
	dataend = getCmdDataEnd(pBuf, len);
	di = (stDataDI*)(pFrame->userdata.data);

	while((uint8_t*)&di[1] <= dataend)
	{
		fail = 0;
		di_data_len = 0;
		di_cnt++;
		nRet = getFn(di->DT, &Fn);
		if (nRet == 0)
		{
			fail = 1;
		}
		nRet = getPn(di->DA, &Pn);
		if (nRet == 0)
		{
			fail = 1;
		}
		if (Pn != 0)	//������ѯPn == 0
		{
			//fail = 1;
		}
		if (fail == 0)
		{
			stDataDI* pDI = (stDataDI*)&ACKbuf[ack_data_len];
			setFn(Fn, &pDI->DT);
			setPn(Pn, &pDI->DA);
			ack_data_len += sizeof(stDataDI);
			switch(Fn)
			{
				case 1: 	//F1������Ҫ�¼�
					nRet = GDW376_1_AFN0EH_01H((uint8_t*)&di[1], &di_data_len, pBuf, len, fd);
					break;
				case 2: 	//F2����һ���¼�
					//nRet = GDW376_1_AFN0EH_02H((uint8_t*)&di[1], &di_data_len, ACKbuf, &ack_data_len, pBuf, len);
					nRet = GDW376_1_AFN0EH_02H((uint8_t*)&di[1], &di_data_len, pBuf, len, fd);
					break;
				case 3: 	//F3���¼�����ERC�����¼�
					nRet = GDW376_1_AFN0EH_03H((uint8_t*)&di[1], &di_data_len, pBuf, len, fd);
					break;
				default:
					nRet = 0;
					break;
			}
			if (nRet == 0)
			{
				ack_data_len -= sizeof(stDataDI);
			}
		}
		if ((nRet == 0) || (fail > 0))
		{
			fail_cnt++;
			
			break;			//����DI�����ж�
		}
		di = (stDataDI*)((uint8_t*)&di[1] + di_data_len);

	}
	
	//Ӧ��
	if (fail_cnt > 0)	//�д���Ӧ�����
	{
		nRet = sendAcktoMainStation(ACK_ALL_ERR, pBuf, len, ACKbuf, 0, fd);
	}
	else	//����Ӧ��
	{
	//nRet = sendAFN0EtoMainStation(LINK_CTRL_UP_RES_USERDATA, pBuf, len, ACKbuf, ack_data_len, fd);
		
	}

	return nRet;
}



/*
u8 updatabuffer[2048]={0};
volatile u16 updata_offset=0;
volatile u16 updata_addr_cnt=0;

NV_UPDATA_FILE_FLAG fileFlag={0};
stTransferFile transferFile={0};
stRepeatUpgrade RepeatUpgrade = {0};

#define BIN_HEAD_LEN      16
*/

u8 updatabuffer[2048]={0};
volatile u16 updata_offset=0;
volatile u16 updata_addr_cnt=0;

NV_UPDATA_FILE_FLAG fileFlag={0};
stRepeatUpgrade RepeatUpgrade = {0};

#define BIN_HEAD_LEN      16


//
u8 ChargeSystemResetFlag=0;


u16 sys_EndianLittle2Local16(u8 *pFrom)
{
	u16 ret=0;

	ret = pFrom[1];
	ret <<= 8;
	ret |= pFrom[0];

	return ret;
}

u32 sys_EndianLittle2Local32(u8 *pFrom)
{
	u32 ret;

	ret = pFrom[3];
	ret <<= 8;
	ret |= pFrom[2];
	ret <<= 8;
	ret |= pFrom[1];
	ret <<= 8;
	ret |= pFrom[0];

	return ret;
}




u16 crc16 (u8 *pD, int len ,u16 crc)
{

    static u16 poly[2]={0, 0xa001};            
    volatile int i,j;
    volatile u16 temp = crc;

    for(j=len; j>0; j--)
    {
        unsigned char ds = *pD++;
        for(i=0; i<8; i++)
        {
            temp = (temp >> 1) ^ poly[(temp ^ ds ) & 1];
            ds = ds >> 1;
        }
    }
     return temp;
}


static uint8_t  GDW376_1_AFN0FH_01H(uint8_t* Pfrm,uint16_t frm_len,uint8_t* pBuf, uint16_t* len)
{
	stFlieTransmit* pPara = (stFlieTransmit *)pBuf;
	//NV_UPDATA_FILE_DATA fileData={0};
	* len=sizeof(stFlieTransmit)+pPara->dataLen;
	//u32 cnt=0;
	volatile  u16 i,ack_data_len=0;
	volatile  u16 end_offset = 0;
	volatile  u16 end_cnt = 0;
	volatile  u16 end_crc = 0xFFFF;
	volatile  u16 file_crc = 0;
	volatile  u32 file_size = 0;

	u8 ACKbuf[50];
	u8 di[4]={0};

	switch(pPara->Flag)
	{
		case 0://��������ļ��� �ָ�������ǰ״̬��
		{
			
		}
		break;
		case 1://�ն������ļ���
		{
			if(pPara->dataLen>460){
				break;
			}
				

			if(pPara->DI == 0){
				updata_offset = 0;
				updata_addr_cnt = 0;
				memset(updatabuffer,0,2048);

				fileFlag.uncopyflag = 0;
				memset(&fileFlag,0,sizeof(NV_UPDATA_FILE_FLAG));
				memcpy(fileFlag.fileHead,pPara->data,BIN_HEAD_LEN); //�������ļ��Ŀ�ʼ��16���ֽڴ洢��������ʹ��ID(2) + Version(4) + crc(2)
//				IAP_sector_write(NV_FILE_FLAG,0,0,sizeof(NV_UPDATA_FILE_FLAG),(u8 *)&fileFlag);
				
			}
		
			if((updata_offset+pPara->dataLen) <= 2048)
			{
				if(pPara->DI == 0)
				{
					memcpy(updatabuffer+updata_offset,pPara->data + BIN_HEAD_LEN,pPara->dataLen-BIN_HEAD_LEN);
					updata_offset += pPara->dataLen - BIN_HEAD_LEN;
				}
				else
				{
					memcpy(updatabuffer+updata_offset,pPara->data,pPara->dataLen);
					updata_offset += pPara->dataLen;
				}	
			}
			else
			{
			
				memcpy(updatabuffer+updata_offset,pPara->data,2048-updata_offset);
//				IAP_sector_write(NV_FILE_DATA+updata_addr_cnt,0,0,2048,updatabuffer);
				updata_addr_cnt += 2;
				
				
				memset(updatabuffer,0,2048);
				memcpy(updatabuffer,pPara->data+2048-updata_offset,pPara->dataLen+updata_offset-2048);
				updata_offset = pPara->dataLen+updata_offset-2048;
				
			}
			if(pPara->mode == END_FRAME)
			{
//					IAP_sector_write(NV_FILE_DATA+updata_addr_cnt,0,0,updata_offset,updatabuffer);
					updata_addr_cnt += 2;
					//end_offset = updata_offset;
					end_cnt = updata_addr_cnt/2;
					updata_offset = 0;
					updata_addr_cnt = 0;
			}
			di[2]=1;
			memcpy(ACKbuf,di,4);
			ack_data_len += 4;
			
			/*
				1.�����ļ���У��
				
				2.У��ɹ�---д��Ӧ�ı�־λ��ϢfileFlag��Ȼ�������������򣬻����·�ת������
				  У��ʧ��---�ظ���վ�����ݴ����д���ģ��˴������ļ�ʧ��
			*/
			if(pPara->mode == END_FRAME)
			{
				end_crc = 0xFFFF;
				//file_size = (fileFlag.fileHead[11]<<24 | fileFlag.fileHead[10]<<16 | fileFlag.fileHead[9]<<8 | fileFlag.fileHead[8]) ;
				file_size = sys_EndianLittle2Local32(&fileFlag.fileHead[8]);
				for(i=0;i<end_cnt;i++)
				{
					memset(updatabuffer,0x00,sizeof(updatabuffer));
//					IAP_sector_read(NV_FILE_DATA+i*2, 0, 0, 2048, updatabuffer);
					
					if(i == end_cnt -1)
					{
						end_offset = file_size - i*2048;
						end_crc = crc16(updatabuffer, end_offset, end_crc);
					}
					else
					{
                         end_crc = crc16(updatabuffer, 2048, end_crc);                          
                    }
				}

				//file_crc = fileFlag.fileHead[7]<<8 | fileFlag.fileHead[6];
				file_crc = sys_EndianLittle2Local16(&fileFlag.fileHead[6]);
				
				if(end_crc != file_crc)	//У������ʱ�򣬻ظ���վ0xFFFFFFFF
				{
				
					memset(ACKbuf+ack_data_len,0xFF,4);
					ack_data_len += 4;
					sendAckFrametoMainStation(LINK_CTRL_UP_RES_USERDATA, Pfrm, frm_len, ACKbuf, ack_data_len,1);
				
					memset(updatabuffer,0,2048);
					fileFlag.uncopyflag = 0;
					memset(&fileFlag,0,sizeof(NV_UPDATA_FILE_FLAG));
				}
				else	//У����ȷ���ظ���վ�յ��ĵ�ǰ�εı�ʶ
				{
				
					memcpy(ACKbuf+ack_data_len,&pPara->DI,4);
					ack_data_len += 4;
					sendAckFrametoMainStation(LINK_CTRL_UP_RES_USERDATA, Pfrm, frm_len, ACKbuf, ack_data_len,1);

					fileFlag.flag = 1;
					fileFlag.uncopyflag = 1;//δ����
					fileFlag.runTestflag = 0;
					fileFlag.updatasize = end_cnt*2;
					fileFlag.total_size = file_size;
//					IAP_sector_write(NV_FILE_FLAG,0,0,sizeof(NV_UPDATA_FILE_FLAG),(u8 *)&fileFlag);

					/*******��������stm32�Ĳ�����ʹϵͳ����bootloader������Ӧ����������******/
					ChargeSystemResetFlag = 1;
				}
			}
			else	//���յ����һ֡����֮ǰ��һֱ�ظ��յ��ĵ�ǰ�εı�ʶ				
			{
				memcpy(ACKbuf+ack_data_len,&pPara->DI,4);
				ack_data_len+=4;
				sendAckFrametoMainStation(LINK_CTRL_UP_RES_USERDATA, Pfrm, frm_len, ACKbuf, ack_data_len,1);
			}
		}
		break;
		case 2://Զ�̣����У�ͨ��ģ�������ļ���
		{

		}
		break;
		case 3://����ͨ��ģ�������ļ���
		case 4://�ӽڵ����оƬ�����ļ���
		case 5://�ӽڵ�ͨ��ģ�������ļ���
		{
			if(pPara->dataLen>460){
				break;
			}	

			if(pPara->DI == 0){
				updata_offset = 0;
				updata_addr_cnt = 0;
				memset(updatabuffer,0,2048);
				memset(&fileFlag,0,sizeof(NV_UPDATA_FILE_FLAG));
				memcpy(fileFlag.fileHead,pPara->data,BIN_HEAD_LEN); 
//				IAP_sector_write(NV_FILE_FLAG,0,0,sizeof(NV_UPDATA_FILE_FLAG),(u8 *)&fileFlag);
			}
		
			
			if((updata_offset+pPara->dataLen) <= 2048){
				
				if(pPara->DI == 0){
					memcpy(updatabuffer+updata_offset,pPara->data + BIN_HEAD_LEN,pPara->dataLen-BIN_HEAD_LEN);
					updata_offset += pPara->dataLen-BIN_HEAD_LEN;
				}else{
					memcpy(updatabuffer+updata_offset,pPara->data,pPara->dataLen);
					updata_offset += pPara->dataLen;
				}	
			}else{
			
				memcpy(updatabuffer+updata_offset,pPara->data,2048-updata_offset);
//				IAP_sector_write(NV_FILE_DATA+updata_addr_cnt,0,0,2048,updatabuffer);
				updata_addr_cnt += 2;
				
				memset(updatabuffer,0,2048);
				memcpy(updatabuffer,pPara->data+2048-updata_offset,pPara->dataLen+updata_offset-2048);
				updata_offset = pPara->dataLen+updata_offset-2048;	
			}

			if(pPara->mode == END_FRAME){
//				IAP_sector_write(NV_FILE_DATA+updata_addr_cnt,0,0,updata_offset,updatabuffer);
				updata_addr_cnt += 2;
				end_cnt = updata_addr_cnt/2;
				updata_offset = 0;
				updata_addr_cnt = 0;
//				IAP_sector_write(NV_FILE_FLAG,0,0,sizeof(NV_UPDATA_FILE_FLAG),(u8 *)&fileFlag);
			}
			di[2]=1;
			memcpy(ACKbuf,di,4);
			ack_data_len += 4;
			
			/*
				1.�����ļ���У��
				2.У��ɹ�---д��Ӧ�ı�־λ��ϢfileFlag��Ȼ�������������򣬻����·�ת������
				  У��ʧ��---�ظ���վ�����ݴ����д���ģ��˴������ļ�ʧ��
			*/
			if(pPara->mode == END_FRAME){
				end_crc = 0xFFFF;
				file_size = sys_EndianLittle2Local32(&fileFlag.fileHead[8]);
				for(i=0;i<end_cnt;i++){
					memset(updatabuffer,0x00,sizeof(updatabuffer));
//					IAP_sector_read(NV_FILE_DATA+i*2, 0, 0, 2048, updatabuffer);
					if(i == end_cnt -1){
						if(pPara->Flag == 4){
							end_offset = file_size + 16 - i*2048;
						}else{
							end_offset = file_size - i*2048;
						}
						
						end_crc = crc16(updatabuffer, end_offset,end_crc);
					}else{
						if(i == 0 && pPara->Flag == 4){
							end_crc = crc16(updatabuffer+16, 2048-16,end_crc);
						}else{
							end_crc = crc16(updatabuffer, 2048,end_crc);
						}	
					}
					
				}			
				file_crc = sys_EndianLittle2Local16(&fileFlag.fileHead[6]);

				if(end_crc != file_crc){//У������ʱ�򣬻ظ���վ0xFFFFFFFF,���ļ�����Ϊ����mcu��ʱ�򣬲���У�飬����ȥ��У��
				
					memset(ACKbuf+ack_data_len,0xFF,4);
					ack_data_len += 4;
					sendAckFrametoMainStation(LINK_CTRL_UP_RES_USERDATA, Pfrm, frm_len, ACKbuf, ack_data_len,1);
					memset(updatabuffer,0,2048);
					memset(&fileFlag,0,sizeof(NV_UPDATA_FILE_FLAG));
					memset(&RepeatUpgrade,0x0,sizeof(stRepeatUpgrade));
				}else{//У����ȷ���ظ���վ�յ��ĵ�ǰ�εı�ʶ
				
					memcpy(ACKbuf+ack_data_len,&pPara->DI,4);
					ack_data_len += 4;
					sendAckFrametoMainStation(LINK_CTRL_UP_RES_USERDATA, Pfrm, frm_len, ACKbuf, ack_data_len,1);

					/*�����ظ������ı��ݹ���*/
					
					RepeatUpgrade.flag = pPara->Flag;
					RepeatUpgrade.update_size = end_cnt*2;
					if(RepeatUpgrade.flag == 4){
						RepeatUpgrade.total_size = file_size + 16;
					}else{
						RepeatUpgrade.total_size = file_size;
					}
					memcpy(RepeatUpgrade.softVer,&fileFlag.fileHead[2],4);
					RepeatUpgrade.repeatUpgradeCount = 0;			
					/*******��ʼ��������ת��������******/
					/*����Ӧ��ת�������ļ��Ľṹ��*/
#if 0	

					memset(&transferFile,0x00,sizeof(stTransferFile));

					transferFile.total_size = RepeatUpgrade.total_size;
					transferFile.update_size = RepeatUpgrade.update_size;
					transferFile.flag = RepeatUpgrade.flag;

					
					transferFile.mode = START_AND_PROCESS_FRAME;
					transferFile.num = getFrameNum(transferFile.total_size ,transferFile.update_size);
					transferFile.di_num = 0;
					transferFile.data_len = AFTER_LEN_FRAME;	
					transferFile.repeat = 0;
					transferFile.condition = 0;
					transferFile.offset = 0;
					transferFile.cnt = 0;
					transferFile.remaine = transferFile.total_size - (transferFile.update_size/2 -1)*2048;
					transferFile.end_cnt = transferFile.remaine / AFTER_LEN_FRAME;
					transferFile.end_cnt_temp = 0;
					IAP_sector_read(NV_FILE_DATA+transferFile.cnt*2, 0, 0, 2048, transferFile.buf);

					//���͵�һ֡����16���ֽڵ����ݳ���
					memcpy(transferFile.data,fileFlag.fileHead,transferFile.data_len);
					transfer_file(transferFile.flag,transferFile.mode, transferFile.num , transferFile.di_num , transferFile.data, transferFile.data_len);
					transferFile.start_transfer = 1;
#else
					//startTransferUpgrade();		//������������û��
#endif
                                        
				}
	
			}else{//���յ����һ֡����֮ǰ��һֱ�ظ��յ��ĵ�ǰ�εı�ʶ				
				memcpy(ACKbuf+ack_data_len,&pPara->DI,4);
				ack_data_len+=4;
				sendAckFrametoMainStation(LINK_CTRL_UP_RES_USERDATA, Pfrm, frm_len, ACKbuf, ack_data_len,1);
			}
		}
		break;
		default:
			break;
	}

	return 1;

}



//�ļ�����
uint8_t AFN_0FH(uint8_t *pBuf, uint16_t len)
{
	stFrame13761Head *pFrame;
	uint8_t nRet = 0;	
	uint8_t Fn = 0;
	uint16_t Pn = 0;
	uint8_t fail_cnt = 0;
	uint8_t fail = 0;
	uint8_t di_cnt = 0;
	uint16_t di_data_len = 0;
	uint8_t di_arry[5*MAX_13761_DI_NUM];
	//uint8_t di_arry_2[5*MAX_13761_DI_NUM];
	uint8_t di_arry_len = 0;
	//uint8_t di_arry_len_2 = 0;
	uint8_t* dataend = NULL;
	stDataDI* di = NULL;
	uint8_t type = ACK_PART;//���ֳɹ�
	u16 i=0;
	if ((pBuf == NULL) || (len == 0))
	{
		return 0;
	}
	pFrame = (stFrame13761Head *)pBuf;
	if (pFrame->userdata.afn != AFN_0F_FILE_TRANSFER)
	{
		return 0;
	}
	
	dataend = getCmdDataEnd(pBuf, len);
	di = (stDataDI*)(pFrame->userdata.data);

	while((uint8_t*)&di[1] <= dataend)
	{
		fail = 0;
		di_data_len = 0;
		memcpy(&di_arry[5*di_cnt], (uint8_t*)&di[0], sizeof(stDataDI));
		di_arry[5*di_cnt + sizeof(stDataDI)] = ACK_ERR_OK;
		di_cnt++;
		nRet = getFn(di->DT, &Fn);
		if (nRet == 0)
		{
			fail = 1;
		}
		nRet = getPn(di->DA, &Pn);
		if (nRet == 0)
		{
			fail = 1;
		}
		if (Pn != 0)
		{
			fail = 1;
		}
		if (fail == 0)
		{
			switch(Fn)
			{
				case 1: //F1�ļ����䷽ʽ1
					nRet = GDW376_1_AFN0FH_01H(pBuf,len,(uint8_t*)&di[1], &di_data_len);
					break;

				default:
					nRet = 0;
					break;
			}
		}
		
		if ((nRet == 0) || (fail > 0))
		{
			fail_cnt++;
			di_arry[5*(di_cnt-1) + sizeof(stDataDI)] = ACK_ERR_OT;
			break;			//����DI�����ж�
		}
		di = (stDataDI*)((uint8_t*)&di[1] + di_data_len);
	}
	di_arry_len = 5 * di_cnt;
	
	//�ж��Ƿ��������������ǣ��洢

	
	//Ӧ��-���ݳɹ���Ӧ��
	if (fail_cnt == di_cnt) //Ӧ��ȫ����
	{
		type = ACK_ALL_ERR;
	}
	else if (fail_cnt == 0) //Ӧ��ȫȷ��
	{
		type = ACK_PART;
	}

	for(i=0;i<di_cnt;i++)
	{
		
		//stDataDI* di = NULL;
		di=(stDataDI*)&di_arry[i*5];
		if(di->DT.DT2==0x00)
		{
			//0F F1
			if(di->DT.DT1==0x01)
			{

				//if(di_cnt == 0x01)
				{
					return 0;
				}

			}
		}
				
	}
	nRet = sendAcktoMainStation(type, pBuf, len, di_arry, di_arry_len,1);
	
	return nRet;
}


















//��������
//LINE2906
uint8_t getBitNo(uint8_t v)
{
	switch(v)
	{
		case 1:
			return 0;
		case 2:
			return 1;
		case 4:
			return 2;
		case 8:
			return 3;
		case 16:
			return 4;
		case 32:
			return 5;
		case 64:
			return 6;
		case 128:
			return 7;
		default:
			return 255;
	}
}

/*****************************************************************************************
��������:   getFn
����˵��:   ��ȡFn
�������:	pDT:DT[],
�������:	Fn:ת�����Fn
����ֵ��	0-ʧ�ܣ�1-�ɹ�
����˵��:	Fn:F1~F248
*****************************************************************************************/
uint8_t getFn(stDT pDT, uint8_t *Fn)
{
	uint16_t tmp = 0;

	if (Fn == NULL)
	{
		return 0;
	}
	
	if (pDT.DT1 == 0)
	{
		return 0;
	}
	if (pDT.DT2 > 30)
	{
		return 0;
	}
	tmp = getBitNo(pDT.DT1);
	if (tmp > 7)
	{
		return 0;
	}
	*Fn = (uint8_t)(pDT.DT2 << 3) + tmp + 1;
	
	return 1;
}

/*****************************************************************************************
��������:   setFn
����˵��:   ����Fn
�������:	Fn
�������:	pDT
����ֵ��	0-ʧ�ܣ�1-�ɹ�
����˵��:	Fn:F1~F248
*****************************************************************************************/
uint8_t setFn(uint8_t Fn, stDT *pDT)
{
	if ((Fn == 0) || (Fn > 248))
	{
		return 0;
	}
	if (pDT == NULL)
	{
		return 0;
	}
	--Fn;
	pDT->DT1 = 1 << (Fn & 7);
	pDT->DT2 = Fn >> 3;
	
	return 1;
}

/*****************************************************************************************
��������:   getPn
����˵��:   ��ȡPn
�������:	pDA:DA[],
�������:	Pn:ת�����Pn
����ֵ��	0-ʧ�ܣ�1-�ɹ�
����˵��:	
*****************************************************************************************/
uint8_t getPn(stDA pDA, uint16_t *Pn)
{
	uint8_t tmp;

	if (Pn == NULL)
	{
		return 0;
	}
	if ((pDA.DA1 == 0) && (pDA.DA2 == 0))
	{
		*Pn = 0;
		return 1;
	}
	if (!pDA.DA1 || !pDA.DA2)
	{
		return 0;
	}
	tmp = getBitNo(pDA.DA1);
	if (tmp > 7)
	{
		return 0;
	}
	*Pn = (uint16_t)((pDA.DA2 - 1) << 3) + tmp + 1;
	
	return 1;
}

/*****************************************************************************************
��������:   setPn
����˵��:   ����Pn
�������:	Pn,
�������:	pDA:DA[]
����ֵ��	0-ʧ�ܣ�1-�ɹ�
����˵��:	Pn:0~2040
*****************************************************************************************/
uint8_t setPn(uint16_t Pn, stDA *pDA)
{
	if ((pDA == NULL) || (Pn > 2040))
	{
		return 0;
	}
	if (Pn == 0)
	{
		pDA->DA1 = 0;
		pDA->DA2 = 0;
		return 1;
	}
	--Pn;
	pDA->DA1 = 1 << (Pn & 7);
	pDA->DA2 = (Pn >> 3) + 1;
	
	return 1;
}

//LINE 3169
/*****************************************************************************************
��������:   sendAcktoMainStation
����˵��:   ����ȷ��/���ϵ���վ
�������:	type:Ӧ������ACK_ALL_OK,ACK_ALL_ERR,ACK_PART 
            frame:�յ���֡��frame_len��֡��
�������:	0-ʧ�ܣ�1-�ɹ�
����˵��: 
*****************************************************************************************/
uint8_t sendAcktoMainStation(uint8_t type, uint8_t *frame, uint16_t frame_len, uint8_t *di_res, uint8_t di_len, int fd)
{
	uint8_t ACKbuf[PROTOCOL_13761_BUF_SIZE];
	stDataDI data_di;
	uint16_t pos = 0;
	stFrame13761Head *pFrame = NULL;
	stFrame13761Head *pAckFrame = NULL;
	
	if ((frame == NULL) || (frame_len == 0))
	{
		return 0;
	}
	if ((type < ACK_ALL_OK) || (type > ACK_PART))
	{
		return 0;
	}
	
	pFrame = (stFrame13761Head *)frame;
	pAckFrame = (stFrame13761Head *)ACKbuf;

	//����Ӧ��֡
	pAckFrame->header1 = FRAME_START_13761;
	pAckFrame->header2 = FRAME_START_13761;
	if ((pFrame->addr.termAddr == 0xFFFF) && (pFrame->addr.mainStn_TermGrp_addr.ma_s.group == 1))	//�㲥֡
	{
		return 1;
	}

	//control
	pAckFrame->control.control_un.resp_s.dir = FRAME_13761_DIR_UP;
	pAckFrame->control.control_un.resp_s.prm = FRAME_13761_PRM_ACK_STATION;
	pAckFrame->control.control_un.resp_s.rsv = 0;
	pAckFrame->control.control_un.resp_s.acd = 1;	//�¼�����
	pAckFrame->control.control_un.resp_s.code = LINK_CTRL_UP_CONFIRM;
	//addr
	memcpy((uint8_t*)&(pAckFrame->addr),(uint8_t*)&GetCiSysInfo()->usModleAddr,sizeof(MODLE_ADDR_STRU));
	pAckFrame->addr.mainStn_TermGrp_addr.value = pFrame->addr.mainStn_TermGrp_addr.value;
	//afn
	pAckFrame->userdata.afn = AFN_00_CONFIRM;
	//seq
	pAckFrame->userdata.appseq.seq = pFrame->userdata.appseq.seq;
	pAckFrame->userdata.appseq.tpv = 1;//ʱ���ǩ
	pAckFrame->userdata.appseq.fir = 1;//��֡
	pAckFrame->userdata.appseq.fin = 1;//��֡
	pAckFrame->userdata.appseq.con = 0;//����ȷ��
	
	//da,dt 
	setPn(0, &data_di.DA);
	setFn(type, &data_di.DT);
	memcpy(&(pAckFrame->userdata.data[0]), (uint8_t*)&(data_di), sizeof(stDataDI));
	pos += sizeof(stDataDI);

	
	if (type == ACK_PART)//���ַ���
	{

		pAckFrame->userdata.data[pos++]=pFrame->userdata.afn;//���ݵ�Ԫ��1�ֽ�Ϊ��ȷ�ϵı��ĵ�AFN
		memcpy(&(pAckFrame->userdata.data[pos]), di_res, di_len);
		pos += di_len;
	}
	
	if (pAckFrame->control.control_un.resp_s.acd == 1)	//�����¼�
	{
		AppAuxEC Ec = {0};
		//AppAuxEC;
		pAckFrame->userdata.data[pos++]=Ec.AppAuxEC1;
		pAckFrame->userdata.data[pos++]=Ec.AppAuxEC2;
	}
	
	if (pAckFrame->userdata.appseq.tpv ==1)	//ʱ���ǩ
	{
		//AppAuxTp
		//memcpy(&(pAckFrame->userdata.data[pos]), (uint8_t*)(pFrame->userdata.data + pFrame->len.len_un.len_s.m_len - 
		//PROTOCOL_13761_CTR_SEQ_LEN - sizeof(AppAuxTp)), sizeof(AppAuxTp));
		
		memcpy(&(pAckFrame->userdata.data[pos]), (uint8_t*)(&pFrame->control + pFrame->len.len_un.len_s.m_len - sizeof(AppAuxTp)), sizeof(AppAuxTp));
		pos += sizeof(AppAuxTp);
	}

	//len
	pAckFrame->len.len_un.len_s.m_len = pos + PROTOCOL_13761_CTR_SEQ_LEN;
	pAckFrame->len.len_un.len_s.m_pid = PROTOCOL_13761_VALID;
	pAckFrame->len1.len_un.len_s.m_len = pAckFrame->len.len_un.len_s.m_len;
	pAckFrame->len1.len_un.len_s.m_pid = pAckFrame->len.len_un.len_s.m_pid;

	//ck
	pAckFrame->userdata.data[pos++] = GDW376_1_CalSum((uint8_t *)&(pAckFrame->control), pAckFrame->len.len_un.len_s.m_len);
	//end
	pAckFrame->userdata.data[pos] = FRAME_END_13761;
	
	pos = pAckFrame->len.len_un.len_s.m_len + PROTOCOL_13761_HEAD_LEN + PROTOCOL_13761_TAIL_LEN;
	if (pos > PROTOCOL_13761_BUF_SIZE)
	{
		return 0;
	}
	
	return Protocl13761SendData2(ACKbuf, pos, fd);
}


/*****************************************************************************************
��������:   sendAckFrametoMainStation
����˵��:   ����Ӧ��֡����վ
�������:	function:�����룬frame:�յ���֡��frame_len��֡��
            ACKdata:Ӧ������ݣ�ack_data_len�����ݳ���
�������:	0-ʧ�ܣ�1-�ɹ�
����˵��: 
*****************************************************************************************/
uint8_t sendAckFrametoMainStation(uint8_t function, uint8_t *frame, uint16_t frame_len, uint8_t *ACKdata, uint16_t ack_data_len, int fd)
{
	uint8_t ACKbuf[PROTOCOL_13761_BUF_SIZE];		//���1200
	uint16_t pos = 0;
	stFrame13761Head *pFrame = NULL;
	stFrame13761Head *pAckFrame = NULL;
	if ((frame == NULL) || (frame_len == 0) || (ACKdata == NULL))
	{
		return 0;
	}
	
	//uint16_t ack_len = 0;
	pFrame = (stFrame13761Head *)frame;
	pAckFrame = (stFrame13761Head *)ACKbuf;
	//����Ӧ��֡
	pAckFrame->header1 = FRAME_START_13761;
	pAckFrame->header2 = FRAME_START_13761;
	if ((pFrame->addr.termAddr == 0xFFFF) && (pFrame->addr.mainStn_TermGrp_addr.ma_s.group == 1))	//�㲥֡
	{
		return 1;
	}
	memcpy((uint8_t*)&(pAckFrame->addr),(uint8_t*)&GetCiSysInfo()->usModleAddr,sizeof(MODLE_ADDR_STRU));
	pAckFrame->addr.mainStn_TermGrp_addr.value = pFrame->addr.mainStn_TermGrp_addr.value;
	pAckFrame->userdata.appseq.seq = pFrame->userdata.appseq.seq;
	pAckFrame->userdata.appseq.tpv = 1;
	pAckFrame->userdata.appseq.fir = 1;
	pAckFrame->userdata.appseq.fin = 1;
	pAckFrame->userdata.appseq.con = 0;
	pAckFrame->userdata.afn = pFrame->userdata.afn;
	
	pAckFrame->control.control_un.resp_s.dir = FRAME_13761_DIR_UP;
	pAckFrame->control.control_un.resp_s.prm = FRAME_13761_PRM_ACK_STATION;
	pAckFrame->control.control_un.resp_s.rsv = 0;
	pAckFrame->control.control_un.resp_s.acd = 1;
	pAckFrame->control.control_un.resp_s.code = function;
	
	pos = 0;
	memcpy(&(pAckFrame->userdata.data[pos]), ACKdata, ack_data_len);
	pos += ack_data_len;
	
	if (pAckFrame->control.control_un.resp_s.acd == 1)	//�����¼�
	{
		AppAuxEC Ec = {0};
		//AppAuxEC;
		pAckFrame->userdata.data[pos++]=Ec.AppAuxEC1;
		pAckFrame->userdata.data[pos++]=Ec.AppAuxEC2;
	}
	
	if (pAckFrame->userdata.appseq.tpv ==1)	//ʱ���ǩ
	{
		//AppAuxTp
		//memcpy(&(pAckFrame->userdata.data[pos]), (uint8_t*)(pFrame->userdata.data + pFrame->len.len_un.len_s.m_len - 
		//PROTOCOL_13761_CTR_SEQ_LEN - sizeof(AppAuxTp)), sizeof(AppAuxTp));
		
		memcpy(&(pAckFrame->userdata.data[pos]), (uint8_t*)(&pFrame->control + pFrame->len.len_un.len_s.m_len - sizeof(AppAuxTp)), sizeof(AppAuxTp));
		pos += sizeof(AppAuxTp);
	}

	
	pAckFrame->len.len_un.len_s.m_len = pos + PROTOCOL_13761_CTR_SEQ_LEN;
	pAckFrame->len.len_un.len_s.m_pid = PROTOCOL_13761_VALID;
	pAckFrame->len1.len_un.len_s.m_len = pAckFrame->len.len_un.len_s.m_len;
	pAckFrame->len1.len_un.len_s.m_pid = pAckFrame->len.len_un.len_s.m_pid;
	
	pAckFrame->userdata.data[pos++] = GDW376_1_CalSum((uint8_t *)&(pAckFrame->control), pAckFrame->len.len_un.len_s.m_len);
	
	pAckFrame->userdata.data[pos] = FRAME_END_13761;
	
	pos = pAckFrame->len.len_un.len_s.m_len + PROTOCOL_13761_HEAD_LEN + PROTOCOL_13761_TAIL_LEN;
	if (pos > PROTOCOL_13761_BUF_SIZE)
	{
		return 0;
	}
	
	return Protocl13761SendData2(ACKbuf, pos, fd);						//��ѯ������������վ-x
}

s32 Protocl13761SendData2(u8 *pSendData, u16 nSendLen, int fd)
{
	SendData(fd, pSendData, nSendLen);
	return 0;
}


s32 Protocl13761ParseData(u8 *pRecvData, u16 nRecvLen, int fd)
{
//	u16 noffent=0;
//	s16 nDataLen=0;
    //    u8 data[1024]={0};
	stFrame13761Head *pFrame;
	//GDW376_1_FRAME_DATA RxFrameData;
	//memcpy(data,pRecvData,nRecvLen);
	u8 *pdata = GDW376_1_CheckOutFrm2(pRecvData,nRecvLen);	//f��������������֡����ʵ��ַ0x68��ͷ

	if(NULL == pdata)
	{
		return ERR_INVALID_DATAUNIT;
	}
	
   // PRINT_OUT(pRecvData,nRecvLen);
	pFrame = (stFrame13761Head *)pdata;
    sTermSysInfo *pPara = GetCiSysInfo();
	printf("[3761Debug]afn:%d\n",pFrame->userdata.afn);
	if (pFrame->control.control_un.act_s.dir != FRAME_13761_DIR_DOWN)	//����վ����
	{
		return 0;
	}
	if ((pFrame->addr.districtCode != pPara->usModleAddr.districtCode) || 
	    (pFrame->addr.termAddr != pPara->usModleAddr.termAddr && pFrame->addr.termAddr != 0xFFFF))	//�ն˵�ַ��ƥ��
	{
		//return 0;
	}
	//gprsLedRX.led=1;
	switch(pFrame->userdata.afn)	//afn + seq + DA1+DA2+DT1+DT2 + ���ݵ�Ԫ
	{
		case AFN_00_CONFIRM:					//ȷ�Ϸ���֡
			AFN_00H(pRecvData, nRecvLen);
			break;
		case AFN_01_RESET:
			AFN_01H(pRecvData, nRecvLen, fd);
			break;
		case AFN_02_LINK_TEST:
			break;
		case AFN_03_RELAYSTATION:
			
			break;
		case AFN_04_SETUP_PARM:					//���ò���
			AFN_04H(pRecvData, nRecvLen,fd);
			break;
		case AFN_05_CONTROL:
			AFN_05H(pRecvData, nRecvLen, fd);
			break;
		case AFN_06_AUTHENTICATION:
			break;
		case AFN_09_TMCONFIG:				//�ն�������Ϣ
			AFN_09H(pRecvData, nRecvLen, fd);
			break;
		case AFN_0A_INQUIRY_PARM:				//��ѯ����
			AFN_0AH(pRecvData, nRecvLen, fd);
			break;

		case AFN_0C_REALTIME_DATA:
			AFN_0CH(pRecvData, nRecvLen,fd);
			break;

		case AFN_0E_EVENT_DATA:					//��ѯ�¼������
			printf("chxunshijian****\n\n\n");
			AFN_0EH(pRecvData, nRecvLen,fd);
			break;
		case AFN_0F_FILE_TRANSFER:
			AFN_0FH(pRecvData, nRecvLen);
			break;
		//case AFN_F0_CHARGE_TEST:
			//AFN_F0H(pRecvData, nRecvLen);
		default:
			break;
	}
	return 0;
}



