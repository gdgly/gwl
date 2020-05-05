#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "util.h"
#include "MESamBase.h"
#include "MESamComMgr.h"

extern unsigned char m_MSendBuf[MESAM_SEND_BUF_MAX_LEN];
//extern unsigned char m_MRecvBuf[MESAM_RECV_BUF_MAX_LEN];
static long max_reply_duration;

// 功能描述：获取终端ESAM信息 
int GetTermESAMInfo(unsigned char* recvDataBuf, unsigned int recvBufSize)
{
	int recvDataBufPos = 0;
	int _rs = -1;
	int _pos = 0; 
	unsigned char _ChipStatus;
	unsigned char *tempDataBuf = (unsigned char *)malloc(40 * sizeof(unsigned char));
	unsigned char ChipSerFramer[] = {0x80,0x0E,0x00,0x02,0x00,0x00,};//
	_rs = MESamComRecv(ChipSerFramer,6,recvDataBuf,&recvDataBufPos, recvBufSize);
	if (_rs != 0)
	{
		printf("GetTermESAMInfo-ChipSerial failed!!!\n");
		return -1;
	}
	printf("GetTermESAMInfo-ChipSerial success!!!\n");
	unsigned char ChipStaSerFramer[] = {0x80,0x0E,0x00,0x05,0x00,0x00,};//芯片状�?
	_rs = MESamComRecv(ChipStaSerFramer,6,&_ChipStatus,&_pos,sizeof(_ChipStatus));
	printf("GetTermESAMInfo-ChipState %d!!!\n", _ChipStatus);
	if (_rs != 0)
	{
		printf("GetTermESAMInfo-ChipState failed!!!\n");
		return -1;
	}
	printf("GetTermESAMInfo-ChipState success!!!\n");
	unsigned char CerSerFramer[] = {0x80,0x32,0x00,0x02,0x00,0x00,};//证书序列�?
	CerSerFramer[2] = _ChipStatus;
	_rs = MESamComRecv(CerSerFramer,6,recvDataBuf,&recvDataBufPos, recvBufSize);
	if (_rs != 0)
	{
		printf("GetTermESAMInfo-CertificationSerial failed!!!\n");
		return -1;
	}
	printf("GetTermESAMInfo-CertificationSerial success!!!\n");
	unsigned char CountFramer[] = {0x80,0x0E,0x00,0x03,0x00,0x00,};//计数�?
	_rs = MESamComRecv(CountFramer,6,recvDataBuf,&recvDataBufPos, recvBufSize);
	if (_rs != 0)
	{
		printf("GetTermESAMInfo-Counter failed!!!\n");
		return -1;
	}
	printf("GetTermESAMInfo-Counter success!!!\n");
	recvDataBuf[recvDataBufPos ++] = _ChipStatus;//芯片状�?
	unsigned char keySerFramer[] = {0x80,0x0E,0x00,0x06,0x00,0x00,};//密钥版本
	_rs = MESamComRecv(keySerFramer,6,recvDataBuf,&recvDataBufPos, recvBufSize);
	if (_rs != 0)
	{
		printf("GetTermESAMInfo-KeyVersion failed!!!\n");
		return -1;
	}
	//作数据的反序处理
	int pos = 0;
	ByteReverse(recvDataBuf, 8, tempDataBuf);
	pos += 8;
	ByteReverse(recvDataBuf+pos, 16, tempDataBuf+pos);
	pos += 16;
	ByteReverse(recvDataBuf+pos, 4, tempDataBuf+pos);
	pos += 4;
	ByteReverse(recvDataBuf+pos, 1, tempDataBuf+pos);
	pos++;
	ByteReverse(recvDataBuf+pos, 8, tempDataBuf+pos);
	//将反序后的数据拷回到原来的buf
	memcpy(recvDataBuf, tempDataBuf, recvDataBufPos);
	//delete tempDataBuf;
	free(tempDataBuf);
	//MY_ACE_ERROR((LM_ERROR,"TMESamComMgr::GetTermESAMInfo-KeyVersion success!!!\n"));
	//MY_ACE_ERROR((LM_ERROR,ACE_TEXT("TMESamComMgr::GetTermESAMInfo success recvDataBufPos=%d !!!\n"), recvDataBufPos));
	printf("GetTermESAMInfo-KeyVersion success!!!\n");
	printf("GetTermESAMInfo success recvDataBufPos=%d !!!\n", recvDataBufPos);
	return recvDataBufPos;
}

