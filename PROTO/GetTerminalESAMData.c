#include <stdlib.h> 
#include <stdio.h> 
#include <errno.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <sys/ioctl.h>
#include <time.h> 
#include <unistd.h>
#include <fcntl.h>
#include <sys/sysinfo.h>
#include <termios.h>  
 
#include <arpa/inet.h> 
#include <unistd.h>
#include <pthread.h>

#include "GetTerminalESAMData.h"
#include "MESamComMgr.h"
#include "util.h"
#include "MESamBase.h"
#include "ProtocolD645.h"

#define	debug_info	1

//��վ��ȡ��վ�����R4,���ͻ�ȡ�ն˰�ȫ��֤��Ϣ����
unsigned char mainStatRandom4[MAIN_STATION_RANDOM_SIZE] = {0};		//R4
unsigned char mainStatRandom4Reverse[MAIN_STATION_RANDOM_SIZE] = {0};		//~R4
unsigned char ESAMID[8] = {0};
extern int m_MFd;
extern int	serial_fd;

int OpenTaUartPort(int fd,char* port)  
{  
     
    fd = open( port, O_RDWR|O_NOCTTY|O_NDELAY);  
    if (-1 == fd)  
    {  
        perror("Can't Open Serial Port");  
        return(-1);  
    }  
    //�ָ�����Ϊ����״̬                                 
    if(fcntl(fd, F_SETFL, 0) < 0)  
    {  
        printf("fcntl failed!\n");  
        return(-1);  
    }       
    else  
    {  
        printf("fcntl=%d\n",fcntl(fd, F_SETFL,0));  
    }  
    //�����Ƿ�Ϊ�ն��豸      
    if(0 == isatty(STDIN_FILENO))  
    {  
        printf("standard input is not a terminal device\n");  
        //return(-1);  
    }  
    else  
    {  
        printf("isatty success!\n");  
    }                
    printf("fd->open=%d\n",fd);  

    return fd;  
}  


/*******************************************************************
* ���ƣ�  UART0_Close
* ���ܣ��رմ��ڲ����ش����豸�ļ�����
* ��ڲ�����fd    :�ļ�������     port :���ں�(ttyS0,ttyS1,ttyS2)
* ���ڲ�����void
*******************************************************************/   
void CloseTaUartPort(int fd)  
{  
    close(fd);  
}  


/*******************************************************************
* ���ƣ�UART0_Set
* ���ܣ�   ���ô�������λ��ֹͣλ��Ч��λ
* ��ڲ�����fd�����ļ�������
*           speed      �����ٶ�
*           flow_ctrl  ����������
*           databits   ����λ   ȡֵΪ 7 ����8
*           stopbits   ֹͣλ   ȡֵΪ 1 ����2
*           parity     Ч������ ȡֵΪN,E,O,,S
*���ڲ�������ȷ����Ϊ1�����󷵻�Ϊ0
*******************************************************************/  
int ComTaUart_Set(int fd,int speed,int flow_ctrl,int databits,int stopbits,int parity)  
{  
     
    int   i;  
    int   status;  
    int   speed_arr[] = {  B115200, B19200, B9600, B4800, B2400, B1200, B300};  
    int   name_arr[] = { 115200,  19200,  9600,  4800,  2400,  1200,  300};  
           
    struct termios options;  
     
    /*tcgetattr(fd,&options)�õ���fdָ��������ز������������Ǳ�����options,�ú��������Բ��������Ƿ���ȷ���ô����Ƿ���õȡ������óɹ�����������ֵΪ0��������ʧ�ܣ���������ֵΪ1.
    */  
    if( tcgetattr( fd,&options)  !=  0)  
    {  
        perror("SetupSerial 1");      
        return(-1);   
    }  
    
    //���ô������벨���ʺ����������  
    for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++)  
    {  
        if  (speed == name_arr[i])  
        {               
            cfsetispeed(&options, speed_arr[i]);   
            cfsetospeed(&options, speed_arr[i]);    
        }  
    }       
     
    //�޸Ŀ���ģʽ����֤���򲻻�ռ�ô���  
    options.c_cflag |= CLOCAL;  
    //�޸Ŀ���ģʽ��ʹ���ܹ��Ӵ����ж�ȡ��������  
    options.c_cflag |= CREAD;  
    
    //��������������  
    switch(flow_ctrl)  
    {  
        
        case 0 ://��ʹ��������  
              options.c_cflag &= ~CRTSCTS;  
              break;     
        
        case 1 ://ʹ��Ӳ��������  
              options.c_cflag |= CRTSCTS;  
              break;  
        case 2 ://ʹ�����������  
              options.c_cflag |= IXON | IXOFF | IXANY;  
              break;  
    }  
    //��������λ  
    //����������־λ  
    options.c_cflag &= ~CSIZE;  
    switch (databits)  
    {    
        case 5    :  
                     options.c_cflag |= CS5;  
                     break;  
        case 6    :  
                     options.c_cflag |= CS6;  
                     break;  
        case 7    :      
                 options.c_cflag |= CS7;  
                 break;  
        case 8:      
                 options.c_cflag |= CS8;  
                 break;    
        default:     
                 fprintf(stderr,"Unsupported data size\n");  
                 return (-1);   
    }  
    //����У��λ  
    switch (parity)  
    {    
        case 'n':  
        case 'N': //����żУ��λ��  
                 options.c_cflag &= ~PARENB;   
                 options.c_iflag &= ~INPCK;      
                 break;   
        case 'o':    
        case 'O'://����Ϊ��У��      
                 options.c_cflag |= (PARODD | PARENB);   
                 options.c_iflag |= INPCK;               
                 break;   
        case 'e':   
        case 'E'://����ΪżУ��    
                 options.c_cflag |= PARENB;         
                 options.c_cflag &= ~PARODD;         
                 options.c_iflag |= INPCK;        
                 break;  
        case 's':  
        case 'S': //����Ϊ�ո�   
                 options.c_cflag &= ~PARENB;  
                 options.c_cflag &= ~CSTOPB;  
                 break;   
        default:    
                 fprintf(stderr,"Unsupported parity\n");      
                 return (-1);   
    }   
    // ����ֹͣλ   
    switch (stopbits)  
    {    
        case 1:     
                 options.c_cflag &= ~CSTOPB; break;   
        case 2:     
                 options.c_cflag |= CSTOPB; break;  
        default:     
                       fprintf(stderr,"Unsupported stop bits\n");   
                       return (-1);  
    }  
     
    //�޸����ģʽ��ԭʼ�������  
    options.c_oflag &= ~OPOST;  
    
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);  
    //options.c_lflag &= ~(ISIG | ICANON);  
     
    //���õȴ�ʱ�����С�����ַ�  
    options.c_cc[VTIME] = 1; /* ��ȡһ���ַ��ȴ�1*(1/10)s */    
    options.c_cc[VMIN] = 1; /* ��ȡ�ַ������ٸ���Ϊ1 */  
     
    //�����������������������ݣ����ǲ��ٶ�ȡ ˢ���յ������ݵ��ǲ���  
    tcflush(fd,TCIFLUSH);  
     
    //�������� (���޸ĺ��termios�������õ������У�  
    if (tcsetattr(fd,TCSANOW,&options) != 0)    
    {  
        perror("com set error!\n");    
        return (-1);   
    }  
    return (0);   
}  



/*******************************************************************
* ���ƣ�   UART0_Recv
* ���ܣ�   ���մ�������
* ��ڲ�����fd:�ļ�������     
*           rcv_buf:���մ��������ݴ���rcv_buf��������
*           data_len:һ֡���ݵĳ���
* ���ڲ�����    ��ȷ����Ϊ1�����󷵻�Ϊ0
*******************************************************************/  
int ComTaUart_Recv(int serial_fd, char *rcv_buf,int data_len)  
{  
    int len,fs_sel;  
    fd_set fs_read;  
     
    struct timeval time;  
     
    FD_ZERO(&fs_read);  
    FD_SET(serial_fd,&fs_read);  
     
    time.tv_sec = 10;  
    time.tv_usec = 0;  
     
    //ʹ��selectʵ�ִ��ڵĶ�·ͨ��  
    fs_sel = select(serial_fd+1,&fs_read,NULL,NULL,&time);  
    printf("fs_sel = %d\n",fs_sel);  
    if(fs_sel)  
    {  
        len = read(serial_fd,rcv_buf,data_len);  
        printf("I am right!(version1.2) len = %d fs_sel = %d\n",len,fs_sel);  
        return len;  
    }  
    else if(fs_sel == 0)  //timeout
    {  
       printf("select timeout\n");
	   return -1;
    }    
	else
	{
		printf("Sorry,I am wrong!");  
		return -1;  
	}
}  

