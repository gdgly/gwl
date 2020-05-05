/******************** (C) COPYRIGHT 2013 ����� **********************
* File Name          : ProtocolD645.h
* Author             :
* Version            : 
* Date               : 
* Description        : 
*********************************************************************/
#ifndef  _PROTOCOLD645_H_
#define  _PROTOCOLD645_H_
//#include "Sgd_types.h"
#include <stdbool.h>
//#include "comtest.h"

//���ڷ��͡����ջ������Ĵ�С
#define BUFMAXSIZE    256

#ifndef REFUSE
#define REFUSE   0xDDDDDDDD
#endif



#ifndef CONFIRM
#define CONFIRM  0xEEEEEEEE
#endif

#if 0

typedef struct
{
	uint8_t Head1;
	uint8_t Address[6];
	uint8_t Head2;
	uint8_t Control;
	uint8_t Length;
	uint8_t Data[1];
}gFrameDLT645;



typedef struct
{
	uint8_t Head1;
	uint8_t Address1[6];
	uint8_t Head2;
	uint8_t Control;
	uint8_t Length;
	uint8_t DI[4];
	uint8_t Port;
	uint8_t C_Words;
	uint8_t Address2[8];
	uint8_t Data[1];
}gFrameDLT645Ex;



typedef struct
{
	uint8_t Head1;
	uint8_t Address1[6];
	uint8_t Head2;
	uint8_t Control;
	uint8_t Length;
	uint8_t DI[4];
	uint8_t PA;
	uint8_t P[3];
	uint8_t C[4];
	uint8_t Data[1];
}gFrameDLT645ExSet;

typedef struct
{
	uint16_t SendLen;              //��Ҫͨ�����ڷ��Ͷ��ٸ��ֽ�����
	uint8_t  SendBuf[BUFMAXSIZE];  //�洢�����ʹ������ݵĻ�����
}tSendPara;

typedef struct
{
	uint8_t  gPort;
	uint8_t  Sprotocol;            //Э������
	uint16_t FrameTime;            //��ʱʱ��
	uint16_t DownOverTime;         //��ʱ����
	uint8_t  SaveLen;              //��ǰ����֡����
	uint8_t  SaveBuf[BUFMAXSIZE];  //��ǰ����֡������ָ��
}tSavePara;




void Protocol645_Up2Down_Process(tSavePara *Data_Up2Down, tSendPara *Data_Return, uint8_t UART_Port);

void SGDProtocolHSProcess(tSavePara *cUpData, tSendPara *tUpSendData, uint8_t UART_Port);

void Protocol645_Down2Up_Process(tSavePara *Data_Down2Up, tSavePara *Data_Up2Down, tSendPara *Data_Return);

//���645Э�飨��ַ���С�ֽڷ�ת��������ȫ����ȥ0x33��
extern void Protocol_645_Unpack(uint8_t *P_String, uint8_t Length);
void Protocol_645_Pack(uint8_t *P_String, uint8_t Length);
//������ĸ��ֽں͵��ֽ�λ��ȫ���Ե�������
extern void  Array_Mirror(uint8_t *P_Array, uint32_t Length);
uint8_t GetSum8(uint8_t *P_Data, uint32_t Length);
#endif
int mrsFind645Frame(unsigned char* in_buf, int in_len, int *start, int *out_len);
#endif
