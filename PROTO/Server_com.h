#ifndef __COM_H_
#define __COM_H_

typedef struct
{
    int     FD;
    char    IP[16];
    int     PORT;
    int     CONNECT_FLAG;
}__attribute__((packed)) CONNECT_TERM;





//int SendData(CONNECT_TERM *TERM,char *buf,int len);
int SendData(int fd, unsigned char *sendbuf, int sendlen);
int RecvData(int fd, unsigned char *recvbuf, int recvlen, int timeout);
int SERVER_ACCEPT(void);
int NETWORKMASTER_LISTEN(void);
int Decode_698_frame(char *buf, int len);


#endif