/********************************************************************
* ���ƣ�   UART0_Send
* ���ܣ���������
* ��ڲ�����fd:�ļ�������     
*           send_buf:��Ŵ��ڷ�������
*           data_len:һ֡���ݵĸ���
* ���ڲ�������ȷ����Ϊ1�����󷵻�Ϊ0
*******************************************************************/  
int ComTaUart_Send(int fd, char *send_buf,int data_len)  
{  
    int len = 0;  
     
    len = write(fd,send_buf,data_len);  
    if (len == data_len )  
    {  
    	int i;
        printf("send_buf: ");
		for(i=0; i<len; i++)
		{
			printf("%02x ",send_buf[i]);
		}
		printf("\n");
        return len;  
    }       
    else     
    {  
                 
        tcflush(fd,TCOFLUSH);  
        return -1;  
    }  
     
}  


//�ն���CTģ��˫�������֤
//*****************************************************************************
//	��������: TerminalCtAuthenticate
//	��������: �ն���CTģ��˫�������֤        
//	����˵��:
//	serial_fd [in] ����������
//  ESAMID [out] TESAM ID,8λ
//	�� �� ֵ: 
//	0 : �ɹ�
//	����Ϊ������  
//*****************************************************************************
//int TerminalCtAuthenticate(int serial_fd/*, unsigned char* ESAMID*/)
int TerminalCtAuthenticate(void)
{
	int recvDataBufPos = 0, rs = -1, len = 0, i = 0;
	unsigned char	TerminFrame00[] = {0x80,0x81,0x08,0x00,0x00,0x00};	//��ȡT-ESAM�����R1
	unsigned char	recvDataBuf[32] = {0};
	unsigned int	recvBufSize = 32;
	unsigned char	datafrm[128] = {0};
	unsigned char	_MSendFrame[50] = {0};	
	int _MSendFramePos = 0, pos=0, sndDataLens = 32, recvpos=0, frmLen=0, uspos=0;
	
	//step01 �ն˶�T-ESAM����,����:808108000000	��ȡT-ESAM�����R1
	//����:9000+0008+R1(8B),�ܼ�12B
	rs = MESamComRecv(TerminFrame00, 6, recvDataBuf, &recvDataBufPos, recvBufSize);
	if (rs != 0)
	{
		printf("send 808108000000 failed!!!\n");
		return -1;
	}
#if debug_info
	printf("recvDataBuf:");
	for (i = 0;i < recvDataBufPos ; i++)
	{
		printf("%02X ", recvDataBuf[i]);
	}
	printf("\n");
#endif
	//step02 CTģ�����,�����֤��һ֡����,���͸�CTģ��
	//CTģ��ִ�������֤��һ֡��Ӧ���񣬷���ER1,ER31,ESAMID,R2
	_MSendFramePos = 0;
	_MSendFrame[_MSendFramePos++] = 0x68;
	_MSendFrame[_MSendFramePos++] = 0xAA;
	_MSendFrame[_MSendFramePos++] = 0xAA;
	_MSendFrame[_MSendFramePos++] = 0xAA;
	_MSendFrame[_MSendFramePos++] = 0xAA;
	_MSendFrame[_MSendFramePos++] = 0xAA;
	_MSendFrame[_MSendFramePos++] = 0xAA;
	_MSendFrame[_MSendFramePos++] = 0x68;
	_MSendFrame[_MSendFramePos++] = 0x03;	//9 
	//�����򳤶�L:04H(���ݱ�ʶ)+04H(�����ߴ���)+m(���ݳ���)
	_MSendFrame[_MSendFramePos++] = 16;
	//���ݱ�ʶ, ռ4�ֽ�
	_MSendFrame[_MSendFramePos++] = 0x04;	//DI3
	_MSendFrame[_MSendFramePos++] = 0x00;	//DI2
	_MSendFrame[_MSendFramePos++] = 0x0F;	//DI1
	_MSendFrame[_MSendFramePos++] = 0x40;	//DI0	//14
	//�����ߴ���ռ4�ֽ�,Ĭ��00000000,����Ҫ��
	_MSendFramePos += 4;
	//T-ESAM�����R1��ռ8�ֽ�
	memcpy(&_MSendFrame[_MSendFramePos], &recvDataBuf, 8);
	_MSendFramePos += 8;
	//���
	Protocol_645_Pack(_MSendFrame, _MSendFramePos);
	//У��� + ������ + ֡����
	_MSendFrame[_MSendFramePos++] = GetSum8(_MSendFrame, 26);
	_MSendFrame[_MSendFramePos++] = 0x16;
#if debug_info
	printf("_MSendFrame:");
	for (i = 0;i < _MSendFramePos ; i++)
	{
		printf("%02X ", _MSendFrame[i]);
	}
	printf("\n");
#endif
	//�������跭ת�ߵ��ֽ�
	Array_Mirror(_MSendFrame + 10, 4);
	Array_Mirror(_MSendFrame + 14, 4);
	Array_Mirror(_MSendFrame + 18, 8);
#if debug_info
	printf("_MSendFrame Reverse:");
	for (i = 0;i < _MSendFramePos ; i++)
	{
		printf("%02X ", _MSendFrame[i]);
	}
	printf("\n");
#endif


	ComTaUart_Send(serial_fd, _MSendFrame, _MSendFramePos);  	
	//ȷ��Ӧ�𲢷���:ER1(8B)+ER31(16B)+ESAMID(8B)+R2(8B)
	//����Ӧ��:SERR������(2B)
	len = ComTaUart_Recv(serial_fd, datafrm, sizeof(datafrm));
	if (len >= MRS_645_FRAME_LENGTH_MIN)
	{	
		printf("frame received:\n");
#if debug_info
		printf("before datafrm:");
		for (i = 0; i < len ; i++)
		{
			printf("%02X ", datafrm[i]);
		}
		printf("\n");
#endif
		
		rs = mrsFind645Frame(datafrm, len, &uspos, &frmLen);
		if (0 != rs)
		{
			return rs;
		}
#if debug_info
		printf("after datafrm:");
		for (i = 0; i < frmLen ; i++)
		{
			printf("%02X ", datafrm[uspos+i]);
		}
		printf("\n");
#endif
		Protocol_645_Unpack(datafrm+uspos, frmLen);
	}
	else if(len == -2)
	{
		printf("error occured\n");
	}
	else
	{
		printf("no frame received, ret %d.\n",len);
		return -1;
	}
	
	sleep(1);
		//}
	
	//step03: �ն˶�T-ESAM����, ����808534350020+ESAMID(8B)+ER1(8B)+ER31(16B) �ն���֤ER1��ER31
	_MSendFramePos = 0, pos=0, sndDataLens = 32;//, recvpos=0;
	_MSendFrame[_MSendFramePos++] = 0x80;
	_MSendFrame[_MSendFramePos++] = 0x85;
	_MSendFrame[_MSendFramePos++] = 0x34;
	_MSendFrame[_MSendFramePos++] = 0x35;
	_MSendFrame[_MSendFramePos++] = 0x00;
	_MSendFrame[_MSendFramePos++] = 0x20;	//6
	//�Է������ݵ���
	unsigned char* tmpDataBuf = (unsigned char *)malloc(sndDataLens * sizeof(unsigned char));
	//ESAMID����
	memcpy(ESAMID, datafrm+pos+MRS_645_FRAME_DATA_OFFSET+uspos+4+24, 8);
	ByteReverse(datafrm+MRS_645_FRAME_DATA_OFFSET+uspos+4+24, 8, tmpDataBuf+pos);
	pos += 8;
	//ER1����
	ByteReverse(datafrm+MRS_645_FRAME_DATA_OFFSET+uspos+4, 8, tmpDataBuf+pos);
	pos += 8;
	//ER31����
	ByteReverse(datafrm+MRS_645_FRAME_DATA_OFFSET+uspos+4+8, 16, tmpDataBuf+pos);
	pos += 16;
#if debug_info
	printf("tmpDataBuf:");
	for (i = 0; i < pos ; i++)
	{
		printf("%02X ", tmpDataBuf[i]);
	}
	printf("\n");
#endif
	memcpy(&_MSendFrame[_MSendFramePos], tmpDataBuf, pos);
	free(tmpDataBuf);
	tmpDataBuf = NULL;
	_MSendFramePos = _MSendFramePos+pos;
	memset(recvDataBuf, 0, sizeof(recvDataBuf));
	recvDataBufPos = 0;
	rs = MESamComRecv(_MSendFrame, _MSendFramePos, recvDataBuf, &recvDataBufPos, recvBufSize);
	if (rs != 0)
	{
		printf("send 808534350020+ESAMID(8B)+ER1(8B)+ER31(16B) failed!!!\n");
		return -1;
	}
	printf("send 808534350020+ESAMID(8B)+ER1(8B)+ER31(16B) success!!!\n");

	//step04: �ն˶�T-ESAM����
	//�ն˷���808334350010(6B)+ESAMID(8B)+R2(8B) �ն˼���R2����ER2������ER32
	_MSendFramePos = 0, pos=0, sndDataLens = 16;//, recvpos=0;
	_MSendFrame[_MSendFramePos++] = 0x80;
	_MSendFrame[_MSendFramePos++] = 0x83;
	_MSendFrame[_MSendFramePos++] = 0x34;
	_MSendFrame[_MSendFramePos++] = 0x35;
	_MSendFrame[_MSendFramePos++] = 0x00;
	_MSendFrame[_MSendFramePos++] = 0x10;	//6
	//�Է������ݵ���
	tmpDataBuf = (unsigned char *)malloc(sndDataLens * sizeof(unsigned char));
	//ESAMID����
	ByteReverse(datafrm+MRS_645_FRAME_DATA_OFFSET+uspos+4+24, 8, tmpDataBuf+pos);
	pos += 8;
	//R2����
	ByteReverse(datafrm+MRS_645_FRAME_DATA_OFFSET+uspos+4+24+8, 8, tmpDataBuf+pos);
	pos += 8;
#if debug_info
	printf("ESAMID+R2:");
	for (i = 0; i < pos ; i++)
	{
		printf("%02X ", tmpDataBuf[i]);
	}
	printf("\n");
#endif
	memcpy(&_MSendFrame[_MSendFramePos], tmpDataBuf, pos);
	free(tmpDataBuf);
	tmpDataBuf = NULL;
	_MSendFramePos = _MSendFramePos+pos;
	memset(recvDataBuf, 0, sizeof(recvDataBuf));
	recvDataBufPos = 0;
	rs = MESamComRecv(_MSendFrame, _MSendFramePos, recvDataBuf, &recvDataBufPos, recvBufSize);
	if (rs != 0)
	{
		printf("send 808334350010+ESAMID(8B)+R2(8B) failed!!!\n");
		return -1;
	}
	printf("send send 808334350010+ESAMID(8B)+R2(8B)  success!!!\n");
#if debug_info
	printf("ER2(8B)+ER32(16B):");
	for (i = 0;i < recvDataBufPos ; i++)
	{
		printf("%02X ", recvDataBuf[i]);
	}
	printf("\n");
#endif

	//step05:�ն˽�ER2��ER32��������֤�ڶ�֡���У����͸�CTģ��
	_MSendFramePos = 0;
	memset(_MSendFrame, 0, sizeof(_MSendFrame));
	//�����֤�ڶ�֡����,������ER2(8B)+ER32(16B)
	_MSendFrame[_MSendFramePos++] = 0x68;
	_MSendFrame[_MSendFramePos++] = 0xAA;
	_MSendFrame[_MSendFramePos++] = 0xAA;
	_MSendFrame[_MSendFramePos++] = 0xAA;
	_MSendFrame[_MSendFramePos++] = 0xAA;
	_MSendFrame[_MSendFramePos++] = 0xAA;
	_MSendFrame[_MSendFramePos++] = 0xAA;
	_MSendFrame[_MSendFramePos++] = 0x68;
	_MSendFrame[_MSendFramePos++] = 0x03;	//9 			
	//�����򳤶�L:04H(���ݱ�ʶ)+04H(�����ߴ���)+24H(���ݳ���)
	_MSendFrame[_MSendFramePos++] = 32;
	//���ݱ�ʶ, ռ4�ֽ�
	_MSendFrame[_MSendFramePos++] = 0x04;
	_MSendFrame[_MSendFramePos++] = 0x00;
	_MSendFrame[_MSendFramePos++] = 0x0F;
	_MSendFrame[_MSendFramePos++] = 0x41;	//14
	//�����ߴ���ռ4�ֽ�,Ĭ��00000000,����Ҫ��
	_MSendFramePos += 4;
	//ER2(8B)+ER32(16B)
	memcpy(&_MSendFrame[_MSendFramePos], &recvDataBuf, 24);	//18
	_MSendFramePos += 24;
	//���
	Protocol_645_Pack(_MSendFrame, 42);
	//У��� + ������ + ֡����
	_MSendFrame[_MSendFramePos++] = GetSum8(_MSendFrame, 42);
	_MSendFrame[_MSendFramePos++] = 0x16;
#if debug_info
	printf("identity authentication 2:");
	for (i = 0;i < _MSendFramePos ; i++)
	{
		printf("%02X ", _MSendFrame[i]);
	}
	printf("\n");
#endif
	//�������跭ת�ߵ��ֽ�
	//���ݱ�ʶ��ת
	Array_Mirror(_MSendFrame + 10, 4);
	//�����ߴ��뷭ת
	Array_Mirror(_MSendFrame + 14, 4);
	//ER2��ת
	Array_Mirror(_MSendFrame + 18, 8);
	//ER32��ת
	Array_Mirror(_MSendFrame + 26, 16);
	//���������֤�ڶ�֡
	ComTaUart_Send(serial_fd, _MSendFrame, _MSendFramePos);
	//Э��ֻ�з���Ӧ��
	memset(datafrm, 0, sizeof(datafrm));
	len = ComTaUart_Recv(serial_fd, datafrm, sizeof(datafrm));
	if (len >= MRS_645_FRAME_LENGTH_MIN)
	{	
		printf("frame received:\n");
#if debug_info
		printf("before datafrm:");
		for (i = 0; i < len ; i++)
		{
			printf("%02X ", datafrm[i]);
		}
		printf("\n");
#endif
		
		rs = mrsFind645Frame(datafrm, len, &uspos, &frmLen);
		if (0 != rs)
		{
			return rs;
		}

		Protocol_645_Unpack(datafrm+uspos, frmLen);
#if debug_info
		printf("after datafrm:");
		for (i = 0; i < frmLen ; i++)
		{
			printf("%02X ", datafrm[uspos+i]);
		}
		printf("\n");
#endif
	}
	else if(len == -2)
	{
		printf("error occured\n");
	}
	else
	{
		printf("no frame received, ret %d.\n",len);
		return -1;
	}
	
	sleep(1);
	return recvDataBufPos;
}