// 功能描述：终端证书更新请�?
int TermCertificateUpdateRequest(unsigned char* recvDataBuf, unsigned char certiType, unsigned int recvBufSize)
{
	//ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, autolock,lock_,-1));

	int recvDataBufPos = 0;
	int _rs = -1; 
	unsigned char ChipSerFramer[] = {0x80,0x32,0x00,0x02,0x00,0x00,};
	ChipSerFramer[2] = certiType;
	_rs = MESamComRecv(ChipSerFramer,6,recvDataBuf,&recvDataBufPos,recvBufSize);
	if (_rs != 0)
	{
		//MY_ACE_ERROR((LM_ERROR,"TMESamComMgr::TermCertificateUpdateRequest chip fail!!!\n"));
		printf("TermCertificateUpdateRequest chip fail!!!\n");
		return -1;
	}
	unsigned char CerSerFramer[] = {0x80,0x2c,0x00,0x01,0x00,0x00,};
	_rs = MESamComRecv(CerSerFramer,6,recvDataBuf,&recvDataBufPos,recvBufSize);
	if (_rs != 0)
	{
		//MY_ACE_ERROR((LM_ERROR,"TMESamComMgr::TermCertificateUpdateRequest cer fail!!!\n"));
		printf("TermCertificateUpdateRequest cer fail!!!\n");
		return -1;
	}
	//数据做反序处�?
	int pos = 0;
	//unsigned char *tmpDataBuf = new unsigned char[recvDataBufPos];
	unsigned char *tmpDataBuf = (unsigned char *)malloc(recvDataBufPos * sizeof(unsigned char));
	ByteReverse(recvDataBuf+pos, 16, tmpDataBuf+pos);
	pos += 16;
	ByteReverse(recvDataBuf+pos, 64, tmpDataBuf+pos);
	pos += 64;
	//将反序后的数据拷回到原来的buf
	memcpy(recvDataBuf, tmpDataBuf, recvDataBufPos);
	//delete tmpDataBuf;
	free(tmpDataBuf);
		
	//MY_ACE_ERROR((LM_ERROR,"TMESamComMgr::TermCertificateUpdateRequest success!!!\n"));
	printf("TermCertificateUpdateRequest success!!!\n");

	return recvDataBufPos;
}

