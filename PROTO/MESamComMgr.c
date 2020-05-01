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
	//ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, autolock,lock_,-1));

	int recvDataBufPos = 0;
	int _rs = -1;
	int _pos = 0; 
	unsigned char _ChipStatus;
	//unsigned char *tempDataBuf = new unsigned char[40];
	unsigned char *tempDataBuf = (unsigned char *)malloc(40 * sizeof(unsigned char));
	unsigned char ChipSerFramer[] = {0x80,0x0E,0x00,0x02,0x00,0x00,};//芯片序列号
	//MY_ACE_ERROR((LM_ERROR,"GetTermESAMInfo ChipSer!!!\n"));
	_rs = MESamComRecv(ChipSerFramer,6,recvDataBuf,&recvDataBufPos, recvBufSize);
	//MY_ACE_ERROR((LM_ERROR,"GetTermESAMInfo ChipSer finish!!!\n"));
	if (_rs != 0)
	{
		//MY_ACE_ERROR((LM_ERROR,"TMESamComMgr::GetTermESAMInfo-ChipSerial failed!!!\n"));
		printf("GetTermESAMInfo-ChipSerial failed!!!\n");
		return -1;
	}
	//MY_ACE_ERROR((LM_ERROR,"TMESamComMgr::GetTermESAMInfo-ChipSerial success!!!\n"));
	printf("GetTermESAMInfo-ChipSerial success!!!\n");
	unsigned char ChipStaSerFramer[] = {0x80,0x0E,0x00,0x05,0x00,0x00,};//芯片状态
	_rs = MESamComRecv(ChipStaSerFramer,6,&_ChipStatus,&_pos,sizeof(_ChipStatus));
	//MY_ACE_ERROR((LM_ERROR,ACE_TEXT("TMESamComMgr::GetTermESAMInfo-ChipState %d!!!\n"), _ChipStatus));
	printf("GetTermESAMInfo-ChipState %d!!!\n", _ChipStatus);
	if (_rs != 0)
	{
		//MY_ACE_ERROR((LM_ERROR,"TMESamComMgr::GetTermESAMInfo-ChipState failed!!!\n"));
		printf("GetTermESAMInfo-ChipState failed!!!\n");
		return -1;
	}
	//MY_ACE_ERROR((LM_ERROR,"TMESamComMgr::GetTermESAMInfo-ChipState success!!!\n"));
	printf("GetTermESAMInfo-ChipState success!!!\n");
	unsigned char CerSerFramer[] = {0x80,0x32,0x00,0x02,0x00,0x00,};//证书序列号
	CerSerFramer[2] = _ChipStatus;
	_rs = MESamComRecv(CerSerFramer,6,recvDataBuf,&recvDataBufPos, recvBufSize);
	if (_rs != 0)
	{
		//MY_ACE_ERROR((LM_ERROR,"TMESamComMgr::GetTermESAMInfo-CertificationSerial failed!!!\n"));
		printf("GetTermESAMInfo-CertificationSerial failed!!!\n");
		return -1;
	}
	//MY_ACE_ERROR((LM_ERROR,"TMESamComMgr::GetTermESAMInfo-CertificationSerial success!!!\n"));
	printf("GetTermESAMInfo-CertificationSerial success!!!\n");
	unsigned char CountFramer[] = {0x80,0x0E,0x00,0x03,0x00,0x00,};//计数器
	_rs = MESamComRecv(CountFramer,6,recvDataBuf,&recvDataBufPos, recvBufSize);
	if (_rs != 0)
	{
		//MY_ACE_ERROR((LM_ERROR,"TMESamComMgr::GetTermESAMInfo-Counter failed!!!\n"));
		printf("GetTermESAMInfo-Counter failed!!!\n");
		return -1;
	}
	//MY_ACE_ERROR((LM_ERROR,"TMESamComMgr::GetTermESAMInfo-Counter success!!!\n"));
	printf("GetTermESAMInfo-Counter success!!!\n");
	recvDataBuf[recvDataBufPos ++] = _ChipStatus;//芯片状态
	unsigned char keySerFramer[] = {0x80,0x0E,0x00,0x06,0x00,0x00,};//密钥版本
	_rs = MESamComRecv(keySerFramer,6,recvDataBuf,&recvDataBufPos, recvBufSize);
	if (_rs != 0)
	{
		//MY_ACE_ERROR((LM_ERROR,"TMESamComMgr::GetTermESAMInfo-KeyVersion failed!!!\n"));
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

// 功能描述：终端证书更新请求
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
	//数据做反序处理
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
	int m_MRecvLen = 0;//帧长度
	int timeout = -1;
	unsigned char m_MRecvBuf[MESAM_RECV_BUF_MAX_LEN];
	
	memset(m_MRecvBuf, 0, MESAM_RECV_BUF_MAX_LEN);
#ifndef ACE_WIN32
	//time_t _MSendDt;
	int _len;
	//MY_ACE_ERROR((LM_ERROR,"MESamComRecv in!!!\n"));
	//if (m_MESAMBaseClass == NULL)
	//{
		//printf("m_MESAMBaseClass is NULL!!!\n");
		//MY_ACE_ERROR((LM_ERROR,"TMESamComMgr::MESamComRecv m_MESAMBaseClass is NULL!!!\n"));
		//return -1;
	//}

	int res=0,retry =1;
#define MAX_WAIT_ESAM_TIME 12
	if (timeout < 0)
		timeout = MAX_WAIT_ESAM_TIME;
	while (retry--)
	{
		//MY_ACE_ERROR((LM_ERROR,"TMESamComMgr::MEsamSend :"));
		printf("MEsamSend :");
		for (_i = 0; _i < _MSendFramepos ; _i++ )
		{
			//MY_ACE_ERROR((LM_ERROR,"%02X ",_MSendFrame[_i]));
			printf("%02X ",_MSendFrame[_i]);
		}
		//MY_ACE_ERROR((LM_ERROR,"\n"));
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
			usleep(100);//避免收到上次命令的应答
		}

		//_MSendDt = time(NULL);
		long start = get_sys_uptime_ms();
		
		//if (MEsamDataRecv(timeout) >= 0
			//&& m_MESAMBaseClass->m_MRecvLen > 0)
		if((m_MRecvLen = MEsamDataRecv(timeout, m_MRecvBuf)) >= 0)
		{
			//MY_ACE_ERROR((LM_ERROR,"\nTMESamComMgr::MESamComRecv data:"));
			printf("MESamComRecv data:");
			//for (int _j = 0; _j < m_MESAMBaseClass->m_MRecvLen ; _j++)
			for (_j = 0; _j < m_MRecvLen; _j++)
			{
				//MY_ACE_ERROR((LM_ERROR,ACE_TEXT("%02X "),m_MESAMBaseClass->m_MRecvBuf[_j]));
				printf("%02X ", m_MRecvBuf[_j]);
			}
			//MY_ACE_ERROR((LM_ERROR,"\n"));
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
						//MY_ACE_ERROR((LM_ERROR,"esam max reply duration increased to %d.%06d\n",max_reply_duration.sec(),max_reply_duration.usec()));
						printf("esam max reply duration increased to %ld\n", max_reply_duration);
					}
					//MY_ACE_ERROR((LM_ERROR,"44444444444\n"));
					//MY_ACE_ERROR((LM_ERROR,"m_MESAMBaseClass->m_MRecvBuf found 0x9000 !!!\n"));
					//_len = (m_MESAMBaseClass->m_MRecvBuf[3] << 8)+m_MESAMBaseClass->m_MRecvBuf[4];
					_len = (m_MRecvBuf[3] << 8)+m_MRecvBuf[4];
					if (*_RecvDataBufPos + _len > recvBufSize)
					{
						//MY_ACE_ERROR((LM_ERROR,"TMESamComMgr::MESamComRecv _RecvDataBufPos+_len %d+%d > recvBufSize %d !!!\n",*_RecvDataBufPos,_len,recvBufSize));
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
					//MY_ACE_ERROR((LM_ERROR,"TMESamComMgr::MESamComRecv m_MESAMBaseClass->m_MRecvBuf not found 0x9000 !!!\n"));
					printf("MESamComRecv m_MESAMBaseClass->m_MRecvBuf not found 0x9000 !!!\n");
					//_rs = -1;
				}
			}
			else
			{
				//MY_ACE_ERROR((LM_ERROR,"TMESamComMgr::MESamComRecv m_MESAMBaseClass->m_MRecvBuf[0] not found 0x55 !!!\n"));
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
		//MY_ACE_ERROR((LM_ERROR,"TMESamComMgr::MESamComRecv timed out %d !!!\n",timeout));
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