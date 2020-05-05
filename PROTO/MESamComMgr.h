#ifndef __MESAM_COM_MGR_H_
#define	__MESAM_COM_MGR_H_

#include "Sgd_types.h"


int GetTermESAMInfo(unsigned char* recvDataBuf, unsigned int recvBufSize);
int TermCertificateUpdateRequest(unsigned char* recvDataBuf, unsigned char certiType, unsigned int recvBufSize);
int MESamComRecv(unsigned char* _MSendFrame,int _MSendFramepos,unsigned char* _RecvDataBuf,int *_RecvDataBufPos, unsigned int recvBufSize);
int ByteReverse(unsigned char *srcData, unsigned int srcSize, unsigned char *destData);
int MainStaGetTermCertiInfo(u8 *MainstaR4,u16 R4Len,u8 *recvDataBuf, u32 *recvBufSize);
#endif