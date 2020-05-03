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

//��վ��ȡ��վ�����R4,���ͻ�ȡ�ն˰�ȫ��֤��Ϣ����
unsigned char mainStatRandom4[MAIN_STATION_RANDOM_SIZE] = {0};		//R4
unsigned char mainStatRandom4Reverse[MAIN_STATION_RANDOM_SIZE] = {0};		//~R4
unsigned char ESAMID[8] = {0};
extern int m_MFd;




static int OpenComPort(int fd,char* port)  
{  
     
    fd = open( port, O_RDWR);  
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
static void CloseComPort(int fd)  
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
static int ComPort_Set(int fd,int speed,int flow_ctrl,int databits,int stopbits,int parity)  
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
static int ComPort_Recv(int fd, char *rcv_buf,int data_len)  
{  
    int len,fs_sel;  
    fd_set fs_read;  
     
    struct timeval time;  
     
    FD_ZERO(&fs_read);  
    FD_SET(fd,&fs_read);  
     
    time.tv_sec = 0;  
    time.tv_usec =150000 ;  
     
    //ʹ��selectʵ�ִ��ڵĶ�·ͨ��  
    fs_sel = select(fd+1,&fs_read,NULL,NULL,&time);  
    //printf("fs_sel = %d\n",fs_sel);  
    if(fs_sel>0)  
    {  
        len = read(fd,rcv_buf,data_len);  
        //printf("I am right!(version1.2) len = %d fs_sel = %d\n",len,fs_sel);  
        return len;  
    }  
    else if(fs_sel == 0)  //timeout
    {  
       printf("select timeoyt");
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
static int ComPort_Send(int fd, char *send_buf,int data_len)  
{  
    int len = 0;  
     
    len = write(fd,send_buf,data_len);  
    if (len == data_len )  
    {  
    	int i;
        printf("ESAM send data is %s\n",send_buf);
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
int TerminalCtAuthenticate(int fd, unsigned char* ESAMID)
{
	int recvDataBufPos = 0, rs = -1;
	unsigned char	TerminFrame00[] = {0x80,0x81,0x08,0x00,0x00,0x00};	//��ȡT-ESAM�����R1
	unsigned char	TerminFrame02[] = {0x80,0x83,0x34,0x35,0x00,0x10};	//�ն˼���R2����ER2������ER32
	unsigned char	TerminSendFrame[40] = {0};	
	unsigned char	recvDataBuf[32] = {0};
	unsigned int	len=0, recvBufSize = 32;
	unsigned char	datafrm[128] = {0};
	unsigned char	_MSendFrame[50];	
	int _MSendFramePos = 0, pos=0, sndDataLens = 32, recvpos=0;
	
	//step01 �ն˶�T-ESAM����,����:808108000000	��ȡT-ESAM�����R1
	//����:9000+0008+R1(8B),�ܼ�12B
	rs = MESamComRecv(TerminFrame00, 6, recvDataBuf, &recvDataBufPos, recvBufSize);
	if (rs != 0)
	{
		printf("send 808108000000 failed!!!\n");
		return -1;
	}

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
	_MSendFrame[_MSendFramePos++] = 0x40;
	_MSendFrame[_MSendFramePos++] = 0x0F;
	_MSendFrame[_MSendFramePos++] = 0x00;
	_MSendFrame[_MSendFramePos++] = 0x04;	//14
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
	//SendLen = 27;

	ComPort_Send(fd, _MSendFrame, _MSendFramePos);  
	
	//ȷ��Ӧ�𲢷���:ER1(8B)+ER31(16B)+ESAMID(8B)+R2(8B)
	//����Ӧ��:SERR������(2B)
	len = ComPort_Recv(fd, datafrm, sizeof(datafrm));
	if (len >= MRS_645_FRAME_LENGTH_MIN)
	{

	}
	else if(len == -2)
	{
	}
	else
	{
		printf("no frame received, ret %d.\n",len);
	}
	
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
	memcpy(ESAMID, datafrm+MRS_645_FRAME_DATA_OFFSET+4+24, 8);
	ByteReverse(datafrm+MRS_645_FRAME_DATA_OFFSET+4+24, 8, tmpDataBuf+pos);
	pos += 8;
	//ER1����
	ByteReverse(datafrm+MRS_645_FRAME_DATA_OFFSET+4, 8, tmpDataBuf+pos);
	pos += 8;
	//ER31����
	ByteReverse(datafrm+MRS_645_FRAME_DATA_OFFSET+4+8, 16, tmpDataBuf+pos);
	pos += 16;
	memcpy(&_MSendFrame[_MSendFramePos], tmpDataBuf, pos);
	free(tmpDataBuf);
	tmpDataBuf = NULL;
	_MSendFramePos = _MSendFramePos+pos;
	rs = MESamComRecv(_MSendFrame, _MSendFramePos, recvDataBuf, &recvDataBufPos, recvBufSize);
	if (rs != 0)
	{
		printf("send 808534350020+ESAMID(8B)+ER1(8B)+ER31(16B) failed!!!\n");
		return -1;
	}

	//step04: �ն˶�T-ESAM����
	//�ն˷���808334350010(6B)+ESAMID(8B)+R2(8B) �ն˼���R2����ER2������ER32
	memset(TerminSendFrame, 0, sizeof(TerminSendFrame));
	memcpy(TerminSendFrame, TerminFrame02, 6);
	memcpy(TerminSendFrame+6, datafrm, 16);	
	memset(recvDataBuf, 0, recvBufSize);
	recvDataBufPos = 0;	
	//����:9000+0018+ER2(8B)+ER32(16B) �ܼ�(28B)
	rs = MESamComRecv(TerminSendFrame, 22, recvDataBuf, &recvDataBufPos, recvBufSize);
	if (rs != 0)
	{
		printf("send 808334350010+ESAMID(8B)+R2(8B) failed!!!\n");
		return -1;
	}

	//step05:�ն˽�ER2��ER32��������֤�ڶ�֡���У����͸�CTģ��
	_MSendFramePos = 0;
	//memset(SendBuf, 0, BUFMAXSIZE);
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
	_MSendFrame[_MSendFramePos++] = 0x41;
	_MSendFrame[_MSendFramePos++] = 0x0F;
	_MSendFrame[_MSendFramePos++] = 0x00;
	_MSendFrame[_MSendFramePos++] = 0x04;	//14
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

	//���������֤�ڶ�֡
	ComPort_Send(fd, _MSendFrame, _MSendFramePos);
	//Э��ֻ�з���Ӧ��
	return recvDataBufPos;
}

//�ն��ϴ�T-ESAM���к�(8B)+֤�����к�(16B)+������(4B)+оƬ״̬(1B)+��Կ�汾(8B)+�ն������R5(8B)+MAC(4B)
//*****************************************************************************
//	��������: GetTASecurityInfo
//	��������: ��ȡTAר��ģ�鰲ȫ��Ϣ        
//	����˵��:
//	sendDataBuf [in] ��վ�����R4
//  sendDataLens [in] ��վ���������
//	�� �� ֵ: 
//	0 : �ɹ�
//	����Ϊ������  
//*****************************************************************************
int GetTASecurityInfo(unsigned char* sendDataBuf, int sendDataLens, unsigned char* recvDataBuf, unsigned int recvBufSize)
{
	//ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, autolock,lock_,-1));
	int recvDataBufPos = 0;
	int _rs = -1;
	unsigned char _ChipStatus;
	unsigned char _MSendFrame[50];	
	int _MSendFramePos = 0, pos=0, sndDataLens = 61, recvpos=0;
	unsigned char *tmpDataBuf = NULL;

	//R4
	memcpy(mainStatRandom4, sendDataBuf, sendDataLens);
	//~R4
	BitReverse(mainStatRandom4, sendDataLens, mainStatRandom4Reverse);	
	
	//step01	����:800E00020000
	//����:9000+0008+T-ESAM���к�
	unsigned char ChipSerFramer[] = {0x80,0x0E,0x00,0x02,0x00,0x00,};//оƬ���к�
	_rs = MESamComRecv(ChipSerFramer,6,recvDataBuf,&recvDataBufPos, recvBufSize);
	if (_rs != 0)
	{
		printf("GetTASecurityInfo-ChipSerial failed!!!\n");
		return -1;
	}
	printf("GetTASecurityInfo-ChipSerial success!!!\n");

	//step02	����:800E00050000		����:9000+0001+оƬ״̬��Ϣ
	unsigned char ChipStaSerFramer[] = {0x80,0x0E,0x00,0x05,0x00,0x00,};//оƬ״̬
	_rs = MESamComRecv(ChipStaSerFramer,6,&_ChipStatus,&pos,sizeof(_ChipStatus));
	printf("GetTASecurityInfo-ChipState %d!!!\n", _ChipStatus);
	if (_rs != 0)
	{
		printf("GetTASecurityInfo-ChipState failed!!!\n");
		return -1;
	}
	printf("GetTASecurityInfo-ChipState success!!!\n");

	//step03 ����:8032+p1(оƬ״̬��Ϣ)+02+0000	��ȡ֤�����к�
	//����:9000+0010+֤�����к�
	unsigned char CerSerFramer[] = {0x80,0x32,0x00,0x02,0x00,0x00,};//֤�����к�
	CerSerFramer[2] = _ChipStatus;
	_rs = MESamComRecv(CerSerFramer,6,recvDataBuf,&recvDataBufPos, recvBufSize);
	if (_rs != 0)
	{
		printf("GetTASecurityInfo-CertificationSerial failed!!!\n");
		return -1;
	}
	printf("GetTASecurityInfo-CertificationSerial success!!!\n");

	//step04 ����800E00030000	����:9000+0004+���߼�������Ϣ
	unsigned char CountFramer[] = {0x80,0x0E,0x00,0x03,0x00,0x00,};//������
	_rs = MESamComRecv(CountFramer,6,recvDataBuf,&recvDataBufPos, recvBufSize);
	if (_rs != 0)
	{
		printf("GetTASecurityInfo-Counter failed!!!\n");
		return -1;
	}
	printf("GetTASecurityInfo-Counter success!!!\n");

	//step05 оƬ״̬
	recvDataBuf[recvDataBufPos ++] = _ChipStatus;//оƬ״̬

	//step06 ����:800E00060000	��ȡ��Կ�汾:8B
	//����:9000+0008+��Կ�汾��Ϣ
	unsigned char keySerFramer[] = {0x80,0x0E,0x00,0x06,0x00,0x00,};//��Կ�汾
	_rs = MESamComRecv(keySerFramer,6,recvDataBuf,&recvDataBufPos, recvBufSize);
	if (_rs != 0)
	{
		printf("GetTASecurityInfo-KeyVersion failed!!!\n");
		return -1;
	}

	//step07 ����:801A08000000 ȡ�ն������R5
	//����:9000+0008+R5(8B)
	unsigned char RandomNumTermin[] = {0x80,0x1A,0x08,0x00,0x00,0x00};	//�ն������R5
	_rs = MESamComRecv(RandomNumTermin,6,recvDataBuf,&recvDataBufPos, recvBufSize);
	if (_rs != 0)
	{
		printf("GetTASecurityInfo-RandomNumberR5 failed!!!\n");
		return -1;
	}

	//step08 ����8048C039+LC+data1	����MAC
	//����:9000+0004+MAC
	//LC:data1����,LCΪ2�ֽ�
	//data1:R4(8B)+~R4(8B)+T-ESAM���к�(8B)+֤�����к�(16B)+������(4B)+оƬ״̬(1B)+��Կ�汾(8B)+�ն������R5(8B),�ܼ�61B
	_MSendFrame[_MSendFramePos++] = 0x80;
	_MSendFrame[_MSendFramePos++] = 0x48;
	_MSendFrame[_MSendFramePos++] = 0xC0;
	_MSendFrame[_MSendFramePos++] = 0x39;
	//LC
	_MSendFrame[_MSendFramePos++] = sndDataLens>>8;	
	_MSendFrame[_MSendFramePos++] = sndDataLens&0xff;
	//�Է������ݵ���
	tmpDataBuf = (unsigned char *)malloc(sndDataLens * sizeof(unsigned char));
	//R4����
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
	_rs = MESamComRecv(_MSendFrame, _MSendFramePos, recvDataBuf, &recvDataBufPos, recvBufSize);
	if (_rs != 0)
	{
		printf("GetTASecurityInfo-MAC failed!!!\n");
		return -1;
	}

	//T-ESAM���к�(8B)+֤�����к�(16B)+������(4B)+оƬ״̬(1B)+��Կ�汾(8B)+�ն������R5(8B)+MAC(4B)
	//�����ݵķ�����
	tmpDataBuf = (unsigned char *)malloc(40 * sizeof(unsigned char));
	pos = 0;
	ByteReverse(recvDataBuf, 8, tmpDataBuf);	//T-ESAM���к�(8B)
	pos += 8;
	ByteReverse(recvDataBuf+pos, 16, tmpDataBuf+pos);	//֤�����к�(16B)
	pos += 16;
	ByteReverse(recvDataBuf+pos, 4, tmpDataBuf+pos);	//������(4B)
	pos += 4;
	ByteReverse(recvDataBuf+pos, 1, tmpDataBuf+pos);	//оƬ״̬(1B)
	pos++;
	ByteReverse(recvDataBuf+pos, 8, tmpDataBuf+pos);	//��Կ�汾(8B)
	pos += 8;
	ByteReverse(recvDataBuf+pos, 8, tmpDataBuf+pos);	//�ն������R5(8B)
	pos += 4;
	ByteReverse(recvDataBuf+pos, 4, tmpDataBuf+pos);	//MAC(4B)	�ܼ�49B
	//�����������ݿ��ص�ԭ����buf
	memcpy(recvDataBuf, tmpDataBuf, recvDataBufPos);
	free(tmpDataBuf);
	tmpDataBuf = NULL;
	return recvDataBufPos;
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
int GetTABaudrate(int fd, unsigned char sendDataBuf, unsigned char *BaudrateWord)
{	
	int _rs = -1;
	unsigned int	recvDataBufPos=0, len;
	unsigned char	datafrm[128] = {0};
	unsigned char	esamID[8] = {0};
	int	pos=0;
	unsigned char recvDataBuf[10];
	unsigned char recvBufSize=10;

	memset(recvDataBuf, 0, 10);
	//step01 �ն˷���˫�������֤����
	TerminalCtAuthenticate(fd, esamID);

	unsigned char _MSendFrame[50];	
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
	_MSendFrame[_MSendFramePos++] = 0x44;
	_MSendFrame[_MSendFramePos++] = 0x0F;
	_MSendFrame[_MSendFramePos++] = 0x00;
	_MSendFrame[_MSendFramePos++] = 0x04;	//14
	//�����ߴ���ռ4�ֽ�,Ĭ��00000000,����Ҫ��
	_MSendFramePos += 4;
	//��ʽ��(1B)
	_MSendFrame[_MSendFramePos++] = sendDataBuf;	//19
	//���
	Protocol_645_Pack(_MSendFrame, 19);
	//У��� + ������ + ֡����
	_MSendFrame[_MSendFramePos++] = GetSum8(_MSendFrame, 19);
	_MSendFrame[_MSendFramePos++] = 0x16;
	//SendLen = 20;

	ComPort_Send(fd, _MSendFrame, _MSendFramePos);  
	
	//ȷ��Ӧ�𲢷���:��ʽ��(1B)+������������(1B)+MAC(4B),�ܼ�6B
	//data1:��ʽ��+������������
	len = ComPort_Recv(fd, datafrm, sizeof(datafrm));

	if (len >= MRS_645_FRAME_LENGTH_MIN)
	{

	}
	else if(len == -2)
	{
	}
	else
	{
		printf("no frame received, ret %d.\n",len);
	}
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
	ByteReverse(esamID, 8, tmpDatabuf+pos);
	pos += 8;
	//��ʽ�뷴��
	ByteReverse(&datafrm[MRS_645_FRAME_DATA_OFFSET+4], 1, tmpDatabuf+pos);
	pos += 1;
	//�����������ַ���
	ByteReverse(&datafrm[MRS_645_FRAME_DATA_OFFSET+5], 1, tmpDatabuf+pos);
	pos += 1;
	//*BaudrateWord
	//MAC1����	
	ByteReverse(&datafrm[MRS_645_FRAME_DATA_OFFSET+6], 4, tmpDatabuf+pos);
	pos += 4;
	memcpy(&_MSendFrame[_MSendFramePos], tmpDatabuf, pos);
	free(tmpDatabuf);
	tmpDatabuf = NULL;
	_MSendFramePos += pos;
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
int SetTABaudrate(int fd, unsigned char formatCode, unsigned char BaudrateWord, unsigned char *MAC, unsigned char* MAC2, unsigned int MAC2Len)
{
	unsigned char	esamID[8] = {0};
	int	len = 0, _rs = -1, recvDataBufPos = 0, pos=0;
	
	//step01 �ն˷���˫�������֤����
	TerminalCtAuthenticate(fd, esamID);
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

	ComPort_Send(fd, _MSendFrame, _MSendFramePos);  
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
int GetCTModuleStatus(int fd, unsigned char *formatCode, unsigned char *phaseABCStatus, unsigned char *MAC2)
{
	unsigned char _MSendFrame[50];	
	int _MSendFramePos = 0, pos=0, sendDataLens = 16;
	unsigned char	esamID[8] = {0};
	int recvLen = 0, _j=0, _rs = -1;
	unsigned char recvBuf[MESAM_RECV_BUF_MAX_LEN] = {0};
	
	//step01 ��վ�·���ȡ�ն˰�ȫ��֤��Ϣ����

	//step02 �ն˶�ȡCTģ��״̬
	//step02.1	�ն˷���˫�������֤����
	TerminalCtAuthenticate(fd, esamID);
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
	_MSendFrame[_MSendFramePos++] = 0x00;
	_MSendFrame[_MSendFramePos++] = 0x00;
	_MSendFrame[_MSendFramePos++] = 0x00;
	_MSendFrame[_MSendFramePos++] = 0x07;
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
	//SendLen = 21;

	ComPort_Send(fd, _MSendFrame, _MSendFramePos);  
	recvLen = ComPort_Recv(fd, recvBuf, sizeof(recvBuf));

	if (recvLen >= MRS_645_FRAME_LENGTH_MIN)
	{

	}
	else if(recvLen == -2)
	{
	}
	else
	{
		printf("no frame received, ret %d.\n",recvLen);
	}

	//Ӧ��֡:�����򳤶�L=04H(���ݱ�ʶ)+m(���ݳ���)
	//memcpy(phaseABCStatus, datafrm+MRS_645_FRAME_DATA_OFFSET+4, 6);
	//��ʽ��
	*formatCode =  recvBuf[MRS_645_FRAME_DATA_OFFSET+4];
	//ABC�����Ե����
	memcpy(phaseABCStatus, recvBuf+MRS_645_FRAME_DATA_OFFSET+4+1, 3);
	//����MAC
	memcpy(MAC2, recvBuf+MRS_645_FRAME_DATA_OFFSET+4+4, 4);	
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
	ByteReverse(esamID, 8, tmpDataBuf+pos);
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

#if 1
	memset(recvBuf, 0, MESAM_RECV_BUF_MAX_LEN);
	pos = 0;
	_rs = MESamComRecv(_MSendFrame, _MSendFramePos, recvBuf, &pos, MESAM_RECV_BUF_MAX_LEN);
	if (_rs != 0)
	{
		printf("SetTABaudrate-Set Baudrate failed!!!\n");
		return -1;
	}
#else
	_rs = MEsamDataSend(_MSendFrame, _MSendFramePos);

	if(_rs < 0)
	{
		printf("GetCTModuleStatus: MEsamDataSend failed\n");
		return -1;
	}

	//����:8049813A0010+data2 ����:9000+0000
	memset(recvBuf, 0, MESAM_RECV_BUF_MAX_LEN);
	if((recvLen = MEsamDataRecv(-1, recvBuf)) >= 0)
	{
		printf("MEsamDataRecv data:");
		for (_j = 0; _j < recvLen; _j++)
		{
			printf("%02X ", recvBuf[_j]);
		}
		printf("\n");
		
		if (recvBuf[0] == 0x55)	//??
		{
			if ((recvBuf[1] == 0x90)&&(recvBuf[2] == 0x00))
			{
				printf("MEsamDataRecv success!!!\n");
			}
			else
			{
				printf("MEsamDataRecv recvBuf not found 0x9000 !!!\n");
			}
		}
		else
		{
			printf("MEsamDataRecv recvBuf[0] not found 0x55 !!!\n");
		}
	}
	else
	{
		printf("MEsamDataRecv failed: recvLen <= 0 !!!\n");
	}
#endif

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

#if 1
	memset(recvBuf, 0, MESAM_RECV_BUF_MAX_LEN);
	pos = 0;
	_rs = MESamComRecv(_MSendFrame, _MSendFramePos, recvBuf, &pos, MESAM_RECV_BUF_MAX_LEN);
	if (_rs != 0)
	{
		printf("SetTABaudrate-Set Baudrate failed!!!\n");
		return -1;
	}

#else
	_rs = MEsamDataSend(_MSendFrame, _MSendFramePos);
	if(_rs < 0)
	{
		printf("GetCTModuleStatus: MEsamDataSend 8048C039+LC+data1 failed\n");
		return -1;
	}

	//����:8049813A0010+data2 ����:9000+0000	
	memset(recvBuf, 0, MESAM_RECV_BUF_MAX_LEN);
	if((recvLen = MEsamDataRecv(-1, recvBuf)) >= 0)
	{
		printf("MEsamDataRecv data:");
		for (_j = 0; _j < recvLen; _j++)
		{
			printf("%02X ", recvBuf[_j]);
		}
		printf("\n");
		
		if (recvBuf[0] == 0x55)	//??
		{
			if ((recvBuf[1] == 0x90)&&(recvBuf[2] == 0x00))
			{
				if((recvBuf[3] == 0x00)&&(recvBuf[4] == 0x04))
				{
					_rs = 0;
					printf("MEsamDataRecv success!!!\n");					
				}
				else
				{
					printf("MEsamDataRecv recvBuf not found 0x0004 !!!\n");
				}
			}
			else
			{
				printf("MEsamDataRecv recvBuf not found 0x9000 !!!\n");
			}
		}
		else
		{
			printf("MEsamDataRecv recvBuf[0] not found 0x55 !!!\n");
		}
	}
	else
	{
		printf("MEsamDataRecv failed: recvLen <= 0 !!!\n");
	}
#endif
}

//*****************************************************************************
//	��������: GetTAModuleEsamInfo
//	��������: ��ȡTAר��ģ�鰲ȫ��Ϣ        
//	����˵��:
//	formatCoce	[in]	��ʽ��
//	mStatRand [in] ��վ�����
//	�� �� ֵ: 
//	0 : �ɹ�
//	����Ϊ������  
//*****************************************************************************
int GetTAModuleEsamInfo(int fd, unsigned char formatCode, unsigned char* mStatRand, int RandSize, unsigned char* recvBuf, int *recvLen)
{
	unsigned char	esamID[8] = {0};
	
	//step01 �ն˷���˫�������֤����
	TerminalCtAuthenticate(fd, esamID);
	//step02 �ն��·���ȡTAר��ģ�鰲ȫ��Ϣ����֡
	//����������: ��ʽ��(1B)/��վ�����(8B)
	unsigned char _MSendFrame[50];	
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
	//�����򳤶�L:04H(���ݱ�ʶ)+04H(�����ߴ���)+(���ݳ���)
	_MSendFrame[_MSendFramePos++] = 14;	//10
	//���ݱ�ʶ, ռ4�ֽ�
	_MSendFrame[_MSendFramePos++] = 0x42;
	_MSendFrame[_MSendFramePos++] = 0x0F;
	_MSendFrame[_MSendFramePos++] = 0x00;
	_MSendFrame[_MSendFramePos++] = 0x04;	//14
	//�����ߴ���ռ4�ֽ�,Ĭ��00000000(4B),����Ҫ��
	_MSendFramePos += 4;
	//��ʽ��(1B)
	_MSendFrame[_MSendFramePos++] = formatCode;	//19
	//��վ�����
	memcpy(&_MSendFrame[_MSendFramePos], mStatRand, RandSize);
	_MSendFramePos += RandSize;
	//���:��ַ��ߵ��ֽڷ�ת,�����������ֽ�+0x33
	Protocol_645_Pack(_MSendFrame, _MSendFramePos);
	//У��� + ������ + ֡����
	_MSendFrame[_MSendFramePos++] = GetSum8(_MSendFrame, _MSendFramePos);
	_MSendFrame[_MSendFramePos++] = 0x16;

	ComPort_Send(fd, _MSendFrame, _MSendFramePos);  

	*recvLen = ComPort_Recv(fd, recvBuf, MRS_PROTO645_DATAGRAM_LEN_MAX);

	if (*recvLen >= MRS_645_FRAME_LENGTH_MIN)
	{

	}
	else if(*recvLen == -2)
	{
	}
	else
	{
		printf("no frame received, ret %d.\n", *recvLen);
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
int TAModuleKeyUpdate(int fd, int keyNums, unsigned char* sendDataBuf, int sendDataLens, unsigned char* MAC, int MACSIZE)
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

	ComPort_Send(fd, _MSendFrame, _MSendFramePos);	

	recvLen = ComPort_Recv(fd, recvBuf, MRS_PROTO645_DATAGRAM_LEN_MAX);

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
