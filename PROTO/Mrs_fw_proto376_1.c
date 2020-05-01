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




stFrame13761AddrType AddrType={0};
CIRCUIT_SYSTEM_INFO_STRU gChargeSystemInfo={0};
TermEventRecordSet TermEventRecord = {0};


/**
  * @brief   376.1Э�����У���
  * @param [in] pBuf��������ָ��
  * @param [in] len������������
  * @retval  u8��У���
  */
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

/**
  * @brief   ���376.1��Լ����֡
  * @param [in] pBuf��������ָ��
  * @param [in] len�����ݳ���
  * @retval  u8*���޺Ϸ�֡�򷵻�NULL
  */
u8* GDW376_1_CheckOutFrm2(u8* pBuf, u16 len)
{
	register u16 i;
	//register u16 FrmLen;
	register u8* p;
	register stFrame13761Len *FrmLen;
	//register u16 Len;

	if(pBuf == NULL)
		return(NULL);
	if(len < 6)
		return(NULL);
	for(i = 0; i < len - 6; i++)
	{
		p = pBuf + i;
		if(*p != 0x68)
			continue;
		FrmLen = (stFrame13761Len *)(p + 1);
		len=FrmLen->len_un.len_s.m_len;
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
#define STATION_CONFIG_PATH    "./config/station_ipport.conf"

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
    ret = SaveConfMultiGroup(TERM698_CONF_FILE_PATH,pData,i);
    *len = sizeof(stTermUpCommuPara);
	
	return (ret==0)? 1:0;    
}

//afn =04 f3
//#pragma optimize=none
//��վ����
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
    ret = SaveConfMultiGroup(TERM698_CONF_FILE_PATH,pData,i);
    *len = sizeof(stMainStationIPPort);
	
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
        "CS_F7_TERMPORT",      val[12],            ":",
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
    ret = SaveConfMultiGroup(TERM698_CONF_FILE_PATH,pData,i);
    *len = sizeof(stMainStationIPPort);
	
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
    if(ret||(i>9)) return 0;
    ret = SaveConfMultiGroup(TERM698_CONF_FILE_PATH,pData,i);
	*len = sizeof(sTermWorkWaysUpComm);
 
	return (ret==0)? 1:0;     
}


//F9�ն��¼���¼��������
static uint8_t GDW376_1_AFN04H_09H(uint8_t* pBuf, uint16_t* len)
{
    if(!pBuf||!len) return 0;
	TermEventRecordSet* pPara = (TermEventRecordSet *)pBuf;

    if(!pPara) return 0;

	memcpy(&TermEventRecord, pPara, sizeof(TermEventRecord));
	u8 buf[20] = {0};
	itoa(TermEventRecord.CS_F9_EventRecordValidFlag[0], buf, 10);
	saveConfPara(TERM698_CONF_FILE_PATH, "CS_F9_EVENTRECORDVALIDFLAG_1", buf,	 ':');

	memset(buf,0,sizeof(buf));
	itoa(TermEventRecord.CS_F9_EventRecordValidFlag[1], buf, 10);
	saveConfPara(TERM698_CONF_FILE_PATH, "CS_F9_EVENTRECORDVALIDFLAG_2", buf,	 ':');

	memset(buf,0,sizeof(buf));
	itoa(TermEventRecord.CS_F9_EventRecordValidFlag[2], buf, 10);
	saveConfPara(TERM698_CONF_FILE_PATH, "CS_F9_EVENTRECORDVALIDFLAG_3", buf,	 ':');

	memset(buf,0,sizeof(buf));
	itoa(TermEventRecord.CS_F9_EventRecordValidFlag[3], buf, 10);
	saveConfPara(TERM698_CONF_FILE_PATH, "CS_F9_EVENTRECORDVALIDFLAG_4", buf,	 ':');

	memset(buf,0,sizeof(buf));
	itoa(TermEventRecord.CS_F9_EventRecordValidFlag[4], buf, 10);
	saveConfPara(TERM698_CONF_FILE_PATH, "CS_F9_EVENTRECORDVALIDFLAG_5", buf,	 ':');


	memset(buf,0,sizeof(buf));
	itoa(TermEventRecord.CS_F9_EventRecordValidFlag[5], buf, 10);
	saveConfPara(TERM698_CONF_FILE_PATH, "CS_F9_EVENTRECORDVALIDFLAG_6", buf,	 ':');

	memset(buf,0,sizeof(buf));
	itoa(TermEventRecord.CS_F9_EventRecordValidFlag[6], buf, 10);
	saveConfPara(TERM698_CONF_FILE_PATH, "CS_F9_EVENTRECORDVALIDFLAG_7", buf,	 ':');

	memset(buf,0,sizeof(buf));
	itoa(TermEventRecord.CS_F9_EventRecordValidFlag[7], buf, 10);
	saveConfPara(TERM698_CONF_FILE_PATH, "CS_F9_EVENTRECORDVALIDFLAG_8", buf,	 ':');


	memset(buf,0,sizeof(buf));
	itoa(TermEventRecord.CS_F9_EventRatingFlag[0], buf, 10);
	saveConfPara(TERM698_CONF_FILE_PATH, "CS_F9_EVENTRATINGFLAG_1", buf,	 ':');

	memset(buf,0,sizeof(buf));
	itoa(TermEventRecord.CS_F9_EventRatingFlag[1], buf, 10);
	saveConfPara(TERM698_CONF_FILE_PATH, "CS_F9_EVENTRATINGFLAG_2", buf,	 ':');

	memset(buf,0,sizeof(buf));
	itoa(TermEventRecord.CS_F9_EventRatingFlag[2], buf, 10);
	saveConfPara(TERM698_CONF_FILE_PATH, "CS_F9_EVENTRATINGFLAG_3", buf,	 ':');

	memset(buf,0,sizeof(buf));
	itoa(TermEventRecord.CS_F9_EventRatingFlag[3], buf, 10);
	saveConfPara(TERM698_CONF_FILE_PATH, "CS_F9_EVENTRATINGFLAG_4", buf,	 ':');


	memset(buf,0,sizeof(buf));
	itoa(TermEventRecord.CS_F9_EventRatingFlag[4], buf, 10);
	saveConfPara(TERM698_CONF_FILE_PATH, "CS_F9_EVENTRATINGFLAG_5", buf,	 ':');


	memset(buf,0,sizeof(buf));
	itoa(TermEventRecord.CS_F9_EventRatingFlag[5], buf, 10);
	saveConfPara(TERM698_CONF_FILE_PATH, "CS_F9_EVENTRATINGFLAG_6", buf,	 ':');


	memset(buf,0,sizeof(buf));
	itoa(TermEventRecord.CS_F9_EventRatingFlag[6], buf, 10);
	saveConfPara(TERM698_CONF_FILE_PATH, "CS_F9_EVENTRATINGFLAG_7", buf,	 ':');

	memset(buf,0,sizeof(buf));
	itoa(TermEventRecord.CS_F9_EventRatingFlag[7], buf, 10);
	saveConfPara(TERM698_CONF_FILE_PATH, "CS_F9_EVENTRATINGFLAG_8", buf,	 ':');

	*len = sizeof(stMainStationIPPort);
	return 1;
}