//�ն��ϴ�T-ESAM���к�(8B)+֤�����к�(16B)+������(4B)+оƬ״̬(1B)+��Կ�汾(8B)+�ն������R5(8B)+MAC(4B) total(49B)
//*****************************************************************************
//	��������: GetTerminalSecurityAuthenInfo
//	��������: ��ȡ�ն˰�ȫ��֤��Ϣ        
//	����˵��:
//	sendDataBuf [in] ��վ�����R4
//  sendDataLens [in] ��վ���������
//	recvDataBuf	[out] �ն˷���T-ESAM���к�(8B)+...+MAC(4B)
//	recvBufSize	[in]  ������󳤶�
//	rx_ok_len	[out] ʵ�ʷ��س���
//	�� �� ֵ: 
//	0 : �ɹ�
//	����Ϊ������  
//*****************************************************************************
int GetTerminalSecurityAuthenInfo(unsigned char* sendDataBuf, int sendDataLens, unsigned char* recvDataBuf, int recvBufSize, int *rx_ok_len)
{
	//ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, autolock,lock_,-1));
	int recvDataBufPos = 0;
	int _rs = -1, i = 0;
	unsigned char _ChipStatus;
	unsigned char _MSendFrame[128] = {0};	
	int _MSendFramePos = 0, pos=0, sndDataLens = 61, recvpos=0;
	unsigned char *tmpDataBuf = NULL;

	//R4
	memcpy(mainStatRandom4, sendDataBuf, sendDataLens);	//32, 93, 43, 41, 19, 49, A1, A0
	//~R4
	BitReverse(mainStatRandom4, sendDataLens, mainStatRandom4Reverse);	
	
	//step01	����:800E00020000
	//����:9000+0008+T-ESAM���к�
	unsigned char ChipSerFramer[] = {0x80,0x0E,0x00,0x02,0x00,0x00,};//оƬ���к�
	_rs = MESamComRecv(ChipSerFramer,6,recvDataBuf,&recvDataBufPos, recvBufSize);
	if (_rs != 0)
	{
		printf("GetTerminalSecurityAuthenInfo-ChipSerial failed!!!\n");
		return -1;
	}
	printf("GetTerminalSecurityAuthenInfo-ChipSerial success!!!\n");
#if debug_info
	printf("TESAM ChipSerial, total(%d) :", recvDataBufPos);
	for (i = 0; i < recvDataBufPos ; i++)
	{
		printf("%02X ", recvDataBuf[i]);
	}
	printf("\n");
#endif

	//step02	����:800E00050000		����:9000+0001+оƬ״̬��Ϣ
	unsigned char ChipStaSerFramer[] = {0x80,0x0E,0x00,0x05,0x00,0x00,};//оƬ״̬
	_rs = MESamComRecv(ChipStaSerFramer,6,&_ChipStatus,&pos,sizeof(_ChipStatus));
	printf("GetTerminalSecurityAuthenInfo-ChipState %d!!!\n", _ChipStatus);
	if (_rs != 0)
	{
		printf("GetTerminalSecurityAuthenInfo-ChipState failed!!!\n");
		return -1;
	}
	printf("GetTerminalSecurityAuthenInfo-ChipState success!!!\n");

	//step03 ����:8032+p1(оƬ״̬��Ϣ)+02+0000	��ȡ֤�����к�
	//����:9000+0010+֤�����к�
	unsigned char CerSerFramer[] = {0x80,0x32,0x00,0x02,0x00,0x00,};//֤�����к�
	CerSerFramer[2] = _ChipStatus;
	_rs = MESamComRecv(CerSerFramer,6,recvDataBuf,&recvDataBufPos, recvBufSize);
	if (_rs != 0)
	{
		printf("GetTerminalSecurityAuthenInfo-CertificationSerial failed!!!\n");
		return -1;
	}
	printf("GetTerminalSecurityAuthenInfo-CertificationSerial success!!!\n");
#if debug_info
	printf("TESAM ChipSerial+CertificationSerial, total(%d) include ChipSerial(8B)+CertificationSerial(16B):", recvDataBufPos);
	for (i = 0; i < recvDataBufPos ; i++)
	{
		printf("%02X ", recvDataBuf[i]);
	}
	printf("\n");
#endif

	//step04 ����800E00030000	����:9000+0004+���߼�������Ϣ
	unsigned char CountFramer[] = {0x80,0x0E,0x00,0x03,0x00,0x00,};//������
	_rs = MESamComRecv(CountFramer,6,recvDataBuf,&recvDataBufPos, recvBufSize);
	if (_rs != 0)
	{
		printf("GetTerminalSecurityAuthenInfo-Counter failed!!!\n");
		return -1;
	}
	printf("GetTerminalSecurityAuthenInfo-Counter success!!!\n");
#if debug_info
	printf("TESAM ChipSerial+CertificationSerial, total(%d) include ChipSerial(8B)+CertificationSerial(16B)+Counter(4B):", recvDataBufPos);
	for (i = 0; i < recvDataBufPos ; i++)
	{
		printf("%02X ", recvDataBuf[i]);
	}
	printf("\n");
#endif

	//step05 оƬ״̬
	recvDataBuf[recvDataBufPos ++] = _ChipStatus;//оƬ״̬

	//step06 ����:800E00060000	��ȡ��Կ�汾:8B
	//����:9000+0008+��Կ�汾��Ϣ
	unsigned char keySerFramer[] = {0x80,0x0E,0x00,0x06,0x00,0x00,};//��Կ�汾
	_rs = MESamComRecv(keySerFramer,6,recvDataBuf,&recvDataBufPos, recvBufSize);
	if (_rs != 0)
	{
		printf("GetTerminalSecurityAuthenInfo-KeyVersion failed!!!\n");
		return -1;
	}
	printf("GetTerminalSecurityAuthenInfo-KeyVersion success!!!\n");
#if debug_info
	printf("total(%d) include ChipSerial(8B)+CertificationSerial(16B)+Counter(4B)+Status(1B)+KeyVersion(8B):", recvDataBufPos);
	for (i = 0; i < recvDataBufPos ; i++)
	{
		printf("%02X ", recvDataBuf[i]);
	}
	printf("\n");
#endif

	//step07 ����:801A08000000 ȡ�ն������R5
	//����:9000+0008+R5(8B)
	unsigned char RandomNumTermin[] = {0x80,0x1A,0x08,0x00,0x00,0x00};	//�ն������R5
	_rs = MESamComRecv(RandomNumTermin,6,recvDataBuf,&recvDataBufPos, recvBufSize);
	if (_rs != 0)
	{
		printf("GetTerminalSecurityAuthenInfo-RandomNumberR5 failed!!!\n");
		return -1;
	}
	printf("GetTerminalSecurityAuthenInfo-RandomNumberR5 success!!!\n");
#if debug_info
	printf("total(%d) include ChipSerial(8B)+CertificationSerial(16B)+Counter(4B)+Status(1B)+KeyVersion(8B)+R5(8B):", recvDataBufPos);
	for (i = 0; i < recvDataBufPos ; i++)
	{
		printf("%02X ", recvDataBuf[i]);
	}
	printf("\n");
#endif

	//step08 ����8048C039+LC+data1	����MAC
	//����:9000+0004+MAC
	//LC:data1����,LCΪ2�ֽ�
	//data1:R4(8B)+~R4(8B)+T-ESAM���к�(8B)+֤�����к�(16B)+������(4B)+оƬ״̬(1B)+��Կ�汾(8B)+�ն������R5(8B),�ܼ�61B
	_MSendFrame[_MSendFramePos++] = 0x80;
	_MSendFrame[_MSendFramePos++] = 0x48;
	_MSendFrame[_MSendFramePos++] = 0xC0;
	_MSendFrame[_MSendFramePos++] = 0x39;
	//LC
	sndDataLens = recvDataBufPos;
	sndDataLens += 16;	//R4(8B)+~R4(8B)
	_MSendFrame[_MSendFramePos++] = sndDataLens>>8;	
	_MSendFrame[_MSendFramePos++] = sndDataLens&0xff;	//6
	//�Է������ݵ���
	tmpDataBuf = (unsigned char *)malloc(sndDataLens * sizeof(unsigned char));
	if(tmpDataBuf == NULL)
	{
		printf("malloc failed \n");
	}
	//R4����
	pos = 0;
	ByteReverse(mainStatRandom4, 8, tmpDataBuf+pos);
	pos += 8;
	//~R4����
	ByteReverse(mainStatRandom4Reverse, 8, tmpDataBuf+pos);
	pos += 8;	
	//T-ESAM���кŵ���
	ByteReverse(recvDataBuf+recvpos, 8, tmpDataBuf+pos);
	pos += 8;
	recvpos += 8;
	//֤�����кŵ���(16B)
	ByteReverse(recvDataBuf+recvpos, 16, tmpDataBuf+pos);
	pos += 16;
	recvpos += 16;
	//����������(4B)
	ByteReverse(recvDataBuf+recvpos, 4, tmpDataBuf+pos);
	pos += 4;
	recvpos += 4;
	//оƬ״̬(1B)
	ByteReverse(recvDataBuf+recvpos, 1, tmpDataBuf+pos);
	pos += 1;
	recvpos += 1;
	//��Կ�汾(8B)
	ByteReverse(recvDataBuf+recvpos, 8, tmpDataBuf+pos);
	pos += 8;
	recvpos += 8;
	//�ն������R5(8B)
	ByteReverse(recvDataBuf+recvpos, 8, tmpDataBuf+pos);
	pos += 8;
	recvpos += 8;
	memcpy(&_MSendFrame[_MSendFramePos], tmpDataBuf, pos);
	free(tmpDataBuf);
	tmpDataBuf = NULL;
	_MSendFramePos = _MSendFramePos+pos;	
#if debug_info
	printf("total(%d):", _MSendFramePos);
	for (i = 0; i < _MSendFramePos ; i++)
	{
		printf("%02X ", _MSendFrame[i]);
	}
	printf("\n");
#endif
	_rs = MESamComRecv(_MSendFrame, _MSendFramePos, recvDataBuf, &recvDataBufPos, recvBufSize);
	if (_rs != 0)
	{
		printf("GetTerminalSecurityAuthenInfo-MAC failed!!!\n");
		return -1;
	}
	printf("GetTerminalSecurityAuthenInfo-MAC  success!!!\n");

	//T-ESAM���к�(8B)+֤�����к�(16B)+������(4B)+оƬ״̬(1B)+��Կ�汾(8B)+�ն������R5(8B)+MAC(4B)
	//�����ݵķ�����
	tmpDataBuf = (unsigned char *)malloc(49 * sizeof(unsigned char));
	pos = 0;
	ByteReverse(recvDataBuf, 8, tmpDataBuf);	//T-ESAM���к�(8B)
	pos += 8;
	ByteReverse(recvDataBuf+pos, 16, tmpDataBuf+pos);	//֤�����к�(16B)
	pos += 16;
	ByteReverse(recvDataBuf+pos, 4, tmpDataBuf+pos);	//������(4B)		//28
	pos += 4;
	ByteReverse(recvDataBuf+pos, 1, tmpDataBuf+pos);	//оƬ״̬(1B)
	pos++;
	ByteReverse(recvDataBuf+pos, 8, tmpDataBuf+pos);	//��Կ�汾(8B)
	pos += 8;
	ByteReverse(recvDataBuf+pos, 8, tmpDataBuf+pos);	//�ն������R5(8B)
	pos += 8;
	ByteReverse(recvDataBuf+pos, 4, tmpDataBuf+pos);	//MAC(4B)	�ܼ�49B
	pos += 4;
	*rx_ok_len = pos;
	//�����������ݿ��ص�ԭ����buf
	//memcpy(recvDataBuf, tmpDataBuf, recvDataBufPos);
	memcpy(recvDataBuf, tmpDataBuf, pos);
	free(tmpDataBuf);
	tmpDataBuf = NULL;
	return pos;
}

