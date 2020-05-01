#include <stdlib.h> 
#include <stdio.h> 
#include <errno.h> 
#include <string.h> 
#include <netdb.h> 
#include <sys/types.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <net/if.h>  
#include <sys/ioctl.h>
#include <time.h>
#include<sys/types.h>
#include<sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/sysinfo.h>
#include<termios.h>  
 
#include<arpa/inet.h> 
//#include<netinet/tcp.h>





#include "Server_com.h"
//#include "uart.h"
//#include "gprs_task.h"

#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <linux/tcp.h>

#include "Mrs_fw_proto376_1.h"
#include "appdata.h"

int login_ok = 0;
// 是否登录系统
CIRCUIT_SYSTEM_INFO_STRU gCircuitSystemInfo={0};

#define AFN_37621_OFFSET 12


//本地服务器监听端口
CONNECT_TERM HOST = 
{
    -1,
    "127.0.0.1",
    10800,
    0,
};
	
//主站地址，读取配置文件初始化
CONNECT_TERM MAINSTATION = 
{
    -1,
    "192.168.1.141",
    10000,
    0,
};

/*************************************************
函 数 名:       
功能描述:         
输    入:           
输    出:      
返    回:                  
*************************************************/
long GET_SYS_UPTIME()
{
	long ret = -1;
	struct sysinfo info;
	if(sysinfo(&info) >= 0)
	{
		ret = info.uptime;
	} else
	{
		printf("sysinfo() fails\n");
	}
	return ret;
}




/*************************************************
函 数 名:       
功能描述:         
输    入:           
输    出:      
返    回:                  
*************************************************/
int printhexdata(unsigned char *data,int len)
{
	int i;
    //DPRINT("[");
	printf("[");
	for(i=0;i<len;i++)
	{
        //DPRINT("%02x ",data[i]);
		printf("%02x ",data[i]);
	}
    //DPRINT("\b]\n");
	printf("\b]\n");
	return len;
}

/*************************************************
函 数 名:        
功能描述:发送数据         
输    入:           
输    出:      
返    回:                  
*************************************************/
//int SendData(CONNECT_TERM *TERM,char *sendbuf,int sendlen)
int SendData(int fd, unsigned char *sendbuf, int sendlen)
{
    int slen=0,len=0;
    //int reconnected=0;

    //if(TERM->FD < 0)
    if(fd < 0)
    {
        return -1;
    }
    
    while(slen<sendlen)
    {
		//len=send(TERM->FD,sendbuf+slen,sendlen-slen,MSG_DONTWAIT);
		len=send(fd,sendbuf+slen,sendlen-slen,MSG_DONTWAIT);
		printf("len=%d\n", len);
        if(len<0)
        {	
			printf("HOST write error,reconnect...\n"); 
			continue;
        }
        slen+=len;
        if(slen >= sendlen)
            break;
        usleep(10*1000);
    }
	printf("send data:\n");
    printhexdata(sendbuf, slen);
    return slen;
}

/*************************************************
函 数 名:       
功能描述:从本体接收数据         
输    入:           
输    出:      
返    回:                  
*************************************************/
int RecvData(int fd, unsigned char *recvbuf, int recvlen, int timeout)
{
	fd_set rd_fd;         
	struct timeval tv; 
    int err=-1,len=0;
    
	FD_ZERO(&rd_fd); 
	FD_SET(fd, &rd_fd);
    
    if(fd <= 0)
        return -1;
    //一次接收
	if (timeout >= 0)
	{
		tv.tv_sec = timeout / 1000;
		tv.tv_usec = (timeout - tv.tv_sec * 1000) * 1000;
		err=select(fd+1,&rd_fd,NULL,NULL,&tv);//
	}
	else
	{
		err=select(fd+1,&rd_fd,NULL,NULL,NULL);//
	}
	if(err>0)
	{
        len = recv(fd,recvbuf,recvlen,0);
        if(len > 0)
        {
            //DPRINT("RecvData HOST READ:\n");			
            //printf("RecvData HOST READ:\n");
            //printhexdata(recvbuf, len);
            return len;
        } 
        else if(len==0)
        {
            //DPRINT("RecvData HOST read error,%s\n",strerror(errno));			
            printf("RecvData HOST read error,%s\n",strerror(errno));
            err=-2;
        }
	} 
    else if(err == 0)   //time out
	{
        //DPRINT("RecvData HOST read timeout\n");		
        printf("RecvData HOST read timeout\n");
        err = -3;
	} 
    else                //error
    {
        //DPRINT("RecvData HOST read error,%s\n",strerror(errno));
        printf("RecvData HOST read error,%s\n",strerror(errno));
        err = -1;
    }
    
	return err;
}