//�ն��߼���ַ(�������롢�ն��߼���ַ)
static uint8_t GDW376_1_AFN04H_89H(uint8_t* pBuf, uint16_t* len)
{
    
	MODLE_ADDR_STRU* pPara = (MODLE_ADDR_STRU *)pBuf;
	if ((pBuf == NULL)||(len == NULL))
	{
		return 0;
	}	

	char databuf[256];
	memset(databuf, 0, 256);
	gChargeSystemInfo.usModleAddr.districtCode = pPara->districtCode;		//�߼���ַ-��������
	gChargeSystemInfo.usModleAddr.termAddr = pPara->termAddr;				//�߼���ַ-�ն˵�ַ

	char buf1[20] = {0};
	char buf2[20] = {0};
	itoa(gChargeSystemInfo.usModleAddr.districtCode, buf1, 10);
	itoa(gChargeSystemInfo.usModleAddr.termAddr, buf2, 10);

	printf("buf1 = %s\n\n\n", buf1);
	printf("buf2 = %s\n\n\n", buf2);
	saveConfPara(TERM698_CONF_FILE_PATH, "AREA_CODE",     buf1, ':');  		//д����������
	saveConfPara(TERM698_CONF_FILE_PATH, "TERMINAL_CODE", buf2, ':');  		//д���ն˵�ַ

	*len = sizeof(MODLE_ADDR_STRU);
	return 1;
}



//�ն˵���λ��
static uint8_t GDW376_1_AFN04H_91H(uint8_t* pBuf, uint16_t* len)
{
	stTermLocation* pPara = (stTermLocation *)pBuf;
	if ((pBuf == NULL)||(len == NULL))
	{
		return 0;
	}	

	UINT8 degree = 0, branch =0;
	float second = 0;
	char databuf[256];
	memset(databuf, 0, 256);
	memcpy(&gChargeSystemInfo.Location, pPara, sizeof(stTermLocation));

	degree = gChargeSystemInfo.Location.latitude_D_BW*100 + gChargeSystemInfo.Location.latitude_D_SW*10+ gChargeSystemInfo.Location.latitude_D_GW;
	branch = gChargeSystemInfo.Location.latitude_B_SW*10 + gChargeSystemInfo.Location.latitude_B_GW;
	second = gChargeSystemInfo.Location.latitude_S_SW*10 + gChargeSystemInfo.Location.latitude_S_GW + gChargeSystemInfo.Location.latitude_S_SFW/10 + gChargeSystemInfo.Location.latitude_S_BFW/100;
	
	if(gChargeSystemInfo.Location.latitude_S_BFW >0 || gChargeSystemInfo.Location.latitude_S_BFW>0)
	{
		if(gChargeSystemInfo.Location.latitude_D_BLW == 8)		//��γ(S)
		{
			sprintf(databuf,"S%d��%d��%.2f��",degree, branch, second);
			printf("1 = %s\n", databuf);
		}
		else //��γ(N)
		{
			sprintf(databuf,"N%d��%d��%.2f��",degree, branch, second);
			printf("2 = %s\n", databuf);
		}
	}
	else
	{
		if(gChargeSystemInfo.Location.latitude_D_BLW == 8)		//��γ(S)
		{
			sprintf(databuf,"S%d��%d��%.0f��",degree, branch, second);
			printf("3 = %s\n", databuf);
			printf("����\n");
		}
		else    //��γ(N)
		{
			sprintf(databuf,"N%d��%d��%.0f��",degree, branch, second);
			printf("4 = %s\n", databuf);
		}
	}
	saveConfPara(TERM698_CONF_FILE_PATH, "CS_F91_DIMENSION", databuf, ':');

	memset(databuf, 0, sizeof(databuf));
	degree = gChargeSystemInfo.Location.longitude_D_BW*100 + gChargeSystemInfo.Location.longitude_D_SW+ gChargeSystemInfo.Location.longitude_D_GW;
	branch = gChargeSystemInfo.Location.longitude_B_SW*10 + gChargeSystemInfo.Location.longitude_B_GW;
	second = gChargeSystemInfo.Location.longitude_S_SW*10 + gChargeSystemInfo.Location.longitude_S_GW + gChargeSystemInfo.Location.longitude_S_SFW/10 + gChargeSystemInfo.Location.longitude_S_BFW/100;
	if(gChargeSystemInfo.Location.longitude_S_BFW >0 || gChargeSystemInfo.Location.longitude_S_BFW>0)
	{
		if(gChargeSystemInfo.Location.longitude_S_BFW == 8)		//����(W)
		{
			sprintf(databuf,"W%d��%d��%.2f��",degree, branch, second);
		}
		else    //����(E)
		{
			sprintf(databuf,"E%d��%d��%.2f��",degree, branch, second);
		}	
	}
	else
	{
		if(gChargeSystemInfo.Location.longitude_S_BFW == 8)		//����(W)
		{
			sprintf(databuf,"W%d��%d��%.0f��",degree, branch, second);
		}
		else    //����(E)
		{
			sprintf(databuf,"E%d��%d��%.0f��",degree, branch, second);
		}	
	}
	saveConfPara(TERM698_CONF_FILE_PATH, "CS_F91_LONGITUDE", databuf, 	':');

	*len = sizeof(stMainStationIPPort);
	return 1;
}