//*****************************************************************************
//	��������: GetTABaudrate
//	��������: ��ѯ������������        
//	����˵��:
//	sendDataBuf [in]	��ʽ��
//	BaudrateWord [out] ������������
//	�� �� ֵ: 
//	0 : �ɹ�
//	����Ϊ������  
//*****************************************************************************
//int GetTABaudrate(int fd, unsigned char formatCode, unsigned char *BaudrateWord)
int GetTABaudrate(unsigned char sendDataBuf, unsigned char *BaudrateWord)
{	
	int _rs = -1, i = 0, frmLen=0, uspos=0, len = 0;
	unsigned int	recvDataBufPos=0;
	unsigned char	datafrm[128] = {0};
	unsigned char	esamID[8] = {0};
	int	pos=0;
	unsigned char recvDataBuf[10];
	unsigned char recvBufSize=10;

	memset(recvDataBuf, 0, 10);
	//step01 �ն˷���˫�������֤����
	//TerminalCtAuthenticate(serial_fd, esamID);

	unsigned char _MSendFrame[50] = {0};	
	int _MSendFramePos = 0, sendDataLens = 16;
	
	//step02 �ն��·���ѯCTģ��Ĳ���������
	_MSendFrame[_MSendFramePos++] = 0x68;
	_MSendFrame[_MSendFramePos++] = 0xAA;
	_MSendFrame[_MSendFramePos++] = 0xAA;
	_MSendFrame[_MSendFramePos++] = 0xAA;
	_MSendFrame[_MSendFramePos++] = 0xAA;
	_MSendFrame[_MSendFramePos++] = 0xAA;
	_MSendFrame[_MSendFramePos++] = 0xAA;
	_MSendFrame[_MSendFramePos++] = 0x68;
	_MSendFrame[_MSendFramePos++] = 0x03; 			
	//�����򳤶�L:04H(���ݱ�ʶ)+04H(�����ߴ���)+01H(���ݳ���)
	_MSendFrame[_MSendFramePos++] = 9;	//10
	//���ݱ�ʶ, ռ4�ֽ�
	_MSendFrame[_MSendFramePos++] = 0x04;
	_MSendFrame[_MSendFramePos++] = 0x00;
	_MSendFrame[_MSendFramePos++] = 0x0F;
	_MSendFrame[_MSendFramePos++] = 0x44;
	//�����ߴ���ռ4�ֽ�,Ĭ��00000000,����Ҫ��
	_MSendFramePos += 4;
	//��ʽ��(1B)
	_MSendFrame[_MSendFramePos++] = sendDataBuf;	//19
	//���
	Protocol_645_Pack(_MSendFrame, 19);
	//У��� + ������ + ֡����
	_MSendFrame[_MSendFramePos++] = GetSum8(_MSendFrame, 19);
	_MSendFrame[_MSendFramePos++] = 0x16;
#if debug_info
	printf("get baudrate:");
	for (i = 0;i < _MSendFramePos ; i++)
	{
		printf("%02X ", _MSendFrame[i]);
	}
	printf("\n");
#endif
	//�������跭ת�ߵ��ֽ�
	//���ݱ�ʶ��ת
	Array_Mirror(_MSendFrame + 10, 4);
	//�����ߴ��뷭ת
	Array_Mirror(_MSendFrame + 14, 4);
	//���������֤�ڶ�֡
	ComTaUart_Send(serial_fd, _MSendFrame, _MSendFramePos);
	//Э��ֻ�з���Ӧ��
	memset(datafrm, 0, sizeof(datafrm));
	len = ComTaUart_Recv(serial_fd, datafrm, sizeof(datafrm));
	if (len >= MRS_645_FRAME_LENGTH_MIN)
	{	
		printf("frame received:\n");
#if debug_info
		printf("before datafrm:");
		for (i = 0; i < len ; i++)
		{
			printf("%02X ", datafrm[i]);
		}
		printf("\n");
#endif
		
		_rs = mrsFind645Frame(datafrm, len, &uspos, &frmLen);
		if (0 != _rs)
		{
			return _rs;
		}

		Protocol_645_Unpack(datafrm+uspos, frmLen);
#if debug_info
		printf("after datafrm:");
		for (i = 0; i < frmLen ; i++)
		{
			printf("%02X ", datafrm[uspos+i]);
		}
		printf("\n");
#endif
	}
	else if(len == -2)
	{
		printf("error occured\n");
	}
	else
	{
		printf("no frame received, ret %d.\n",len);
		return -1;
	}
	*BaudrateWord = datafrm[uspos+MRS_645_FRAME_DATA_OFFSET+5];

	//step04 ����: 8049813A000E(6B)+ESAMID(8B)+data1(��ʽ��(1B)+������������(1B))+MAC(4B),�ܼ�20B
	_MSendFramePos = 0, sendDataLens = 14, pos = 0;
	unsigned char* tmpDatabuf = (unsigned char *)malloc(sendDataLens * sizeof(unsigned char));
	_MSendFrame[_MSendFramePos++] = 0x80;
	_MSendFrame[_MSendFramePos++] = 0x49;	
	_MSendFrame[_MSendFramePos++] = 0x81;
	_MSendFrame[_MSendFramePos++] = 0x3A;	
	_MSendFrame[_MSendFramePos++] = 0x00;
	_MSendFrame[_MSendFramePos++] = 0x0E;	
	//ESAMID����
	ByteReverse(ESAMID, 8, tmpDatabuf+pos);
	pos += 8;
	//��ʽ�뷴��
	ByteReverse(&datafrm[uspos+MRS_645_FRAME_DATA_OFFSET+4], 1, tmpDatabuf+pos);
	pos += 1;
	//�����������ַ���
	ByteReverse(BaudrateWord, 1, tmpDatabuf+pos);
	pos += 1;
	//MAC1����	
	ByteReverse(&datafrm[uspos+MRS_645_FRAME_DATA_OFFSET+6], 4, tmpDatabuf+pos);
	pos += 4;
	memcpy(&_MSendFrame[_MSendFramePos], tmpDatabuf, pos);
	free(tmpDatabuf);
	tmpDatabuf = NULL;
	_MSendFramePos += pos;
#if debug_info
	printf("GetTABaudrate:");
	for (i = 0; i < _MSendFramePos; i++)
	{
		printf("%02X ", _MSendFrame[i]);
	}
	printf("\n");
#endif
	_rs = MESamComRecv(_MSendFrame, _MSendFramePos, recvDataBuf,&recvDataBufPos, recvBufSize);
	if (_rs != 0)
	{
		printf("GetTABaudrate failed!!!\n");
		return -1;
	}
	printf("GetTABaudrate success!!!\n");
	return 0;
}

