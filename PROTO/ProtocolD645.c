/******************** (C) COPYRIGHT 2019 深国电 **********************
* File Name          : protocolD645.c
* Author             : 李深龙
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

//645协议--地址(正序)
const uint8_t PROTOCOL_645_ADDRESS_UNIVERSAL[6] = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};  //通配地址
const uint8_t PROTOCOL_645_ADDRESS_ADVERTISE[6] = {0x99, 0x99, 0x99, 0x99, 0x99, 0x99};  //广播地址

//645协议--控制码
#define		PROTOCOL_645_CONTROL_CODE	0x03

//645协议--数据域标识
const uint8_t PROTOCOL_645_DATA_FILE_TRAMSMIT[] = {0x0F, 0x00, 0x00, 0x01};  		//文件传输指令
const uint8_t PROTOCOL_645_DATA_PROGRAM_SWITCH[] = {0x0F, 0x00, 0x00, 0x02};  		//程序切换
const uint8_t PROTOCOL_645_DATA_ID_AUTHEN_FIRST[] = {0x04, 0x00, 0x0F, 0x40}; 	 	//身份认证第一帧下行
const uint8_t PROTOCOL_645_DATA_ID_AUTHEN_SECOND[] = {0x04, 0x00, 0x0F, 0x41}; 	 	//身份认证第二帧下行
const uint8_t PROTOCOL_645_DATA_GET_SECURITY_INFO[] = {0x04, 0x00, 0x0F, 0x42}; 	//获取TA专用模块安全信息下行帧
const uint8_t PROTOCOL_645_DATA_KEY_UPDATE[] = {0x04, 0x00, 0x0F, 0x43};  			//密钥更新
const uint8_t PROTOCOL_645_DATA_GET_BAUDRATE[] = {0x04, 0x00, 0x0F, 0x44};  		//获取波特率特征字下行帧
const uint8_t PROTOCOL_645_DATA_SET_BAUDRATE[] = {0x04, 0x00, 0x0F, 0x45};  		//设置波特率特征字下行帧
const uint8_t PROTOCOL_645_DATA_GET_TA_TRANSFORMER[] = {0x04, 0x00, 0x0F, 0x46};  	//获取TA变比参数下行帧
const uint8_t PROTOCOL_645_DATA_SET_TA_TRANSFORMER[] = {0x04, 0x00, 0x0F, 0x47};  	//设置TA变比参数下行帧
const uint8_t PROTOCOL_645_DATA_GET_TA_STATUS[] = {0x07, 0x00, 0x00, 0x00};  		//获取TA专用模块状态下行帧
const uint8_t PROTOCOL_645_DATA_GET_TA_WORKING[] = {0x07, 0x00, 0x00, 0x01};  		//获取TA专用模块工况信息
const uint8_t PROTOCOL_645_DATA_GET_REAL_DATA[] = {0x07, 0x00, 0x00, 0x03};  		//获取TA专用模块实时测量数据
const uint8_t PROTOCOL_645_DATA_GET_GUIDE_PARAM[] = {0x07, 0x00, 0x00, 0x04};  		//获取TA专用模块调教参数信息
const uint8_t PROTOCOL_645_DATA_GET_TIME[]        = {0xEE, 0x02, 0x00, 0x00};  		//获取时间
const uint8_t PROTOCOL_645_DATA_SET_TIME[]        = {0xEE, 0x02, 0x00, 0x00};  		//设置时间 


//用于测试模式下LTU和HPLC的通信
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


//把数组的高字节和低字节位置全部对调（镜像）
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


//解包645协议
void Protocol_645_Unpack(uint8_t *P_String, uint8_t Length)
{
	uint32_t i = 0;
	
	//第一步，地址域高低字节翻转，恢复正常
	Array_Mirror(P_String + 1, 6);
	
	//第二步，数据域减去0x33
	for(i = 0; i < P_String[9]; i++)
	{
		P_String[10 + i] -= 0x33;
	}
}

//打包645协议
void Protocol_645_Pack(uint8_t *P_String, uint8_t Length)
{
	uint32_t	i = 0;
	
	//第一步，地址域高低字节翻转，小端传送
	Array_Mirror(P_String + 1, 6);
	
	//第二步，数据域所有字节+0x33
	for(i = 0; i < P_String[9]; i++)
	{
		P_String[10 + i] += 0x33;
	}
}

//计算校验和
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



//645协议的偏移字节
#define PROTOCOL_645_OFFSET_ADDRESS         1   //地址码在645协议中的偏移字节量
#define PROTOCOL_645_OFFSET_CONTROL         8   //控制码在645协议中的偏移字节量
#define PROTOCOL_645_OFFSET_DATA_LENGTH     9   //数据域长度在645协议中的偏移字节量
#define PROTOCOL_645_OFFSET_DATA            10  //数据域在645协议中的偏移字节量
//接收到645协议以后的处理
void Protocol645_Up2Down_Process(tSavePara *Data_Up2Down, tSendPara *Data_Return, uint8_t UART_Port)
{	
	uint8_t  Protocol[256]        = {0};  //整条协议
	uint8_t  Protocol_Length      =  0;   //接收到的帧总长度
	uint8_t  Protocol_Address[6]  = {0};  //地址码
	uint8_t  Protocol_Control     =  0;   //控制码
	uint8_t  Protocol_Data_Length =  0;   //数据域长度
	uint8_t  Protocol_Data[256]   = {0};  //数据域
	//uint8_t  Protocol_SEQ         = 0;    //帧序号
	uint32_t i                         = 0;
	uint8_t  Flag_Process              = 0;  //是否需要处理

	struct tm Time;
	
	//转存接收到的协议
	memcpy(Protocol, Data_Up2Down->SaveBuf, Data_Up2Down->SaveLen);
	Protocol_Length = Data_Up2Down->SaveLen;
	
	//解包645协议
	Protocol_645_Unpack(Protocol, Data_Up2Down->SaveLen);
	
	//数据分段存储
	memcpy( Protocol_Address,      Protocol + 1,  6);                     //地址域
	memcpy(&Protocol_Control,      Protocol + 8,  1);                     //控制码
	memcpy(&Protocol_Data_Length,  Protocol + 9,  1);                     //数据域长度
	memcpy( Protocol_Data,         Protocol + 10, Protocol_Data_Length);  //数据域
		
	//打印出原始数据
	printf("进入主站消息处理函数\r\n");
	printf("接收到的帧数据: ");
	for(i = 0; i < Protocol_Length; i++)
	{
		printf("0x%X ", Data_Up2Down->SaveBuf[i]);
	}
	printf("\r\n");
	
	//打印出分段数据
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
//如果是通配地址、广播地址、本机地址，那么需要应答，否则不进行应答
	if((memcmp(Protocol_Address, PROTOCOL_645_ADDRESS_UNIVERSAL, 6) == 0) ||
   		(memcmp(Protocol_Address, PROTOCOL_645_ADDRESS_ADVERTISE, 6) == 0))
#else
	//如果是通配地址、广播地址、本机地址，那么需要应答，否则不进行应答
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

	//如果需要回应
	if(Flag_Process)
	{
		if(PROTOCOL_645_CONTROL_CODE == Protocol_Control)
		{
			//翻转数据域标识
			Array_Mirror(Protocol_Data, 4);
			
			if(memcmp(Protocol_Data, PROTOCOL_645_DATA_FILE_TRAMSMIT, 4) == 0)	//文件传输指令
			{
			}
			else if(memcmp(Protocol_Data, PROTOCOL_645_DATA_PROGRAM_SWITCH, 4) == 0)	//程序切换
			{
			}			
			else if(memcmp(Protocol_Data, PROTOCOL_645_DATA_ID_AUTHEN_FIRST, 4) == 0)	//身份认证第一帧下行
			{
			}
			else if(memcmp(Protocol_Data, PROTOCOL_645_DATA_ID_AUTHEN_SECOND, 4) == 0)	//身份认证第二帧下行
			{
			}
			else if(memcmp(Protocol_Data, PROTOCOL_645_DATA_GET_SECURITY_INFO, 4) == 0)	//获取TA专用模块安全信息下行帧
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
			else if(memcmp(Protocol_Data, PROTOCOL_645_DATA_GET_TA_TRANSFORMER, 4) == 0)	//获取TA变比参数下行帧
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
				Data_Return->SendBuf[10] = Protocol_Data[8];	//格式码
				Data_Return->SendBuf[11] = Protocol_Data[9];	//A相窃电情况
				Data_Return->SendBuf[12] = Protocol_Data[10];	//B相窃电情况
				Data_Return->SendBuf[13] = Protocol_Data[11];	//C相窃电情况
				//MAC
				Array_Mirror(Protocol_Data + 12, 4);
				Data_Return->SendBuf[14] = Protocol_Data[12];
				Data_Return->SendBuf[15] = Protocol_Data[13];
				Data_Return->SendBuf[16] = Protocol_Data[14];
				Data_Return->SendBuf[17] = Protocol_Data[15];
				//打包
				Protocol_645_Pack(Data_Return->SendBuf, 18);

				//校验和 + 结束符 + 帧长度
				Data_Return->SendBuf[18] = GetSum8(Data_Return->SendBuf, 18);
				Data_Return->SendBuf[19] = 0x16;
				Data_Return->SendLen = 20;
			}
			else if(memcmp(Protocol_Data, PROTOCOL_645_DATA_SET_TA_TRANSFORMER, 4) == 0)	//设置TA变比参数下行帧
			{
			}
			else if(memcmp(Protocol_Data, PROTOCOL_645_DATA_GET_TA_STATUS, 4) == 0)	//获取TA专用模块状态下行帧
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
				Data_Return->SendBuf[10] = Protocol_Data[8];	//格式码
				Data_Return->SendBuf[11] = Protocol_Data[9];	//A相窃电情况
				Data_Return->SendBuf[12] = Protocol_Data[10];	//B相窃电情况
				Data_Return->SendBuf[13] = Protocol_Data[11];	//C相窃电情况
				//MAC
				Array_Mirror(Protocol_Data + 12, 4);
				Data_Return->SendBuf[14] = Protocol_Data[12];
				Data_Return->SendBuf[15] = Protocol_Data[13];
				Data_Return->SendBuf[16] = Protocol_Data[14];
				Data_Return->SendBuf[17] = Protocol_Data[15];
				//打包
				Protocol_645_Pack(Data_Return->SendBuf, 18);

				//校验和 + 结束符 + 帧长度
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
			else if(memcmp(Protocol_Data, PROTOCOL_645_DATA_GET_TIME, 4) == 0)	//获取时间
			{
				printf("获取时间\r\n");
				
				//从时钟芯片中获取时间
				GetRtc(&Time);
				
				//返回应答
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
				
				//打包
				Protocol_645_Pack(Data_Return->SendBuf, 20);
				//数据域高低字节按需翻转
				Array_Mirror(Data_Return->SendBuf + 10, 4);  //数据域标识
				Array_Mirror(Data_Return->SendBuf + 14, 6);  //时间
				
				//校验和 + 结束符 + 帧长度
				Data_Return->SendBuf[20] = GetSum8(Data_Return->SendBuf, 20);
				Data_Return->SendBuf[21] = 0x16;
				Data_Return->SendLen     = 22;
			}
		}
	}
	#if 0
	//如果错误控制码不是0，或者是错误类型不为空，那么说明发生错误
	if((Error_Control != 0) || (Error_Type != ERROR_TYPE_NONE))
	{
		//返回错误数据帧
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
		
		//打包
		Protocol_645_Pack(Data_Return->SendBuf, 12);
		//校验和 + 结束符 + 帧长度
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


//接收到从站数据的处理
void Protocol645_Down2Up_Process(tSavePara *Data_Down2Up, tSavePara *Data_Up2Down, tSendPara *Data_Return)
{
	
}
