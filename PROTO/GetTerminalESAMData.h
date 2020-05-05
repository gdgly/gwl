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


int TerminalCtAuthenticate(int fd, unsigned char* ESAMID);
int GetTASecurityInfo(unsigned char* sendDataBuf, int sendDataLens, unsigned char* recvDataBuf, int recvBufSize, int *rx_ok_len);
int GetTABaudrate(int serial_fd, unsigned char formatCode, unsigned char *BaudrateWord);
int SetTABaudrate(int fd, unsigned char formatCode, unsigned char BaudrateWord, unsigned char *MAC, unsigned char* MAC2, unsigned int MAC2Len);
#endif