//���õ�����·ʹ�ܲ���
#define CURRENT_CIRCUIT_ENABLE_PATH    "./config/current_circuit_param.conf"
/*************************************************
�� �� ��:get_current_circuit_enable
��������:��ȡA/B/C���������·ʹ�ܲ���
��    ��: ��
��    ��: ��
��    ��: 0:�ɹ�	-1:ʧ��
*************************************************/
int get_current_circuit_enable(current_circuit_param_stru *currcirparam)
{
	unsigned char phase_a_curr = 0;
	unsigned char phase_b_curr = 0;	
	unsigned char phase_c_curr = 0;
	unsigned char bvalid = 1;
    unsigned char currentbuf[256];
    char *pos = NULL;
    FILE *fd;
    
    memset(currentbuf, 0, sizeof(currentbuf));
    fd = fopen(CURRENT_CIRCUIT_ENABLE_PATH,"r");    
    if(fd<=0)    //�ļ������ڣ���ʼ��
    {
        fd = fopen(CURRENT_CIRCUIT_ENABLE_PATH,"w+");
        if(fd<=0)
        {
            printf("Creat current_circuit_param.conf failed\n");
            return -1;
        }        
        sprintf(currentbuf, "phase_a:%d,phase_b:%d,phase_c:%d\n", phase_a_curr, phase_b_curr, phase_c_curr);
        fwrite((char *)&currentbuf, strlen(currentbuf), 1,  fd);
        printf("Creat and Init ./config/current_circuit_param.conf success\n");
		currcirparam->phase_a_curr = phase_a_curr;
		currcirparam->phase_b_curr = phase_b_curr;		
		currcirparam->phase_c_curr = phase_c_curr;
        fclose(fd);
    }
    else
    {
        fread(currentbuf,sizeof(currentbuf)-1,1,fd);
        fclose(fd);
        printf("Open ./config/current_circuit_param.conf ok\n");
        
        if(pos = strstr(currentbuf,"phase_a:"))  //�ҵ���ֵ����ȡ
        {
            pos += 8;
            currcirparam->phase_a_curr = strtol(pos,NULL,10);
        }
		else
		{
			bvalid = 0;
		}

		if(pos = strstr(currentbuf,"phase_b:"))  //�ҵ���ֵ����ȡ
        {
            pos += 8;
            currcirparam->phase_b_curr = strtol(pos,NULL,10);
        }
		else
		{
			bvalid = 0;
		}

		if(pos = strstr(currentbuf,"phase_c:"))  //�ҵ���ֵ����ȡ
        {
            pos += 8;
            currcirparam->phase_c_curr = strtol(pos,NULL,10);
        }
		else
		{
			bvalid = 0;
		}
		
        if(bvalid == 0)       //δ�ҵ���ʼ��
        {
            memset(currentbuf, 0, sizeof(currentbuf));      
			sprintf(currentbuf, "phase_a:%d,phase_b:%d,phase_c:%d\n", phase_a_curr, phase_b_curr, phase_c_curr);
            fd = fopen(CURRENT_CIRCUIT_ENABLE_PATH,"a+");
            if(fd<=0)
            {
                return -1;
            }
            fwrite(currentbuf,1,strlen(currentbuf),fd);
			currcirparam->phase_a_curr = phase_a_curr;
			currcirparam->phase_b_curr = phase_b_curr;		
			currcirparam->phase_c_curr = phase_c_curr;
            fclose(fd);
        }
    }
    return 0;
}

