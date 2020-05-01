/******************** (C) COPYRIGHT 2019 ����� **********************
* File Name          : protocolD645.c
* Author             : ������
* Version            : 1.0
* Date               : 2019/05/22
* Description        : 
*********************************************************************/

#include "ProtocolD645.h"
#include <string.h>
#include <stdio.h>
#include "rtc.h"
#include <time.h>
#include "Tools.h"

//645Э��--��ַ(����)
const uint8_t PROTOCOL_645_ADDRESS_UNIVERSAL[6] = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};  //ͨ���ַ
const uint8_t PROTOCOL_645_ADDRESS_ADVERTISE[6] = {0x99, 0x99, 0x99, 0x99, 0x99, 0x99};  //�㲥��ַ

//645Э��--������
#define		PROTOCOL_645_CONTROL_CODE	0x03

//645Э��--�������ʶ
const uint8_t PROTOCOL_645_DATA_FILE_TRAMSMIT[] = {0x0F, 0x00, 0x00, 0x01};  		//�ļ�����ָ��
const uint8_t PROTOCOL_645_DATA_PROGRAM_SWITCH[] = {0x0F, 0x00, 0x00, 0x02};  		//�����л�
const uint8_t PROTOCOL_645_DATA_ID_AUTHEN_FIRST[] = {0x04, 0x00, 0x0F, 0x40}; 	 	//�����֤��һ֡����
const uint8_t PROTOCOL_645_DATA_ID_AUTHEN_SECOND[] = {0x04, 0x00, 0x0F, 0x41}; 	 	//�����֤�ڶ�֡����
const uint8_t PROTOCOL_645_DATA_GET_SECURITY_INFO[] = {0x04, 0x00, 0x0F, 0x42}; 	//��ȡTAר��ģ�鰲ȫ��Ϣ����֡
const uint8_t PROTOCOL_645_DATA_KEY_UPDATE[] = {0x04, 0x00, 0x0F, 0x43};  			//��Կ����
const uint8_t PROTOCOL_645_DATA_GET_BAUDRATE[] = {0x04, 0x00, 0x0F, 0x44};  		//��ȡ����������������֡
const uint8_t PROTOCOL_645_DATA_SET_BAUDRATE[] = {0x04, 0x00, 0x0F, 0x45};  		//���ò���������������֡
const uint8_t PROTOCOL_645_DATA_GET_TA_TRANSFORMER[] = {0x04, 0x00, 0x0F, 0x46};  	//��ȡTA��Ȳ�������֡
const uint8_t PROTOCOL_645_DATA_SET_TA_TRANSFORMER[] = {0x04, 0x00, 0x0F, 0x47};  	//����TA��Ȳ�������֡
const uint8_t PROTOCOL_645_DATA_GET_TA_STATUS[] = {0x07, 0x00, 0x00, 0x00};  		//��ȡTAר��ģ��״̬����֡
const uint8_t PROTOCOL_645_DATA_GET_TA_WORKING[] = {0x07, 0x00, 0x00, 0x01};  		//��ȡTAר��ģ�鹤����Ϣ
const uint8_t PROTOCOL_645_DATA_GET_REAL_DATA[] = {0x07, 0x00, 0x00, 0x03};  		//��ȡTAר��ģ��ʵʱ��������
const uint8_t PROTOCOL_645_DATA_GET_GUIDE_PARAM[] = {0x07, 0x00, 0x00, 0x04};  		//��ȡTAר��ģ����̲�����Ϣ
const uint8_t PROTOCOL_645_DATA_GET_TIME[]        = {0xEE, 0x02, 0x00, 0x00};  		//��ȡʱ��
const uint8_t PROTOCOL_645_DATA_SET_TIME[]        = {0xEE, 0x02, 0x00, 0x00};  		//����ʱ�� 


//���ڲ���ģʽ��LTU��HPLC��ͨ��
const uint8_t PROTOCOL_TEST_LTU_TO_HPLC[] = {0x68, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x68, 0x00, 0x00, 0xCC, 0x16};
const uint8_t PROTOCOL_TEST_HPLC_TO_LTU[] = {0x68, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x68, 0x00, 0x00, 0xCC, 0x16};
#ifdef LTU_TRANSFORMS_RATE
extern Struct_LTU_Transformer	LTU_Transformer_Rate;
#endif