static inline void msleep(unsigned long ms)
{
    struct timeval         tv;
    tv.tv_sec = ms/1000;
    tv.tv_usec = (ms%1000)*1000;

    select(0, NULL, NULL, NULL, &tv);
}

//服务器线程
void *serrver_worker(void *arg)    /* 创建出来的子线程所执行的权力函数 */
{
	int	client_fd;    /* 客户端文件描述符 */
	unsigned char	*buf = NULL;
	unsigned char	datafrm[128] = {0};
	int	rv;	
    int usDataLen = 0, len = 0;

	UINT32 lasttime = 0;
	UINT32 curtime = 0;
	
	if(!arg)    /* 判断参数是否被赋值 */
	{
		printf("Invalid arg\n");
		return NULL;
	}

	client_fd = *((int*)arg);

	buf = (unsigned char *)malloc(sizeof(unsigned char) * 50);
	memset(buf, 0, sizeof(char)*50);
	sendLinkPacktoMainStaion(1, buf, &usDataLen);						  //发送登录
	if((rv = SendData(client_fd, buf, usDataLen)) >= usDataLen)
	{
		free(buf);
		login_ok = 1;
		printf("login success\n");
	}
	else
	{				
		free(buf);
		usleep(2000);
	}

	while(1)    
	{
		memset(datafrm, 0, sizeof(datafrm)); 
		curtime = GET_SYS_UPTIME();
		printf("curtime = %d\n",curtime);
		if(curtime - lasttime > 60)
		{
			lasttime = curtime;
			buf = (unsigned char *)malloc(sizeof(unsigned char) * 50);
			memset(buf, 0, sizeof(char)*50);
			sendLinkPacktoMainStaion(3, buf, &usDataLen);					//发送心跳
			if((rv = SendData(client_fd, buf, usDataLen)) >= usDataLen)
			{
				free(buf);
				login_ok = 1;
				printf("login success\n");
			}
			else
			{				
				free(buf);
				usleep(2000);
				continue;
			}
		}
		
		//read the login frame to get terminal address
		usleep(100*1000);
		len = RecvData(client_fd, datafrm, sizeof(datafrm), 2*1000);
		if (len >= 16)
		{
			printf("recv data:\n");
			printhexdata(datafrm, len);
			Protocl13761ParseData((unsigned char *)datafrm, len,client_fd);
		}
		else if(len == -2)
		{
			close(client_fd);
			pthread_exit(NULL);
			printf("%d = Disconnect\n",client_fd);
		}
		else
		{
			printf("no frame received, ret %d.\n",len);
		}
		usleep(1000);
	} 
}


