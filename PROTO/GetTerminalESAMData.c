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

//主站获取主站随机数R4,发送获取终端安全认证信息命令
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
    //恢复串口为阻塞状态                                 
    if(fcntl(fd, F_SETFL, 0) < 0)  
    {  
        printf("fcntl failed!\n");  
        return(-1);  
    }       
    else  
    {  
        printf("fcntl=%d\n",fcntl(fd, F_SETFL,0));  
    }  
    //测试是否为终端设备      
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
* 名称：  UART0_Close
* 功能：关闭串口并返回串口设备文件描述
* 入口参数：fd    :文件描述符     port :串口号(ttyS0,ttyS1,ttyS2)
* 出口参数：void
*******************************************************************/   
static void CloseComPort(int fd)  
{  
    close(fd);  
}  


/*******************************************************************
* 名称：UART0_Set
* 功能：   设置串口数据位，停止位和效验位
* 入口参数：fd串口文件描述符
*           speed      串口速度
*           flow_ctrl  数据流控制
*           databits   数据位   取值为 7 或者8
*           stopbits   停止位   取值为 1 或者2
*           parity     效验类型 取值为N,E,O,,S
*出口参数：正确返回为1，错误返回为0
*******************************************************************/  
static int ComPort_Set(int fd,int speed,int flow_ctrl,int databits,int stopbits,int parity)  
{  
     
    int   i;  
    int   status;  
    int   speed_arr[] = {  B115200, B19200, B9600, B4800, B2400, B1200, B300};  
    int   name_arr[] = { 115200,  19200,  9600,  4800,  2400,  1200,  300};  
           
    struct termios options;  
     
    /*tcgetattr(fd,&options)得到与fd指向对象的相关参数，并将它们保存于options,该函数还可以测试配置是否正确，该串口是否可用等。若调用成功，函数返回值为0，若调用失败，函数返回值为1.
    */  
    if( tcgetattr( fd,&options)  !=  0)  
    {  
        perror("SetupSerial 1");      
        return(-1);   
    }  
    
    //设置串口输入波特率和输出波特率  
    for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++)  
    {  
        if  (speed == name_arr[i])  
        {               
            cfsetispeed(&options, speed_arr[i]);   
            cfsetospeed(&options, speed_arr[i]);    
        }  
    }       
     
    //修改控制模式，保证程序不会占用串口  
    options.c_cflag |= CLOCAL;  
    //修改控制模式，使得能够从串口中读取输入数据  
    options.c_cflag |= CREAD;  
    
    //设置数据流控制  
    switch(flow_ctrl)  
    {  
        
        case 0 ://不使用流控制  
              options.c_cflag &= ~CRTSCTS;  
              break;     
        
        case 1 ://使用硬件流控制  
              options.c_cflag |= CRTSCTS;  
              break;  
        case 2 ://使用软件流控制  
              options.c_cflag |= IXON | IXOFF | IXANY;  
              break;  
    }  
    //设置数据位  
    //屏蔽其他标志位  
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
    //设置校验位  
    switch (parity)  
    {    
        case 'n':  
        case 'N': //无奇偶校验位。  
                 options.c_cflag &= ~PARENB;   
                 options.c_iflag &= ~INPCK;      
                 break;   
        case 'o':    
        case 'O'://设置为奇校验      
                 options.c_cflag |= (PARODD | PARENB);   
                 options.c_iflag |= INPCK;               
                 break;   
        case 'e':   
        case 'E'://设置为偶校验    
                 options.c_cflag |= PARENB;         
                 options.c_cflag &= ~PARODD;         
                 options.c_iflag |= INPCK;        
                 break;  
        case 's':  
        case 'S': //设置为空格   
                 options.c_cflag &= ~PARENB;  
                 options.c_cflag &= ~CSTOPB;  
                 break;   
        default:    
                 fprintf(stderr,"Unsupported parity\n");      
                 return (-1);   
    }   
    // 设置停止位   
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
     
    //修改输出模式，原始数据输出  
    options.c_oflag &= ~OPOST;  
    
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);  
    //options.c_lflag &= ~(ISIG | ICANON);  
     
    //设置等待时间和最小接收字符  
    options.c_cc[VTIME] = 1; /* 读取一个字符等待1*(1/10)s */    
    options.c_cc[VMIN] = 1; /* 读取字符的最少个数为1 */  
     
    //如果发生数据溢出，接收数据，但是不再读取 刷新收到的数据但是不读  
    tcflush(fd,TCIFLUSH);  
     
    //激活配置 (将修改后的termios数据设置到串口中）  
    if (tcsetattr(fd,TCSANOW,&options) != 0)    
    {  
        perror("com set error!\n");    
        return (-1);   
    }  
    return (0);   
}  



