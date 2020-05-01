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
// �Ƿ��¼ϵͳ
CIRCUIT_SYSTEM_INFO_STRU gCircuitSystemInfo={0};

#define AFN_37621_OFFSET 12


//���ط����������˿�
CONNECT_TERM HOST = 
{
    -1,
    "127.0.0.1",
    10800,
    0,
};
	
//��վ��ַ����ȡ�����ļ���ʼ��
CONNECT_TERM MAINSTATION = 
{
    -1,
    "192.168.1.141",
    10000,
    0,
};

/*************************************************
�� �� ��:       
��������:         
��    ��:           
��    ��:      
��    ��:                  
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
�� �� ��:       
��������:         
��    ��:           
��    ��:      
��    ��:                  
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
�� �� ��:        
��������:��������         
��    ��:           
��    ��:      
��    ��:                  
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
�� �� ��:       
��������:�ӱ����������         
��    ��:           
��    ��:      
��    ��:                  
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
    //һ�ν���
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

//�������߳�
void *serrver_worker(void *arg)    /* �������������߳���ִ�е�Ȩ������ */
{
	int	client_fd;    /* �ͻ����ļ������� */
	unsigned char	*buf = NULL;
	unsigned char	datafrm[128] = {0};
	int	rv;	
    int usDataLen = 0, len = 0;

	UINT32 lasttime = 0;
	UINT32 curtime = 0;
	
	if(!arg)    /* �жϲ����Ƿ񱻸�ֵ */
	{
		printf("Invalid arg\n");
		return NULL;
	}

	client_fd = *((int*)arg);

	buf = (unsigned char *)malloc(sizeof(unsigned char) * 50);
	memset(buf, 0, sizeof(char)*50);
	sendLinkPacktoMainStaion(1, buf, &usDataLen);						  //���͵�¼
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
			sendLinkPacktoMainStaion(3, buf, &usDataLen);					//��������
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
	
    /* ����������ʼ���� sockfd������ */ 
	HOST.FD=socket(AF_INET,SOCK_STREAM,0);
    if(HOST.FD<0)// AF_INET:Internet;SOCK_STREAM:TCP	
    { 		
        printf("Use socket() to create a TCP socket failure: %s\n",strerror(errno));
        return(-1); 
    } 	
	
	//�������ñ���ַ�Ͷ˿�
	retval = setsockopt(HOST.FD, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    
	//��ʹ��Nagle�㷨
	if (retval >= 0)
	{
		retval = setsockopt(HOST.FD, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on));
	}
	
	if( retval >= 0 )
	{
		struct timeval timeo;
		timeo.tv_sec = 2;
		timeo.tv_usec = 0;
		//����socket�������ݳ�ʱʱ��ͷ������ݳ�ʱʱ��
		setsockopt(HOST.FD, IPPROTO_TCP, SO_RCVTIMEO, &timeo, sizeof(struct timeval));
		setsockopt(HOST.FD, IPPROTO_TCP, SO_SNDTIMEO, &timeo, sizeof(struct timeval));
	}

	if( retval >= 0 )
	{
		struct linger tcplinger;
		tcplinger.l_onoff = 0;
		tcplinger.l_linger = 0;
		//���� l_onoffΪ0�����ѡ��رգ�l_linger��ֵ�����ԣ������ں�ȱʡ���
		//close���û��������ظ������ߣ�������ܽ��ᴫ���κ�δ���͵�����
		setsockopt(HOST.FD, IPPROTO_TCP, SO_LINGER, &tcplinger, sizeof(struct linger));
	}
	
	if (retval < 0)
	{
		close(HOST.FD);
		HOST.FD = -1;
		return HOST.FD;
	}
	
	/* ����������������˵����� */
    bzero(&server_addr,sizeof(server_addr));    // ��ʼ��,��0	
    server_addr.sin_family=AF_INET;  // IPV4	
    server_addr.sin_port=htons(HOST.PORT);   // (���������ϵ�short����ת��Ϊ�����ϵ�short����)�˿ں�
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


//�ͻ����̣߳���̫��������վ
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

	//�������ļ���ȡ��վIP�Ͷ˿ں� todo

connect:	
	client_fd = socket(AF_INET,SOCK_STREAM,0); 					 //����socket�����socket�ļ�������
	if(client_fd <0){
		perror("socket");
		return;
	}

	int flags = fcntl(client_fd, F_GETFL, 0);
	fcntl(client_fd, F_SETFL, flags|O_NONBLOCK);
	
	serveraddr.sin_family = AF_INET;    						//��������ַ����
	serveraddr.sin_port = htons(MAINSTATION.PORT);   			//ע�⣬�˿ں���short,
	serveraddr.sin_addr.s_addr = inet_addr(MAINSTATION.IP);

		while(1)
		{
			ret = connect(client_fd, (struct sockaddr *)&serveraddr,sizeof(serveraddr));  	// ���ӷ�����
		    if(ret <0)
			{
				perror("connect");
				sleep(10);			//û10s����һ����վ
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
				break;				//����ѭ����ʼ�׷�����
			}
		}
	
	//�׷�����
	while(1)  
	{
		memset(datafrm, 0, sizeof(datafrm));    /* �Ի��������� */  
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


//485������վ
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

	while(1)    //�շ�����
	{
		memset(datafrm, 0, sizeof(datafrm));    /* �Ի��������� */  
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
void CloseComPort(int fd)  
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
int ComPort_Set(int fd,int speed,int flow_ctrl,int databits,int stopbits,int parity)  
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
int ComPort_Recv(int fd, char *rcv_buf,int data_len)  
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
	pthread_t	CLIENT_WORKER_THREAD;		//�ͻ����߳�
	pthread_t	SERVER_WORKER_THREAD;		//�������̣߳�������̫��������վ
	pthread_t	RS485_CONNE_MAST;			//485������վ�߳�


    CircuitParamInit();	
    struct sockaddr_in	cliaddr; 
	socklen_t clilen = sizeof(cliaddr);

	//�ͻ���
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

	
	//������
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

