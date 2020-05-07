#include <stdlib.h> 
#include <stdio.h> 
#include <string.h> 
#include <errno.h> 
#include <netdb.h> 
#include <sys/types.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <net/if.h>  
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <time.h>
#include <pthread.h>
#include "AcSample.h"
#include "Mrs_fw_proto376_1.h"

tstMCurDataStruct ACdate;


//���ɳ����������ַ
com_645_2007_CONNECT_TERM com_645_2007_HOST = 
{
	-1,
	"127.0.0.1",
	8100,
	0,
};

//�Ͽ�����
int CLOSE_645_2007_TERM(com_645_2007_CONNECT_TERM *TERM)
{
	if(NULL == TERM)
		return -1;
	close(TERM->FD);
	TERM->CONNECT_FLAG = 0;
	TERM->FD = -1;
	return 0;
}

//���ӷ�����
int com_645_2007_CONNECT(com_645_2007_CONNECT_TERM *TERM)
{ 		
    fd_set wr_fds;
    struct timeval tv;
    int err=0,nonblock;
    struct sockaddr_in server_addr; 	

    if(TERM->FD > 0)
        return 0;
	
    /* �ͻ�����ʼ���� sockfd������ */ 
    if((TERM->FD=socket(AF_INET,SOCK_STREAM,0))<0)// AF_INET:Internet;SOCK_STREAM:TCP	
    { 		
        printf("Socket Error:%s\n",strerror(errno));
        return(-1); 
    } 	
    
    /* �ͻ�����������˵����� */
    bzero(&server_addr,sizeof(server_addr));    // ��ʼ��,��0	
    server_addr.sin_family=AF_INET;  			// IPV4	
    server_addr.sin_port=htons(TERM->PORT);     // (���������ϵ�short����ת��Ϊ�����ϵ�short����)�˿ں�
    if(inet_pton(AF_INET, TERM->IP, &(server_addr.sin_addr)) == -1)
    {
        //DPRINT("Error: inet_pton failed(%s)!\n", strerror(errno));
        return -1;
    }

    /* �ͻ��������������� */ 
    if(connect(TERM->FD,(struct sockaddr *)(&server_addr),sizeof(server_addr))<0) 
    {       
        if(errno==EINPROGRESS)
        {
            FD_ZERO(&wr_fds);   
            FD_SET(TERM->FD,&wr_fds);
            tv.tv_sec = 5;//timeout
            tv.tv_usec = 0;
            err=select( TERM->FD + 1,NULL,&wr_fds,NULL,&tv);//
            if(err > 0)
            {
                nonblock = sizeof(err);
                err = 0;
                if(getsockopt(TERM->FD,SOL_SOCKET,SO_ERROR,&err,&nonblock) < 0 || err)
                    err = -2;
            } 
            else
                err = -2;

        }
        else
        {
            //DPRINT("connect to %s:%d failed\n",TERM->IP,TERM->PORT);
            err = -3;
        }
    } 
    else
    {
        TERM->CONNECT_FLAG = 1;
       // DPRINT("connect to %s:%d success\n",TERM->IP,TERM->PORT);
    }
    
    if(err < 0)
	{
		printf("connect error sockfd=%d,err=%d\n",TERM->FD,err);        
		CLOSE_645_2007_TERM(TERM);
		return -2;
	}
    return 0;
}

//��������
int com_645_2007_SendData(com_645_2007_CONNECT_TERM *TERM,char *sendbuf,int sendlen)
{
    int slen=0,len=0,reconnected=0;

    if(TERM->FD < 0)
        return -1;
    
    while(slen<sendlen)
    {
        if((len=send(TERM->FD,sendbuf+slen,sendlen-slen,MSG_DONTWAIT))<0)
        {
            //DPRINT("Send error,reconnect...\n"); 
			CLOSE_645_2007_TERM(TERM);
            if(reconnected == 0)
            {
                reconnected = 1;
                com_645_2007_CONNECT(TERM/*,1*/);
                if(TERM->FD < 0)
                    return -2;
                else
                    continue;
            } 
            else
                return -3;
        }
        slen+=len;
        if(slen >= sendlen)
            break;
        usleep(10*1000);
    }
    //DPRINT("645_Send data:\n");
    printhexdata(sendbuf, slen);	//��ӡ���͵�����
    return slen;
}