int MESamComRecv(unsigned char* _MSendFrame,int _MSendFramepos,unsigned char* _RecvDataBuf,int *_RecvDataBufPos, unsigned int recvBufSize)
{
	int _rs = -1, _i=0, _j=0;
	int m_MRecvLen = 0;//帧长�?
	int timeout = -1;
	unsigned char m_MRecvBuf[MESAM_RECV_BUF_MAX_LEN];
	
	memset(m_MRecvBuf, 0, MESAM_RECV_BUF_MAX_LEN);
#ifndef ACE_WIN32
	int _len;
	int res=0,retry =1;
#define MAX_WAIT_ESAM_TIME 12
	if (timeout < 0)
		timeout = MAX_WAIT_ESAM_TIME;
	while (retry--)
	{
		printf("MEsamSend :");
		for (_i = 0; _i < _MSendFramepos ; _i++ )
		{
			printf("%02X ",_MSendFrame[_i]);
		}
		printf("\n");
		if (_MSendFramepos > 0)
		{
			//res=m_MESAMBaseClass->MEsamDataSend(_MSendFrame,_MSendFramepos);
			res=MEsamDataSend(_MSendFrame,_MSendFramepos);
			if(res < 0)
			{
				if (retry)
				{
					usleep(100*1000);
					continue;
				}
				break;
			}
			usleep(100);//避免收到上次命令的应�?
		}

		//_MSendDt = time(NULL);
		long start = get_sys_uptime_ms();
		
		//if (MEsamDataRecv(timeout) >= 0
			//&& m_MESAMBaseClass->m_MRecvLen > 0)
		if((m_MRecvLen = MEsamDataRecv(timeout, m_MRecvBuf)) >= 0)
		{
			printf("MESamComRecv data:");
			//for (int _j = 0; _j < m_MESAMBaseClass->m_MRecvLen ; _j++)
			for (_j = 0; _j < m_MRecvLen; _j++)
			{
				printf("%02X ", m_MRecvBuf[_j]);
			}
			printf("\n");

			//if (m_MESAMBaseClass->m_MRecvBuf[0] == 0x55)
			if (m_MRecvBuf[0] == 0x55)
			{
				//MY_ACE_ERROR((LM_ERROR,"33333333333\n"));
				//MY_ACE_ERROR((LM_ERROR,"m_MESAMBaseClass->m_MRecvBuf found 0x55 !!!\n"));
				//if ((m_MESAMBaseClass->m_MRecvBuf[1] == 0x90)&&(m_MESAMBaseClass->m_MRecvBuf[2] == 0x00))
				if ((m_MRecvBuf[1] == 0x90)&&(m_MRecvBuf[2] == 0x00))
				{
					long end = get_sys_uptime_ms();
					long time_past = (end - start);
					if (time_past > max_reply_duration)
					{
						max_reply_duration = time_past;
						printf("esam max reply duration increased to %ld\n", max_reply_duration);
					}
					//MY_ACE_ERROR((LM_ERROR,"44444444444\n"));
					//MY_ACE_ERROR((LM_ERROR,"m_MESAMBaseClass->m_MRecvBuf found 0x9000 !!!\n"));
					//_len = (m_MESAMBaseClass->m_MRecvBuf[3] << 8)+m_MESAMBaseClass->m_MRecvBuf[4];
					_len = (m_MRecvBuf[3] << 8)+m_MRecvBuf[4];
					if (*_RecvDataBufPos + _len > recvBufSize)
					{
						printf("MESamComRecv _RecvDataBufPos+_len %d+%d > recvBufSize %d !!!\n", *_RecvDataBufPos,_len,recvBufSize);
						return -1;
					}
					//memcpy(&_RecvDataBuf[*_RecvDataBufPos],&m_MESAMBaseClass->m_MRecvBuf[5],_len);
					memcpy(&_RecvDataBuf[*_RecvDataBufPos],&m_MRecvBuf[5],_len);
					*_RecvDataBufPos = *_RecvDataBufPos + _len;
					_rs = 0;
					break;
				}
				else
				{
					printf("MESamComRecv m_MESAMBaseClass->m_MRecvBuf not found 0x9000 !!!\n");
					//_rs = -1;
				}
			}
			else
			{
				printf("MESamComRecv m_MESAMBaseClass->m_MRecvBuf[0] not found 0x55 !!!\n");
				//_rs = -1;
			}
		}
		else
		{
			//MY_ACE_ERROR((LM_ERROR,"m_MESAMBaseClass->m_MRecvLen <= 0 !!!\n"));
			//MY_ACE_ERROR((LM_ERROR,"m_MESAMBaseClass->MEsamDataRecv failed!!!\n"));
		}
		if (retry)
			usleep(100*1000);
	}//while
	if (_rs < 0/*abs(_MSendDt-time(NULL)) >=timeout*/)
	{
		printf("MESamComRecv timed out %d !!!\n", timeout);
		//_rs = -1;
	}
	//MY_ACE_ERROR((LM_ERROR,"TMESamComMgr::MESamComRecv quit %d!!!\n",_rs));
#endif
	return _rs;

}

//字节倒项,例如12 34 56->56 34 12
int ByteReverse(unsigned char *srcData, unsigned int srcSize, unsigned char *destData)
{
	if (NULL == srcData && NULL == destData)
	{
		return -1;
	}
	int i = 0;
	for (i=0;i<srcSize;i++)
	{
		*(destData+i) = *(srcData+srcSize-1-i);
	}
	return 0;
}