int Server_listen(int port)
{
    struct sockaddr_in server_addr; 
	int	on = 1, retval=0;
	
    /* 服务器程序开始建立 sockfd描述符 */ 
	HOST.FD=socket(AF_INET,SOCK_STREAM,0);
    if(HOST.FD<0)// AF_INET:Internet;SOCK_STREAM:TCP	
    { 		
        printf("Use socket() to create a TCP socket failure: %s\n",strerror(errno));
        return(-1); 
    } 	
	
	//允许重用本地址和端口
	retval = setsockopt(HOST.FD, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    
	//不使用Nagle算法
	if (retval >= 0)
	{
		retval = setsockopt(HOST.FD, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on));
	}
	
	if( retval >= 0 )
	{
		struct timeval timeo;
		timeo.tv_sec = 2;
		timeo.tv_usec = 0;
		//设置socket接收数据超时时间和发送数据超时时间
		setsockopt(HOST.FD, IPPROTO_TCP, SO_RCVTIMEO, &timeo, sizeof(struct timeval));
		setsockopt(HOST.FD, IPPROTO_TCP, SO_SNDTIMEO, &timeo, sizeof(struct timeval));
	}

	if( retval >= 0 )
	{
		struct linger tcplinger;
		tcplinger.l_onoff = 0;
		tcplinger.l_linger = 0;
		//设置 l_onoff为0，则该选项关闭，l_linger的值被忽略，等于内核缺省情况
		//close调用会立即返回给调用者，如果可能将会传输任何未发送的数据
		setsockopt(HOST.FD, IPPROTO_TCP, SO_LINGER, &tcplinger, sizeof(struct linger));
	}
	
	if (retval < 0)
	{
		close(HOST.FD);
		HOST.FD = -1;
		return HOST.FD;
	}
	
	/* 服务器程序填充服务端的资料 */
    bzero(&server_addr,sizeof(server_addr));    // 初始化,置0	
    server_addr.sin_family=AF_INET;  // IPV4	
    server_addr.sin_port=htons(HOST.PORT);   // (将本机器上的short数据转化为网络上的short数据)端口号
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(HOST.FD, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("Use bind() to bind the TCP socket failure: %s\n", strerror(errno));
		close(HOST.FD);
        HOST.FD = -1;
		return HOST.FD;
    }

    if (listen(HOST.FD, 1024) < 0)
	{
		printf("Error: Server listen error(%s)!\n", strerror(errno));
		close(HOST.FD);
		HOST.FD = -1;
		return HOST.FD;
	}
	return HOST.FD;
}


//客户端线程，以太网连接主站
void *client_worker()
{
	int	client_fd;   
	unsigned char	*buf = NULL;
	unsigned char	datafrm[128] = {0};
	int	rv;	
    int usDataLen = 0, len = 0;
	int ret = 0;
	struct sockaddr_in serveraddr;

	UINT32 lasttime = 0;
	UINT32 curtime = 0;

	//从配置文件读取主站IP和端口号 todo

connect:	
	client_fd = socket(AF_INET,SOCK_STREAM,0); 					 //创建socket，获得socket文件描述符
	if(client_fd <0){
		perror("socket");
		return;
	}

	int flags = fcntl(client_fd, F_GETFL, 0);
	fcntl(client_fd, F_SETFL, flags|O_NONBLOCK);
	
	serveraddr.sin_family = AF_INET;    						//填充网络地址类型
	serveraddr.sin_port = htons(MAINSTATION.PORT);   			//注意，端口号是short,
	serveraddr.sin_addr.s_addr = inet_addr(MAINSTATION.IP);

		while(1)
		{
			ret = connect(client_fd, (struct sockaddr *)&serveraddr,sizeof(serveraddr));  	// 连接服务器
		    if(ret <0)
			{
				perror("connect");
				sleep(10);			//没10s连接一次主站
				continue;
			}
			else
			{
				buf = (unsigned char *)malloc(sizeof(unsigned char) * 50);
				memset(buf, 0, sizeof(char)*50);
				sendLinkPacktoMainStaion(1, buf, &usDataLen);
				if((rv = SendData(client_fd, buf, usDataLen)) >= usDataLen)
				{
					free(buf);
					login_ok = 1;
					printf("login success\n");
				}
				else
				{				
					free(buf);
					usleep(2000);
					continue;
				}
				break;				//结束循环开始首发数据
			}
		}
	
	//首发数据
	while(1)  
	{
		memset(datafrm, 0, sizeof(datafrm));    /* 对缓冲区清零 */  
		curtime = GET_SYS_UPTIME();
		if(curtime - lasttime > 60)
		{
			lasttime = curtime;
			buf = (unsigned char *)malloc(sizeof(unsigned char) * 50);
			memset(buf, 0, sizeof(char)*50);
			sendLinkPacktoMainStaion(3, buf, &usDataLen);
			if((rv = SendData(client_fd, buf, usDataLen)) >= usDataLen)
			{
				free(buf);
				login_ok = 1;
				printf("login success\n");
			}
			else
			{				
				free(buf);
				usleep(2000);
				continue;
			}
		}
		
		//read the login frame to get terminal address
		usleep(100*1000);
		len = RecvData(client_fd, datafrm, sizeof(datafrm), 2*1000);
		if (len >= 16)
		{
			printf("recv data:\n");
			printhexdata(datafrm, len);
			Protocl13761ParseData((unsigned char *)datafrm, len,client_fd);
		}
		else if(len == -2)
		{
			close(client_fd);
			login_ok = 0;
			goto connect;
		}
		else
		{
			printf("no frame received, ret %d.\n",len);
		}
		usleep(1000);
	} 
}