//��������
int com_645_2007_recv(com_645_2007_CONNECT_TERM *TERM, char *rcv_buf,int data_len)  
{  
    int len,fs_sel;  
    fd_set fs_read;  
	
    if(TERM->FD < 0)
        return -1;
	
    struct timeval time;  
     
    FD_ZERO(&fs_read);  
    FD_SET(TERM->FD,&fs_read);  
     
    time.tv_sec = 0;  
    time.tv_usec =100000 ;  
     
    //ʹ��selectʵ�ִ��ڵĶ�·ͨ��  
    fs_sel = select(TERM->FD+1,&fs_read,NULL,NULL,&time);  
    if(fs_sel>0)  
    {  
        len = read(TERM->FD,rcv_buf,data_len);  
        printf("I am right!(version1.2) len = %d fs_sel = %d\n",len,fs_sel);  
        return len;  
    }  
    else  
    {  
        printf("Sorry,I am wrong!");  
        return -1;  
    }       
}  

//�ۼӺ�У��
uint8_t LIB_CheckSum(uint8_t *pBuf, uint16_t len)
{
	uint8_t sum = 0;
	while (len--)
	{
		sum += *pBuf++;
	}
	return sum;
}

//��������֡
int Analyze_protocol_645_2007(u8 *buf, u32 Len, u8 *data, u8 *data_len)
{
	u8 index = 0;
	int i;
	for (i = 0; i < Len; i++)
	{
		if ((Len - i) < 12)
		{
			//return -1;
		}
		if (buf[i] != 0x68 || buf[i + 7] != 0x68)
		{
			continue;
		}
		uint8_t datalen = buf[i + 9];
		if ((i + datalen + 12) > Len) 			//����֡������
		{
			return -1;
		}
		if (buf[i + datalen + 11] != 0x16)		//֡β��־����ȷ
		{
			continue;
		}

		uint8_t cs = LIB_CheckSum((buf + i), (uint8_t)(datalen + 10));
		if (cs != buf[i + datalen + 10]) 		//У��Ͳ���ȷ
		{
			continue;
		}

		if (((uint8_t)(buf[i + 12] - 0x33) == 0xe5) && ((uint8_t)(buf[i + 13] - 0x33) == 0xe5))//�Ǳ�׼645-07��Լ
		{
			continue;
		}
		if ((buf[i + 13] - 0x33 == 0x04) && (buf[i + 12] - 0x33 == 0x00) && (buf[i + 11] - 0x33 == 0x01) && (buf[i + 10] - 0x33 == 0x0c))
		{
			continue;
		}

		if ((buf[i + 13] - 0x33 == 0x04) && (buf[i + 12] - 0x33 == 0xA0) && (buf[i + 11] - 0x33 == 0x06) && (buf[i + 10] - 0x33 == 0x00))
		{
			continue;
		}

		if ((buf[i + 8] & 0x80) == 0x80)
		{
			//retComPtr->cUpDown = 1;
		}
		else
		{
			//retComPtr->cUpDown = 0;
		}
		*data_len = datalen;
		memcpy(data, &buf[i+10], datalen);
		return 1;
	}
	return -1;

}


//����Ϊ��ȥ33
void Protocol_645_Pack_1(u8 *P_String, u8 Length)
{
	u8 i = 0;	
	//�ڶ����������������ֽ�+0x33
	for(i = 0; i < Length; i++)
	{
		P_String[i] -= 0x33;
	}
}