//*****************************************************************************
//	��������: SetTABaudrate
//	��������: ���ò�����������        
//	����˵��:
//	formatCoce	[in]	��ʽ��
//	BaudrateWord [in] ������������
//	MAC	[in]	//��·״̬Ѳ���ǰ�ȫģ��V2.0֧�ֶ���վ���ն˽���MACУ��
//	MAC��DATA(������MAC)�������
//	MAC2 [out]	����80488138000A+data3(ESAMID+��ʽ��+������������)��T-ESAM,����:9000+0004+MAC2
//	�� �� ֵ: 
//	0 : �ɹ�
//	����Ϊ������  
//*****************************************************************************
int SetTABaudrate(unsigned char formatCode, unsigned char BaudrateWord, unsigned char *MAC, unsigned char* MAC2, unsigned int MAC2Len)
{
	unsigned char	esamID[8] = {0};
	int	len = 0, _rs = -1, recvDataBufPos = 0, pos=0;
	
	//step01 �ն˷���˫�������֤����
	//TerminalCtAuthenticate(fd, esamID);
	//step02 �ն��·�����CTģ��Ĳ���������
	//���������Ƹ�ʽ��(1B)/������������(1B)/MAC(4B)
	unsigned char _MSendFrame[50];	
	int _MSendFramePos = 0, sendDataLens = 16;

	
	_MSendFrame[_MSendFramePos++] = 0x68;
	_MSendFrame[_MSendFramePos++] = 0xAA;
	_MSendFrame[_MSendFramePos++] = 0xAA;
	_MSendFrame[_MSendFramePos++] = 0xAA;
	_MSendFrame[_MSendFramePos++] = 0xAA;
	_MSendFrame[_MSendFramePos++] = 0xAA;
	_MSendFrame[_MSendFramePos++] = 0xAA;
	_MSendFrame[_MSendFramePos++] = 0x68;
	_MSendFrame[_MSendFramePos++] = 0x03; 			
	//�����򳤶�L:04H(���ݱ�ʶ)+04H(�����ߴ���)+01H(���ݳ���)
	_MSendFrame[_MSendFramePos++] = 14;	//10
	//���ݱ�ʶ, ռ4�ֽ�
	_MSendFrame[_MSendFramePos++] = 0x45;
	_MSendFrame[_MSendFramePos++] = 0x0F;
	_MSendFrame[_MSendFramePos++] = 0x00;
	_MSendFrame[_MSendFramePos++] = 0x04;	//14
	//�����ߴ���ռ4�ֽ�,Ĭ��00000000(4B),����Ҫ��
	_MSendFramePos += 4;
	//��ʽ��(1B)
	_MSendFrame[_MSendFramePos++] = formatCode;	//19
	_MSendFrame[_MSendFramePos++] = BaudrateWord;	//20
	memcpy(&_MSendFrame[_MSendFramePos], MAC, 4);
	_MSendFramePos += 4;	//24
	//���
	Protocol_645_Pack(_MSendFrame, 24);
	//У��� + ������ + ֡����
	_MSendFrame[_MSendFramePos++] = GetSum8(_MSendFrame, 24);
	_MSendFrame[_MSendFramePos++] = 0x16;

	ComTaUart_Send(serial_fd, _MSendFrame, _MSendFramePos);  
	//��·״̬Ѳ�����ն˰�ȫоƬʹ���ֲ�û����ȷ��Ӧ֡��ʲô
	
	//step03 �ն˶�T-ESAM����,����:80488138000A(6B)+data3(ESAMID(8B)+��ʽ��(1B)+������������(1B))
	_MSendFramePos = 0, sendDataLens = 10, pos = 0;
	unsigned char* tmpDatabuf = (unsigned char *)malloc(sendDataLens * sizeof(unsigned char));
	_MSendFrame[_MSendFramePos++] = 0x80;
	_MSendFrame[_MSendFramePos++] = 0x48;	
	_MSendFrame[_MSendFramePos++] = 0x81;
	_MSendFrame[_MSendFramePos++] = 0x38;	
	_MSendFrame[_MSendFramePos++] = 0x00;
	_MSendFrame[_MSendFramePos++] = 0x0A;
	//ESAMID ����
	ByteReverse(esamID, 8, tmpDatabuf+pos);
	pos += 8;
	//��ʽ�� ����	
	ByteReverse(&formatCode, 1, tmpDatabuf+pos);
	pos += 1;
	//�����������ַ���	
	ByteReverse(&BaudrateWord, 1, tmpDatabuf+pos);
	pos += 1;
	//����ESAMID/��ʽ��/������������
	memcpy(&_MSendFrame[_MSendFramePos], tmpDatabuf, pos);
	_MSendFramePos += pos;

	_rs = MESamComRecv(_MSendFrame, _MSendFramePos, MAC2, &recvDataBufPos, MAC2Len);
	if (_rs != 0)
	{
		printf("SetTABaudrate-Set Baudrate failed!!!\n");
		return -1;
	}

	return 0;
}

