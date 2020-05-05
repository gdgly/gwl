#ifndef __ME_SAM_BASE_H_
#define __ME_SAM_BASE_H_

#define MESAM_DEV_NAME_MAX_LEN	20
#define MESAM_RECV_BUF_MAX_LEN	2040
#define	MESAM_SEND_BUF_MAX_LEN	MESAM_RECV_BUF_MAX_LEN

int MEsamDevOpen(void);
int MEsamDevClose(void);
int MEsamDataSend(unsigned char* _MBuf,int _MBufLen);
int MEsamDataRecv(int _WakeDt, unsigned char m_MRecvBuf[MESAM_RECV_BUF_MAX_LEN]);
int MEsamParaSet(void);
int MEsamGetLRCl(unsigned char* _MBuf,int _MBufLen, int *pos);
int MEsamCheckLRC2(unsigned char* _MBuf,int _MBufLen);
#endif	