//��������֡������������
int com_645_2007_SendFrame(p645_tasktype tasktype645,void *taskpara)
{
	u8 p645_2007_buf[] = {0x68, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x68, 0x11, 0x04, 0x33, 0x32, 0x58, 0x35, 0xD7, 0x16};
	u8 recv_645_buf[256] = {0};
	u8 recv_len = 0;
	int ret = 0;
	u8 data[256] = {0};
	u8 data_len = 0;
	if(com_645_2007_HOST.CONNECT_FLAG == 0)
	{
		//��������
		if(0 != com_645_2007_CONNECT(&com_645_2007_HOST))
		{
			printf("���ӷ�����ʧ��\n");
			return -1;
		}		
	}
	
	//��������֡
	if(sizeof(p645_2007_buf) != com_645_2007_SendData(&com_645_2007_HOST, p645_2007_buf, sizeof(p645_2007_buf)))
	{
		CLOSE_645_2007_TERM(&com_645_2007_HOST);
		printf("����֡����ʧ��\n");
		return -1;
	}
	
	//��������
	while((ret = com_645_2007_recv(&com_645_2007_HOST, recv_645_buf, sizeof(recv_645_buf))) > 0)
	{
		recv_len += ret;
	}
	
	//DPRINT("645_Recv data:\n");
    printhexdata(recv_645_buf, recv_len);	//��ӡ���յ�����
    
	if(1 == Analyze_protocol_645_2007(recv_645_buf, recv_len, data, &data_len))
	{	
		Protocol_645_Pack_1(data, data_len);	
		BackStage_645_2007(data, data_len, 0, 0);
	}
	else
	{
		return -1;
	}
	CLOSE_645_2007_TERM(&com_645_2007_HOST);
	return 0;
}


void BackStage_645_2007(u8 *data,int datalen,int isfirst,int islast)
{
   if(66 != datalen)
   	return;
   printf("datalen = %d\n",datalen);
   
   u8 dqdy_buf[6] = {0}; 
   u8 dqdl_buf[12] = {0};		//����������������
   u8 zyggl_buf[12] = {0};
   u8 zwggl_buf[12] = {0};
   u8 zglys_buf[8] = {0};
   
   SJGS_07 *dqdy;				 //��ǰA��B��C���ѹ
   SJGS_25 *dqdl;	 			 //��ǰA��B��C�����
   SJGS_09 *zyggl; 	 			 //,*ABCxyggl; �й�����
   SJGS_09 *zwggl;  			 //,*ABCxwggl; �޹�����
   SJGS_05 *zglys;   			 //,*ABCxglys; ��������
   int value,Inzs=0,Inxs=0;
   int i;
   
   memcpy(zyggl_buf, &data[4], 12);		//�й�����
   memcpy(zwggl_buf, &data[16], 12);	//�޹�����
   memcpy(dqdy_buf, &data[28], 6);		//��ѹ
   memcpy(dqdl_buf, &data[34], 9);		//����
   memcpy(&dqdl_buf[9], &data[63], 3);	//���ߵ���
   memcpy(zglys_buf, &data[43], 8);		//��������
   
  //���㵱ǰ��ѹ����
  //��ѹ
  for(i=0;i<3;i++)
  {
	  dqdy = (SJGS_07 *)&dqdy_buf[i*2];
	  if(dqdy->SFW>9 || dqdy->GW>9 || dqdy->SW>9 || dqdy->BW>9)
	  {
		  printf("get AC date error\n");
	  }
	  else
	  {
		  value = dqdy->BW*100+dqdy->SW*10+dqdy->GW; 	  
		  ACdate.U[i] = value;
		  value = dqdy->SFW;
		  ACdate.U[i] += ((float)value)/10;
		  printf("ACdate.U[%d] = %f\n",i,ACdate.U[i]);
	  }
  }
  
  //����
  for(i=0;i<4;i++)
  {
	 dqdl = (SJGS_25 *)&dqdl_buf[i*3];
	 if(dqdl->QFW>9 || dqdl->BFW>9 || dqdl->SFW>9 || dqdl->GW>9 || dqdl->SW>9 || dqdl->BW>9)
	 {
		 printf("get AC date error\n");
	 }
	 else
	{
		 value = dqdl->BW*100+dqdl->SW*10+dqdl->GW; 
		 ACdate.I[i] = value;
		 value = dqdl->SFW*100+dqdl->BFW*10+dqdl->QFW; 
		 ACdate.I[i] += ((float)value)/1000;
		 printf("ACdate.I[%d] = %f\n",i,ACdate.I[i]);
	 }
  }
}


