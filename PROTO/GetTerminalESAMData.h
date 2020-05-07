#ifndef _GET_TERMINAL_ESAM_DATA_H_
#define	_GET_TERMINAL_ESAM_DATA_H_
//串口发送、接收缓冲区的大小
#define	MAIN_STATION_RANDOM_SIZE			8
#define BUFMAXSIZE    256

#define MRS_645_FRAME_LENGTH_MIN            (16)
#define MRS_645_FRAME_1ST_HEAD_LEN          (1)
#define MRS_645_FRAME_METERADD_LEN          (6)
#define MRS_645_FRAME_2ND_HEAD_LEN          (1)
#define MRS_645_FRAME_CTRL_LEN              (1)
#define MRS_645_FRAME_LENGTH_LEN            (1)

#define MRS_645_FRAME_1ST_HEAD_OFFSET       (0)
#define MRS_645_FRAME_METERADD_OFFSET       (MRS_645_FRAME_1ST_HEAD_OFFSET + MRS_645_FRAME_1ST_HEAD_LEN)//0+1
#define MRS_645_FRAME_2ND_HEAD_OFFSET       (MRS_645_FRAME_METERADD_OFFSET + MRS_645_FRAME_METERADD_LEN)//0+1+6
#define MRS_645_FRAME_CTRL_OFFSET           (MRS_645_FRAME_2ND_HEAD_OFFSET + MRS_645_FRAME_2ND_HEAD_LEN)//0+1+6+1
#define MRS_645_FRAME_LENGTH_OFFSET         (MRS_645_FRAME_CTRL_OFFSET + MRS_645_FRAME_CTRL_LEN)//0+1+6+1+1
#define MRS_645_FRAME_DATA_OFFSET           (MRS_645_FRAME_LENGTH_OFFSET + MRS_645_FRAME_LENGTH_LEN)//0+1+6+1+1+1

//#define MRS_PROTO645_DATAGRAM_LEN_MAX		252

int OpenTaUartPort(int fd,char* port);
void CloseTaUartPort(int fd);
int ComTaUart_Set(int fd,int speed,int flow_ctrl,int databits,int stopbits,int parity);
int ComTaUart_Recv(int serial_fd, char *rcv_buf,int data_len);
int ComTaUart_Send(int fd, char *send_buf,int data_len);
int TerminalCtAuthenticate(void);
int GetTerminalSecurityAuthenInfo(unsigned char* sendDataBuf, int sendDataLens, unsigned char* recvDataBuf, int recvBufSize, int *rx_ok_len);
int GetTABaudrate(unsigned char sendDataBuf, unsigned char *BaudrateWord);
int SetTABaudrate(unsigned char formatCode, unsigned char BaudrateWord, unsigned char *MAC, unsigned char* MAC2, unsigned int MAC2Len);
int GetCTModuleStatus(unsigned char *formatCode, unsigned char *phaseABCStatus, unsigned char *MAC2);
int GetTAModuleSecurityInfo(unsigned char formatCode, unsigned char* mStatRand, int RandSize, unsigned char* Frame, int *FrameLen);
int TAModuleKeyUpdate(int keyNums, unsigned char* sendDataBuf, int sendDataLens, unsigned char* MAC, int MACSIZE);
int GetTerminalEnableControlParam(unsigned char* mStatRand, int RandSize, unsigned char* phaseAEnable, unsigned char* phaseBEnable, unsigned char* phaseCEnable);
#endif