//*****************************************************************************
//	��������: GetCTModuleStatus
//	��������: ��ȡCTģ��״̬        
//	����˵��:
//	�� �� ֵ: 
//	0 : �ɹ�
//	����Ϊ������  
//*****************************************************************************
int GetCTModuleStatus(unsigned char *formatCode, unsigned char *phaseABCStatus, unsigned char *MAC2)
{
	unsigned char _MSendFrame[50];	
	int _MSendFramePos = 0, pos=0, sendDataLens = 16, i = 0;
	unsigned char	esamID[8] = {0};
	int recvLen = 0, _rs = -1, frmLen=0, uspos=0;
	unsigned char recvBuf[MESAM_RECV_BUF_MAX_LEN] = {0};
	
	//step01 ��վ�·���ȡ�ն˰�ȫ��֤��Ϣ����

	//step02 �ն˶�ȡCTģ��״̬
	//step02.1	�ն˷���˫�������֤����
	//TerminalCtAuthenticate(fd, esamID);
	//step02.2	�ն��·���ȡCTģ��״̬����
	memset(_MSendFrame, 0, 50);
	_MSendFrame[_MSendFramePos++] = 0x68;
	_MSendFrame[_MSendFramePos++] = 0xAA;
	_MSendFrame[_MSendFramePos++] = 0xAA;
	_MSendFrame[_MSendFramePos++] = 0xAA;
	_MSendFrame[_MSendFramePos++] = 0xAA;
	_MSendFrame[_MSendFramePos++] = 0xAA;
	_MSendFrame[_MSendFramePos++] = 0xAA;
	_MSendFrame[_MSendFramePos++] = 0x68;
	_MSendFrame[_MSendFramePos++] = 0x03; 			
	//�����򳤶�L:04H(���ݱ�ʶ)+04H(�����ߴ���)+01H(���ݳ���)
	_MSendFrame[_MSendFramePos++] = 9;
	//���ݱ�ʶ, ռ4�ֽ�	
	_MSendFrame[_MSendFramePos++] = 0x07;
	_MSendFrame[_MSendFramePos++] = 0x00;
	_MSendFrame[_MSendFramePos++] = 0x00;
	_MSendFrame[_MSendFramePos++] = 0x00;
	//�����ߴ���ռ4�ֽ�,Ĭ��00000000(4B),����Ҫ��
	_MSendFramePos += 4;
	//��ʽ��(1B)
	_MSendFrame[_MSendFramePos++] = *formatCode;
	//���
	Protocol_645_Pack(_MSendFrame, _MSendFramePos);
	//У��� + ������ + ֡����
	_MSendFrame[_MSendFramePos++] = GetSum8(_MSendFrame, 20);
	_MSendFrame[_MSendFramePos++] = 0x16;	//21
	//645֡֡����

#if debug_info
	printf("GetCTModuleStatus:");
	for (i = 0;i < _MSendFramePos ; i++)
	{
		printf("%02X ", _MSendFrame[i]);
	}
	printf("\n");
#endif
	//�������跭ת�ߵ��ֽ�
	//���ݱ�ʶ��ת
	Array_Mirror(_MSendFrame + 10, 4);
	//�����ߴ��뷭ת
	Array_Mirror(_MSendFrame + 14, 4);
	//�ն��·���ȡCTģ��״̬����
	ComTaUart_Send(serial_fd, _MSendFrame, _MSendFramePos);
	//ȷ��Ӧ��֡
	memset(recvBuf, 0, sizeof(recvBuf));
	recvLen = ComTaUart_Recv(serial_fd, recvBuf, sizeof(recvBuf));
	if (recvLen >= MRS_645_FRAME_LENGTH_MIN)
	{	
		printf("frame received:\n");
#if debug_info
		printf("before datafrm:");
		for (i = 0; i < recvLen ; i++)
		{
			printf("%02X ", recvBuf[i]);
		}
		printf("\n");
#endif
		
		_rs = mrsFind645Frame(recvBuf, recvLen, &uspos, &frmLen);
		if (0 != _rs)
		{
			return _rs;
		}

		Protocol_645_Unpack(recvBuf+uspos, frmLen);
#if debug_info
		printf("after datafrm:");
		for (i = 0; i < frmLen ; i++)
		{
			printf("%02X ", recvBuf[uspos+i]);
		}
		printf("\n");
#endif
	}
	else if(recvLen == -2)
	{
		printf("error occured\n");
	}
	else
	{
		printf("no frame received, ret %d.\n",recvLen);
		return -1;
	}

	//Ӧ��֡:�����򳤶�L=04H(���ݱ�ʶ)+m(���ݳ���)
	//memcpy(phaseABCStatus, datafrm+MRS_645_FRAME_DATA_OFFSET+4, 6);
	//��ʽ��
	*formatCode =  recvBuf[uspos+MRS_645_FRAME_DATA_OFFSET+4];
	//ABC�����Ե����
	memcpy(phaseABCStatus, recvBuf+uspos+MRS_645_FRAME_DATA_OFFSET+4+1, 3);
	//����MAC
	memcpy(MAC2, recvBuf+uspos+MRS_645_FRAME_DATA_OFFSET+4+4, 4);	
	//step02.3	����:8049813A0010(6B)+data2(ESAMID(8B)+��ʽ��(1B)+A��״̬(1B)+B��״̬(1B)+C��״̬(1B)+MAC2(4B))	
	//unsigned char _MSendFrame[50];
	unsigned char* tmpDataBuf=NULL;
	_MSendFramePos = 0, pos=0, sendDataLens = 16;
	_MSendFrame[_MSendFramePos++] = 0x80;
	_MSendFrame[_MSendFramePos++] = 0x49;
	_MSendFrame[_MSendFramePos++] = 0x81;
	_MSendFrame[_MSendFramePos++] = 0x3A;
	_MSendFrame[_MSendFramePos++] = 0x00;
	_MSendFrame[_MSendFramePos++] = 0x10;
	tmpDataBuf = (unsigned char *)malloc(sendDataLens * sizeof(unsigned char));
	//ESAMID����
	ByteReverse(ESAMID, 8, tmpDataBuf+pos);
	pos += 8;
	//��ʽ�뷴��
	ByteReverse(formatCode, 1, tmpDataBuf+pos);
	pos += 1;
	//A��״̬����
	ByteReverse(phaseABCStatus, 1, tmpDataBuf+pos);
	pos += 1;
	//B��״̬����
	ByteReverse(phaseABCStatus+1, 1, tmpDataBuf+pos);
	pos += 1;
	//C��״̬����
	ByteReverse(phaseABCStatus+2, 1, tmpDataBuf+pos);
	pos += 1;
	//MAC2����
	ByteReverse(MAC2, 4, tmpDataBuf+pos);
	pos += 4;

	memcpy(&_MSendFrame[_MSendFramePos], tmpDataBuf, pos);
	free(tmpDataBuf);
	tmpDataBuf = NULL;
	_MSendFramePos = _MSendFramePos + pos;
#if debug_info
	printf("Send 8049813A0010+data2:");
	for (i = 0; i < _MSendFramePos ; i++)
	{
		printf("%02X ", _MSendFrame[i]);
	}
	printf("\n");
#endif

	memset(recvBuf, 0, MESAM_RECV_BUF_MAX_LEN);
	pos = 0;
	_rs = MESamComRecv(_MSendFrame, _MSendFramePos, recvBuf, &pos, MESAM_RECV_BUF_MAX_LEN);
	if (_rs != 0)
	{
		printf("GetCTModuleStatus-Send 8049813A0010+data2 return 9000+0000 failed!!!\n");
		return -1;
	}	
	printf("GetCTModuleStatus-Send 8049813A0010+data2 return 9000+0000 success!!!\n");

	//step03 ����8048C039(4B)+LC(2B)+data1(R4(8B)+~R4(8B)+A��״̬(1B)+B��״̬(1B)+C��״̬(1B)) �ܼ�:25B
	_MSendFramePos = 0, sendDataLens = 19, pos=0;
	_MSendFrame[_MSendFramePos++] = 0x80;
	_MSendFrame[_MSendFramePos++] = 0x48;
	_MSendFrame[_MSendFramePos++] = 0xC0;
	_MSendFrame[_MSendFramePos++] = 0x39;
	//LC
	_MSendFrame[_MSendFramePos++] = sendDataLens>>8;	
	_MSendFrame[_MSendFramePos++] = sendDataLens&0xff;
	//�Է������ݵ���
	tmpDataBuf = (unsigned char *)malloc(sendDataLens * sizeof(unsigned char));
	//R4����
	ByteReverse(mainStatRandom4, 8, tmpDataBuf+pos);
	pos += 8;
	//~R4����
	ByteReverse(mainStatRandom4Reverse, 8, tmpDataBuf+pos);
	pos += 8;	
	//A��״̬����
	ByteReverse(phaseABCStatus, 1, tmpDataBuf+pos);
	pos += 1;
	//B��״̬����
	ByteReverse(phaseABCStatus+1, 1, tmpDataBuf+pos);
	pos += 1;
	//C��״̬����
	ByteReverse(phaseABCStatus+2, 1, tmpDataBuf+pos);
	pos += 1;
	memcpy(&_MSendFrame[_MSendFramePos], tmpDataBuf, pos);
	_MSendFramePos = _MSendFramePos + pos;
	free(tmpDataBuf);
	tmpDataBuf = NULL;
	memset(recvBuf, 0, MESAM_RECV_BUF_MAX_LEN);
	pos = 0;
	_rs = MESamComRecv(_MSendFrame, _MSendFramePos, recvBuf, &pos, MESAM_RECV_BUF_MAX_LEN);
	if (_rs != 0)
	{
		printf("GetCTModuleStatus-Send 8048C039+LC+data1 return 9000+0004+MAC1 failed!!!\n");
		return -1;
	}	
	printf("GetCTModuleStatus-Send 8048C039+LC+data1 return 9000+0004+MAC1 success!!!\n");
}

