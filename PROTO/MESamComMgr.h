#ifndef __MESAM_COM_MGR_H_
#define	__MESAM_COM_MGR_H_
int GetTermESAMInfo(unsigned char* recvDataBuf, unsigned int recvBufSize);
int TermCertificateUpdateRequest(unsigned char* recvDataBuf, unsigned char certiType, unsigned int recvBufSize);
int MESamComRecv(unsigned char* _MSendFrame,int _MSendFramepos,unsigned char* _RecvDataBuf,int *_RecvDataBufPos, unsigned int recvBufSize);
int ByteReverse(unsigned char *srcData, unsigned int srcSize, unsigned char *destData);
#endif