int MainStaGetTermCertiInfo(u8 *MainstaR4, u16 R4Len, u8 *recvDataBuf, u32 *recvBufSize)
{
	if(R4Len != 8) return -1;
	if(!MainstaR4||!recvDataBuf||!recvBufSize) return -1;

	int ret = 0,pos=0,i;
	u8 sBuf[200],*pRev,*pTem,status;
	u8 GChipNo[] = {0x80,0x0e,0x00,0x02,0x00,0x00};
	u8 GOffCount[] = {0x80,0x0e,0x00,0x03,0x00,0x00};
	u8 GChipStatus[] = {0x80,0x0e,0x00,0x05,0x00,0x00};
	u8 GCertiNo[6] = {0x80,0x32,0x01,0x02,0x00,0x00};
	u8 GKeyVer[] = {0x80,0x0e,0x00,0x06,0x00,0x00};
	u8 GTermR5[] = {0x80,0x1a,0x08,0x00,0x00,0x00};
	u8 GMac[] = {0x80,0x48,0xc0,0x39,0x02};
	u8 *pGetInfo[] = {GChipNo,GOffCount,GChipStatus,GCertiNo,GKeyVer,GTermR5};
	u8 RevLen[] = {8,4,1,16,8,8};
	u8 SendLen[] = {sizeof(GChipNo),sizeof(GOffCount),sizeof(GChipStatus),sizeof(GCertiNo),sizeof(GKeyVer),sizeof(GTermR5)};

	pTem = sBuf;
	memset(sBuf,0,sizeof(sBuf));
	for(i=0;i<sizeof(pGetInfo)/sizeof(pGetInfo[0]);i++)
	{
		pos = 0;
		ret = MESamComRecv(pGetInfo[i],SendLen[i],pTem,&pos,RevLen[i]);
		if(ret||(pos!=RevLen[i])) 
		{
			printf("certificate info Get fail-%d",i);
			return ret;
		}
		if(2==i) GCertiNo[2] = *pTem;
		pTem += pos;		
	}

	
	pRev = recvDataBuf;
	pos = 0;
	memcpy(pRev,sBuf,RevLen[0]);//Term certificate number
	pRev += RevLen[0];
	
	pos = RevLen[0]+RevLen[1]+RevLen[2];
	memcpy(pRev,sBuf+pos,RevLen[3]);//
	pRev += RevLen[3];
	
	pos = (int)(RevLen[0]);
	memcpy(pRev,sBuf+pos,RevLen[1]);//counter
	pRev += RevLen[1];
	
	pos = (int)(RevLen[0]+RevLen[1]);
	memcpy(pRev,sBuf+pos,RevLen[2]);//status
	pRev += RevLen[2];
	
	pos = (int)(RevLen[0]+RevLen[1]+RevLen[2]+RevLen[3]);
	memcpy(pRev,sBuf+pos,RevLen[4]);//secret key
	pRev += RevLen[4];
	
	pos = (int)(RevLen[0]+RevLen[1]+RevLen[2]+RevLen[3]+RevLen[4]);	
	memcpy(pRev,sBuf+pos,RevLen[5]);//R5
	pRev += RevLen[5];
	
	memset(sBuf,0,sizeof(sBuf));

	pTem = sBuf;
	memcpy(pTem,GMac,sizeof(GMac));
	pTem += sizeof(GMac);
	for(i=0;i<R4Len;i++)
	{
		pTem[i] = MainstaR4[i];
		pTem[R4Len+i] = ~MainstaR4[i];
	}
	pTem += 2*R4Len;
	memcpy(pTem,recvDataBuf,pRev-recvDataBuf);
	pTem += pRev-recvDataBuf;
	pos = 0;	
	ret = MESamComRecv(sBuf,pTem-sBuf,pRev,&pos,4);
	if(ret||(pos!=4)) return -1;
	pRev += 4;
	*recvBufSize = pRev-recvDataBuf;
	return 0;
}