void SGDProtocolHSProcess(tSavePara *cUpData, tSendPara *tUpSendData, uint8_t UART_Port)
{

}

uint8_t DEC_To_BCD(uint8_t Data_DEC)
{
	uint8_t Data_BCD = 0;
	
	Data_BCD = ((Data_DEC / 10) << 4) + (Data_DEC % 10);
	
	return Data_BCD;
}


//������ĸ��ֽں͵��ֽ�λ��ȫ���Ե�������
void Array_Mirror(uint8_t *P_Array, uint32_t Length)
{
	uint32_t i = 0;
	uint8_t  Temp = 0;
	uint32_t Length_Half = 0;
	
	Length_Half = Length / 2;
	
	for(i = 0; i < Length_Half; i++)
	{
		Temp = *(P_Array + i);
		*(P_Array + i) = *(P_Array + Length - 1 - i);
		*(P_Array + Length - 1 - i) = Temp;
	}
}


//���645Э��
void Protocol_645_Unpack(uint8_t *P_String, uint8_t Length)
{
	uint32_t i = 0;
	
	//��һ������ַ��ߵ��ֽڷ�ת���ָ�����
	Array_Mirror(P_String + 1, 6);
	
	//�ڶ������������ȥ0x33
	for(i = 0; i < P_String[9]; i++)
	{
		P_String[10 + i] -= 0x33;
	}
}

//���645Э��
void Protocol_645_Pack(uint8_t *P_String, uint8_t Length)
{
	uint32_t	i = 0;
	
	//��һ������ַ��ߵ��ֽڷ�ת��С�˴���
	Array_Mirror(P_String + 1, 6);
	
	//�ڶ����������������ֽ�+0x33
	for(i = 0; i < P_String[9]; i++)
	{
		P_String[10 + i] += 0x33;
	}
}

//����У���
uint8_t GetSum8(uint8_t *P_Data, uint32_t Length)
{
	uint8_t	Sum = 0;
	uint32_t	i   = 0;
	
	for(i = 0; i < Length; i++)
	{
		Sum += P_Data[i];
	}
	
	return Sum;
}