//*****************************************************************************
//	��������: GetTAModuleSecurityInfo
//	��������: ��ȡTAר��ģ�鰲ȫ��Ϣ        
//	����˵��:
//	formatCoce	[in]	��ʽ��
//	mStatRand [in] ��վ�����
//	�� �� ֵ: 
//	0 : �ɹ�
//	����Ϊ������  
//*****************************************************************************
int GetTAModuleSecurityInfo(unsigned char formatCode, unsigned char* mStatRand, int RandSize, unsigned char* Frame, int *FrameLen)
{
	unsigned char	esamID[8] = {0};
	int i = 0, frmLen=0, uspos=0, recvLen = 0, _rs = 0;
	
	//step01 �ն˷���˫�������֤����
	//TerminalCtAuthenticate(fd, esamID);
	//step02 �ն��·���ȡTAר��ģ�鰲ȫ��Ϣ����֡
	//����������: ��ʽ��(1B)/��վ�����(8B)
	unsigned char _MSendFrame[50] = {0};	
	int _MSendFramePos = 0;

	_MSendFrame[_MSendFramePos++] = 0x68;
	_MSendFrame[_MSendFramePos++] = 0xAA;
	_MSendFrame[_MSendFramePos++] = 0xAA;
	_MSendFrame[_MSendFramePos++] = 0xAA;
	_MSendFrame[_MSendFramePos++] = 0xAA;
	_MSendFrame[_MSendFramePos++] = 0xAA;
	_MSendFrame[_MSendFramePos++] = 0xAA;
	_MSendFrame[_MSendFramePos++] = 0x68;
	_MSendFrame[_MSendFramePos++] = 0x03; 			
	//�����򳤶�L:04H(���ݱ�ʶ)+04H(�����ߴ���)+01H(��ʽ��)+08H(��վ�����)
	_MSendFrame[_MSendFramePos++] = 17;	//10
	//���ݱ�ʶ, ռ4�ֽ�
	_MSendFrame[_MSendFramePos++] = 0x04;
	_MSendFrame[_MSendFramePos++] = 0x00;
	_MSendFrame[_MSendFramePos++] = 0x0F;
	_MSendFrame[_MSendFramePos++] = 0x42;	//14
	//�����ߴ���ռ4�ֽ�,Ĭ��00000000(4B),����Ҫ��
	_MSendFramePos += 4;	//18
	//��ʽ��(1B)
	_MSendFrame[_MSendFramePos++] = formatCode;	//19
	//��վ�����
	memcpy(&_MSendFrame[_MSendFramePos], mStatRand, RandSize);
	_MSendFramePos += RandSize;	//27
	//���
	Protocol_645_Pack(_MSendFrame, _MSendFramePos);
	//У��� + ������ + ֡����
	_MSendFrame[_MSendFramePos++] = GetSum8(_MSendFrame, 27);
	_MSendFrame[_MSendFramePos++] = 0x16;	//29
	//645֡֡����

#if debug_info
	printf("GetTAModuleSecurityInfo: ");
	for (i = 0;i < _MSendFramePos ; i++)
	{
		printf("%02X ", _MSendFrame[i]);
	}
	printf("\n");
#endif
	//�������跭ת�ߵ��ֽ�
	//���ݱ�ʶ��ת
	Array_Mirror(_MSendFrame + 10, 4);
	//�����ߴ��뷭ת
	Array_Mirror(_MSendFrame + 14, 4);
	//��վ�������ת	
	Array_Mirror(_MSendFrame + 19, 8);
	//�ն��·���ȡTAר��ģ�鰲ȫ��Ϣ����֡
	ComTaUart_Send(serial_fd, _MSendFrame, _MSendFramePos);
	//ȷ��Ӧ��֡
	memset(Frame, 0, sizeof(Frame));
	recvLen = ComTaUart_Recv(serial_fd, Frame, sizeof(Frame));
	if (recvLen >= MRS_645_FRAME_LENGTH_MIN)
	{	
		printf("frame received:\n");
#if debug_info
		printf("before datafrm:");
		for (i = 0; i < recvLen ; i++)
		{
			printf("%02X ", Frame[i]);
		}
		printf("\n");
#endif
		
		_rs = mrsFind645Frame(Frame, recvLen, &uspos, &frmLen);
		if (0 != _rs)
		{
			return _rs;
		}

		Protocol_645_Unpack(Frame+uspos, frmLen);
#if debug_info
		printf("after datafrm:");
		for (i = 0; i < frmLen ; i++)
		{
			printf("%02X ", Frame[uspos+i]);
		}
		printf("\n");
		*FrameLen = frmLen;
#endif
	}
	else if(recvLen == -2)
	{
		printf("error occured\n");
	}
	else
	{
		printf("no frame received, ret %d.\n",recvLen);
#if debug_info
		printf("Frame: ");
		for (i = 0; i < recvLen ; i++)
		{
			printf("%02X ", Frame[i]);
		}
		printf("\n");
#endif
		return -1;
	}
	return 0;
}