/*******************************************************************
* 名称：   UART0_Recv
* 功能：   接收串口数据
* 入口参数：fd:文件描述符     
*           rcv_buf:接收串口中数据存入rcv_buf缓冲区中
*           data_len:一帧数据的长度
* 出口参数：    正确返回为1，错误返回为0
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
     
    //使用select实现串口的多路通信  
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
* 名称：   UART0_Send
* 功能：发送数据
* 入口参数：fd:文件描述符     
*           send_buf:存放串口发送数据
*           data_len:一帧数据的个数
* 出口参数：正确返回为1，错误返回为0
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


//终端与CT模块双向身份认证
int TerminalCtAuthenticate(int fd, unsigned char* ESAMID)
{
	int recvDataBufPos = 0, rs = -1;
	unsigned char	TerminFrame00[] = {0x80,0x81,0x08,0x00,0x00,0x00};	//获取T-ESAM随机数R1
	unsigned char	TerminFrame02[] = {0x80,0x83,0x34,0x35,0x00,0x10};	//终端加密R2产生ER2并产生ER32
	unsigned char	TerminSendFrame[40] = {0};	
	unsigned char	recvDataBuf[32] = {0};
	unsigned int	len=0, recvBufSize = 32;
	unsigned char	datafrm[128] = {0};
	unsigned char	_MSendFrame[50];	
	int _MSendFramePos = 0, pos=0, sndDataLens = 32, recvpos=0;
	
	//step01 终端对T-ESAM操作,发送:808108000000	获取T-ESAM随机数R1
	//返回:9000+0008+R1(8B),总计12B
	rs = MESamComRecv(TerminFrame00, 6, recvDataBuf, &recvDataBufPos, recvBufSize);
	if (rs != 0)
	{
		printf("send 808108000000 failed!!!\n");
		return -1;
	}

	//step02 CT模块操作,身份认证第一帧下行,发送给CT模块
	//CT模块执行身份认证第一帧对应任务，返回ER1,ER31,ESAMID,R2
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
	//数据域长度L:04H(数据标识)+04H(操作者代码)+m(数据长度)
	_MSendFrame[_MSendFramePos++] = 16;
	//数据标识, 占4字节
	_MSendFrame[_MSendFramePos++] = 0x40;
	_MSendFrame[_MSendFramePos++] = 0x0F;
	_MSendFrame[_MSendFramePos++] = 0x00;
	_MSendFrame[_MSendFramePos++] = 0x04;	//14
	//操作者代码占4字节,默认00000000,不需要变
	_MSendFramePos += 4;
	//T-ESAM随机数R1，占8字节
	memcpy(&_MSendFrame[_MSendFramePos], &recvDataBuf, 8);
	_MSendFramePos += 8;
	//打包
	Protocol_645_Pack(_MSendFrame, _MSendFramePos);
	//校验和 + 结束符 + 帧长度
	_MSendFrame[_MSendFramePos++] = GetSum8(_MSendFrame, 26);
	_MSendFrame[_MSendFramePos++] = 0x16;
	//SendLen = 27;

	ComPort_Send(fd, _MSendFrame, _MSendFramePos);  
	
	//确认应答并返回:ER1(8B)+ER31(16B)+ESAMID(8B)+R2(8B)
	//否认应答:SERR特征字(2B)
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
	
	//step03: 终端对T-ESAM操作, 发送808534350020+ESAMID(8B)+ER1(8B)+ER31(16B) 终端认证ER1和ER31
	_MSendFramePos = 0, pos=0, sndDataLens = 32;//, recvpos=0;
	_MSendFrame[_MSendFramePos++] = 0x80;
	_MSendFrame[_MSendFramePos++] = 0x85;
	_MSendFrame[_MSendFramePos++] = 0x34;
	_MSendFrame[_MSendFramePos++] = 0x35;
	_MSendFrame[_MSendFramePos++] = 0x00;
	_MSendFrame[_MSendFramePos++] = 0x20;	//6
	//对发送数据倒序
	unsigned char* tmpDataBuf = (unsigned char *)malloc(sndDataLens * sizeof(unsigned char));
	//ESAMID倒序
	memcpy(ESAMID, datafrm+MRS_645_FRAME_DATA_OFFSET+4+24, 8);
	ByteReverse(datafrm+MRS_645_FRAME_DATA_OFFSET+4+24, 8, tmpDataBuf+pos);
	pos += 8;
	//ER1倒序
	ByteReverse(datafrm+MRS_645_FRAME_DATA_OFFSET+4, 8, tmpDataBuf+pos);
	pos += 8;
	//ER31倒序
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

	//step04: 终端对T-ESAM操作
	//终端发送808334350010(6B)+ESAMID(8B)+R2(8B) 终端加密R2产生ER2并产生ER32
	memset(TerminSendFrame, 0, sizeof(TerminSendFrame));
	memcpy(TerminSendFrame, TerminFrame02, 6);
	memcpy(TerminSendFrame+6, datafrm, 16);	
	memset(recvDataBuf, 0, recvBufSize);
	recvDataBufPos = 0;	
	//返回:9000+0018+ER2(8B)+ER32(16B) 总计(28B)
	rs = MESamComRecv(TerminSendFrame, 22, recvDataBuf, &recvDataBufPos, recvBufSize);
	if (rs != 0)
	{
		printf("send 808334350010+ESAMID(8B)+R2(8B) failed!!!\n");
		return -1;
	}

	//step05:终端将ER2和ER32组成身份认证第二帧下行，发送给CT模块
	_MSendFramePos = 0;
	//memset(SendBuf, 0, BUFMAXSIZE);
	//身份认证第二帧下行,数据项ER2(8B)+ER32(16B)
	_MSendFrame[_MSendFramePos++] = 0x68;
	_MSendFrame[_MSendFramePos++] = 0xAA;
	_MSendFrame[_MSendFramePos++] = 0xAA;
	_MSendFrame[_MSendFramePos++] = 0xAA;
	_MSendFrame[_MSendFramePos++] = 0xAA;
	_MSendFrame[_MSendFramePos++] = 0xAA;
	_MSendFrame[_MSendFramePos++] = 0xAA;
	_MSendFrame[_MSendFramePos++] = 0x68;
	_MSendFrame[_MSendFramePos++] = 0x03;	//9 			
	//数据域长度L:04H(数据标识)+04H(操作者代码)+24H(数据长度)
	_MSendFrame[_MSendFramePos++] = 32;
	//数据标识, 占4字节
	_MSendFrame[_MSendFramePos++] = 0x41;
	_MSendFrame[_MSendFramePos++] = 0x0F;
	_MSendFrame[_MSendFramePos++] = 0x00;
	_MSendFrame[_MSendFramePos++] = 0x04;	//14
	//操作者代码占4字节,默认00000000,不需要变
	_MSendFramePos += 4;
	//ER2(8B)+ER32(16B)
	memcpy(&_MSendFrame[_MSendFramePos], &recvDataBuf, 24);	//18
	_MSendFramePos += 24;
	//打包
	Protocol_645_Pack(_MSendFrame, 42);

	//校验和 + 结束符 + 帧长度
	_MSendFrame[_MSendFramePos++] = GetSum8(_MSendFrame, 42);
	_MSendFrame[_MSendFramePos++] = 0x16;

	//发送身份认证第二帧
	ComPort_Send(fd, _MSendFrame, _MSendFramePos);
	//协议只有否认应答
	return recvDataBufPos;
}

//终端上传T-ESAM序列号(8B)+证书序列号(16B)+计数器(4B)+芯片状态(1B)+密钥版本(8B)+终端随机数R5(8B)+MAC(4B)
//*****************************************************************************
//	函数名称: GetTASecurityInfo
//	功能描述: 获取TA专用模块安全信息        
//	参数说明:
//	sendDataBuf [in] 主站随机数R4
//  sendDataLens [in] 主站随机数长度
//	返 回 值: 
//	0 : 成功
//	其它为错误码  
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
	
	//step01	发送:800E00020000
	//返回:9000+0008+T-ESAM序列号
	unsigned char ChipSerFramer[] = {0x80,0x0E,0x00,0x02,0x00,0x00,};//芯片序列号
	_rs = MESamComRecv(ChipSerFramer,6,recvDataBuf,&recvDataBufPos, recvBufSize);
	if (_rs != 0)
	{
		printf("GetTASecurityInfo-ChipSerial failed!!!\n");
		return -1;
	}
	printf("GetTASecurityInfo-ChipSerial success!!!\n");

	//step02	发送:800E00050000		返回:9000+0001+芯片状态信息
	unsigned char ChipStaSerFramer[] = {0x80,0x0E,0x00,0x05,0x00,0x00,};//芯片状态
	_rs = MESamComRecv(ChipStaSerFramer,6,&_ChipStatus,&pos,sizeof(_ChipStatus));
	printf("GetTASecurityInfo-ChipState %d!!!\n", _ChipStatus);
	if (_rs != 0)
	{
		printf("GetTASecurityInfo-ChipState failed!!!\n");
		return -1;
	}
	printf("GetTASecurityInfo-ChipState success!!!\n");

	//step03 发送:8032+p1(芯片状态信息)+02+0000	获取证书序列号
	//返回:9000+0010+证书序列号
	unsigned char CerSerFramer[] = {0x80,0x32,0x00,0x02,0x00,0x00,};//证书序列号
	CerSerFramer[2] = _ChipStatus;
	_rs = MESamComRecv(CerSerFramer,6,recvDataBuf,&recvDataBufPos, recvBufSize);
	if (_rs != 0)
	{
		printf("GetTASecurityInfo-CertificationSerial failed!!!\n");
		return -1;
	}
	printf("GetTASecurityInfo-CertificationSerial success!!!\n");

	//step04 发送800E00030000	返回:9000+0004+离线计数器信息
	unsigned char CountFramer[] = {0x80,0x0E,0x00,0x03,0x00,0x00,};//计数器
	_rs = MESamComRecv(CountFramer,6,recvDataBuf,&recvDataBufPos, recvBufSize);
	if (_rs != 0)
	{
		printf("GetTASecurityInfo-Counter failed!!!\n");
		return -1;
	}
	printf("GetTASecurityInfo-Counter success!!!\n");

	//step05 芯片状态
	recvDataBuf[recvDataBufPos ++] = _ChipStatus;//芯片状态

	//step06 发送:800E00060000	获取密钥版本:8B
	//返回:9000+0008+密钥版本信息
	unsigned char keySerFramer[] = {0x80,0x0E,0x00,0x06,0x00,0x00,};//密钥版本
	_rs = MESamComRecv(keySerFramer,6,recvDataBuf,&recvDataBufPos, recvBufSize);
	if (_rs != 0)
	{
		printf("GetTASecurityInfo-KeyVersion failed!!!\n");
		return -1;
	}

	//step07 发送:801A08000000 取终端随机数R5
	//返回:9000+0008+R5(8B)
	unsigned char RandomNumTermin[] = {0x80,0x1A,0x08,0x00,0x00,0x00};	//终端随机数R5
	_rs = MESamComRecv(RandomNumTermin,6,recvDataBuf,&recvDataBufPos, recvBufSize);
	if (_rs != 0)
	{
		printf("GetTASecurityInfo-RandomNumberR5 failed!!!\n");
		return -1;
	}

	//step08 发送8048C039+LC+data1	计算MAC
	//返回:9000+0004+MAC
	//LC:data1长度,LC为2字节
	//data1:R4(8B)+~R4(8B)+T-ESAM序列号(8B)+证书序列号(16B)+计数器(4B)+芯片状态(1B)+密钥版本(8B)+终端随机数R5(8B),总计61B
	_MSendFrame[_MSendFramePos++] = 0x80;
	_MSendFrame[_MSendFramePos++] = 0x48;
	_MSendFrame[_MSendFramePos++] = 0xC0;
	_MSendFrame[_MSendFramePos++] = 0x39;
	//LC
	_MSendFrame[_MSendFramePos++] = sndDataLens>>8;	
	_MSendFrame[_MSendFramePos++] = sndDataLens&0xff;
	//对发送数据倒序
	tmpDataBuf = (unsigned char *)malloc(sndDataLens * sizeof(unsigned char));
	//R4倒序
	ByteReverse(mainStatRandom4, 8, tmpDataBuf+pos);
	pos += 8;
	//~R4倒序
	ByteReverse(mainStatRandom4Reverse, 8, tmpDataBuf+pos);
	pos += 8;	
	//T-ESAM序列号倒序
	ByteReverse(recvDataBuf+recvpos, 8, tmpDataBuf+pos);
	pos += 8;
	recvpos += 8;
	//证书序列号倒序(16B)
	ByteReverse(recvDataBuf+recvpos, 16, tmpDataBuf+pos);
	pos += 16;
	recvpos += 16;
	//计数器倒序(4B)
	ByteReverse(recvDataBuf+recvpos, 4, tmpDataBuf+pos);
	pos += 4;
	recvpos += 4;
	//芯片状态(1B)
	ByteReverse(recvDataBuf+recvpos, 1, tmpDataBuf+pos);
	pos += 1;
	recvpos += 1;
	//密钥版本(8B)
	ByteReverse(recvDataBuf+recvpos, 8, tmpDataBuf+pos);
	pos += 8;
	recvpos += 8;
	//终端随机数R5(8B)
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

	//T-ESAM序列号(8B)+证书序列号(16B)+计数器(4B)+芯片状态(1B)+密钥版本(8B)+终端随机数R5(8B)+MAC(4B)
	//作数据的反序处理
	tmpDataBuf = (unsigned char *)malloc(40 * sizeof(unsigned char));
	pos = 0;
	ByteReverse(recvDataBuf, 8, tmpDataBuf);	//T-ESAM序列号(8B)
	pos += 8;
	ByteReverse(recvDataBuf+pos, 16, tmpDataBuf+pos);	//证书序列号(16B)
	pos += 16;
	ByteReverse(recvDataBuf+pos, 4, tmpDataBuf+pos);	//计数器(4B)
	pos += 4;
	ByteReverse(recvDataBuf+pos, 1, tmpDataBuf+pos);	//芯片状态(1B)
	pos++;
	ByteReverse(recvDataBuf+pos, 8, tmpDataBuf+pos);	//密钥版本(8B)
	pos += 8;
	ByteReverse(recvDataBuf+pos, 8, tmpDataBuf+pos);	//终端随机数R5(8B)
	pos += 4;
	ByteReverse(recvDataBuf+pos, 4, tmpDataBuf+pos);	//MAC(4B)	总计49B
	//将反序后的数据拷回到原来的buf
	memcpy(recvDataBuf, tmpDataBuf, recvDataBufPos);
	free(tmpDataBuf);
	tmpDataBuf = NULL;
	return recvDataBufPos;
}

//*****************************************************************************
//	函数名称: GetTABaudrate
//	功能描述: 查询波特率特征字        
//	参数说明:
//	sendDataBuf [in]	格式码
//	BaudrateWord [out] 波特率特征字
//	返 回 值: 
//	0 : 成功
//	其它为错误码  
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
	//step01 终端发起双向身份认证流程
	TerminalCtAuthenticate(fd, esamID);

	unsigned char _MSendFrame[50];	
	int _MSendFramePos = 0, sendDataLens = 16;
	
	//step02 终端下发查询CT模块的波特率命令
	_MSendFrame[_MSendFramePos++] = 0x68;
	_MSendFrame[_MSendFramePos++] = 0xAA;
	_MSendFrame[_MSendFramePos++] = 0xAA;
	_MSendFrame[_MSendFramePos++] = 0xAA;
	_MSendFrame[_MSendFramePos++] = 0xAA;
	_MSendFrame[_MSendFramePos++] = 0xAA;
	_MSendFrame[_MSendFramePos++] = 0xAA;
	_MSendFrame[_MSendFramePos++] = 0x68;
	_MSendFrame[_MSendFramePos++] = 0x03; 			
	//数据域长度L:04H(数据标识)+04H(操作者代码)+01H(数据长度)
	_MSendFrame[_MSendFramePos++] = 9;	//10
	//数据标识, 占4字节
	_MSendFrame[_MSendFramePos++] = 0x44;
	_MSendFrame[_MSendFramePos++] = 0x0F;
	_MSendFrame[_MSendFramePos++] = 0x00;
	_MSendFrame[_MSendFramePos++] = 0x04;	//14
	//操作者代码占4字节,默认00000000,不需要变
	_MSendFramePos += 4;
	//格式码(1B)
	_MSendFrame[_MSendFramePos++] = sendDataBuf;	//19
	//打包
	Protocol_645_Pack(_MSendFrame, 19);
	//校验和 + 结束符 + 帧长度
	_MSendFrame[_MSendFramePos++] = GetSum8(_MSendFrame, 19);
	_MSendFrame[_MSendFramePos++] = 0x16;
	//SendLen = 20;

	ComPort_Send(fd, _MSendFrame, _MSendFramePos);  
	
	//确认应答并返回:格式码(1B)+波特率特征字(1B)+MAC(4B),总计6B
	//data1:格式码+波特率特征字
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
	//step04 发送: 8049813A000E(6B)+ESAMID(8B)+data1(格式码(1B)+波特率特征字(1B))+MAC(4B),总计20B
	_MSendFramePos = 0, sendDataLens = 14, pos = 0;
	unsigned char* tmpDatabuf = (unsigned char *)malloc(sendDataLens * sizeof(unsigned char));
	_MSendFrame[_MSendFramePos++] = 0x80;
	_MSendFrame[_MSendFramePos++] = 0x49;	
	_MSendFrame[_MSendFramePos++] = 0x81;
	_MSendFrame[_MSendFramePos++] = 0x3A;	
	_MSendFrame[_MSendFramePos++] = 0x00;
	_MSendFrame[_MSendFramePos++] = 0x0E;	
	//ESAMID反序
	ByteReverse(esamID, 8, tmpDatabuf+pos);
	pos += 8;
	//格式码反序
	ByteReverse(&datafrm[MRS_645_FRAME_DATA_OFFSET+4], 1, tmpDatabuf+pos);
	pos += 1;
	//波特率特征字反序
	ByteReverse(&datafrm[MRS_645_FRAME_DATA_OFFSET+5], 1, tmpDatabuf+pos);
	pos += 1;
	//*BaudrateWord
	//MAC1反序	
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
//	函数名称: SetTABaudrate
//	功能描述: 设置波特率特征字        
//	参数说明:
//	formatCoce	[in]	格式码
//	BaudrateWord [in] 波特率特征字
//	MAC	[in]	//回路状态巡检仪安全模块V2.0支持对主站或终端进行MAC校验
//	MAC由DATA(不包含MAC)计算产生
//	MAC2 [out]	发送80488138000A+data3(ESAMID+格式码+波特率特征字)到T-ESAM,返回:9000+0004+MAC2
//	返 回 值: 
//	0 : 成功
//	其它为错误码  
//*****************************************************************************
int SetTABaudrate(int fd, unsigned char formatCode, unsigned char BaudrateWord, unsigned char *MAC, unsigned char* MAC2, unsigned int MAC2Len)
{
	unsigned char	esamID[8] = {0};
	int	len = 0, _rs = -1, recvDataBufPos = 0, pos=0;
	
	//step01 终端发起双向身份认证流程
	TerminalCtAuthenticate(fd, esamID);
	//step02 终端下发设置CT模块的波特率命令
	//数据项名称格式码(1B)/波特率特征字(1B)/MAC(4B)
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
	//数据域长度L:04H(数据标识)+04H(操作者代码)+01H(数据长度)
	_MSendFrame[_MSendFramePos++] = 14;	//10
	//数据标识, 占4字节
	_MSendFrame[_MSendFramePos++] = 0x45;
	_MSendFrame[_MSendFramePos++] = 0x0F;
	_MSendFrame[_MSendFramePos++] = 0x00;
	_MSendFrame[_MSendFramePos++] = 0x04;	//14
	//操作者代码占4字节,默认00000000(4B),不需要变
	_MSendFramePos += 4;
	//格式码(1B)
	_MSendFrame[_MSendFramePos++] = formatCode;	//19
	_MSendFrame[_MSendFramePos++] = BaudrateWord;	//20
	memcpy(&_MSendFrame[_MSendFramePos], MAC, 4);
	_MSendFramePos += 4;	//24
	//打包
	Protocol_645_Pack(_MSendFrame, 24);
	//校验和 + 结束符 + 帧长度
	_MSendFrame[_MSendFramePos++] = GetSum8(_MSendFrame, 24);
	_MSendFrame[_MSendFramePos++] = 0x16;

	ComPort_Send(fd, _MSendFrame, _MSendFramePos);  
	//回路状态巡检仪终端安全芯片使用手册没有明确响应帧是什么
	
	//step03 终端对T-ESAM操作,发送:80488138000A(6B)+data3(ESAMID(8B)+格式码(1B)+波特率特征字(1B))
	_MSendFramePos = 0, sendDataLens = 10, pos = 0;
	unsigned char* tmpDatabuf = (unsigned char *)malloc(sendDataLens * sizeof(unsigned char));
	_MSendFrame[_MSendFramePos++] = 0x80;
	_MSendFrame[_MSendFramePos++] = 0x48;	
	_MSendFrame[_MSendFramePos++] = 0x81;
	_MSendFrame[_MSendFramePos++] = 0x38;	
	_MSendFrame[_MSendFramePos++] = 0x00;
	_MSendFrame[_MSendFramePos++] = 0x0A;
	//ESAMID 反序
	ByteReverse(esamID, 8, tmpDatabuf+pos);
	pos += 8;
	//格式码 反序	
	ByteReverse(&formatCode, 1, tmpDatabuf+pos);
	pos += 1;
	//波特率特征字反序	
	ByteReverse(&BaudrateWord, 1, tmpDatabuf+pos);
	pos += 1;
	//拷贝ESAMID/格式码/波特率特征字
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
//	函数名称: GetCTModuleStatus
//	功能描述: 获取CT模块状态        
//	参数说明:
//	返 回 值: 
//	0 : 成功
//	其它为错误码  
//*****************************************************************************
int GetCTModuleStatus(int fd, unsigned char *formatCode, unsigned char *phaseABCStatus, unsigned char *MAC2)
{
	unsigned char _MSendFrame[50];	
	int _MSendFramePos = 0, pos=0, sendDataLens = 16;
	unsigned char	esamID[8] = {0};
	int recvLen = 0, _j=0, _rs = -1;
	unsigned char recvBuf[MESAM_RECV_BUF_MAX_LEN] = {0};
	
	//step01 主站下发获取终端安全认证信息命令

	//step02 终端读取CT模块状态
	//step02.1	终端发起双向身份认证流程
	TerminalCtAuthenticate(fd, esamID);
	//step02.2	终端下发获取CT模块状态命令
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
	//数据域长度L:04H(数据标识)+04H(操作者代码)+01H(数据长度)
	_MSendFrame[_MSendFramePos++] = 9;
	//数据标识, 占4字节
	_MSendFrame[_MSendFramePos++] = 0x00;
	_MSendFrame[_MSendFramePos++] = 0x00;
	_MSendFrame[_MSendFramePos++] = 0x00;
	_MSendFrame[_MSendFramePos++] = 0x07;
	//操作者代码占4字节,默认00000000(4B),不需要变
	_MSendFramePos += 4;
	//格式码(1B)
	_MSendFrame[_MSendFramePos++] = *formatCode;
	//打包
	Protocol_645_Pack(_MSendFrame, _MSendFramePos);
	//校验和 + 结束符 + 帧长度
	_MSendFrame[_MSendFramePos++] = GetSum8(_MSendFrame, 20);
	_MSendFrame[_MSendFramePos++] = 0x16;	//21
	//645帧帧长度
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

	//应答帧:数据域长度L=04H(数据标识)+m(数据长度)
	//memcpy(phaseABCStatus, datafrm+MRS_645_FRAME_DATA_OFFSET+4, 6);
	//格式码
	*formatCode =  recvBuf[MRS_645_FRAME_DATA_OFFSET+4];
	//ABC三相窃电情况
	memcpy(phaseABCStatus, recvBuf+MRS_645_FRAME_DATA_OFFSET+4+1, 3);
	//拷贝MAC
	memcpy(MAC2, recvBuf+MRS_645_FRAME_DATA_OFFSET+4+4, 4);	
	//step02.3	发送:8049813A0010(6B)+data2(ESAMID(8B)+格式码(1B)+A相状态(1B)+B相状态(1B)+C相状态(1B)+MAC2(4B))	
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
	//ESAMID反序
	ByteReverse(esamID, 8, tmpDataBuf+pos);
	pos += 8;
	//格式码反序
	ByteReverse(formatCode, 1, tmpDataBuf+pos);
	pos += 1;
	//A相状态反序
	ByteReverse(phaseABCStatus, 1, tmpDataBuf+pos);
	pos += 1;
	//B相状态反序
	ByteReverse(phaseABCStatus+1, 1, tmpDataBuf+pos);
	pos += 1;
	//C相状态反序
	ByteReverse(phaseABCStatus+2, 1, tmpDataBuf+pos);
	pos += 1;
	//MAC2反序
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

	//发送:8049813A0010+data2 返回:9000+0000
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

	//step03 发送8048C039(4B)+LC(2B)+data1(R4(8B)+~R4(8B)+A相状态(1B)+B相状态(1B)+C相状态(1B)) 总计:25B
	_MSendFramePos = 0, sendDataLens = 19, pos=0;
	_MSendFrame[_MSendFramePos++] = 0x80;
	_MSendFrame[_MSendFramePos++] = 0x48;
	_MSendFrame[_MSendFramePos++] = 0xC0;
	_MSendFrame[_MSendFramePos++] = 0x39;
	//LC
	_MSendFrame[_MSendFramePos++] = sendDataLens>>8;	
	_MSendFrame[_MSendFramePos++] = sendDataLens&0xff;
	//对发送数据倒序
	tmpDataBuf = (unsigned char *)malloc(sendDataLens * sizeof(unsigned char));
	//R4倒序
	ByteReverse(mainStatRandom4, 8, tmpDataBuf+pos);
	pos += 8;
	//~R4倒序
	ByteReverse(mainStatRandom4Reverse, 8, tmpDataBuf+pos);
	pos += 8;	
	//A相状态倒序
	ByteReverse(phaseABCStatus, 1, tmpDataBuf+pos);
	pos += 1;
	//B相状态倒序
	ByteReverse(phaseABCStatus+1, 1, tmpDataBuf+pos);
	pos += 1;
	//C相状态倒序
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

	//发送:8049813A0010+data2 返回:9000+0000	
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
//	函数名称: GetTAModuleEsamInfo
//	功能描述: 获取TA专用模块安全信息        
//	参数说明:
//	formatCoce	[in]	格式码
//	mStatRand [in] 主站随机数
//	返 回 值: 
//	0 : 成功
//	其它为错误码  
//*****************************************************************************
int GetTAModuleEsamInfo(int fd, unsigned char formatCode, unsigned char* mStatRand, int RandSize, unsigned char* recvBuf, int *recvLen)
{
	unsigned char	esamID[8] = {0};
	
	//step01 终端发起双向身份认证流程
	TerminalCtAuthenticate(fd, esamID);
	//step02 终端下发获取TA专用模块安全信息下行帧
	//数据项名称: 格式码(1B)/主站随机数(8B)
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
	//数据域长度L:04H(数据标识)+04H(操作者代码)+(数据长度)
	_MSendFrame[_MSendFramePos++] = 14;	//10
	//数据标识, 占4字节
	_MSendFrame[_MSendFramePos++] = 0x42;
	_MSendFrame[_MSendFramePos++] = 0x0F;
	_MSendFrame[_MSendFramePos++] = 0x00;
	_MSendFrame[_MSendFramePos++] = 0x04;	//14
	//操作者代码占4字节,默认00000000(4B),不需要变
	_MSendFramePos += 4;
	//格式码(1B)
	_MSendFrame[_MSendFramePos++] = formatCode;	//19
	//主站随机数
	memcpy(&_MSendFrame[_MSendFramePos], mStatRand, RandSize);
	_MSendFramePos += RandSize;
	//打包:地址域高低字节翻转,数据域所有字节+0x33
	Protocol_645_Pack(_MSendFrame, _MSendFramePos);
	//校验和 + 结束符 + 帧长度
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
//	函数名称: TAModuleKeyUpdate
//	功能描述: 密钥更新        
//	参数说明:
//	keyNums	[in]	本帧密钥条数
//	sendDataBuf [in]	密钥1-N
//	MAC	[in]	MAC由DATA(不包含MAC)计算产生
//	返 回 值: 
//	0 : 成功
//	其它为错误码  
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
	//数据域长度L:04H(数据标识)+04H(操作者代码)+01H(数据长度)
	_MSendFrame[_MSendFramePos++] = 8+1+sendDataLens+MACSIZE;	//10
	//数据标识, 占4字节
	_MSendFrame[_MSendFramePos++] = 0x43;
	_MSendFrame[_MSendFramePos++] = 0x0F;
	_MSendFrame[_MSendFramePos++] = 0x00;
	_MSendFrame[_MSendFramePos++] = 0x04;	//14
	//操作者代码占4字节,默认00000000(4B),不需要变
	_MSendFramePos += 4;
	//格式码(1B)
	_MSendFrame[_MSendFramePos++] = keyNums;	//19
	//密钥总条数(1B)+备用(1B)+密钥编号(1B)+密钥状态标识(1B)+密钥(32B)
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
	
	//打包:地址域高低字节翻转,数据域所有字节+0x33
	Protocol_645_Pack(_MSendFrame, _MSendFramePos);
	//校验和 + 结束符 + 帧长度
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