//485连接主站
void *rs485_worker(void *arg)    
{
	int	client_fd;    
	unsigned char	*buf = NULL;
	unsigned char	datafrm[128] = {0};
	int	rv;	
    int usDataLen = 0, len = 0;
	
connect:
	while(1)
	{
		client_fd = OpenComPort(client_fd,"");
		if(client_fd > 0)
		{
			printf("OpenComPort\n");
			break;
		}
		else
		{
			sleep(10);
		}
	}

	while(1)    //收发数据
	{
		memset(datafrm, 0, sizeof(datafrm));    /* 对缓冲区清零 */  
		if(!login_ok)
		{
			buf = (unsigned char *)malloc(sizeof(unsigned char) * 50);
			memset(buf, 0, sizeof(char)*50);
			sendLinkPacktoMainStaion(1, buf, &usDataLen);
			if((rv = SendData(client_fd, buf, usDataLen)) >= usDataLen)
			{
				free(buf);
				login_ok = 1;
				printf("login success\n");
			}
			else
			{				
				free(buf);
				usleep(2000);
				continue;
			}
		}
		sleep(1000);
		len = ComPort_Recv(client_fd, datafrm, sizeof(datafrm));
		if (len >= 16)
		{
			printf("recv data:\n");
			printhexdata(datafrm, len);
			Protocl13761ParseData((unsigned char *)datafrm, len, client_fd);
		}
		else if(len == -2)
		{
			close(client_fd);
			login_ok = 0;
			goto connect;
		}
		else
		{
			printf("no frame received, ret %d.\n",len);
		}
		usleep(1000);


		
	} 
}



int OpenComPort(int fd,char* port)  
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
void CloseComPort(int fd)  
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
int ComPort_Set(int fd,int speed,int flow_ctrl,int databits,int stopbits,int parity)  
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
int ComPort_Recv(int fd, char *rcv_buf,int data_len)  
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
int ComPort_Send(int fd, char *send_buf,int data_len)  
{  
    int len = 0;  
     
    len = write(fd,send_buf,data_len);  
    if (len == data_len )  
    {  
    	int i;
        printf("send data is %s\n",send_buf);
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



int main(int argc, char *argv[])
{
	int	connfd = 0;
	int res;
	pthread_t	CLIENT_WORKER_THREAD;		//客户端线程
	pthread_t	SERVER_WORKER_THREAD;		//服务器线程，用于以太网连接主站
	pthread_t	RS485_CONNE_MAST;			//485连接主站线程


    CircuitParamInit();	
    struct sockaddr_in	cliaddr; 
	socklen_t clilen = sizeof(cliaddr);

	//客户端
	res = pthread_create(&CLIENT_WORKER_THREAD,NULL,(void *)client_worker,NULL);
	if(res!=0)
	{
			printf("Create HOST_LISTEN thread error!\n");
			exit(1);
	}

	//485
	res = pthread_create(&RS485_CONNE_MAST,NULL,(void *)rs485_worker,NULL);
	if(res!=0)
	{
			printf("Create HOST_LISTEN thread error!\n");
			exit(1);
	}

	
	//服务器
	HOST.FD = Server_listen(HOST.PORT);
	if(HOST.FD < 0)
	{
		printf( "Create TCP Sever fail %d\n", HOST.PORT);
		return -1;
	}
	while(1)
	{
		connfd=accept(HOST.FD, (struct sockaddr *)&cliaddr, &clilen);
		printf("Connect connfd = %d\n", connfd);
		if(connfd < 0)
		{
			printf( "Connect fail %d\n", connfd);
			continue;
		}
		res = pthread_create(&SERVER_WORKER_THREAD, NULL, serrver_worker, (void *)&connfd);
		if(res!=0)
		{
				printf("Create HOST_LISTEN thread error!\n");
				exit(1);
		}
		else
		{
			printf("Coming from port %d \n",ntohs(cliaddr.sin_port));
			printf("Coming from IP %s \n",inet_ntoa(cliaddr.sin_addr));
		}	
	}
	
	close(HOST.FD);	
	getchar();
	getchar();
	return 0;
}