//*****************************************************************************
//	��������: TAModuleKeyUpdate
//	��������: ��Կ����        
//	����˵��:
//	keyNums	[in]	��֡��Կ����
//	sendDataBuf [in]	��Կ1-N
//	MAC	[in]	MAC��DATA(������MAC)�������
//	�� �� ֵ: 
//	0 : �ɹ�
//	����Ϊ������  
//*****************************************************************************
int TAModuleKeyUpdate(int keyNums, unsigned char* sendDataBuf, int sendDataLens, unsigned char* MAC, int MACSIZE)
{
	//ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, autolock,lock_,-1));
	unsigned char _MSendFrame[50];
	int _MSendFramePos = 0;
	int _rs, _i=0;
	unsigned char recvBuf[MESAM_RECV_BUF_MAX_LEN] = {0};
	int _MTempLens = 0, pos = 0, recvLen = 0;

	_MSendFrame[_MSendFramePos++] = 0x68;
	_MSendFrame[_MSendFramePos++] = 0xAA;
	_MSendFrame[_MSendFramePos++] = 0xAA;
	_MSendFrame[_MSendFramePos++] = 0xAA;
	_MSendFrame[_MSendFramePos++] = 0xAA;
	_MSendFrame[_MSendFramePos++] = 0xAA;
	_MSendFrame[_MSendFramePos++] = 0xAA;
	_MSendFrame[_MSendFramePos++] = 0x68;
	_MSendFrame[_MSendFramePos++] = 0x03; 			
	//�����򳤶�L:04H(���ݱ�ʶ)+04H(�����ߴ���)+01H(���ݳ���)
	_MSendFrame[_MSendFramePos++] = 8+1+sendDataLens+MACSIZE;	//10
	//���ݱ�ʶ, ռ4�ֽ�
	_MSendFrame[_MSendFramePos++] = 0x43;
	_MSendFrame[_MSendFramePos++] = 0x0F;
	_MSendFrame[_MSendFramePos++] = 0x00;
	_MSendFrame[_MSendFramePos++] = 0x04;	//14
	//�����ߴ���ռ4�ֽ�,Ĭ��00000000(4B),����Ҫ��
	_MSendFramePos += 4;
	//��ʽ��(1B)
	_MSendFrame[_MSendFramePos++] = keyNums;	//19
	//��Կ������(1B)+����(1B)+��Կ���(1B)+��Կ״̬��ʶ(1B)+��Կ(32B)
	for(_i=0; _i<keyNums; _i++)
	{
		memcpy(&_MSendFrame[_MSendFramePos], sendDataBuf+pos, 4);
		pos += 4;
		_MSendFramePos += 4;
		Array_Mirror(sendDataBuf+pos, 32);
		memcpy(&_MSendFrame[_MSendFramePos], sendDataBuf+pos, 32);
		pos += 32;
		_MSendFramePos += 32;
	}
	memcpy(&_MSendFrame[_MSendFramePos], MAC, MACSIZE);	
	Array_Mirror(&_MSendFrame[_MSendFramePos], MACSIZE);
	_MSendFramePos += 4;
	
	//���:��ַ��ߵ��ֽڷ�ת,�����������ֽ�+0x33
	Protocol_645_Pack(_MSendFrame, _MSendFramePos);
	//У��� + ������ + ֡����
	_MSendFrame[_MSendFramePos++] = GetSum8(_MSendFrame, _MSendFramePos);
	_MSendFrame[_MSendFramePos++] = 0x16;

	ComTaUart_Send(serial_fd, _MSendFrame, _MSendFramePos);	

	recvLen = ComTaUart_Recv(serial_fd, recvBuf, MRS_PROTO645_DATAGRAM_LEN_MAX);

	if (recvLen >= MRS_645_FRAME_LENGTH_MIN)
	{

	}
	else if(recvLen == -2)
	{
	}
	else
	{
		printf("no frame received, ret %d.\n", recvLen);
	}
	return 0;
}
//*****************************************************************************
//	��������: GetTerminalEnableControlParam
//	��������: �ն�ʹ�ܿ��Ʋ�����ѯ        
//	����˵��:
//	keyNums	[in]	��֡��Կ����
//	sendDataBuf [in]	��Կ1-N
//	MAC	[in]	MAC��DATA(������MAC)�������
//	�� �� ֵ: 
//	0 : �ɹ�
//	����Ϊ������  
//*****************************************************************************

int GetTerminalEnableControlParam(unsigned char* mStatRand, int RandSize, unsigned char* phaseAEnable, unsigned char* phaseBEnable, unsigned char* phaseCEnable)
{
	unsigned char _MSendFrame[50];
	int _MSendFramePos = 0, sendDataLens = 11, pos = 0;
	
	//_MSendFramePos = 0, pos=0, sndDataLens = 32;//, recvpos=0;
	_MSendFrame[_MSendFramePos++] = 0x80;
	_MSendFrame[_MSendFramePos++] = 0x48;
	_MSendFrame[_MSendFramePos++] = 0xC0;
	_MSendFrame[_MSendFramePos++] = 0x39;
	//LC
	_MSendFrame[_MSendFramePos++] = sendDataLens>>8;	
	_MSendFrame[_MSendFramePos++] = sendDataLens&0xff;
	//�Է������ݵ���
	unsigned  char* tmpDataBuf = (unsigned char *)malloc(sendDataLens * sizeof(unsigned char));
	if(tmpDataBuf == NULL)
	{
		printf("GetTerminalEnableControlParam Malloc failed\n");
	}
	//R4����
	ByteReverse(mainStatRandom4, 8, tmpDataBuf+pos);
	pos += 8;
	//~R4����
	ByteReverse(mainStatRandom4Reverse, 8, tmpDataBuf+pos);
	pos += 8;	
	//�ն�ʹ�ܿ���A
	ByteReverse(phaseAEnable, 1, tmpDataBuf+pos);
	pos += 1;
	//�ն�ʹ�ܿ���B
	ByteReverse(phaseBEnable, 1, tmpDataBuf+pos);
	pos += 1;
	//�ն�ʹ�ܿ���C
	ByteReverse(phaseCEnable, 1, tmpDataBuf+pos);
	
	return 0;
}