//645Э���ƫ���ֽ�
#define PROTOCOL_645_OFFSET_ADDRESS         1   //��ַ����645Э���е�ƫ���ֽ���
#define PROTOCOL_645_OFFSET_CONTROL         8   //��������645Э���е�ƫ���ֽ���
#define PROTOCOL_645_OFFSET_DATA_LENGTH     9   //�����򳤶���645Э���е�ƫ���ֽ���
#define PROTOCOL_645_OFFSET_DATA            10  //��������645Э���е�ƫ���ֽ���
//���յ�645Э���Ժ�Ĵ���
void Protocol645_Up2Down_Process(tSavePara *Data_Up2Down, tSendPara *Data_Return, uint8_t UART_Port)
{	
	uint8_t  Protocol[256]        = {0};  //����Э��
	uint8_t  Protocol_Length      =  0;   //���յ���֡�ܳ���
	uint8_t  Protocol_Address[6]  = {0};  //��ַ��
	uint8_t  Protocol_Control     =  0;   //������
	uint8_t  Protocol_Data_Length =  0;   //�����򳤶�
	uint8_t  Protocol_Data[256]   = {0};  //������
	//uint8_t  Protocol_SEQ         = 0;    //֡���
	uint32_t i                         = 0;
	uint8_t  Flag_Process              = 0;  //�Ƿ���Ҫ����

	struct tm Time;
	
	//ת����յ���Э��
	memcpy(Protocol, Data_Up2Down->SaveBuf, Data_Up2Down->SaveLen);
	Protocol_Length = Data_Up2Down->SaveLen;
	
	//���645Э��
	Protocol_645_Unpack(Protocol, Data_Up2Down->SaveLen);
	
	//���ݷֶδ洢
	memcpy( Protocol_Address,      Protocol + 1,  6);                     //��ַ��
	memcpy(&Protocol_Control,      Protocol + 8,  1);                     //������
	memcpy(&Protocol_Data_Length,  Protocol + 9,  1);                     //�����򳤶�
	memcpy( Protocol_Data,         Protocol + 10, Protocol_Data_Length);  //������
		
	//��ӡ��ԭʼ����
	printf("������վ��Ϣ������\r\n");
	printf("���յ���֡����: ");
	for(i = 0; i < Protocol_Length; i++)
	{
		printf("0x%X ", Data_Up2Down->SaveBuf[i]);
	}
	printf("\r\n");
	
	//��ӡ���ֶ�����
	printf("Protocol_Length:      %d\r\n",   Protocol_Length);
	printf("Protocol_Address:     0x%X 0x%X 0x%X 0x%X 0x%X 0x%X\r\n", Protocol_Address[0], Protocol_Address[1], Protocol_Address[2], Protocol_Address[3], Protocol_Address[4], Protocol_Address[5]);
	printf("Protocol_Control:     0x%X\r\n", Protocol_Control);
	printf("Protocol_Data_Length: %d\r\n",   Protocol_Data_Length);
	printf("Protocol_Data:        ");
	for(i = 0; i < Protocol_Data_Length; i++)
	{
		printf("0x%X ", Protocol_Data[i]);
	}
	printf("\r\n");

#if 1
//�����ͨ���ַ���㲥��ַ��������ַ����ô��ҪӦ�𣬷��򲻽���Ӧ��
	if((memcmp(Protocol_Address, PROTOCOL_645_ADDRESS_UNIVERSAL, 6) == 0) ||
   		(memcmp(Protocol_Address, PROTOCOL_645_ADDRESS_ADVERTISE, 6) == 0))
#else
	//�����ͨ���ַ���㲥��ַ��������ַ����ô��ҪӦ�𣬷��򲻽���Ӧ��
	if((memcmp(Protocol_Address, PROTOCOL_645_ADDRESS_UNIVERSAL, 6) == 0) ||
	   (memcmp(Protocol_Address, PROTOCOL_645_ADDRESS_ADVERTISE, 6) == 0) ||
	   /*(memcmp(Protocol_Address, LTU_MAC.MAC, 6) == 0)*/)
#endif
	{
		Flag_Process = 1;
	}
	else
	{
		Flag_Process = 0;
	}

	//�����Ҫ��Ӧ
	if(Flag_Process)
	{
		if(PROTOCOL_645_CONTROL_CODE == Protocol_Control)
		{
			//��ת�������ʶ
			Array_Mirror(Protocol_Data, 4);
			
			if(memcmp(Protocol_Data, PROTOCOL_645_DATA_FILE_TRAMSMIT, 4) == 0)	//�ļ�����ָ��
			{
			}
			else if(memcmp(Protocol_Data, PROTOCOL_645_DATA_PROGRAM_SWITCH, 4) == 0)	//�����л�
			{
			}			
			else if(memcmp(Protocol_Data, PROTOCOL_645_DATA_ID_AUTHEN_FIRST, 4) == 0)	//�����֤��һ֡����
			{
			}
			else if(memcmp(Protocol_Data, PROTOCOL_645_DATA_ID_AUTHEN_SECOND, 4) == 0)	//�����֤�ڶ�֡����
			{
			}
			else if(memcmp(Protocol_Data, PROTOCOL_645_DATA_GET_SECURITY_INFO, 4) == 0)	//��ȡTAר��ģ�鰲ȫ��Ϣ����֡
			{
			}			
			else if(memcmp(Protocol_Data, PROTOCOL_645_DATA_KEY_UPDATE, 4) == 0)
			{
			}
			else if(memcmp(Protocol_Data, PROTOCOL_645_DATA_GET_BAUDRATE, 4) == 0)
			{
			}
			else if(memcmp(Protocol_Data, PROTOCOL_645_DATA_SET_BAUDRATE, 4) == 0)
			{
			}
			else if(memcmp(Protocol_Data, PROTOCOL_645_DATA_GET_TA_TRANSFORMER, 4) == 0)	//��ȡTA��Ȳ�������֡
			{
				Data_Return->SendBuf[0] = 0x68;
				Data_Return->SendBuf[1] = Protocol_Address[0];
				Data_Return->SendBuf[2] = Protocol_Address[1];
				Data_Return->SendBuf[3] = Protocol_Address[2];
				Data_Return->SendBuf[4] = Protocol_Address[3];
				Data_Return->SendBuf[5] = Protocol_Address[4];
				Data_Return->SendBuf[6] = Protocol_Address[5];
				Data_Return->SendBuf[7] = 0x68;
				Data_Return->SendBuf[8] = 0x03;				
				Data_Return->SendBuf[9] = 8;
				Data_Return->SendBuf[10] = Protocol_Data[8];	//��ʽ��
				Data_Return->SendBuf[11] = Protocol_Data[9];	//A���Ե����
				Data_Return->SendBuf[12] = Protocol_Data[10];	//B���Ե����
				Data_Return->SendBuf[13] = Protocol_Data[11];	//C���Ե����
				//MAC
				Array_Mirror(Protocol_Data + 12, 4);
				Data_Return->SendBuf[14] = Protocol_Data[12];
				Data_Return->SendBuf[15] = Protocol_Data[13];
				Data_Return->SendBuf[16] = Protocol_Data[14];
				Data_Return->SendBuf[17] = Protocol_Data[15];
				//���
				Protocol_645_Pack(Data_Return->SendBuf, 18);

				//У��� + ������ + ֡����
				Data_Return->SendBuf[18] = GetSum8(Data_Return->SendBuf, 18);
				Data_Return->SendBuf[19] = 0x16;
				Data_Return->SendLen = 20;
			}
			else if(memcmp(Protocol_Data, PROTOCOL_645_DATA_SET_TA_TRANSFORMER, 4) == 0)	//����TA��Ȳ�������֡
			{
			}
			else if(memcmp(Protocol_Data, PROTOCOL_645_DATA_GET_TA_STATUS, 4) == 0)	//��ȡTAר��ģ��״̬����֡
			{
				Data_Return->SendBuf[0] = 0x68;
				Data_Return->SendBuf[1] = Protocol_Address[0];
				Data_Return->SendBuf[2] = Protocol_Address[1];
				Data_Return->SendBuf[3] = Protocol_Address[2];
				Data_Return->SendBuf[4] = Protocol_Address[3];
				Data_Return->SendBuf[5] = Protocol_Address[4];
				Data_Return->SendBuf[6] = Protocol_Address[5];
				Data_Return->SendBuf[7] = 0x68;
				Data_Return->SendBuf[8] = 0x03;				
				Data_Return->SendBuf[9] = 8;
				Data_Return->SendBuf[10] = Protocol_Data[8];	//��ʽ��
				Data_Return->SendBuf[11] = Protocol_Data[9];	//A���Ե����
				Data_Return->SendBuf[12] = Protocol_Data[10];	//B���Ե����
				Data_Return->SendBuf[13] = Protocol_Data[11];	//C���Ե����
				//MAC
				Array_Mirror(Protocol_Data + 12, 4);
				Data_Return->SendBuf[14] = Protocol_Data[12];
				Data_Return->SendBuf[15] = Protocol_Data[13];
				Data_Return->SendBuf[16] = Protocol_Data[14];
				Data_Return->SendBuf[17] = Protocol_Data[15];
				//���
				Protocol_645_Pack(Data_Return->SendBuf, 18);

				//У��� + ������ + ֡����
				Data_Return->SendBuf[18] = GetSum8(Data_Return->SendBuf, 18);
				Data_Return->SendBuf[19] = 0x16;
				Data_Return->SendLen = 20;
			}
			else if(memcmp(Protocol_Data, PROTOCOL_645_DATA_GET_TA_WORKING, 4) == 0)
			{
			}
			else if(memcmp(Protocol_Data, PROTOCOL_645_DATA_GET_REAL_DATA, 4) == 0)
			{
			}
			else if(memcmp(Protocol_Data, PROTOCOL_645_DATA_GET_GUIDE_PARAM, 4) == 0)
			{
			}
			else if(memcmp(Protocol_Data, PROTOCOL_645_DATA_GET_TIME, 4) == 0)	//��ȡʱ��
			{
				printf("��ȡʱ��\r\n");
				
				//��ʱ��оƬ�л�ȡʱ��
				GetRtc(&Time);
				
				//����Ӧ��
				Data_Return->SendBuf[0]  = 0x68;
				Data_Return->SendBuf[1] = Protocol_Address[0];
				Data_Return->SendBuf[2] = Protocol_Address[1];
				Data_Return->SendBuf[3] = Protocol_Address[2];
				Data_Return->SendBuf[4] = Protocol_Address[3];
				Data_Return->SendBuf[5] = Protocol_Address[4];
				Data_Return->SendBuf[6] = Protocol_Address[5];
				Data_Return->SendBuf[7]  = 0x68;
				Data_Return->SendBuf[8]  = 0x91;
				Data_Return->SendBuf[9]  = 10;
				Data_Return->SendBuf[10] = PROTOCOL_645_DATA_GET_TIME[0];
				Data_Return->SendBuf[11] = PROTOCOL_645_DATA_GET_TIME[1];
				Data_Return->SendBuf[12] = PROTOCOL_645_DATA_GET_TIME[2];
				Data_Return->SendBuf[13] = PROTOCOL_645_DATA_GET_TIME[3];
				Data_Return->SendBuf[14] = DEC_To_BCD(Time.tm_year);
				Data_Return->SendBuf[15] = DEC_To_BCD(Time.tm_mon);
				Data_Return->SendBuf[16] = DEC_To_BCD(Time.tm_mday);
				Data_Return->SendBuf[17] = DEC_To_BCD(Time.tm_hour);
				Data_Return->SendBuf[18] = DEC_To_BCD(Time.tm_min);
				Data_Return->SendBuf[19] = DEC_To_BCD(Time.tm_sec);
				
				//���
				Protocol_645_Pack(Data_Return->SendBuf, 20);
				//������ߵ��ֽڰ��跭ת
				Array_Mirror(Data_Return->SendBuf + 10, 4);  //�������ʶ
				Array_Mirror(Data_Return->SendBuf + 14, 6);  //ʱ��
				
				//У��� + ������ + ֡����
				Data_Return->SendBuf[20] = GetSum8(Data_Return->SendBuf, 20);
				Data_Return->SendBuf[21] = 0x16;
				Data_Return->SendLen     = 22;
			}
		}
	}
	#if 0
	//�����������벻��0�������Ǵ������Ͳ�Ϊ�գ���ô˵����������
	if((Error_Control != 0) || (Error_Type != ERROR_TYPE_NONE))
	{
		//���ش�������֡
		Data_Return->SendBuf[0]  = 0x68;
		Data_Return->SendBuf[1]  = LTU_MAC.MAC[0];
		Data_Return->SendBuf[2]  = LTU_MAC.MAC[1];
		Data_Return->SendBuf[3]  = LTU_MAC.MAC[2];
		Data_Return->SendBuf[4]  = LTU_MAC.MAC[3];
		Data_Return->SendBuf[5]  = LTU_MAC.MAC[4];
		Data_Return->SendBuf[6]  = LTU_MAC.MAC[5];
		Data_Return->SendBuf[7]  = 0x68;
		Data_Return->SendBuf[8]  = Error_Control;
		Data_Return->SendBuf[9]  = 2;
		Data_Return->SendBuf[10] = 0x00;
		Data_Return->SendBuf[11] = Error_Type;
		
		//���
		Protocol_645_Pack(Data_Return->SendBuf, 12);
		//У��� + ������ + ֡����
		Data_Return->SendBuf[12] = GetSum8(Data_Return->SendBuf, 12);
		Data_Return->SendBuf[13] = 0x16;
		Data_Return->SendLen = 14;
		
		Error_Control = 0;
		Error_Type    = ERROR_TYPE_NONE;
	}
	#endif

	Data_Up2Down->FrameTime    = 0;
	Data_Up2Down->DownOverTime = 0;
}


//���յ���վ���ݵĴ���
void Protocol645_Down2Up_Process(tSavePara *Data_Down2Up, tSavePara *Data_Up2Down, tSendPara *Data_Return)
{
	
}