int set_current_circuit_enable(current_circuit_param_stru *currcirparam)
{
	unlink(CURRENT_CIRCUIT_ENABLE_PATH);    //./config/current_circuit_param.conf  ɾ���ļ�
	char currentbuf[256];
	FILE *fd;

	memset(currentbuf, 0, sizeof(currentbuf));
	sprintf(currentbuf, "phase_a:%d,phase_b:%d,phase_c:%d\n", currcirparam->phase_a_curr, currcirparam->phase_b_curr, currcirparam->phase_c_curr);
	fd = fopen(CURRENT_CIRCUIT_ENABLE_PATH,"a+");
	if(fd<=0)
	{
		return -1;
	}
	fwrite(currentbuf, 1, strlen(currentbuf), fd);
	fclose(fd);
    return 0;
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
    ret = SaveConfMultiGroup(CURRENT_CIRCUIT_ENABLE_PATH,pData,i);
    *len = sizeof(current_circuit_param_stru);
	
	return (ret==0)? 1:0;    
}


//����TA��Ȳ���
static uint8_t GDW376_1_AFN04H_154H(uint8_t* pBuf, uint16_t* len)
{
    if(!pBuf||!len) return 0;

    current_transformer_rate_stru* pPara = (current_circuit_param_stru *)pBuf;
	
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
    ret = SaveConfMultiGroup(CURRENT_CIRCUIT_ENABLE_PATH,pData,i);
    *len = sizeof(current_circuit_param_stru);
	
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
//                case 5://�ն�����ͨ����Ϣ��֤��������
//                    nRet = GDW376_1_AFN04H_05H((uint8_t*)&di[1], &di_data_len);
//                    break;
				case 7:		//�ն� IP ��ַ�Ͷ˿�
					nRet = GDW376_1_AFN04H_07H((uint8_t*)&di[1], &di_data_len);		
					break;
				case 8:
					nRet = GDW376_1_AFN04H_08H((uint8_t*)&di[1], &di_data_len);
					break;
				case 9:		//F9�ն��¼���¼��������
					nRet = GDW376_1_AFN04H_09H((uint8_t*)&di[1], &di_data_len);
					break;
				case 89:	//�����ն��߼���ַ
					printf("luojidizhi***\n\n");
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


//F3��վIP���˿ڡ�APN��ѯ
static uint8_t GDW376_1_AFN0AH_03H(uint8_t* pBuf, uint16_t* len, uint8_t* pAckBuf, uint16_t* ack_pos)
{
	if ((pBuf == NULL)||(len == NULL) || (pAckBuf == NULL) || (ack_pos == NULL))
	{
		return 0;
	}
	stMainStationIPPort pPara;
	memcpy(&pPara, &gChargeSystemInfo.MainStationIPPort, sizeof(stMainStationIPPort));
	memcpy(pAckBuf, &pPara, sizeof(stMainStationIPPort));
	*ack_pos += sizeof(stMainStationIPPort);
	return 1;
}

//F7�ն�IP�Ͷ˿ںŲ�ѯ
static uint8_t GDW376_1_AFN0AH_07H(uint8_t* pBuf, uint16_t* len, uint8_t* pAckBuf, uint16_t* ack_pos)
{
	if ((pBuf == NULL)||(len == NULL) || (pAckBuf == NULL) || (ack_pos == NULL))
	{
		return 0;
	}
	stMainStationIPPort pPara;
	memcpy(&pPara, &gChargeSystemInfo.MainStationIPPort, sizeof(stMainStationIPPort));
	memcpy(pAckBuf, &pPara, sizeof(stMainStationIPPort));
	*ack_pos += sizeof(stMainStationIPPort);
	return 1;
}


//F9�ն��¼���¼���ò�ѯ
static uint8_t GDW376_1_AFN0AH_09H(uint8_t* pBuf, uint16_t* len, uint8_t* pAckBuf, uint16_t* ack_pos)
{
	if ((pBuf == NULL)||(len == NULL) || (pAckBuf == NULL) || (ack_pos == NULL))
	{
		return 0;
	}
	stMainStationIPPort pPara;
	memcpy(&pPara, &gChargeSystemInfo.MainStationIPPort, sizeof(stMainStationIPPort));
	memcpy(pAckBuf, &pPara, sizeof(stMainStationIPPort));
	*ack_pos += sizeof(stMainStationIPPort);
	return 1;
}


//F89�ն��߼���ַ��ѯ
static uint8_t GDW376_1_AFN0AH_89H(uint8_t* pBuf, uint16_t* len, uint8_t* pAckBuf, uint16_t* ack_pos)
{
	if ((pBuf == NULL)||(len == NULL) || (pAckBuf == NULL) || (ack_pos == NULL))
	{
		return 0;
	}
	
	memcpy(pAckBuf, &gChargeSystemInfo.usModleAddr, sizeof(gChargeSystemInfo.usModleAddr));
	*ack_pos += sizeof(gChargeSystemInfo.usModleAddr);
	return 1;
}


//F91�ն˵���λ����Ϣ
static uint8_t GDW376_1_AFN0AH_91H(uint8_t* pBuf, uint16_t* len, uint8_t* pAckBuf, uint16_t* ack_pos)
{
	if ((pBuf == NULL)||(len == NULL) || (pAckBuf == NULL) || (ack_pos == NULL))
	{
		return 0;
	}

	memcpy(&pAckBuf[*ack_pos], (uint8_t*)&gChargeSystemInfo.Location, sizeof(stTermLocation));
	*ack_pos += sizeof(stTermLocation);
	return 1;
}

////������·ʹ�ܲ���-x
static uint8_t GDW376_1_AFN0AH_153H(uint8_t* pBuf, uint16_t* len, uint8_t* pAckBuf, uint16_t* ack_pos)
{
	if ((pBuf == NULL)||(len == NULL) || (pAckBuf == NULL) || (ack_pos == NULL))
	{
		return 0;
	}

	//memcpy(&pAckBuf[*ack_pos], (uint8_t*)&gChargeSystemInfo.Location, sizeof(stTermLocation));
	//*ack_pos += sizeof(stTermLocation);
	return 1;
}

//F154//TA��Ȳ���-x
static uint8_t GDW376_1_AFN0AH_154H(uint8_t* pBuf, uint16_t* len, uint8_t* pAckBuf, uint16_t* ack_pos)
{
	if ((pBuf == NULL)||(len == NULL) || (pAckBuf == NULL) || (ack_pos == NULL))
	{
		return 0;
	}

	//memcpy(&pAckBuf[*ack_pos], (uint8_t*)&gChargeSystemInfo.Location, sizeof(stTermLocation));
	//*ack_pos += sizeof(stTermLocation);
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
				case 1: 	//F1�ն�����ͨ�Ų�����ѯ
					//nRet = GDW376_1_AFN0AH_01H((uint8_t*)&di[1], &di_data_len, ACKbuf, &ack_data_len);
					break;
				case 2: 	//F2�ն˵���λ����Ϣ
					//nRet = GDW376_1_AFN0AH_02H((uint8_t*)&di[1], &di_data_len, ACKbuf, &ack_data_len);
					break;
				case 3: 	//F3��վIP���˿ڡ�APN
					nRet = GDW376_1_AFN0AH_03H((uint8_t*)&di[1], &di_data_len, ACKbuf, &ack_data_len);
					break;
				case 4: 	//F4��վ�绰����Ͷ������ĺ���
					//nRet = GDW376_1_AFN0AH_04H((uint8_t*)&di[1], &di_data_len, ACKbuf, &ack_data_len);
					break;
				case 5: 	//F5�ն�����ͨ����Ϣ��֤������ѯ
					//nRet = GDW376_1_AFN0AH_05H((uint8_t*)&di[1], &di_data_len, ACKbuf, &ack_data_len);
					break;
				case 6: 	//F6�ն����ַ��ѯ
					//nRet = GDW376_1_AFN0AH_06H((uint8_t*)&di[1], &di_data_len, ACKbuf, &ack_data_len);
					break;
				case 7: 	//F7�ն�ip�Ͷ˿ں�
					nRet = GDW376_1_AFN0AH_07H((uint8_t*)&di[1], &di_data_len, ACKbuf, &ack_data_len);
					break;
				case 8: 	//F8���ڿ��Ʋ�����ѯ
					//nRet = GDW376_1_AFN0AH_08H((uint8_t*)&di[1], &di_data_len, ACKbuf, &ack_data_len);
					break;
				case 9: 	//F9�ն��¼���¼���ò�ѯ				//F9����ͨ��ģ��汾��ѯ
					nRet = GDW376_1_AFN0AH_09H((uint8_t*)&di[1], &di_data_len, ACKbuf, &ack_data_len);
					
					break;
				case 10: 	//F10�ӽڵ����оƬ�汾��ѯ
					//nRet = GDW376_1_AFN0AH_010H(Pn,(uint8_t*)&di[1], &di_data_len, ACKbuf, &ack_data_len);
					break;
				
				case 11:	//F11�ӽڵ�ͨ��ģ��汾��Ϣ��ѯ
					//nRet = GDW376_1_AFN0AH_011H(Pn,(uint8_t*)&di[1], &di_data_len, ACKbuf, &ack_data_len);
					break;
				case 12:	//F12Զ��ͨ��ģ��汾��Ϣ��ѯ
					//nRet = GDW376_1_AFN0AH_012H((uint8_t*)&di[1], &di_data_len, ACKbuf, &ack_data_len);
					break;
				case 14:	//F14Զ�̣����У�ͨ��ģ��汾��Ϣ��ѯ
					//nRet = GDW376_1_AFN0AH_014H((uint8_t*)&di[1], &di_data_len, ACKbuf, &ack_data_len);
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
				case 200: //�ն�������Ϣ��ѯ
				//nRet = GDW376_1_AFN0AH_200H((uint8_t*)&di[1], &di_data_len, ACKbuf, &ack_data_len);
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


static uint8_t AFN_05H(uint8_t *pBuf, uint16_t len)
{
/*
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
				case 1: //F1�������
#if 1
					if(Pn==2){
						nRet = sys_power_StartCharge(Pn,pBuf,len,(uint8_t*)&di[1], &di_data_len);
					}else{
						nRet = GDW376_1_AFN05H_01H(Pn,pBuf,len,(uint8_t*)&di[1], &di_data_len);
					}
#else
					nRet = GDW376_1_AFN05H_01H(Pn,pBuf,len,(uint8_t*)&di[1], &di_data_len);
#endif		
					break;
				case 2: //F2ֹͣ���
#if 1
					if(Pn==2){
						nRet = sys_power_AFN05H_02H(Pn,pBuf,len,(uint8_t*)&di[1], &di_data_len);
					}else{
						nRet = GDW376_1_AFN05H_02H(Pn,pBuf,len,(uint8_t*)&di[1], &di_data_len);
					}
#else
					nRet = GDW376_1_AFN05H_02H(Pn,pBuf,len,(uint8_t*)&di[1], &di_data_len);
#endif
					break;
				case 3: //F3�����ӳ��
					nRet = GDW376_1_AFN05H_03H(Pn,pBuf,len,(uint8_t*)&di[1], &di_data_len);
					
					break;
				case 27: //�����ն�����վͨ��
					nRet = GDW376_1_AFN05H_027H((uint8_t*)&di[1], &di_data_len);
					break;
				case 29: //�����ն������ϱ�
					nRet = GDW376_1_AFN05H_029H((uint8_t*)&di[1], &di_data_len);
					
					break;
				case 31: //��ʱ����
					nRet = GDW376_1_AFN05H_031H(Pn,(uint8_t*)&di[1], &di_data_len);
					break;
				case 32: //������Ϣ
					nRet = GDW376_1_AFN05H_032H((uint8_t*)&di[1], &di_data_len);
					
					break;
				case 35: //��ֹ�ն�����վͨ��
					nRet = GDW376_1_AFN05H_035H((uint8_t*)&di[1], &di_data_len);
					break;
				case 37: //��ֹ�ն������ϱ�
					nRet = GDW376_1_AFN05H_037H((uint8_t*)&di[1], &di_data_len);
					
					break;
				case 38: //�����ն�������վ
					nRet = GDW376_1_AFN05H_038H((uint8_t*)&di[1], &di_data_len);
					break;
				case 39: //�����ն˶Ͽ�����
					nRet = GDW376_1_AFN05H_039H((uint8_t*)&di[1], &di_data_len);
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
	nRet = sendAcktoMainStation(type, pBuf, len, di_arry, di_arry_len);
	*/
	//return nRet;
}


extern int GetImportanceEvent_sum;
extern int GetordinaryEvent_sum;
extern int GetERCEvent_sum;
extern u8 m_EC1;
extern u8 m_EC2;
extern u8 EventBuf[80][528];
//������Ҫ�¼�
static uint8_t GDW376_1_AFN0EH_01H(uint8_t* pBuf, uint16_t* len, uint8_t* pAckBuf, uint16_t* ack_pos)
{
	if ((pBuf == NULL)||(len == NULL) || (pAckBuf == NULL) || (ack_pos == NULL))
	{
		return 0;
	}

	DATA_AFN0E_F1 *Point = (DATA_AFN0E_F1 *)pBuf;
	u8 m_Pm = Point->Pm;
	u8 m_Pn = Point->Pn;

	//stru_sjjl_ERC1 ev1 = {0};
	//create_table();
	//CreateEvent1(ev1);

	//�����ݿ��ȡ��Ҫ�¼�	
	int ret = GetImportanceEvent(m_Pm, m_Pn);
	if (ret < 0)		//ʧ��
	{
		
	}

	//�������֡ͷ��
	EventsBlockHead head = {0};
	//head.DA[0] = 0x00;
	//head.DA[1] = 0x00;
	//head.DT[0] = 0x01;
	//head.DT[1] = 0x00;
	head.EC1 = m_EC1;
	head.EC2 = m_EC2;
	head.Pm = m_Pm;
	head.Pn = m_Pn;

	int sumlen = sizeof(head);
	memcpy(&pAckBuf[4], &head, sizeof(head));
	
	int Event_sum = GetImportanceEvent_sum;			//�����ݿ��ȡ�¼��ĸ���
	int i;
	for(i=0; i<Event_sum; i++)
	{
		printf("EventBuf[%d] = %s\n",i,EventBuf[i]);
		int len = strlen(EventBuf[i]);
		int m;
		for(m=0; m<len/2; m++)
		{
			sscanf(&EventBuf[i][m*2], "%02x", &pAckBuf[m+sumlen+4]);
		}	
		sumlen = sumlen + len/2;
	}

	*ack_pos += sumlen;
	return 1;
}


//����һ���¼�
static uint8_t GDW376_1_AFN0EH_02H(uint8_t* pBuf, uint16_t* len, uint8_t* pAckBuf, uint16_t* ack_pos)
{
	if ((pBuf == NULL)||(len == NULL) || (pAckBuf == NULL) || (ack_pos == NULL))
	{
		return 0;
	}

	DATA_AFN0E_F1 *Point = (DATA_AFN0E_F1 *)pBuf;
	u8 m_Pm = Point->Pm;
	u8 m_Pn = Point->Pn;
	
	//�����ݿ��ȡһ���¼�	
	int ret = GetordinaryEvent(m_Pm, m_Pn);
	if (ret < 0)//ʧ��
	{
		
	}

	//�������֡ͷ��
	EventsBlockHead head = {0};
	//head.DA[0] = 0x00;
	//head.DA[1] = 0x00;
	//head.DT[0] = 0x02;
	//head.DT[1] = 0x00;
	head.EC1 = m_EC1;
	head.EC2 = m_EC2;
	head.Pm = m_Pm;
	head.Pn = m_Pn;

	int sumlen = 4;
	memcpy(&pAckBuf[sumlen], &head, sizeof(head));

	sumlen += sizeof(head);
	
	int Event_sum = GetordinaryEvent_sum;			//�����ݿ��ȡ�¼��ĸ���
	int i;
	for(i=0; i<Event_sum; i++)
	{
		printf("EventBuf[%d] = %s\n",i,EventBuf[i]);
		//�������¼�������¼���¼�����Ƿ��Ӧ
		//parseEvent(EventBuf,  );

		
		int len = strlen(EventBuf[i]);

		if(sumlen+len > 1022)		//���÷�֡
		{
			//if()					//��һ֡
			{

				
			}
			//else				//�м�֡
			{
				
			}
		}
		
		int m;
		for(m=0; m<len/2; m++)
		{
			sscanf(&EventBuf[i][m*2], "%02x", &pAckBuf[m+sumlen+4]);

		}	
		sumlen = sumlen + len/2;

		if(sumlen > 1200)
		{

		}
	}


	//���һ֡

	*ack_pos += sumlen;
	return 1;
}


//���¼�����ERC�����¼�
static uint8_t GDW376_1_AFN0EH_03H(uint8_t* pBuf, uint16_t* len, uint8_t* pAckBuf, uint16_t* ack_pos)
{
	DATA_AFN0E_F3 *Point = (DATA_AFN0E_F3 *)pBuf;
	
	//�����ݿ��ȡһ���¼�	
	int ret = GetErcEvent(Point->ERC);
	if (ret < 0)		//ʧ��
	{
		
	}

	//�������֡ͷ��
	EventReqReply head;	
	//head.DA[0] = 0x00;
	//head.DA[1] = 0x00;
	//head.DT[0] = 0x03;
	//head.DT[1] = 0x00;
	
	int Event_sum = GetERCEvent_sum;			//�����ݿ��ȡ�¼��ĸ���
	
	if(Point->Num < Event_sum)
		Event_sum = Point->Num;

	head.EventNum = Event_sum;

	int sumlen = sizeof(head);
	memcpy(&pAckBuf[4], &head, sizeof(head));
	
	int i;
	for(i=0; i<Event_sum; i++)
	{
		printf("EventBuf[%d] = %s\n",i,EventBuf[i]);
		//�������¼�������¼���¼�����Ƿ��Ӧ
		//parseEvent(EventBuf,  );

		
		int len = strlen(EventBuf[i]);
		int m;
		for(m=0; m<len/2; m++)
		{
			sscanf(&EventBuf[i][m*2], "%02x", &pAckBuf[m+sumlen+4]);
		}	
		sumlen = sumlen + len/2;
	}

	*ack_pos += sumlen;

	return 1;
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
	memcpy((uint8_t*)&(pAckFrame->addr), (uint8_t*)&(pFrame->addr), sizeof(stFrame13761AddrType));
	
	pAckFrame->userdata.appseq.seq = pFrame->userdata.appseq.seq;
	pAckFrame->userdata.appseq.tpv = 1;
	pAckFrame->userdata.appseq.fir = 1;
	pAckFrame->userdata.appseq.fin = 0;			//1;
	pAckFrame->userdata.appseq.con = 0;
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
					nRet = GDW376_1_AFN0EH_01H((uint8_t*)&di[1], &di_data_len, ACKbuf, &ack_data_len);
					break;
				case 2: 	//F2����һ���¼�
					nRet = GDW376_1_AFN0EH_02H((uint8_t*)&di[1], &di_data_len, ACKbuf, &ack_data_len);
					break;
				case 3: 	//F3���¼�����ERC�����¼�
					nRet = GDW376_1_AFN0EH_03H((uint8_t*)&di[1], &di_data_len, ACKbuf, &ack_data_len);
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
		nRet = sendAFN0EtoMainStation(LINK_CTRL_UP_RES_USERDATA, pBuf, len, ACKbuf, ack_data_len, fd);
		
	}
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
	memcpy((uint8_t*)&(pAckFrame->addr), (uint8_t*)&(pFrame->addr), sizeof(stFrame13761AddrType));
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
	memcpy((uint8_t*)&(pAckFrame->addr), (uint8_t*)&(pFrame->addr), sizeof(stFrame13761AddrType));
	
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
	
	if (pFrame->control.control_un.act_s.dir != FRAME_13761_DIR_DOWN)	//����վ����
	{
		return 0;
	}
	if ((pFrame->addr.districtCode != gChargeSystemInfo.usModleAddr.districtCode) || 
	    (pFrame->addr.termAddr != gChargeSystemInfo.usModleAddr.termAddr && pFrame->addr.termAddr != 0xFFFF))	//�ն˵�ַ��ƥ��
	{
		//return 0;
	}
	//gprsLedRX.led=1;
	switch(pFrame->userdata.afn)	//afn + seq + DA1+DA2+DT1+DT2 + ���ݵ�Ԫ
	{
		case AFN_00_CONFIRM:
			//AFN_00H(pRecvData, nRecvLen);
			break;
		case AFN_01_RESET:
			//AFN_01H(pRecvData, nRecvLen);
			break;
		case AFN_02_LINK_TEST:
			break;
		case AFN_03_RELAYSTATION:
			
			break;
		case AFN_04_SETUP_PARM:					//���ò���
			printf("shezhicanshu***\n\n\n\n");
			AFN_04H(pRecvData, nRecvLen,fd);
			break;
		case AFN_05_CONTROL:
			//AFN_05H(pRecvData, nRecvLen);
			break;
		case AFN_06_AUTHENTICATION:
			break;
			
		case AFN_0A_INQUIRY_PARM:				//��ѯ����
			AFN_0AH(pRecvData, nRecvLen, fd);
			break;

		case AFN_0C_REALTIME_DATA:
			//AFN_0CH(pRecvData, nRecvLen);
			break;

		case AFN_0E_EVENT_DATA:					//��ѯ�¼������
			printf("chxunshijian****\n\n\n");
			AFN_0EH(pRecvData, nRecvLen,fd);
			break;
		case AFN_0F_FILE_TRANSFER:
			//AFN_0FH(pRecvData, nRecvLen);
			break;
		//case AFN_F0_CHARGE_TEST:
			//AFN_F0H(pRecvData, nRecvLen);
		default:
			break;
	}
	return 0;
}

/*������ת���ɶ���BCD��,������*/
u32 intToBcd(u32 data,u8 *bcd,u8 len)
{
    u32 i = 0;
    u32 remain = (u32)data;
    u32 tail = 0;
    
    if (!bcd) 
    {
        return (u32)-1;
    }

    for (i = 0; i < len; i++)
    {
        tail = remain % 100;
        remain /= 100;
        
        bcd[i] = (u8)(tail / 10 * 16 + tail % 10);
    }
    return 0;
}

//status:1��¼��2�˳���3����
int sendLinkPacktoMainStaion(int stauts, unsigned char *ucDatagram, int *length)
{

	//uint8_t ACKbuf[50];
	//stFrame13761Head *pAckFrame = (stFrame13761Head *)ACKbuf;
	stFrame13761Head *pAckFrame = (stFrame13761Head *)ucDatagram;
	stDataDI data_di;
	u16 pos = 0;
	//u16 len=0;
	//add for heart
	u8 month;
	//u8 week;
	//u16 year;
	stTime time;
	
	#if 1
	DateTime calendar;
	calendar.year = 20;
	calendar.month = 1;
	calendar.day = 1;
	calendar.hour = 0;
	calendar.minute = 0;
	calendar.second = 0;
	#endif
	
	//����Ӧ��֡
	//head1
	pAckFrame->header1 = FRAME_START_13761; // 0x68
	//len
	pAckFrame->len.len_un.len_s.m_pid = PROTOCOL_13761_VALID;   // 2
	pAckFrame->len.len_un.len_s.m_len = sizeof(stDataDI) + PROTOCOL_13761_CTR_SEQ_LEN;//12
	
	if(stauts==3)
		pAckFrame->len.len_un.len_s.m_len += 6;
	
	pAckFrame->len1.len_un.len_s.m_pid = pAckFrame->len.len_un.len_s.m_pid;
	pAckFrame->len1.len_un.len_s.m_len = pAckFrame->len.len_un.len_s.m_len;
	
	//head2
	pAckFrame->header2 = FRAME_START_13761; // 0x68
	//ctrl
	pAckFrame->control.control_un.resp_s.dir = FRAME_13761_DIR_UP;	//1
	pAckFrame->control.control_un.resp_s.prm = FRAME_13761_PRM_START_STATION;
	pAckFrame->control.control_un.resp_s.rsv = 0;
	pAckFrame->control.control_un.resp_s.acd = 0;	//���¼�����
	pAckFrame->control.control_un.resp_s.code = LINK_CTRL_UP_RES_NODATA;
	//addr
	memcpy((uint8_t*)&(pAckFrame->addr), &AddrType, sizeof(stFrame13761AddrType));

	//usr data
	pAckFrame->userdata.afn = AFN_02_LINK_TEST;
	pAckFrame->userdata.appseq.seq = 0;
	pAckFrame->userdata.appseq.con = 1;
	pAckFrame->userdata.appseq.fin = 1;
	pAckFrame->userdata.appseq.fir = 1;
	pAckFrame->userdata.appseq.tpv = 0;
	//pos = sizeof(stFrame13761Head) - 
	//DA,DT
	pos=0;
	setPn(0, &data_di.DA);
	setFn(stauts, &data_di.DT);
	memcpy(&(pAckFrame->userdata.data[0]), (uint8_t*)&(data_di), sizeof(stDataDI));
	pos += sizeof(stDataDI);// 4
	if(stauts == 3)
	{
		//DateTime *datatime=RTC_GetTime2();
		DateTime *datatime = &calendar;
		intToBcd(datatime->year,&time.year,1);
		intToBcd(datatime->month,&month,1);
		time.month=month;
		intToBcd(datatime->day,&time.day,1);

		//year=datatime->year+2000;
		//time.week=RTC_Get_Week(year,datatime->month,datatime->day);
		intToBcd(datatime->hour,&time.hour,1);
		intToBcd(datatime->minute,&time.min,1); 
		intToBcd(datatime->second,&time.sec,1);
			//��ȡ��ǰʱ��
		memcpy(&(pAckFrame->userdata.data[0])+pos, (u8 *)&time, sizeof(stTermTime));
		pos+=6;
	}
	
	pAckFrame->userdata.data[pos++] = GDW376_1_CalSum((uint8_t *)&(pAckFrame->control), pAckFrame->len.len_un.len_s.m_len);
	pAckFrame->userdata.data[pos++] = FRAME_END_13761;
	*length = sizeof(stFrame13761Head)+pos;
	//len =sizeof(stFrame13761Head)+pos;
	//��·ά��֡�����ط�
	return 0;							//Protocl13761SendData2(ACKbuf, len);
}
#if 0
int main(int argc, char *argv[])
{
	getchar();
	getchar();
	return 0;
}
#endif
