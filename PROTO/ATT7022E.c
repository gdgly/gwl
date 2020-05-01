#include "ATT7022E.h"

#if 0
#define Dummy_Byte    0xFF  //��Ч����

static uint8_t Flag_ATT7022E_Check_Result[ATT7022E_NUM_TOTAL] = {1, 1, 1, 1, 1,};  //�ж����һ�ε�SPIͨ���Ƿ���ȷ
static uint8_t Flag_ATT7022E_Check_Error[ATT7022E_NUM_TOTAL]  = {0, 0, 0, 0, 0,};  //��¼�Ƿ����˴����¼�

//���ڽ����¼�
static uint8_t Flag_A_No_Voltage[ATT7022E_NUM_TOTAL]              = {0};  //A��ʧѹ��־
static uint8_t Count_A_No_Voltage[ATT7022E_NUM_TOTAL]             = {0};  //A��ʧѹ��ʱ
static uint8_t Flag_B_No_Voltage[ATT7022E_NUM_TOTAL]              = {0};  //B��ʧѹ��־
static uint8_t Count_B_No_Voltage[ATT7022E_NUM_TOTAL]             = {0};  //B��ʧѹ��ʱ
static uint8_t Flag_C_No_Voltage[ATT7022E_NUM_TOTAL]              = {0};  //C��ʧѹ��־
static uint8_t Count_C_No_Voltage[ATT7022E_NUM_TOTAL]             = {0};  //C��ʧѹ��ʱ
static uint8_t Flag_A_Low_Voltage[ATT7022E_NUM_TOTAL]             = {0};  //A��Ƿѹ��־
static uint8_t Count_A_Low_Voltage[ATT7022E_NUM_TOTAL]            = {0};  //A��Ƿѹ��ʱ
static uint8_t Flag_B_Low_Voltage[ATT7022E_NUM_TOTAL]             = {0};  //B��Ƿѹ��־
static uint8_t Count_B_Low_Voltage[ATT7022E_NUM_TOTAL]            = {0};  //B��Ƿѹ��ʱ
static uint8_t Flag_C_Low_Voltage[ATT7022E_NUM_TOTAL]             = {0};  //C��Ƿѹ��־
static uint8_t Count_C_Low_Voltage[ATT7022E_NUM_TOTAL]            = {0};  //C��Ƿѹ��ʱ
static uint8_t Flag_A_Over_Voltage[ATT7022E_NUM_TOTAL]            = {0};  //A���ѹ��־
static uint8_t Count_A_Over_Voltage[ATT7022E_NUM_TOTAL]           = {0};  //A���ѹ��ʱ
static uint8_t Flag_B_Over_Voltage[ATT7022E_NUM_TOTAL]            = {0};  //B���ѹ��־
static uint8_t Count_B_Over_Voltage[ATT7022E_NUM_TOTAL]           = {0};  //B���ѹ��ʱ
static uint8_t Flag_C_Over_Voltage[ATT7022E_NUM_TOTAL]            = {0};  //C���ѹ��־
static uint8_t Count_C_Over_Voltage[ATT7022E_NUM_TOTAL]           = {0};  //C���ѹ��ʱ
static uint8_t Flag_A_No_PhSregpq[ATT7022E_NUM_TOTAL]             = {0};  //A������־
static uint8_t Count_A_No_PhSregpq[ATT7022E_NUM_TOTAL]            = {0};  //A������ʱ
static uint8_t Flag_B_No_PhSregpq[ATT7022E_NUM_TOTAL]             = {0};  //B������־
static uint8_t Count_B_No_PhSregpq[ATT7022E_NUM_TOTAL]            = {0};  //B������ʱ
static uint8_t Flag_C_No_PhSregpq[ATT7022E_NUM_TOTAL]             = {0};  //C������־
static uint8_t Count_C_No_PhSregpq[ATT7022E_NUM_TOTAL]            = {0};  //C������ʱ
static uint8_t Flag_A_No_Current[ATT7022E_NUM_TOTAL]              = {0};  //A��ʧ����־
static uint8_t Count_A_No_Current[ATT7022E_NUM_TOTAL]             = {0};  //A��ʧ����ʱ
static uint8_t Flag_B_No_Current[ATT7022E_NUM_TOTAL]              = {0};  //B��ʧ����־
static uint8_t Count_B_No_Current[ATT7022E_NUM_TOTAL]             = {0};  //B��ʧ����ʱ
static uint8_t Flag_C_No_Current[ATT7022E_NUM_TOTAL]              = {0};  //C��ʧ����־
static uint8_t Count_C_No_Current[ATT7022E_NUM_TOTAL]             = {0};  //C��ʧ����ʱ
static uint8_t Flag_A_Over_Current[ATT7022E_NUM_TOTAL]            = {0};  //A�������־
static uint8_t Count_A_Over_Current[ATT7022E_NUM_TOTAL]           = {0};  //A�������ʱ
static uint8_t Flag_B_Over_Current[ATT7022E_NUM_TOTAL]            = {0};  //B�������־
static uint8_t Count_B_Over_Current[ATT7022E_NUM_TOTAL]           = {0};  //B�������ʱ
static uint8_t Flag_C_Over_Current[ATT7022E_NUM_TOTAL]            = {0};  //C�������־
static uint8_t Count_C_Over_Current[ATT7022E_NUM_TOTAL]           = {0};  //C�������ʱ
static uint8_t Flag_ABC_All_No_Voltage[ATT7022E_NUM_TOTAL]        = {0};  //ABC��ȫʧѹ��־
static uint8_t Count_ABC_All_No_Voltage[ATT7022E_NUM_TOTAL]       = {0};  //ABC��ȫʧѹ��ʱ
static uint8_t Flag_ABC_Voltage_PhSregpq_Err[ATT7022E_NUM_TOTAL]  = {0};  //ABC���ѹ�������־
static uint8_t Count_ABC_Voltage_PhSregpq_Err[ATT7022E_NUM_TOTAL] = {0};  //ABC���ѹ�������ʱ
static uint8_t Flag_ABC_Current_PhSregpq_Err[ATT7022E_NUM_TOTAL]  = {0};  //ABC������������־
static uint8_t Count_ABC_Current_PhSregpq_Err[ATT7022E_NUM_TOTAL] = {0};  //ABC������������ʱ
static uint8_t Flag_ABC_Current_UnBalance[ATT7022E_NUM_TOTAL]     = {0};  //ABC�������ƽ���־
static uint8_t Count_ABC_Current_UnBalance[ATT7022E_NUM_TOTAL]    = {0};  //ABC�������ƽ���ʱ
static uint8_t Flag_ABC_Voltage_UnBalance[ATT7022E_NUM_TOTAL]     = {0};  //ABC�������ƽ���־
static uint8_t Count_ABC_Voltage_UnBalance[ATT7022E_NUM_TOTAL]    = {0};  //ABC�������ƽ���ʱ

#ifdef	LTU_TRANSFORMS_RATE
extern Struct_LTU_Transformer      LTU_Transformer_Rate;             //���������
#endif



void ATT7022E_Init(void)
{
	printf("��ʼ��ATT7022E����\r\n");
	
	//����һ�����ڳ�ʼ��SPI����Ľṹ�����
	SPI_InitTypeDef  SPI_InitStructure;
	
	//����һ�����ڳ�ʼ��GPIO���ŵĽṹ�����
	GPIO_InitTypeDef GPIO_InitStructure;

	//ʹ��PORT_B��PORT_C��SPI�����ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC , ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

	//��ʼ��SPI2�����Ӧ�����ţ���ʼ��Ϊ�����������
	GPIO_InitStructure.GPIO_Pin   = ATT7022E_SPI_PINS;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ATT7022E_SPI_PORT, &GPIO_InitStructure);

	//��ʼ��ATT7022E_1��Ƭѡ����
	GPIO_InitStructure.GPIO_Pin   = ATT7022E_SPI_CS1_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ATT7022E_SPI_CS1_PORT, &GPIO_InitStructure);

	//��ʼ��ATT7022E_2��Ƭѡ����
	GPIO_InitStructure.GPIO_Pin   = ATT7022E_SPI_CS2_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ATT7022E_SPI_CS2_PORT, &GPIO_InitStructure);

	//��ʼ��ATT7022E_3��Ƭѡ����
	GPIO_InitStructure.GPIO_Pin   = ATT7022E_SPI_CS3_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ATT7022E_SPI_CS3_PORT, &GPIO_InitStructure);

	//��ʼ��ATT7022E_4��Ƭѡ����
	GPIO_InitStructure.GPIO_Pin   = ATT7022E_SPI_CS4_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ATT7022E_SPI_CS4_PORT, &GPIO_InitStructure);

	//��ʼ��ATT7022E_5��Ƭѡ����
	GPIO_InitStructure.GPIO_Pin   = ATT7022E_SPI_CS5_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ATT7022E_SPI_CS5_PORT, &GPIO_InitStructure);

	//����CS���ţ�������ͨ��
	ATT7022E_SPI_CS0_H();
	ATT7022E_SPI_CS1_H();
	ATT7022E_SPI_CS2_H();
	ATT7022E_SPI_CS3_H();
	ATT7022E_SPI_CS4_H();

	//��ʼ��SPI2����
	SPI_InitStructure.SPI_Direction         = SPI_Direction_2Lines_FullDuplex;  //˫��˫��ȫ˫��
	SPI_InitStructure.SPI_Mode              = SPI_Mode_Master;                  //����ģʽ
	SPI_InitStructure.SPI_DataSize          = SPI_DataSize_8b;                  //8������λ
	SPI_InitStructure.SPI_CPOL              = SPI_CPOL_Low;                     //SCLK����ʱΪ�͵�ƽ
	SPI_InitStructure.SPI_CPHA              = SPI_CPHA_2Edge;                   //SCLK�ĵڶ��������ض�ȡ���ߴ�������
	SPI_InitStructure.SPI_NSS               = SPI_NSS_Soft;                     //�������Ƭѡ����
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;         //SCLK��ʱ��Ƶ����ΪfPCLK��16��Ƶ
	SPI_InitStructure.SPI_FirstBit          = SPI_FirstBit_MSB;                 //��λ�ȴ�
	SPI_InitStructure.SPI_CRCPolynomial     = 7;                                //����У�飬�����
	SPI_Init(SPI2, &SPI_InitStructure);

	//ʹ��SPI2����
	SPI_Cmd(SPI2, ENABLE);
}



static uint8_t SPI2_SendByte(uint8_t byte)
{
	//�ȴ�SPI2����ķ��ͻ�����Ϊ��
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);

	//ͨ��SPI2��������ݷ��ͳ�ȥ
	SPI_I2S_SendData(SPI2, byte);

	//�ȴ�SPI2����Ľ��ջ�������Ϊ�գ����ȴ�SPI2������յ���1���ֽ����ݣ�
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
	
	//����SPI2������յ�������
	return SPI_I2S_ReceiveData(SPI2);
}



static void ATT7022E_SPI_CS_Selete(uint8_t Num_CS, uint8_t Level)
{
	switch(Num_CS)
	{
		case 0:
		if(Level)
		{
			ATT7022E_SPI_CS0_H();
		}
		else
		{
			ATT7022E_SPI_CS0_L();
		}
		break;
		
		case 1:
		if(Level)
		{
			ATT7022E_SPI_CS1_H();
		}
		else
		{
			ATT7022E_SPI_CS1_L();
		}
		break;
		
		case 2:
		if(Level)
		{
			ATT7022E_SPI_CS2_H();
		}
		else
		{
			ATT7022E_SPI_CS2_L();
		}
		break;
		
		case 3:
		if(Level)
		{
			ATT7022E_SPI_CS3_H();
		}
		else
		{
			ATT7022E_SPI_CS3_L();
		}
		break;
		
		case 4:
		if(Level)
		{
			ATT7022E_SPI_CS4_H();
		}
		else
		{
			ATT7022E_SPI_CS4_L();
		}
		break;
	}
	
	Delay_us(5);
}



static uint32  ATT7022E_Read3Bytes(uint8_t SPIADDR)
{
	uint32 Data_24bit  = 0;
	uint32 Data_8bit_H = 0;
	uint32 Data_8bit_M = 0;
	uint32 Data_8bit_L = 0;

	//������Ҫ��ȡ���ݵĵ�ַ��ȥ
	SPI2_SendByte(SPIADDR);

	//��ȡ���ֽ�����
	//��Ϊ˫��˫��ȫ˫��ģʽ�����͵�ͬʱҲ�ܽ������ݣ��������﷢��һ�����������ݣ������ڷ��͹����н�������
	Data_8bit_H = SPI2_SendByte(Dummy_Byte);

	//��ȡ���ֽ�����
	Data_8bit_M = SPI2_SendByte(Dummy_Byte);

	//��ȡ���ֽ�����
	Data_8bit_L = SPI2_SendByte(Dummy_Byte);

	//�ѽ��յ���������ϳ�һ����������
	Data_24bit = (Data_8bit_H << 16) | (Data_8bit_M << 8) | Data_8bit_L;

	return Data_24bit;
}



static void ATT7022E_Write3Bytes(uint8_t SPIADDR, uint32 SPIDATA)
{
	//����д��ļĴ�����ַ
	SPI2_SendByte(SPIADDR | 0x80);

	//���͸�8λ
	SPI2_SendByte((SPIDATA & 0xFF0000) >> 16);
	//������8λ
	SPI2_SendByte((SPIDATA & 0xFF00) >> 8);
	//���͵�8λ
	SPI2_SendByte(SPIDATA & 0xFF);
}



uint32 ATT7022E_ReadID(void)
{
	uint32 Data_24bit  = 0;
	uint32 Data_8bit_H = 0;
	uint32 Data_8bit_M = 0;
	uint32 Data_8bit_L = 0;

	//����ID�Ĵ����ĵ�ַ
	SPI2_SendByte(0x00);

	//��ȡ��8λ
	Data_8bit_H = SPI2_SendByte(Dummy_Byte);

	//��ȡ��8λ
	Data_8bit_M = SPI2_SendByte(Dummy_Byte);

	//��ȡ��8λ
	Data_8bit_L = SPI2_SendByte(Dummy_Byte);

	//�������
	Data_24bit = (Data_8bit_H << 16) | (Data_8bit_M << 8) | Data_8bit_L;

	return Data_24bit;
}



//У������ͨ�ŵĿɿ���
static bool ATT7022E_Check_Result(uint8_t Num_ATT7022E)
{
	uint32 Reg_BackUp            = 0;  //��¼���ݼĴ����������
	uint8_t  CheckSum_Reg_BackUp   = 0;  //��¼���ݼĴ���������ݵ�У���
	uint32 Reg_CheckSum          = 0;  //��¼ͨ��У��ͼĴ����������
	uint8_t  CheckSum_Reg_CheckSum = 0;  //��¼ͨ��У��ͼĴ�����ݵ�У���
	uint32 Reg_Device            = 0;  //��¼�豸�Ĵ�����ֵ
	
	//��ATT7022E�ı��ݼĴ������л�ȡ����һ��SPIͨ�ŵ�����
	ATT7022E_SPI_CS_Selete(Num_ATT7022E, 0);
	Reg_BackUp = ATT7022E_Read3Bytes(0x2D);
	ATT7022E_SPI_CS_Selete(Num_ATT7022E, 1);
	
	//������ֽڡ����ֽڡ����ֽڵ�У���
	CheckSum_Reg_BackUp = ((Reg_BackUp & 0xFF0000) >> 16) + ((Reg_BackUp & 0x00FF00) >> 8) + ((Reg_BackUp & 0x0000FF) >> 0);

	//��ATT7022E��ͨѶУ��ͼĴ����л�ȡ������
	ATT7022E_SPI_CS_Selete(Num_ATT7022E, 0);
	Reg_CheckSum = ATT7022E_Read3Bytes(0x2E);
	ATT7022E_SPI_CS_Selete(Num_ATT7022E, 1);
	
	//ע: ÿһ�ε�SPIͨ�ţ�ATT7022E����������ֽڣ��Ĵ�����ַ�ֽڣ������ݵĸ��ֽڡ����ֽڡ����ֽڵ��ĸ��ֽ���ӣ����ͨ��У��ͼĴ�������
	//ͨ��У��ͼĴ����ĸ��ֽڴ洢������ֽڡ����ֽڹ�ͬ�洢У���
	//�˴����ǰ�ͨ��У��ͼĴ�����ֵ��ȥ�����ֽڵ�ֵ���ó�����У��͵�ֵ
	CheckSum_Reg_CheckSum = (Reg_CheckSum & 0x00FFFF) - ((Reg_CheckSum & 0xFF0000) >> 16);

	//������������У��ͺ�ͨ��У��ͼĴ������ֵ����ȣ�˵��ͨ�ŷ����˴���
	if (CheckSum_Reg_CheckSum != CheckSum_Reg_BackUp)
	{
		printf("ATT7022E: SPIͨ��У��ͷ�������\r\n");
		
		//��ȡ��0x00��ַ�ļĴ�������
		ATT7022E_SPI_CS_Selete(Num_ATT7022E, 0);
		Reg_Device = ATT7022E_Read3Bytes(0x00);
		ATT7022E_SPI_CS_Selete(Num_ATT7022E, 1);
		
		//�жϵ�ǰATT7022E���ں���״̬
		switch (Reg_Device)
		{
			//��ǰ��У��ģʽ��У��ģʽ����Ҫ����У��
			case 0x00AAAA:
			printf("У��ģʽ ����У��\r\n");
			return true;
			
			//��ǰ�Ǽ���ģʽ��ͨ��ȷʵ�����˴���
			case 0x7122A0:
			printf("����ģʽ У�����\r\n");
			return false;
			
			//����
			default:
			printf("��ȡ 0x00 �Ĵ�������\r\n");
			return false;
		}
	}
	
	return true;
};



void ATT7022E_Write3Bytes_CheckCRC(uint8_t Num_ATT7022E, uint8_t ADDR, uint32 Data_24bit)
{
	while (1)
	{
		//������д���Ӧ�ļĴ�������
		ATT7022E_SPI_CS_Selete(Num_ATT7022E, 0);
		ATT7022E_Write3Bytes(ADDR, Data_24bit);
		ATT7022E_SPI_CS_Selete(Num_ATT7022E, 1);
		
		//�ж�ͨ���Ƿ���ȷ�����ͨ����ȷ��������ѭ�����������ȷ��10ms��ʱ�Ժ����ͨ��
		if (ATT7022E_Check_Result(Num_ATT7022E))
		{
			break;
		}
		else
		{
			printf("��ATT7022E�ĵ�ַ0x%2xд��0x%6xʱ��������д��\r\n", ADDR, Data_24bit);
			Delay_ms(2);
		}
	}
}



int  ATT7022E_Read3Bytes_CheckCRC(uint8_t Num_ATT7022E, uint8_t addr)
{
	//��ȡ��Ӧ��ַ���������
	ATT7022E_SPI_CS_Selete(Num_ATT7022E, 0);
	int Data = ATT7022E_Read3Bytes(addr);
	ATT7022E_SPI_CS_Selete(Num_ATT7022E, 1);
	
	//���ͨ�Ž���Ƿ������������ȷ����Ϊ1���������������ô��¼�´˴�ͨ�Ž����ȷ��־Ϊ0
	if (ATT7022E_Check_Result(Num_ATT7022E))
	{
		ATT7022E_Check_Result_Set(Num_ATT7022E, 1);
	}
	else
	{
		ATT7022E_Check_Result_Set(Num_ATT7022E, 0);
	}
	
	return Data;
};



//��¼�����һ��SPIͨ�Ž���Ƿ���ȷ
void ATT7022E_Check_Result_Set(uint8_t Num_ATT7022E, uint8_t Flag)
{
	Flag_ATT7022E_Check_Result[Num_ATT7022E] = Flag;
}



//��ȡ���һ��SPIͨ�Ž���Ƿ���ȷ
uint8_t_t ATT7022E_Check_Result_Get(uint8_t Num_ATT7022E)
{
	return Flag_ATT7022E_Check_Result[Num_ATT7022E];
}



//��¼�Ƿ����˴����¼�
void ATT7022E_Check_Error_Set(uint8_t Num_ATT7022E, uint8_t Flag)
{
	Flag_ATT7022E_Check_Error[Num_ATT7022E] = Flag;
}



//��ȡ�Ƿ����˴����¼�
uint8_t_t ATT7022E_Check_Error_Get(uint8_t Num_ATT7022E)
{
	return Flag_ATT7022E_Check_Error[Num_ATT7022E];
}



//��λ
void ATT7022E_Reset_Soft(uint8_t Num_ATT7022E)
{
	//��0xD3��д��0x000000���ܹ���ATT7022E������λ����
	ATT7022E_SPI_CS_Selete(Num_ATT7022E, 0);
	ATT7022E_Write3Bytes(0xD3, 0x000000);
	ATT7022E_SPI_CS_Selete(Num_ATT7022E, 1);
	
	vTaskDelay(1000);
};



//��λУ׼�Ĵ���
void ATT7022E_Clear_AdjustParam(uint8_t Num_ATT7022E)
{
	//��0xC3��ַд��0x000000�����Ը�λ����У׼�Ĵ����ڵ�ֵΪ�ϵ�״̬
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, 0xC3, 0x000000);
}



//�����Ƿ��ܹ�д��У�����
void ATT7022E_Write_AdjustParam_Set(uint8_t Num_ATT7022E, uint8_t Flag)
{
	if(Flag == 0)
	{
		//��0xC9��ַд��*��0x00005A*���ݣ����Խ�ֹд��У׼�Ĵ���
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, 0xC9, 0x000000);
	}
	else
	{
		//��0xC9��ַд��0x00005A������ʹ��д��У׼�Ĵ���
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, 0xC9, 0x00005A);
	}
}



//�����Ƿ��ȡУ׼�Ĵ������ݣ�1: ��ȡУ׼�Ĵ�������  0: ��ȡ�����Ĵ������ݣ�
void ATT7022E_Read_AdjustParam_Set(uint8_t Num_ATT7022E, uint8_t Flag)
{
	if(Flag == 0)
	{
		//��0xC6��ַ��д��*��0x00005A*���ݣ���ôѡ���ȡ�����Ĵ���������
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, 0xC6, 0x000000);
	}
	else
	{
		//��0xC6��ַ��д��0x00005A����ôѡ���ȡУ׼�Ĵ���������
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, 0xC6, 0x00005A);
	}
}



//����HFconstֵ������INT������
int  ATT7022E_Calculate_HFconst(int _EC, float _Un, float _Ib, float _Vu, float Vi)
{
	int _HFconst = 0;
	
	_HFconst = (int)((2.592 * pow(10, 10) * 1.163 * 1.163 * _Vu * Vi) / (_EC * _Un * _Ib));
	printf("��Ƶ���峣��: %d\r\n", _HFconst);
	
	return _HFconst;
};



int ATT7022E_Calculate_Istartup(float Ib, float N, float Rate)
{
	float Io       = 0.0;
	int   Istartup = 0.0;
	
	Io       = Ib * N * Rate;
	Istartup = (int)(0.8 * Io * 8192);  //2^13 = 8192
	printf("��ʼ����: %d\r\n", Istartup);
	
	return Istartup;
}



int ATT7022E_Calculate_Ufail(float Un, float D)
{
	int Ufail = 0.0;
	
	Ufail = (int)(Un * 32 * D);  //2^5 = 32
	printf("ʧѹ��ֵ: %d\r\n", Ufail);
	
	return (int)Ufail;
}



//�����ѹ��ЧֵUgain������INT
int  ATT7022E_Calculate_Ugain(uint8_t Num_ATT7022E, float Ur, uint8_t_t Ugain_ABC)
{
	int     DataU        = 0.0;  //��¼��ȡ���ĵ�ѹ��Чֵ
	float   Urms_Old     = 0.0;  //��¼��һ�β⵽�ĵ�ѹ��Чֵ
	float   Urms_Now     = 0.0;  //��¼��ǰ�⵽�ĵ�ѹ��Чֵ
	float   Urms_Average = 0.0;  //ƽ����Ч��ѹֵ
	int     Num_Urms     = 0;    //��ȡ���ĵ�ѹ��Чֵ�ĸ���
	float   Sum_Urms     = 0.0;  //��¼��ѹ��Чֵ�ĺ�
	float   Ugain        = 0.0;  //��ѹ����У׼�Ĵ�����ֵ
	uint8_t ADDR_Ugain   = 0;    //��¼��ѹ����У׼��ַ��A��B��C�����ѹ�������һ�ࣩ
	
	switch(Ugain_ABC)
	{
		//A���ѹ����У׼
		case ATT7022E_ADJUST_Ugain_A:
		ADDR_Ugain = 0x0D;
		break;
		
		case ATT7022E_ADJUST_Ugain_B:
		ADDR_Ugain = 0x0E;
		break;
		
		case ATT7022E_ADJUST_Ugain_C:
		ADDR_Ugain = 0x0F;
		break;
		
		default:
		printf("���δ���\r\n");
		return 0;
		break;
	}
	
	printf("��ѹ����У׼��ʼ......\r\n");
		
	printf("��ѹ��Чֵ: ");
	while (1)
	{
		//��ȡ����ǰ�������ĵ�ѹ��Чֵ
		DataU = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, ADDR_Ugain);
		if (DataU & 0x00800000L)
		{
			DataU |= 0xff000000L;
		}
		Urms_Now = (float)DataU / 8192; //2^13 = 8192
		printf("%f ", Urms_Now);
		
		//�����βɼ����ĵ�ѹ��Чֵ����һ�εĵ�ѹ��Чֵ��ȣ�������׼��ѹֵ��1%��˵�����̫������Ƿ����˵�ѹ���������㲢���²ɼ�
		if (abs((int)(Urms_Now - Urms_Old)) > (Ur * 0.01))
		{
			//���µ�ѹ��Чֵ�������´ζԱ�
			Urms_Old = Urms_Now;
			
			//�ɼ����ĵ�ѹ��Чֵ�ĸ������㣬��ѹ��Чֵ�ĺ�����
			Num_Urms = 0;
			Sum_Urms = 0;
		}
		
		//����ɼ����ĵ�ѹ��Чֵ�ĺͣ����ڼ���ƽ����ѹ��Чֵ
		Sum_Urms += Urms_Now;
		
		//���㵱ǰһ���ɼ����˶��ٸ���ѹ��Чֵ�����ڼ���ƽ����ѹ��Чֵ
		Num_Urms++;
		if (Num_Urms >= 10)
		{
			break;
		}
		
		//��ʱ
		vTaskDelay(125);
	}
	printf("\r\n");
	
	//����ɼ����ĵ�ѹֵ��ƽ����
	Urms_Average = Sum_Urms / Num_Urms;
	printf("ƽ����ѹ��Чֵ��%f\r\n", Urms_Average);
	
	//���ݹ�ʽ�����ѹ����У׼�Ĵ�����ֵ
	Ugain = Ur / Urms_Average - 1;
	if (Ugain >= 0)
	{
		Ugain = (Ugain * pow(2, 15));
	}
	else
	{
		Ugain = (pow(2, 16) + Ugain * pow(2, 15));
	}
	printf("��ѹ����У׼ֵ: %f %u\r\n", Ugain, (int)Ugain);
		
	printf("��ѹ����У׼����\r\n");
		
	return (int)Ugain;
};



//���������ЧֵIgain������INT
int  ATT7022E_Calculate_Igain(uint8_t Num_ATT7022E, float Ir, float N, uint8_t_t Igain_ABC)
{
	int     DataI        = 0;    //ͨ��SPI��ȡ���ĵ��������Ĵ�����ֵ
	float   Irms_Now     = 0.0;  //��¼��ǰ�ɼ����ĵ�����Чֵ
	float   Irms_Old     = 0.0;  //��¼�ϴβɼ����ĵ�����Чֵ
	float   Irms_Average = 0.0;  //��¼ƽ��������Чֵ
	int     Num_Irms     = 0;    //��¼һ���ɼ����˶��ٸ�������Чֵ
	float   Sum_Irms     = 0.0;  //��¼�ɼ����ĵ�����Чֵ�ĺͣ����ڼ���ƽ����
	float   Igain        = 0.0;  //��¼
	uint8_t ADDR_Igain   = 0;    //��¼��������Ĵ����ĵ�ַ
	
	switch(Igain_ABC)
	{
		case ATT7022E_ADJUST_Igain_A:
		ADDR_Igain = 0x10;
		break;
		
		case ATT7022E_ADJUST_Igain_B:
		ADDR_Igain = 0x11;
		break;
		
		case ATT7022E_ADJUST_Igain_C:
		ADDR_Igain = 0x12;
		break;
		
		default:
		printf("���δ���\r\n");
		return 0;
		break;
	}
	
	printf("��������У׼��ʼ\r\n");
		
	printf("������Чֵ: ");
	while (1)
	{
		//��ȡ��ǰ�ɼ����ĵ�����Чֵ
		DataI = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, ADDR_Igain);
		if (DataI & 0x00800000L)
		{
			DataI |= 0xff000000L;
		}
		Irms_Now = (float)DataI / 8192 / N;  //2^13 = 8192
		printf("%f ", Irms_Now);
		
		//�����ǰ������Чֵ���ϴε�����Чֵ��ȣ���೬���������1%����ô��Ϊ����������仯���߷���������������Ҫ���¿�ʼ��ȡ
		if (abs((int)(Irms_Now - Irms_Old)) > (Ir * 0.01))
		{
			//���µ�����Чֵ
			Irms_Old = Irms_Now;
			
			//�����ȡ���ĵ�����Чֵ������������Чֵ�ĺ�
			Num_Irms = 0;
			Sum_Irms = 0;
		}
		
		//���㵱ǰ�ɼ����ĵ�����Чֵ�ĺ�
		Sum_Irms += Irms_Now;
		
		//���㵱ǰ�ɼ����ĵ�����Чֵ�ĸ�����������ڵ����趨�ĸ�������ô������ѭ��
		Num_Irms++;
		if (Num_Irms >= 10)
		{
			break;
		}
		vTaskDelay(125);
	}
	printf("\r\n");

	//����ƽ��������Чֵ
	Irms_Average = Sum_Irms / Num_Irms;
	printf("ƽ��������Чֵ: %f\r\n", Irms_Average);
	
	//�����������У׼
	Igain = Ir / Irms_Average - 1;
	if (Igain >= 0)
	{
		Igain = (Igain * pow(2, 15));
	}
	else
	{
		Igain = ((1 << 16) + Igain * pow(2, 15));
	}
	printf("��������У׼ֵ: %u\r\n", (int)Igain);
	
	printf("��������У׼����");
	
	return (int)Igain;
}



void ATT7022E_Calculate_PQSgain_PhSregpq_1(uint8_t Num_ATT7022E, float P_Real, float Q_Real, int HFconst, int _EC, uint8_t_t PQgain_PhSregpq_ABC)
{
	extern Struct_LTU_ATT7022E_Param_Reg  LTU_ATT7022E_AdjustParam_Reg;
	
	uint8_t ADDR_P     = 0;    //�й����ʼĴ���
	uint8_t ADDR_Q     = 0;    //�޹����ʼĴ���
	int32_t DataP      = 0;    //��¼���й����ʼĴ����ж�ȡ����ԭʼֵ
	int32_t DataQ      = 0;    //��¼���޹����ʼĴ����ж�ȡ����ԭʼֵ
	float   P_Now      = 0.0;  //��¼��ǰ���������й�����ֵ����λ: ����W��
	float   Q_Now      = 0.0;  //��¼��ǰ���������޹�����ֵ����λ: ����W��
	float   P_Old      = 0.0;  //��¼�ϴβ��������й�����ֵ����λ: ����W��
	float   Q_Old      = 0.0;  //��¼�ϴβ��������޹�����ֵ����λ: ����W��
	float   Sum_P      = 0.0;  //��¼�й����ʵĺ�
	float   Sum_Q      = 0.0;  //��¼�޹����ʵĺ�
	int32_t Num_PQ     = 0;    //��¼�ɼ����˶��ٸ��й������޹�����
	float   K          = 0.0;  //���ʲ���������ο�ATT7022E�����ֲᣩ
	float   P_Average  = 0.0;  //��¼ƽ���й����ʣ���λ: ����W��
	float   Q_Average  = 0.0;  //��¼ƽ���޹����ʣ���λ: ����W��
	float   S_Real     = 0.0;  //��׼���ڹ���
	float   S_Average  = 0.0;  //������ڹ���
	float   Pf_Real    = 0.0;  //��׼��������
	float   Pf_Average = 0.0;  //��ù�������
	float   P_Err      = 0.0;  //����й����ʺͱ�׼�й�����֮������
	float   Q_Err      = 0.0;  //����޹����ʺͱ�׼�޹�����֮������
	float   Angle_Err  = 0.0;  //�ǲ�
	int     PhSregpq   = 0;    //�ǲ�У��ֵ
	float   PP         = 0.0;  //��¼У���ǲ��Ĺ���ֵ
	float   Pgain      = 0.0;  //�й���������У׼ֵ
	int     Pgain_Int  = 0;    //��¼��Ҫд�빦������Ĵ��������ֵ
	
	switch(PQgain_PhSregpq_ABC)
	{
		//У׼A���й��������桢�޹���������
		case ATT7022E_ADJUST_PQSgain_PhSregpq_A:
		ADDR_P = 0x01;
		ADDR_Q = 0x05;
		break;
		
		//У׼B���й��������桢�޹���������
		case ATT7022E_ADJUST_PQSgain_PhSregpq_B:
		ADDR_P = 0x02;
		ADDR_Q = 0x06;
		break;
		
		//У׼C���й��������桢�޹���������
		case ATT7022E_ADJUST_PQSgain_PhSregpq_C:
		ADDR_P = 0x03;
		ADDR_Q = 0x07;
		break;
	}
	
	printf("��������У����ʼ ...\r\n");
	
	//���㹦�ʲ���K
	K = 2.592f * pow(10, 10) / (HFconst * _EC * pow(2, 23));
	printf("���ʲ���: %f\r\n", K);
	
	while (1)
	{
		//�ӼĴ����ж�ȡ����ǰ���������й����ʣ��ٳ���Kֵת��������(W)Ϊ��λ���й�������Чֵ
		DataP = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, ADDR_P);
		if(DataP > pow(2, 23))
		{
			DataP -= pow(2, 24);
		}
		P_Now = DataP * K;
		printf("�й�����: %f\r\n", P_Now);
		
		//�ӼĴ����ж�ȡ����ǰ���������޹����ʣ��ٳ���Kֵ�����޹�����ת��������(W)Ϊ��λ���޹�������Чֵ
		DataQ = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, ADDR_Q);
		if(DataQ > pow(2, 23))
		{
			DataQ -= pow(2, 24);
		}
		Q_Now = DataQ * K * 1.0005;
		printf("�޹�����: %f\r\n", Q_Now);
				
		//�жϵ�ǰ��ù��ʺ��ϴι�����ȣ��仯�Ƿ񳬹�1%
		if ((abs((int)(P_Now - P_Old)) > abs((int)(P_Real * 0.01))) || (abs((int)(Q_Now - Q_Old)) > abs((int)(Q_Real * 0.01))))
		{
			//�����й����ʡ��޹�����
			P_Old = P_Now;
			Q_Old = Q_Now;
			
			//����⵽���й������޹����ʵĸ������й����ʺ͡��޹����ʺ�
			Num_PQ = 0;
			Sum_P  = 0;
			Sum_Q  = 0;
		}
		
		//�����й����ʺ͡��޹����ʺ͡��й������޹����ʸ������������㹦�ʵ�ƽ��ֵ
		Sum_P += P_Now;
		Sum_Q += Q_Now;
		Num_PQ++;
		
		//����⵽���й������޹����ʸ�������10������ô������ѭ��
		if (Num_PQ >= 10)
		{
			break;
		}
		
		vTaskDelay(125);
	}
	
	//�����й����ʡ��޹����ʵ�ƽ������ֵ
	P_Average = Sum_P / Num_PQ;
	Q_Average = Sum_Q / Num_PQ;
	printf("ƽ���й�����: %fW\r\n",   P_Average);
	printf("ƽ���޹�����: %fVar\r\n", Q_Average);
	
	//�������й����ʺͱ�׼�й�����֮������
	P_Err = (P_Average - P_Real) / P_Real;
	Q_Err = (Q_Average - Q_Real) / Q_Real;
	printf("�й��������: %f\r\n", P_Err);
	printf("�޹��������: %f\r\n", Q_Err);
	
	//�������ڹ���ֵ
	S_Real    = sqrt((P_Real * P_Real) + (Q_Real * Q_Real));
	S_Average = sqrt((P_Average * P_Average) + (Q_Average * Q_Average));
	printf("��׼���ڹ���: %fW\r\n",   S_Real);
	printf("������ڹ���: %fVar\r\n", S_Average);
	
	//�����׼��������������
	Pf_Real    = P_Real / S_Real;
	Pf_Average = P_Average / S_Average;
	printf("��׼��������: %f\r\n", Pf_Real);
	printf("��ù�������: %f\r\n", Pf_Average);
	
	//����ǲ�У��ֵ
	Angle_Err = ((P_Real * Q_Average) - (P_Average * Q_Real)) / ((P_Average * P_Real) + (Q_Average * Q_Real));
	if(Angle_Err >= 0)
	{
		PhSregpq = (int)(Angle_Err * pow(2, 15));
	}
	else
	{
		PhSregpq = (int)(pow(2, 16) + (Angle_Err * pow(2, 15)));
	}
	printf("�ǲ�: %f\r\n",       Angle_Err);
	printf("�ǲ�У��ֵ: %d\r\n", PhSregpq);
	
	//����У���ǲ��Ĺ���ֵ
	PP = P_Average + (Q_Average * Angle_Err);
	
	printf("����У����\r\n");
	Pgain = P_Real / PP - 1;
	if(Pgain >= 0)
	{
		Pgain_Int = (int)(Pgain * pow(2, 15));
	}
	else
	{
		Pgain_Int = (int)(pow(2, 16) + (Pgain * pow(2, 15)));
	}
	printf("�й���������У��ԭʼֵ:   %f\r\n", Pgain);
	printf("�й���������У���Ĵ���ֵ: %d  0x%x\r\n", Pgain_Int, Pgain_Int);
	
	switch(PQgain_PhSregpq_ABC)
	{
		//У׼A���й��������桢�޹���������
		case ATT7022E_ADJUST_PQSgain_PhSregpq_A:
		LTU_ATT7022E_AdjustParam_Reg.Pga  = Pgain_Int;
		LTU_ATT7022E_AdjustParam_Reg.Qga  = Pgain_Int;
		LTU_ATT7022E_AdjustParam_Reg.Sga  = Pgain_Int;
		LTU_ATT7022E_AdjustParam_Reg.Pha0 = PhSregpq;
		LTU_ATT7022E_AdjustParam_Reg.Pha1 = PhSregpq;
		break;
		
		//У׼B���й��������桢�޹���������
		case ATT7022E_ADJUST_PQSgain_PhSregpq_B:
		LTU_ATT7022E_AdjustParam_Reg.Pgb  = Pgain_Int;
		LTU_ATT7022E_AdjustParam_Reg.Qgb  = Pgain_Int;
		LTU_ATT7022E_AdjustParam_Reg.Sgb  = Pgain_Int;
		LTU_ATT7022E_AdjustParam_Reg.Phb0 = PhSregpq;
		LTU_ATT7022E_AdjustParam_Reg.Phb1 = PhSregpq;
		break;
		
		//У׼C���й��������桢�޹���������
		case ATT7022E_ADJUST_PQSgain_PhSregpq_C:
		LTU_ATT7022E_AdjustParam_Reg.Pgc  = Pgain_Int;
		LTU_ATT7022E_AdjustParam_Reg.Qgc  = Pgain_Int;
		LTU_ATT7022E_AdjustParam_Reg.Sgc  = Pgain_Int;
		LTU_ATT7022E_AdjustParam_Reg.Phc0 = PhSregpq;
		LTU_ATT7022E_AdjustParam_Reg.Phc1 = PhSregpq;
		break;
	}
}



void ATT7022E_Calculate_PQSgain_PhSregpq_2(uint8_t Num_ATT7022E, float P_Real, float Q_Real, int HFconst, int _EC, uint8_t_t PQgain_PhSregpq_ABC)
{
	extern Struct_LTU_ATT7022E_Param_Reg  LTU_ATT7022E_AdjustParam_Reg;
	
	uint8_t ADDR_P     = 0;    //�й����ʼĴ���
	uint8_t ADDR_Q     = 0;    //�޹����ʼĴ���
	int32_t DataP      = 0;    //��¼���й����ʼĴ����ж�ȡ����ԭʼֵ
	int32_t DataQ      = 0;    //��¼���޹����ʼĴ����ж�ȡ����ԭʼֵ
	float   P_Now      = 0.0;  //��¼��ǰ���������й�����ֵ����λ: ����W��
	float   Q_Now      = 0.0;  //��¼��ǰ���������޹�����ֵ����λ: ����W��
	float   P_Old      = 0.0;  //��¼�ϴβ��������й�����ֵ����λ: ����W��
	float   Q_Old      = 0.0;  //��¼�ϴβ��������޹�����ֵ����λ: ����W��
	float   Sum_P      = 0.0;  //��¼�й����ʵĺ�
	float   Sum_Q      = 0.0;  //��¼�޹����ʵĺ�
	int32_t Num_PQ     = 0;    //��¼�ɼ����˶��ٸ��й������޹�����
	float   K          = 0.0;  //���ʲ���������ο�ATT7022E�����ֲᣩ
	float   P_Average  = 0.0;  //��¼ƽ���й����ʣ���λ: ����W��
	float   Q_Average  = 0.0;  //��¼ƽ���޹����ʣ���λ: ����W��
	float   S_Real     = 0.0;  //��׼���ڹ���
	float   S_Average  = 0.0;  //������ڹ���
	float   Pf_Real    = 0.0;  //��׼��������
	float   Pf_Average = 0.0;  //��ù�������
	float   P_Err      = 0.0;  //����й����ʺͱ�׼�й�����֮������
	float   Q_Err      = 0.0;  //����޹����ʺͱ�׼�޹�����֮������
	float   Angle_Err  = 0.0;  //�ǲ�
	int     PhSregpq   = 0;    //�ǲ�У��ֵ
	float   Pgain      = 0.0;  //�й���������У׼ֵ
	int     Pgain_Int  = 0;    //��¼��Ҫд�빦������Ĵ��������ֵ
	
	switch(PQgain_PhSregpq_ABC)
	{
		//У׼A���й��������桢�޹���������
		case ATT7022E_ADJUST_PQSgain_PhSregpq_A:
		ADDR_P = 0x01;
		ADDR_Q = 0x05;
		break;
		
		//У׼B���й��������桢�޹���������
		case ATT7022E_ADJUST_PQSgain_PhSregpq_B:
		ADDR_P = 0x02;
		ADDR_Q = 0x06;
		break;
		
		//У׼C���й��������桢�޹���������
		case ATT7022E_ADJUST_PQSgain_PhSregpq_C:
		ADDR_P = 0x03;
		ADDR_Q = 0x07;
		break;
	}
	
	printf("��������У����ʼ ...\r\n");
	
	//���㹦�ʲ���K
	K = 2.592f * pow(10, 10) / (HFconst * _EC * pow(2, 23));
	printf("���ʲ���: %f\r\n", K);
	
	while (1)
	{
		//�ӼĴ����ж�ȡ����ǰ���������й����ʣ��ٳ���Kֵת��������(W)Ϊ��λ���й�������Чֵ
		DataP = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, ADDR_P);
		if(DataP > pow(2, 23))
		{
			DataP -= pow(2, 24);
		}
		P_Now = DataP * K;
		printf("�й�����: %f\r\n", P_Now);
		
		//�ӼĴ����ж�ȡ����ǰ���������޹����ʣ��ٳ���Kֵ�����޹�����ת��������(W)Ϊ��λ���޹�������Чֵ
		DataQ = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, ADDR_Q);
		if(DataQ > pow(2, 23))
		{
			DataQ -= pow(2, 24);
		}
		Q_Now = DataQ * K * 1.0005;
		printf("�޹�����: %f\r\n", Q_Now);
				
		//�жϵ�ǰ��ù��ʺ��ϴι�����ȣ��仯�Ƿ񳬹�1%
		if ((abs((int)(P_Now - P_Old)) > abs((int)(P_Real * 0.01))) || (abs((int)(Q_Now - Q_Old)) > abs((int)(Q_Real * 0.01))))
		{
			//�����й����ʡ��޹�����
			P_Old = P_Now;
			Q_Old = Q_Now;
			
			//����⵽���й������޹����ʵĸ������й����ʺ͡��޹����ʺ�
			Num_PQ = 0;
			Sum_P  = 0;
			Sum_Q  = 0;
		}
		
		//�����й����ʺ͡��޹����ʺ͡��й������޹����ʸ������������㹦�ʵ�ƽ��ֵ
		Sum_P += P_Now;
		Sum_Q += Q_Now;
		Num_PQ++;
		
		//����⵽���й������޹����ʸ�������10������ô������ѭ��
		if (Num_PQ >= 10)
		{
			break;
		}
		
		vTaskDelay(125);
	}
	
	//�����й����ʡ��޹����ʵ�ƽ������ֵ
	P_Average = Sum_P / Num_PQ;
	Q_Average = Sum_Q / Num_PQ;
	printf("ƽ���й�����: %fW\r\n",   P_Average);
	printf("ƽ���޹�����: %fVar\r\n", Q_Average);
	
	//�������й����ʺͱ�׼�й�����֮������
	P_Err = (P_Average - P_Real) / P_Real;
	Q_Err = (Q_Average - Q_Real) / Q_Real;
	printf("�й��������: %f\r\n", P_Err);
	printf("�޹��������: %f\r\n", Q_Err);
	
	//�������ڹ���ֵ
	S_Real    = sqrt((P_Real * P_Real) + (Q_Real * Q_Real));
	S_Average = sqrt((P_Average * P_Average) + (Q_Average * Q_Average));
	printf("��׼���ڹ���: %fW\r\n",   S_Real);
	printf("������ڹ���: %fVar\r\n", S_Average);
	
	//�����׼��������������
	Pf_Real    = P_Real / S_Real;
	Pf_Average = P_Average / S_Average;
	printf("��׼��������: %f\r\n", Pf_Real);
	printf("��ù�������: %f\r\n", Pf_Average);
	
	//����ǲ�У��ֵ
	Angle_Err = (Pf_Average - Pf_Real) / Pf_Real;
	if(Angle_Err >= 0)
	{
		PhSregpq = (int)(Angle_Err * pow(2, 15));
	}
	else
	{
		PhSregpq = (int)(pow(2, 16) + (Angle_Err * pow(2, 15)));
	}
	printf("�ǲ�: %f\r\n",       Angle_Err);
	printf("�ǲ�У��ֵ: %d\r\n", PhSregpq);
	
	//���㹦������У��ֵ
	Pgain = S_Real / S_Average - 1;
	if(Pgain >= 0)
	{
		Pgain_Int = (int)(Pgain * pow(2, 15));
	}
	else
	{
		Pgain_Int = (int)(pow(2, 16) + (Pgain * pow(2, 15)));
	}
	printf("�й���������У��ԭʼֵ:   %f\r\n", Pgain);
	printf("�й���������У���Ĵ���ֵ: %d  0x%x\r\n", Pgain_Int, Pgain_Int);
	
	switch(PQgain_PhSregpq_ABC)
	{
		//У׼A���й��������桢�޹���������
		case ATT7022E_ADJUST_PQSgain_PhSregpq_A:
		LTU_ATT7022E_AdjustParam_Reg.Pga  = Pgain_Int;
		LTU_ATT7022E_AdjustParam_Reg.Qga  = Pgain_Int;
		LTU_ATT7022E_AdjustParam_Reg.Sga  = Pgain_Int;
		LTU_ATT7022E_AdjustParam_Reg.Pha0 = PhSregpq;
		LTU_ATT7022E_AdjustParam_Reg.Pha1 = PhSregpq;
		break;
		
		//У׼B���й��������桢�޹���������
		case ATT7022E_ADJUST_PQSgain_PhSregpq_B:
		LTU_ATT7022E_AdjustParam_Reg.Pgb  = Pgain_Int;
		LTU_ATT7022E_AdjustParam_Reg.Qgb  = Pgain_Int;
		LTU_ATT7022E_AdjustParam_Reg.Sgb  = Pgain_Int;
		LTU_ATT7022E_AdjustParam_Reg.Phb0 = PhSregpq;
		LTU_ATT7022E_AdjustParam_Reg.Phb1 = PhSregpq;
		break;
		
		//У׼C���й��������桢�޹���������
		case ATT7022E_ADJUST_PQSgain_PhSregpq_C:
		LTU_ATT7022E_AdjustParam_Reg.Pgc  = Pgain_Int;
		LTU_ATT7022E_AdjustParam_Reg.Qgc  = Pgain_Int;
		LTU_ATT7022E_AdjustParam_Reg.Sgc  = Pgain_Int;
		LTU_ATT7022E_AdjustParam_Reg.Phc0 = PhSregpq;
		LTU_ATT7022E_AdjustParam_Reg.Phc1 = PhSregpq;
		break;
	}
}



void ATT7022E_Calculate_PQSgain_PhSregpq_1_Extra(uint8_t Num_ATT7022E, float P_Real, int HFconst, int _EC, uint8_t PQgain_PhSregpq_ABC, float U_Real, uint8_t Ugain_ABC, float I_Real, float N, uint8_t Igain_ABC)
{
	extern Struct_LTU_ATT7022E_Param_Reg  LTU_ATT7022E_AdjustParam_Reg;
	
	uint8_t ADDR_P    = 0;    //�й����ʼĴ���
	int32 Ugain     = 0;    //��ѹ����У׼ֵ
	int32 Igain     = 0;    //��������У׼ֵ
	int32 DataP     = 0;    //��¼���й����ʼĴ����ж�ȡ����ԭʼֵ
	float P_Now     = 0.0;  //��¼��ǰ���������й�����ֵ����λ: ����W��
	float P_Old     = 0.0;  //��¼�ϴβ��������й�����ֵ����λ: ����W��
	float Sum_P     = 0.0;  //��¼�й����ʵĺ�
	int32 Num_P     = 0;    //��¼�ɼ����˶��ٸ��й������޹�����
	float K         = 0.0;  //���ʲ���������ο�ATT7022E�����ֲᣩ
	float P_Average = 0.0;  //��¼ƽ���й����ʣ���λ: ����W��
	float P_Err     = 0.0;  //����й����ʺͱ�׼�й�����֮������
	float Angle_Err = 0.0;  //�ǲ�
	int32 PhSregpq  = 0;    //�ǲ�У��ֵ
	float Pgain     = 0.0;  //�й���������У׼ֵ
	int32 Pgain_Int = 0;    //��¼��Ҫд�빦������Ĵ��������ֵ
	
	//У����ѹ����ֵ
	Ugain = ATT7022E_Calculate_Ugain(Num_ATT7022E, U_Real, Ugain_ABC);
	switch(Ugain_ABC)
	{
		case ATT7022E_ADJUST_Ugain_A:
		LTU_ATT7022E_AdjustParam_Reg.Uga = Ugain;
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Uga, LTU_ATT7022E_AdjustParam_Reg.Uga);
		break;
		
		case ATT7022E_ADJUST_Ugain_B:
		LTU_ATT7022E_AdjustParam_Reg.Ugb = Ugain;
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Ugb, LTU_ATT7022E_AdjustParam_Reg.Ugb);
		break;
		
		case ATT7022E_ADJUST_Ugain_C:
		LTU_ATT7022E_AdjustParam_Reg.Ugc = Ugain;
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Ugc, LTU_ATT7022E_AdjustParam_Reg.Ugc);
		break;
	}
	
	//У����������ֵ
	Igain = ATT7022E_Calculate_Igain(Num_ATT7022E, I_Real, N, Igain_ABC);
	switch(Igain_ABC)
	{
		case ATT7022E_ADJUST_Igain_A:
		LTU_ATT7022E_AdjustParam_Reg.Iga = Igain;
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Iga, LTU_ATT7022E_AdjustParam_Reg.Iga);
		break;
		
		case ATT7022E_ADJUST_Igain_B:
		LTU_ATT7022E_AdjustParam_Reg.Igb = Igain;
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Igb, LTU_ATT7022E_AdjustParam_Reg.Igb);
		break;
		
		case ATT7022E_ADJUST_Igain_C:
		LTU_ATT7022E_AdjustParam_Reg.Igc = Igain;
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Igc, LTU_ATT7022E_AdjustParam_Reg.Igc);
		break;
	}
	
	vTaskDelay(1200);
	
	//���㹦������У��ϵ��
	Pgain = ((Ugain + 1) * (Igain + 1) * HFconst * _EC * pow(2, 20) / (2.592 * pow(10, 10) * N)) - 1;
	if(Pgain >= 0)
	{
		Pgain_Int = (int32)(Pgain * pow(2, 15));
	}
	else
	{
		Pgain_Int = (int32)(pow(2, 16) + (Pgain * pow(2, 15)));
	}
	
	//д�빦������У��ϵ��
	switch(PQgain_PhSregpq_ABC)
	{
		//У׼A���й��������桢�޹���������
		case ATT7022E_ADJUST_PQSgain_PhSregpq_A:
		LTU_ATT7022E_AdjustParam_Reg.Pga = Pgain_Int;
		LTU_ATT7022E_AdjustParam_Reg.Qga = Pgain_Int;
		LTU_ATT7022E_AdjustParam_Reg.Sga = Pgain_Int;
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Pga, LTU_ATT7022E_AdjustParam_Reg.Pga);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Qga, LTU_ATT7022E_AdjustParam_Reg.Qga);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Sga, LTU_ATT7022E_AdjustParam_Reg.Sga);
		break;
		
		//У׼B���й��������桢�޹���������
		case ATT7022E_ADJUST_PQSgain_PhSregpq_B:
		LTU_ATT7022E_AdjustParam_Reg.Pgb = Pgain_Int;
		LTU_ATT7022E_AdjustParam_Reg.Qgb = Pgain_Int;
		LTU_ATT7022E_AdjustParam_Reg.Sgb = Pgain_Int;
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Pgb, LTU_ATT7022E_AdjustParam_Reg.Pgb);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Qgb, LTU_ATT7022E_AdjustParam_Reg.Qgb);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Sgb, LTU_ATT7022E_AdjustParam_Reg.Sgb);
		break;
		
		//У׼C���й��������桢�޹���������
		case ATT7022E_ADJUST_PQSgain_PhSregpq_C:
		LTU_ATT7022E_AdjustParam_Reg.Pgc = Pgain_Int;
		LTU_ATT7022E_AdjustParam_Reg.Qgc = Pgain_Int;
		LTU_ATT7022E_AdjustParam_Reg.Sgc = Pgain_Int;
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Pgc, LTU_ATT7022E_AdjustParam_Reg.Pgc);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Qgc, LTU_ATT7022E_AdjustParam_Reg.Qgc);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Sgc, LTU_ATT7022E_AdjustParam_Reg.Sgc);
		break;
	}
	
	//�ȴ�1.2s
	vTaskDelay(1200);

	//������������Ĺ���ֵ
	switch(PQgain_PhSregpq_ABC)
	{
		//У׼A���й��������桢�޹���������
		case ATT7022E_ADJUST_PQSgain_PhSregpq_A:
		ADDR_P = rPa;
		break;
		
		//У׼B���й��������桢�޹���������
		case ATT7022E_ADJUST_PQSgain_PhSregpq_B:
		ADDR_P = rPb;
		break;
		
		//У׼C���й��������桢�޹���������
		case ATT7022E_ADJUST_PQSgain_PhSregpq_C:
		ADDR_P = rPc;
		break;
	}
	
	//���㹦�ʲ���K
	K = 2.592f * pow(10, 10) / (HFconst * _EC * pow(2, 23));
	printf("���ʲ���: %f\r\n", K);
	
	while (1)
	{
		//�ӼĴ����ж�ȡ����ǰ���������й����ʣ��ٳ���Kֵת��������(W)Ϊ��λ���й�������Чֵ
		DataP = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, ADDR_P);
		if(DataP > pow(2, 23))
		{
			DataP -= pow(2, 24);
		}
		P_Now = DataP * K;
		printf("�й�����: %f\r\n", P_Now);

		//�жϵ�ǰ��ù��ʺ��ϴι�����ȣ��仯�Ƿ񳬹�1%
		if (abs((int)(P_Now - P_Old)) > abs((int)(P_Real * 0.01)))
		{
			//�����й����ʡ��޹�����
			P_Old = P_Now;
			
			//����⵽���й������޹����ʵĸ������й����ʺ͡��޹����ʺ�
			Num_P = 0;
			Sum_P = 0;
		}
		
		//�����й����ʺ͡��޹����ʺ͡��й������޹����ʸ������������㹦�ʵ�ƽ��ֵ
		Sum_P += P_Now;
		Num_P++;
		
		//����⵽���й������޹����ʸ�������10������ô������ѭ��
		if (Num_P >= 10)
		{
			break;
		}
		
		vTaskDelay(125);
	}
	
	//�����й����ʵ�ƽ��ֵ
	P_Average = Sum_P / Num_P;
	printf("ƽ���й�����: %fW\r\n",   P_Average);
	
	//����ǲ�
	P_Err = (P_Average - P_Real) / P_Real;
	printf("�й��������: %f\r\n", P_Err);
	
	//����ǲ�У��ϵ��
	Angle_Err = -P_Err / 1.732;
	if(Angle_Err >= 0)
	{
		PhSregpq = (int32)(Angle_Err * pow(2, 15));
	}
	else
	{
		PhSregpq = (int32)(pow(2, 16) + (Angle_Err * pow(2, 15)));
	}
	printf("�ǲ�У��ֵ: %d  0x%X\r\n", PhSregpq, PhSregpq);
	
	
	switch(PQgain_PhSregpq_ABC)
	{
		//У׼A���й��������桢�޹���������
		case ATT7022E_ADJUST_PQSgain_PhSregpq_A:
		LTU_ATT7022E_AdjustParam_Reg.Pha0 = PhSregpq;
		LTU_ATT7022E_AdjustParam_Reg.Pha1 = PhSregpq;
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Pha0, LTU_ATT7022E_AdjustParam_Reg.Pha0);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Pha1, LTU_ATT7022E_AdjustParam_Reg.Pha1);
		break;
		
		//У׼B���й��������桢�޹���������
		case ATT7022E_ADJUST_PQSgain_PhSregpq_B:
		LTU_ATT7022E_AdjustParam_Reg.Phb0 = PhSregpq;
		LTU_ATT7022E_AdjustParam_Reg.Phb1 = PhSregpq;
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Phb0, LTU_ATT7022E_AdjustParam_Reg.Phb0);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Phb1, LTU_ATT7022E_AdjustParam_Reg.Phb1);
		break;
		
		//У׼C���й��������桢�޹���������
		case ATT7022E_ADJUST_PQSgain_PhSregpq_C:
		LTU_ATT7022E_AdjustParam_Reg.Phc0 = PhSregpq;
		LTU_ATT7022E_AdjustParam_Reg.Phc1 = PhSregpq;
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Phc0, LTU_ATT7022E_AdjustParam_Reg.Phc0);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Phc1, LTU_ATT7022E_AdjustParam_Reg.Phc1);
		break;
	}
}



//�洢У�������EEPROM����
void ATT7022E_Write_AdjustParam_To_EEPROM(uint8_t Num_ATT7022E, uint8_t Length, uint8_t *P_AdjustParam)
{
	uint16 Address = 0;
	
	switch(Num_ATT7022E)
	{
		case 0:
		Address = ADDRESS_EEPROM_REG_ATT7022E_PARAM_0;
		break;
		
		case 1:
		Address = ADDRESS_EEPROM_REG_ATT7022E_PARAM_1;
		break;
		
		case 2:
		Address = ADDRESS_EEPROM_REG_ATT7022E_PARAM_2;
		break;
		
		case 3:
		Address = ADDRESS_EEPROM_REG_ATT7022E_PARAM_3;
		break;
		
		case 4:
		Address = ADDRESS_EEPROM_REG_ATT7022E_PARAM_4;
		break;
		
		default:
		return;
	}
	
	EEPROM_WriteString(Address, Length, P_AdjustParam);
}



void ATT7022E_Read_AdjustParam_From_EEPROM(uint8_t Num_ATT7022E, uint8_t Length, uint8_t *P_AdjustParam)
{
	uint16 Address = 0;
	
	switch(Num_ATT7022E)
	{
		case 0:
		Address = ADDRESS_EEPROM_REG_ATT7022E_PARAM_0;
		break;
		
		case 1:
		Address = ADDRESS_EEPROM_REG_ATT7022E_PARAM_1;
		break;
		
		case 2:
		Address = ADDRESS_EEPROM_REG_ATT7022E_PARAM_2;
		break;
		
		case 3:
		Address = ADDRESS_EEPROM_REG_ATT7022E_PARAM_3;
		break;
		
		case 4:
		Address = ADDRESS_EEPROM_REG_ATT7022E_PARAM_4;
		break;
		
		default:
		return;
	}
	
	EEPROM_WriteString(Address, Length, P_AdjustParam);
}



//�ж�ATT7022E�ĵ�ǰ״̬
void ATT7022E_Check_Status(uint8_t Num_ATT7022E)
{
	//�ⲿ�����У�������ȫ�ֱ���
	extern Struct_LTU_ATT7022E_Param_Reg LTU_ATT7022E_AdjustParam_Reg;
	
	int Device_ID;
	
	//��ȡоƬ��Device ID
	Device_ID = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, 0x00);
	//printf("ATT7022E��Device ID: 0x%06x\r\n", Device_ID);
	
	if(Device_ID == 0x7122A0)
	{
		//printf("ATT7022E��������״̬���ж��Ƿ��Ѿ�У��\r\n");
		
		//ѡ���ȡУ׼�Ĵ�����ֵ����ֹ��ȡ�����Ĵ�����ֵ
		ATT7022E_Read_AdjustParam_Set(Num_ATT7022E, 1);
		
		if (ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, 0x1E) != LTU_ATT7022E_AdjustParam_Reg.HFconst)
		{
			//���EEPROM�ǿյģ�˵��û�н��й�У������ֱ�����ϵ縴λ��Ĭ��ֵ���в���
			if(EEPROM_Check_Empty_Get())
			{
				//printf("ATT7022Eû��У��ֱ����Ĭ��ֵ���в���");
			}
			//���EEPROM����У�����ݣ�˵��У��ʧ��
			else
			{
				ATT7022E_Check_Error_Set(Num_ATT7022E, 1);
				//printf("ATT7022EУ��ʧ�ܣ���Ҫ����У��\r\n");
			}
		}
		else
		{
			//printf("ATT7022EУ��ɹ�������......\r\n");
		}

		//ѡ���ȡ�����Ĵ�����ֵ����ֹ��ȡУ׼�Ĵ�����ֵ
		ATT7022E_Read_AdjustParam_Set(Num_ATT7022E, 0);
	}
	else
	{
		//printf("ATT7022E״̬����\r\n");
	}
	
	//printf("\r\n");
};



//����У����: ����2
void ATT7022E_Adjust_2(uint8_t Num_ATT7022E, uint8_t *P_Param_Uint8, uint16 Length)
{
	extern const uint32 Address_EEPROM_Reg_ATT7022E_Param[ATT7022E_NUM_TOTAL];
	extern const uint32 Length_EEPROM_Reg_ATT7022E_Param[ATT7022E_NUM_TOTAL];
	extern Struct_LTU_ATT7022E_Param_Reg LTU_ATT7022E_AdjustParam_Reg;
	
	//��ʱ��¼����
	int32 Data_Reg = 0; //��¼��Ҫ����У��Ĵ�����ֵ
	int32 Ugain    = 0; //��¼ABC�����ѹ����ֵ
	int32 Igain    = 0; //��¼ABC�����������ֵ
	uint8_t Flag_Result = 0;  //��¼���
	
	//����һ����¼У������Ľṹ�����
	Struct_ATT7022E_AdjustParam_Standard AdjustParam;
	
	//�����������Ļ�����ָ��Ϊ�գ������ǳ��Ȳ���60���ֽڣ���ôУ���������ֻ���˳�
	if ((P_Param_Uint8 == NULL) || (Length != 60))
	{
		return;
	}

	memcpy((uint8_t *)&AdjustParam, P_Param_Uint8, 60);

	printf("|-------------------------------------------------|\r\n");
	printf("|              ��ʼУ��(����2): %3d               |\r\n", Num_ATT7022E);
	printf("|-------------------------------------------------|\r\n");
	
	printf("\r\n");
	
	printf("���峣��: %d\r\n", AdjustParam._EC);
	printf("���ѹ: %f\r\n", AdjustParam.Un);
	printf("�����: %f\r\n", AdjustParam.Ib);
	printf("A���ѹ: %f  ����: %f  �й�����: %f  �޹�����: %f\r\n", AdjustParam.UA, AdjustParam.IA, AdjustParam.PA, AdjustParam.QA);
	printf("B���ѹ: %f  ����: %f  �й�����: %f  �޹�����: %f\r\n", AdjustParam.UB, AdjustParam.IB, AdjustParam.PB, AdjustParam.QB);
	printf("C���ѹ: %f  ����: %f  �й�����: %f  �޹�����: %f\r\n", AdjustParam.UC, AdjustParam.IC, AdjustParam.PC, AdjustParam.QC);
	
	printf("\r\n");

	//��λ����У׼�Ĵ����ڵ�ֵ
	ATT7022E_Clear_AdjustParam(Num_ATT7022E);
	Delay_us(100);
	
	//ʹ��д��У׼�Ĵ���
	ATT7022E_Write_AdjustParam_Set(Num_ATT7022E, 1);
	
	//�������ϵ��Nֵ������������ѹΪ50mVʱ��
	LTU_ATT7022E_AdjustParam_Reg.N   = 60 / AdjustParam.Ib;  //�������ϵ��Nֵ
	LTU_ATT7022E_AdjustParam_Reg._EC = AdjustParam._EC;      //��������EC
	
	//����ģʽ�Ĵ���
	Data_Reg = 0;
	Data_Reg &= ~BIT(0);   //���ߵ���I0��ADCͨ����0: �ر�  1: �򿪣�
	Data_Reg |=  BIT(1);   //A�����Ia��ADCͨ����0: �ر�  1: �򿪣�
	Data_Reg |=  BIT(2);   //A���ѹUa��ADCͨ����0: �ر�  1: �򿪣�
	Data_Reg |=  BIT(3);   //B�����Ib��ADCͨ����0: �ر�  1: �򿪣�
	Data_Reg |=  BIT(4);   //B���ѹUb��ADCͨ����0: �ر�  1: �򿪣�
	Data_Reg |=  BIT(5);   //C�����Ic��ADCͨ����0: �ر�  1: �򿪣�
	Data_Reg |=  BIT(6);   //C���ѹUc��ADCͨ����0: �ر�  1: �򿪣�
	Data_Reg |=  BIT(7);   //ADC��Chop���ܣ��ܹ����ADC�����ܣ�0: �ر�  1: �򿪣��Ƽ��򿪣�
	Data_Reg |=  BIT(8);   //Femuʱ��ѡ��921.6kHz��[BIT9:BIT8]  00: 1.8432MHz  01: 921.6kHz  1X: 460.8kHz��
	Data_Reg &= ~BIT(9);
	Data_Reg &= ~BIT(10);  //ѡ��Irefƫ�õ�����һ���˹��ĺ����ܣ�����ѡ��7.5uA��[BIT11:BIT10]  00: 5uA  01: 5uA  10: 7.5uA  11: 10uA��
	Data_Reg |=  BIT(11);
	Data_Reg |=  BIT(12);  //��Чֵ�͹��ʵĸ�������ѡ��һ��Ϊ��ȡ�ȶ���ֵ����Ϊ���٣�0: ����28.1Hz  1: ����1.76Hz��
	Data_Reg |=  BIT(13);  //ѡ����Чֵ���ȶ�ʱ�䣬һ��Ϊ��ȡ�ȶ���ֵ����Ϊ���٣�����С��0: ����  1: ���٣�
	Data_Reg &= ~BIT(14);  //��������ʱ�����ѹUb��Чֵ������Դѡ��0: �ڲ�Ua-Uc  1: Ubͨ����
	Data_Reg |=  BIT(15);  //����Ref��Chop���ܣ�Ϊ�õ����ȶ���Vref���Ƽ��򿪣�0: �ر�  1: �򿪣�
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_ModeCfg, Data_Reg);
	//��¼��ȫ�ֱ�������
	LTU_ATT7022E_AdjustParam_Reg.ModeCfg = Data_Reg;
	
	//����EMU��Ԫ
	Data_Reg = 0;
	Data_Reg &= ~BIT(0);   //���β���Ƶ��ѡ��[BIT2:BIT0]  1XX: 14.4K  011: 7.2K  010: 3.6K  001: 1.8K  000: 0.9K��
	Data_Reg &= ~BIT(1);
	Data_Reg |=  BIT(2);
	Data_Reg &= ~BIT(3);   //���ڹ��ʡ������Ĵ���������ʽ��0: PQS��ʽ����  1: RMS��ʽ������
	Data_Reg &= ~BIT(4);   //ѡ���ѹ��0�жϷ�ʽ��[BIT5:BIT4]  1X: ˫���0�ж�  01: �����0�ж�  00: �����0�жϣ�
	Data_Reg &= ~BIT(5);
	Data_Reg &= ~BIT(6);   //�ۼӷ�ʽ��0: ���������þ���ֵ�ۼӣ����������ô���ֵ�ۼ�  1: ���������ô���ֵ�ۼӣ����������þ���ֵ�ۼӣ�
	Data_Reg |=  BIT(7);   //�����Ĵ������㷽ʽ��0: �����Ĵ���������0  1: �����Ĵ���������0��
	Data_Reg |=  BIT(8);   //���λ�������Դѡ��[BIT9:BIT8]  00: ADC���ݲ�����ͨ�˲�����ԭʼ����  01: ADC���ݾ���ͨ�˲����Ҿ�����У��  1X: ADC�������ݾ������˲�����
	Data_Reg &= ~BIT(9);
	Data_Reg &= ~BIT(10);  //����/г���������ܣ�0: �ر�  1: ������
	Data_Reg |=  BIT(11);  //Ǳ��/���ж����ݣ�0: ѡ�������Чֵ��ΪǱ��/�����ж�����  1: ѡ������ΪǱ��/�����ж����ݣ��Ƽ����ʣ�
	Data_Reg |=  BIT(12);  //�й�����CF1ͨ·�����������ܣ�0: �ر�  1: ������
	Data_Reg |=  BIT(13);  //�޹�����CF2ͨ·�����������ܣ�0: �ر�  1: ������
	Data_Reg |=  BIT(14);  //��������CF3ͨ·�����������ܣ�0: �ر�  1: ������
	Data_Reg |=  BIT(15);  //�����й�����CF4ͨ·�����������ܣ�0: �ر�  1: ������
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_EMUCfg, Data_Reg);
	//��¼��ȫ�ֱ�������
	LTU_ATT7022E_AdjustParam_Reg.EMUCfg = Data_Reg;
	
	//ģ��ģ��ʹ�ܼĴ���
	Data_Reg = 0;
	Data_Reg |=  BIT(0);   //����ͨ�����ָ�ͨ�˲�����0: �ر�  1: ������
	Data_Reg |=  BIT(1);   //����ͨ�����ָ�ͨ�˲�����0: �ر�  1: ������
	Data_Reg |=  BIT(2);   //��ѹͨ�����ָ�ͨ�˲�����0: �ر�  1: ������
	Data_Reg &= ~BIT(3);   //����ͨ����������Ȧ���ܣ�0: �ر�  1: ������
	Data_Reg |=  BIT(4);   //TPS���ܣ�0: �ر�  1: ������
	Data_Reg |=  BIT(5);   //BOR���ܣ�0: �ر�  1: ������
	Data_Reg &= ~BIT(6);   //�жϵ�ƽ��0: �͵�ƽ��Ч  1: �ߵ�ƽ��Ч��
	Data_Reg &= ~BIT(7);   //�¶ȴ���������ѡ���źţ�0: PN���������Ƽ�  1: ���贫������
	Data_Reg &= ~BIT(11);  //SPI����ѡ��0: ����ģʽ  1: ����ģʽ��
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_ModuleCfg, Data_Reg);
	//��¼��ȫ�ֱ�������
	LTU_ATT7022E_AdjustParam_Reg.ModuleCfg = Data_Reg;

	//����ADC����
	Data_Reg = 0;
	Data_Reg &= ~BIT(0);  //���ߵ���ADC���棨[BIT1:BIT0]  00: 1��  01: 2��  10: 8��  11: 16����
	Data_Reg &= ~BIT(1);
	Data_Reg |=  BIT(2);  //A�����ADC���棨[BIT3:BIT2]  00: 1��  01: 2��  10: 8��  11: 16����
	Data_Reg &= ~BIT(3);
	Data_Reg |=  BIT(4);  //B�����ADC���棨[BIT5:BIT4]  00: 1��  01: 2��  10: 8��  11: 16����
	Data_Reg &= ~BIT(5);
	Data_Reg |=  BIT(6);  //C�����ADC���棨[BIT7:BIT6]  00: 1��  01: 2��  10: 8��  11: 16����
	Data_Reg &= ~BIT(7);
	Data_Reg |=  BIT(8);  //�����ѹADC���棨[BIT9:BIT8]  00: 1��  01: 2��  10: 8��  11: 8����
	Data_Reg &= ~BIT(9);
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_PGACtrl, LTU_ATT7022E_AdjustParam_Reg.PGACtrl);
	//��¼��ȫ�ֱ�������
	LTU_ATT7022E_AdjustParam_Reg.PGACtrl = Data_Reg;
	
	//У������ź�: 0.072V
	//У���ѹ�ź�: 0.050V
	
	//���ø�Ƶ���峣��
	Data_Reg = 0;
	Data_Reg = (int32)(2.592 * pow(10, 10) * 1.163 * 1.163 * (0.050 * 2) * (0.072 * 2) / (AdjustParam._EC * AdjustParam.Un * AdjustParam.Ib));
	if(Data_Reg < 0x000002)
	{
		Data_Reg = 0x000002;
	}
	else
	if(Data_Reg > 0x000D00)
	{
		Data_Reg = 0x000D00;
	}
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_HFconst, LTU_ATT7022E_AdjustParam_Reg.HFconst);
	//��¼��ȫ�ֱ�������
	LTU_ATT7022E_AdjustParam_Reg.HFconst = Data_Reg;
	
	//�����𶯵���
	Data_Reg = 0;
	Data_Reg = (int32)(0.8 * (AdjustParam.Ib * LTU_ATT7022E_AdjustParam_Reg.N * 0.004) * pow(2, 13));
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Istart, LTU_ATT7022E_AdjustParam_Reg.Istart);
	//��¼��ȫ�ֱ�������
	LTU_ATT7022E_AdjustParam_Reg.Istart = Data_Reg;
	
	//����ʧѹ��ֵ
	Data_Reg = 0;
	Data_Reg = (int32)(AdjustParam.Un * pow(2, 5) * 0.7);
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Ufail, LTU_ATT7022E_AdjustParam_Reg.Ufail);
	//��¼��ȫ�ֱ�������
	LTU_ATT7022E_AdjustParam_Reg.Ufail = Data_Reg;
	
	//�����𶯹���
	Data_Reg = 0;
	Data_Reg = (int32)(0.6 * AdjustParam.Un * AdjustParam.Ib * LTU_ATT7022E_AdjustParam_Reg.HFconst * AdjustParam._EC * 0.004 * pow(2, 23) / (2.592 * pow(10, 10)));
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Pstart, Data_Reg);
	//��¼��ȫ�ֱ�������
	LTU_ATT7022E_AdjustParam_Reg.Pstart = Data_Reg;
	
	//������λ��������
	//......
	
	//У׼A�๦�����油���Ĵ���
	printf("A�๦������У׼��ʼ\r\n");
	ATT7022E_Calculate_PQSgain_PhSregpq_1(Num_ATT7022E,          //У׼�ĸ�ATT7022E
										  AdjustParam.PA,        //A���׼�й�����
										  AdjustParam.QA,        //A���׼�޹�����
										  LTU_ATT7022E_AdjustParam_Reg.HFconst,  //��Ƶ���峣���Ĵ�����ֵ
										  AdjustParam._EC,       //�����
										  ATT7022E_ADJUST_PQSgain_PhSregpq_A);  //У׼A�๦��
	printf("A�๦������У׼����\r\n");
	
	printf("\r\n");
	
	//У׼B�๦�����油���Ĵ���
	printf("B�๦������У׼��ʼ\r\n");
	ATT7022E_Calculate_PQSgain_PhSregpq_1(Num_ATT7022E,          //У׼�ĸ�ATT7022E
										  AdjustParam.PB,        //B���׼�й�����
										  AdjustParam.QB,        //B���׼�޹�����
										  LTU_ATT7022E_AdjustParam_Reg.HFconst,  //��Ƶ���峣���Ĵ�����ֵ
										  AdjustParam._EC,       //�����
										  ATT7022E_ADJUST_PQSgain_PhSregpq_B);  //У׼B�๦��
	printf("B�๦������У׼����\r\n");
	
	printf("\r\n");
	
	//У׼C�๦�����油���Ĵ���
	printf("C�๦������У׼��ʼ\r\n");
	ATT7022E_Calculate_PQSgain_PhSregpq_1(Num_ATT7022E,          //У׼�ĸ�ATT7022E
										  AdjustParam.PC,        //C���׼�й�����
										  AdjustParam.QC,        //C���׼�޹�����
										  LTU_ATT7022E_AdjustParam_Reg.HFconst,  //��Ƶ���峣���Ĵ�����ֵ
										  AdjustParam._EC,       //�����
										  ATT7022E_ADJUST_PQSgain_PhSregpq_C);  //У׼C�๦��
	printf("C�๦������У׼����\r\n");
	
	printf("\r\n");
	
	//У׼A���ѹ����
	printf("A���ѹ����У����ʼ......\r\n");
	Ugain = ATT7022E_Calculate_Ugain(Num_ATT7022E, AdjustParam.UA, ATT7022E_ADJUST_Ugain_A);
	if (Ugain >= 0)
	{
		LTU_ATT7022E_AdjustParam_Reg.Uga = Ugain;
		printf("A���ѹ����У���ɹ���\r\n");
	}
	else
	{
		printf("A���ѹ����У��ʧ�ܣ�\r\n");
	}
	
	printf("\r\n");
	
	//У׼B���ѹ����
	printf("B���ѹ����У����ʼ......\r\n");
	Ugain = ATT7022E_Calculate_Ugain(Num_ATT7022E, AdjustParam.UB, ATT7022E_ADJUST_Ugain_B);
	if (Ugain >= 0)
	{
		LTU_ATT7022E_AdjustParam_Reg.Ugb = Ugain;
		printf("B���ѹ����У���ɹ���\r\n");
	}
	else
	{
		printf("B���ѹ����У��ʧ�ܣ�\r\n");
	}
	
	printf("\r\n");
	
	//У׼C���ѹ����
	printf("C���ѹ����У����ʼ......\r\n");
	Ugain = ATT7022E_Calculate_Ugain(Num_ATT7022E, AdjustParam.UC, ATT7022E_ADJUST_Ugain_C);
	if (Ugain >= 0)
	{
		LTU_ATT7022E_AdjustParam_Reg.Ugc = Ugain;
		printf("C���ѹ����У���ɹ���\r\n");
	}
	else
	{
		printf("C���ѹ����У��ʧ�ܣ�\r\n");
	}
	
	printf("\r\n");
	
	//У׼A���������
	printf("A���������У����ʼ......\r\n");
	Igain = ATT7022E_Calculate_Igain(Num_ATT7022E, AdjustParam.IA, LTU_ATT7022E_AdjustParam_Reg.N, ATT7022E_ADJUST_Igain_A);
	if (Igain >= 0)
	{
		LTU_ATT7022E_AdjustParam_Reg.Iga = Igain;
		printf("A���������У���ɹ���\r\n");
	}
	else
	{
		printf("A���������У��ʧ�ܣ�\r\n");
	}
	
	printf("\r\n");
	
	//У׼B���������
	printf("B���������У����ʼ......\r\n");
	Igain = ATT7022E_Calculate_Igain(Num_ATT7022E, AdjustParam.IB, LTU_ATT7022E_AdjustParam_Reg.N, ATT7022E_ADJUST_Igain_B);
	if (Igain >= 0)
	{
		LTU_ATT7022E_AdjustParam_Reg.Igb = Igain;
		printf("B���������У���ɹ���\r\n");
	}
	else
	{
		printf("B���������У��ʧ�ܣ�\r\n");
	}
	
	printf("\r\n");
	
	//У׼C���������
	printf("C���������У����ʼ......\r\n");
	Igain = ATT7022E_Calculate_Igain(Num_ATT7022E, AdjustParam.IC, LTU_ATT7022E_AdjustParam_Reg.N, ATT7022E_ADJUST_Igain_C);
	if (Igain >= 0)
	{
		LTU_ATT7022E_AdjustParam_Reg.Igc = Igain;
		printf("C���������У���ɹ���\r\n");
	}
	else
	{
		printf("C���������У��ʧ�ܣ�\r\n");
	}
	
	printf("\r\n");
	
	//��У�����ABC���๦������У׼ֵд�빦������У׼�Ĵ���
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Pga, LTU_ATT7022E_AdjustParam_Reg.Pga);
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Qga, LTU_ATT7022E_AdjustParam_Reg.Qga);
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Sga, LTU_ATT7022E_AdjustParam_Reg.Sga);
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Pgb, LTU_ATT7022E_AdjustParam_Reg.Pgb);
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Qgb, LTU_ATT7022E_AdjustParam_Reg.Qgb);
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Sgb, LTU_ATT7022E_AdjustParam_Reg.Sgb);
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Pgc, LTU_ATT7022E_AdjustParam_Reg.Pgc);
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Qgc, LTU_ATT7022E_AdjustParam_Reg.Qgc);
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Sgc, LTU_ATT7022E_AdjustParam_Reg.Sgc);
	
	//��У�������λУ��ֵд����λУ׼�Ĵ���
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Pha0, LTU_ATT7022E_AdjustParam_Reg.Pha0);
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Phb0, LTU_ATT7022E_AdjustParam_Reg.Phb0);
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Phc0, LTU_ATT7022E_AdjustParam_Reg.Phc0);
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Pha1, LTU_ATT7022E_AdjustParam_Reg.Pha1);
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Phb1, LTU_ATT7022E_AdjustParam_Reg.Phb1);
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Phc1, LTU_ATT7022E_AdjustParam_Reg.Phc1);
	
	//��У�����ABC�����ѹ����У׼ֵд���ѹ����У׼�Ĵ���
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Uga, LTU_ATT7022E_AdjustParam_Reg.Uga);
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Ugb, LTU_ATT7022E_AdjustParam_Reg.Ugb);
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Ugc, LTU_ATT7022E_AdjustParam_Reg.Ugc);
	
	//��У�����ABC�����������У׼ֵд���������У׼�Ĵ���
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Iga, LTU_ATT7022E_AdjustParam_Reg.Iga);
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Igb, LTU_ATT7022E_AdjustParam_Reg.Igb);
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Igc, LTU_ATT7022E_AdjustParam_Reg.Igc);
	
	//��ֹд��У׼�Ĵ�������ֹ��д��
	ATT7022E_Write_AdjustParam_Set(Num_ATT7022E, 0);
	
	printf("ModeCfg:   0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.ModeCfg);
	printf("EMUCfg:    0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.EMUCfg);
	printf("ModuleCfg: 0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.ModuleCfg);
	printf("PGACtrl:   0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.PGACtrl);
	printf("HFconst:   0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.HFconst);
	printf("Istart:    0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.Istart);
	printf("Ufail:     0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.Ufail);
	printf("Pstart:    0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.Pstart);
	printf("Pga:       0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.Pga);
	printf("Qga:       0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.Qga);
	printf("Sga:       0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.Sga);
	printf("Pgb:       0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.Pgb);
	printf("Qgb:       0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.Qgb);
	printf("Sgb:       0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.Sgb);
	printf("Pgc:       0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.Pgc);
	printf("Qgc:       0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.Qgc);
	printf("Sgc:       0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.Sgc);
	printf("Pha0:      0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.Pha0);
	printf("Phb0:      0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.Phb0);
	printf("Phc0:      0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.Phc0);
	printf("Pha1:      0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.Pha1);
	printf("Phb1:      0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.Phb1);
	printf("Phc1:      0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.Phc1);
	printf("Uga:       0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.Uga);
	printf("Ugb:       0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.Ugb);
	printf("Ugc:       0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.Ugc);
	printf("Iga:       0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.Iga);
	printf("Igb:       0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.Igb);
	printf("Igc:       0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.Igc);
	
	//��¼���ʲ���K
	LTU_ATT7022E_AdjustParam_Reg.K = 2.592 * pow(10, 10) / (LTU_ATT7022E_AdjustParam_Reg.HFconst * AdjustParam._EC * pow(2, 23));
	
	//��¼У���
	LTU_ATT7022E_AdjustParam_Reg.CheckSum = GetSum8((uint8_t *)&LTU_ATT7022E_AdjustParam_Reg, sizeof(LTU_ATT7022E_AdjustParam_Reg) - sizeof(LTU_ATT7022E_AdjustParam_Reg.CheckSum));
	printf("����У���: 0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.CheckSum);
	
	printf("\r\n");
	
	//��У�����д��EEPROM��
	printf("У�����ݴ���EEPROM\r\n");
	Flag_Result = EEPROM_Write_ATT7022E_Param_Reg(&LTU_ATT7022E_AdjustParam_Reg, Num_ATT7022E);
	if(Flag_Result == 0)
	{
		printf("У�����ݴ���EEPROMУ�����\r\n");
	}
	printf("\r\n");
	printf("У�����ݴ洢���\r\n");
	
	//���EEPROM�ǿ�Ƭ����ôд���ض��ַ����������Ϊ�ǿ�Ƭ
	if(EEPROM_Check_Empty_Get())
	{
		EEPROM_WriteString(ADDRESS_EEPROM_REG_EMPTY_CHECK, LENGTH_EEPROM_REG_EMPTY_CHECK, STRING_EEPROM_REG_EMPTY_CHECK);
		//����һ���Ƭ������
		EEPROM_Check_Empty();
	}
	
	printf("\r\n");
	
	printf("У�����\r\n");
	
	printf("\r\n");
}



//��EEPROM�е�У�����д��ATT7022E����ȥ
void ATT7022E_Load_AdjustParam_From_EEPROM(uint8_t Num_ATT7022E)
{
	//�ⲿ�����EEPROM�洢�������ݵ���ʼ��ַ����
	extern const uint32 Address_EEPROM_Reg_ATT7022E_Param[ATT7022E_NUM_TOTAL];
	extern const uint32 Length_EEPROM_Reg_ATT7022E_Param[ATT7022E_NUM_TOTAL];
	extern Struct_LTU_ATT7022E_Param_Reg LTU_ATT7022E_AdjustParam_Reg;
	extern Struct_LTU_Data LTU_Data;
	
	uint8_t  Buffer_AdjustParam[LENGTH_EEPROM_REG_ATT7022E_PARAM_0];  //�洢��EEPROM��ֱ��ȡ����ԭʼ����
	uint8_t Flag_Result = 0;
	
	//��ʼ����ر���
	memset(Buffer_AdjustParam, 0, LENGTH_EEPROM_REG_ATT7022E_PARAM_0);
	
	//��EEPROM�ж�ȡ��У������
	Flag_Result = EEPROM_Read_ATT7022E_Param_Reg(&LTU_ATT7022E_AdjustParam_Reg, Num_ATT7022E);
	if(Flag_Result)
	{
		printf("��EEPROM�ж�ȡУ������ɹ�\r\n");
		
		//��λ����У׼�Ĵ����ڵ�ֵ
		ATT7022E_Clear_AdjustParam(Num_ATT7022E);
		
		//ʹ��д��У׼�Ĵ���
		ATT7022E_Write_AdjustParam_Set(Num_ATT7022E, 1);
		
		//У׼��ؼĴ���
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_ModeCfg,   LTU_ATT7022E_AdjustParam_Reg.ModeCfg);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_EMUCfg,    LTU_ATT7022E_AdjustParam_Reg.EMUCfg);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_ModuleCfg, LTU_ATT7022E_AdjustParam_Reg.ModuleCfg);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_PGACtrl,   LTU_ATT7022E_AdjustParam_Reg.PGACtrl);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_HFconst,   LTU_ATT7022E_AdjustParam_Reg.HFconst);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Istart,    LTU_ATT7022E_AdjustParam_Reg.Istart);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Ufail,     LTU_ATT7022E_AdjustParam_Reg.Ufail);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Pstart,    LTU_ATT7022E_AdjustParam_Reg.Pstart);
		
		//У׼��������
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Pga, LTU_ATT7022E_AdjustParam_Reg.Pga);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Qga, LTU_ATT7022E_AdjustParam_Reg.Qga);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Sga, LTU_ATT7022E_AdjustParam_Reg.Sga);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Pgb, LTU_ATT7022E_AdjustParam_Reg.Pgb);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Qgb, LTU_ATT7022E_AdjustParam_Reg.Qgb);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Sgb, LTU_ATT7022E_AdjustParam_Reg.Sgb);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Pgc, LTU_ATT7022E_AdjustParam_Reg.Pgc);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Qgc, LTU_ATT7022E_AdjustParam_Reg.Qgc);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Sgc, LTU_ATT7022E_AdjustParam_Reg.Sgc);
		
		//У׼��ǲ���
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Pha0, LTU_ATT7022E_AdjustParam_Reg.Pha0);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Phb0, LTU_ATT7022E_AdjustParam_Reg.Phb0);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Phc0, LTU_ATT7022E_AdjustParam_Reg.Phc0);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Pha1, LTU_ATT7022E_AdjustParam_Reg.Pha1);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Phb1, LTU_ATT7022E_AdjustParam_Reg.Phb1);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Phc1, LTU_ATT7022E_AdjustParam_Reg.Phc1);
		
		//У׼��ѹ����
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Uga, LTU_ATT7022E_AdjustParam_Reg.Uga);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Ugb, LTU_ATT7022E_AdjustParam_Reg.Ugb);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Ugc, LTU_ATT7022E_AdjustParam_Reg.Ugc);
		
		//У׼��������
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Iga, LTU_ATT7022E_AdjustParam_Reg.Iga);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Igb, LTU_ATT7022E_AdjustParam_Reg.Igb);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Igc, LTU_ATT7022E_AdjustParam_Reg.Igc);
		
		//��ֹд��У׼�Ĵ�����У׼�Ĵ�����д������
		ATT7022E_Write_AdjustParam_Set(Num_ATT7022E, 0);
		
		printf("������ؼĴ���\r\n");
		printf("ModeCfg:   0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.ModeCfg);
		printf("EMUCfg:    0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.EMUCfg);
		printf("ModuleCfg: 0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.ModuleCfg);
		printf("PGACtrl:   0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.PGACtrl);
		printf("HFconst:   0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.HFconst);
		printf("Istart:    0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.Istart);
		printf("Ufail:     0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.Ufail);
		printf("Pstart:    0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.Pstart);
		printf("Pga:       0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.Pga);
		printf("Qga:       0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.Qga);
		printf("Sga:       0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.Sga);
		printf("Pgb:       0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.Pgb);
		printf("Qgb:       0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.Qgb);
		printf("Sgb:       0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.Sgb);
		printf("Pgc:       0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.Pgc);
		printf("Qgc:       0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.Qgc);
		printf("Sgc:       0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.Sgc);
		printf("Pha0:      0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.Pha0);
		printf("Phb0:      0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.Phb0);
		printf("Phc0:      0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.Phc0);
		printf("Pha1:      0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.Pha1);
		printf("Phb1:      0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.Phb1);
		printf("Phc1:      0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.Phc1);
		printf("Uga:       0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.Uga);
		printf("Ugb:       0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.Ugb);
		printf("Ugc:       0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.Ugc);
		printf("Iga:       0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.Iga);
		printf("Igb:       0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.Igb);
		printf("Igc:       0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.Igc);
		printf("�Ѿ��ɹ���У�����װ�ؽ�ATT7022E��\r\n");
	}
	else
	{
		printf("��EEPROM�л�ȡУ������ʱУ��ʧ��\r\n");
	}
	printf("\r\n");
}



//��ȡ��ǰ˲ʱ��ѹ
void ATT7022E_Get_Voltage(uint8_t Num_ATT7022E)
{
	extern Struct_LTU_Data LTU_Data;
	
	int   Data_Ua   = 0;
	int   Data_Ub   = 0;
	int   Data_Uc   = 0;
	int   Data_YUab = 0;
	int   Data_YUac = 0;
	int   Data_YUbc = 0;
	float Ua        = 0.0f;
	float Ub        = 0.0f;
	float Uc        = 0.0f;
	float YUab      = 0.0f;
	float YUac      = 0.0f;
	float YUbc      = 0.0f;
	
	//�ӼĴ����л�ȡ�����ѹ��ԭʼֵ
	Data_Ua   = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rUa);
	Data_Ub   = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rUb);
	Data_Uc   = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rUc);
	Data_YUab = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rYUaUb);
	Data_YUac = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rYUaUc);
	Data_YUbc = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rYUbUc);
	
	//���ͨ�ŷ����˹���
	if(ATT7022E_Check_Result_Get(Num_ATT7022E) == 0)
	{
		printf("��ȡ���ѹʱ����ͨ�Ź���\r\n");
	}
	
	//����ɹ��ʵ�λֵ
	Ua = (float)Data_Ua / pow(2, 13);
	Ub = (float)Data_Ub / pow(2, 13);
	Uc = (float)Data_Uc / pow(2, 13);
	YUab = (float)Data_YUab / pow(2, 20) * 180;
	YUac = (float)Data_YUac / pow(2, 20) * 180;
	YUbc = (float)Data_YUbc / pow(2, 20) * 180;
	
	//�����ȫ�ֱ�����
	LTU_Data.ATT7022E.UA[Num_ATT7022E]  = Ua;
	LTU_Data.ATT7022E.UB[Num_ATT7022E]  = Ub;
	LTU_Data.ATT7022E.UC[Num_ATT7022E]  = Uc;
	LTU_Data.ATT7022E.YUA[Num_ATT7022E] = 0;
	LTU_Data.ATT7022E.YUB[Num_ATT7022E] = YUab;
	LTU_Data.ATT7022E.YUC[Num_ATT7022E] = (YUac + (YUab + YUbc)) / 2;
}



//��ȡ��ǰ˲ʱ����
void ATT7022E_Get_Current(uint8_t Num_ATT7022E)
{
	extern Struct_LTU_ATT7022E_Param_Reg LTU_ATT7022E_AdjustParam_Reg;
	extern Struct_LTU_Data                 LTU_Data;
	
	int   Data_Ia = 0;
	int   Data_Ib = 0;
	int   Data_Ic = 0;
	float N       = 0.0f;
	float Ia      = 0.0f;
	float Ib      = 0.0f;
	float Ic      = 0.0f;
	
	//��ȡ��������
	N = LTU_ATT7022E_AdjustParam_Reg.N;
	
	//�ӼĴ����л�ȡ���������ԭʼֵ
	Data_Ia = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rIa);
	Data_Ib = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rIb);
	Data_Ic = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rIc);
	
	//���ͨ�ŷ����˹���
	if(ATT7022E_Check_Result_Get(Num_ATT7022E) == 0)
	{
		printf("��ȡ�����ʱ����ͨ�Ź���\r\n");
	}
	
	//������԰���AΪ��λ
	Ia = (float)(Data_Ia / pow(2, 13)) / N;
	Ib = (float)(Data_Ib / pow(2, 13)) / N;
	Ic = (float)(Data_Ic / pow(2, 13)) / N;

#ifdef LTU_TRANSFORMS_RATE
	//printf("\nLTU_Transformer_Rate.Numerator=%d, Denominator=%d, %.2f, %.2f, %.2f\n", LTU_Transformer_Rate.Numerator, LTU_Transformer_Rate.Denominator, Ia, Ib, Ic);
	Ia = (float)(LTU_Transformer_Rate.Numerator / LTU_Transformer_Rate.Denominator)*Ia;
	Ib = (float)(LTU_Transformer_Rate.Numerator / LTU_Transformer_Rate.Denominator)*Ib;
	Ic = (float)(LTU_Transformer_Rate.Numerator / LTU_Transformer_Rate.Denominator)*Ic;
#endif
	
	//�����ȫ�ֱ�����
	LTU_Data.ATT7022E.IA[Num_ATT7022E] = Ia;
	LTU_Data.ATT7022E.IB[Num_ATT7022E] = Ib;
	LTU_Data.ATT7022E.IC[Num_ATT7022E] = Ic;
}



//��ȡ��ǰ���
void ATT7022E_Get_PhSregpq(uint8_t Num_ATT7022E)
{
	extern Struct_LTU_Data LTU_Data;
	
	int   Data_PhSregpq_A = 0;
	int   Data_PhSregpq_B = 0;
	int   Data_PhSregpq_C = 0;
	float PhSregpq_A      = 0.0f;
	float PhSregpq_B      = 0.0f;
	float PhSregpq_C      = 0.0f;
	
	//����ǼĴ�����ȡ��ԭʼֵ
	Data_PhSregpq_A = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rPga);
	Data_PhSregpq_B = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rPgb);
	Data_PhSregpq_C = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rPgc);
	
	//���ͨ�ŷ����˹���
	if(ATT7022E_Check_Result_Get(Num_ATT7022E) == 0)
	{
		printf("��ȡ�������ʱ����ͨ�Ź���\r\n");
		return;
	}
	
	//����ɹ��ʵ�λֵ����λ: �㣩
	if(Data_PhSregpq_A >= pow(2, 20))
	{
		Data_PhSregpq_A -= pow(2, 24);
	}
	PhSregpq_A = ((float)Data_PhSregpq_A / pow(2, 20)) * 180;
	if(Data_PhSregpq_B >= pow(2, 20))
	{
		Data_PhSregpq_B -= pow(2, 24);
	}
	PhSregpq_B = ((float)Data_PhSregpq_B / pow(2, 20)) * 180;
	if(Data_PhSregpq_C >= pow(2, 20))
	{
		Data_PhSregpq_C -= pow(2, 24);
	}
	PhSregpq_C = ((float)Data_PhSregpq_C / pow(2, 20)) * 180;
	
	//�����ȫ�ֱ�����
	LTU_Data.ATT7022E.PhSregpqA[Num_ATT7022E] = PhSregpq_A;
	LTU_Data.ATT7022E.PhSregpqB[Num_ATT7022E] = PhSregpq_B;
	LTU_Data.ATT7022E.PhSregpqC[Num_ATT7022E] = PhSregpq_C;
}



/*
//��ȡ��ǰ���
void ATT7022E_Get_PhSregpq(uint8_t Num_ATT7022E)
{
	extern Struct_LTU_Data LTU_Data;
	
	int   Data_PhSregpq_A = 0;
	int   Data_PhSregpq_B = 0;
	int   Data_PhSregpq_C = 0;
	uint8_t Flag_Negative   = 0;  //�����궨�Ƿ�Ϊ����
	float PhSregpq_A      = 0.0f;
	float PhSregpq_B      = 0.0f;
	float PhSregpq_C      = 0.0f;
	
	//����ǼĴ�����ȡ��ԭʼֵ
	Data_PhSregpq_A = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rPga);
	Data_PhSregpq_B = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rPgb);
	Data_PhSregpq_C = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rPgc);
	
	//���ͨ�ŷ����˹���
	if(ATT7022E_Check_Result_Get(Num_ATT7022E) == 0)
	{
		printf("��ȡ�������ʱ����ͨ�Ź���\r\n");
		return;
	}
	
	//����ɹ��ʵ�λֵ����λ: �㣩
	if(Data_PhSregpq_A >= pow(2, 20))
	{
		Data_PhSregpq_A -= pow(2, 24);
		Flag_Negative = 1;
	}
	PhSregpq_A = ((float)Data_PhSregpq_A / pow(2, 20)) * 180;
	if(Flag_Negative)
	{
		PhSregpq_A = -PhSregpq_A;
		Flag_Negative = 0;
	}
	
	if(Data_PhSregpq_B >= pow(2, 20))
	{
		Data_PhSregpq_B -= pow(2, 24);
		Flag_Negative = 1;
	}
	PhSregpq_B = ((float)Data_PhSregpq_B / pow(2, 20)) * 180;
	if(Flag_Negative)
	{
		PhSregpq_B = -PhSregpq_B;
		Flag_Negative = 0;
	}
	
	if(Data_PhSregpq_C >= pow(2, 20))
	{
		Data_PhSregpq_C -= pow(2, 24);
		Flag_Negative = 1;
	}
	PhSregpq_C = ((float)Data_PhSregpq_C / pow(2, 20)) * 180;
	if(Flag_Negative)
	{
		PhSregpq_C = -PhSregpq_C;
		Flag_Negative = 0;
	}
	
	//�����ȫ�ֱ�����
	LTU_Data.ATT7022E.PhSregpqA[Num_ATT7022E] = PhSregpq_A;
	LTU_Data.ATT7022E.PhSregpqB[Num_ATT7022E] = PhSregpq_B;
	LTU_Data.ATT7022E.PhSregpqC[Num_ATT7022E] = PhSregpq_C;
}
*/



//��ȡ��ǰ˲ʱ����
void ATT7022E_Get_Power(uint8_t Num_ATT7022E)
{
	extern Struct_LTU_ATT7022E_Param_Reg LTU_ATT7022E_AdjustParam_Reg;
	extern Struct_LTU_Data                 LTU_Data;
	
	//�洢�ӼĴ�����ȡ����ԭʼ����
	int Data_Pa  = 0;
	int Data_Pb  = 0;
	int Data_Pc  = 0;
	int Data_Pt  = 0;
	int Data_Qa  = 0;
	int Data_Qb  = 0;
	int Data_Qc  = 0;
	int Data_Qt  = 0;
	int Data_Sa  = 0;
	int Data_Sb  = 0;
	int Data_Sc  = 0;
	int Data_St  = 0;
	int Data_Pfa = 0;
	int Data_Pfb = 0;
	int Data_Pfc = 0;
	int Data_Pft = 0;
	
	//�洢ת���ɹ��ʵ�λ�Ƶ�����
	float Pa  = 0.0f;
	float Pb  = 0.0f;
	float Pc  = 0.0f;
	float Pt  = 0.0f;
	float Qa  = 0.0f;
	float Qb  = 0.0f;
	float Qc  = 0.0f;
	float Qt  = 0.0f;
	float Sa  = 0.0f;
	float Sb  = 0.0f;
	float Sc  = 0.0f;
	float St  = 0.0f;
	float Pfa = 0.0f;
	float Pfb = 0.0f;
	float Pfc = 0.0f;
	float Pft = 0.0f;
	
	//�ӹ��ʼĴ����ж�ȡ��ԭʼֵ
	Data_Pa  = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rPa);
	Data_Pb  = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rPb);
	Data_Pc  = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rPc);
	Data_Pt  = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rPt);
	Data_Qa  = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rQa);
	Data_Qb  = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rQb);
	Data_Qc  = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rQc);
	Data_Qt  = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rQt);
	Data_Sa  = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rSa);
	Data_Sb  = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rSb);
	Data_Sc  = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rSc);
	Data_St  = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rSt);
	Data_Pfa = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rPfa);
	Data_Pfb = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rPfb);
	Data_Pfc = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rPfc);
	Data_Pft = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rPft);
	
	//���ͨ�ŷ����˹���
	if(ATT7022E_Check_Result_Get(Num_ATT7022E) == 0)
	{
		printf("��ȡ��������ʱ����ͨ�Ź���\r\n");
		return;
	}
	
	//����ɹ��ʵ�λֵ
	if(Data_Pa > pow(2, 23))
	{
		Data_Pa -= pow(2, 24);
	}
	Pa = (float)Data_Pa * LTU_ATT7022E_AdjustParam_Reg.K;
	if(Data_Pb > pow(2, 23))
	{
		Data_Pb -= pow(2, 24);
	}
	Pb = (float)Data_Pb * LTU_ATT7022E_AdjustParam_Reg.K;
	if(Data_Pc > pow(2, 23))
	{
		Data_Pc -= pow(2, 24);
	}
	Pc = (float)Data_Pc * LTU_ATT7022E_AdjustParam_Reg.K;
	if(Data_Pt > pow(2, 23))
	{
		Data_Pt -= pow(2, 24);
	}
	Pt = (float)Data_Pt * 2 * LTU_ATT7022E_AdjustParam_Reg.K;
	if(Data_Qa > pow(2, 23))
	{
		Data_Qa -= pow(2, 24);
	}
	Qa = (float)Data_Qa * LTU_ATT7022E_AdjustParam_Reg.K;
	if(Data_Qb > pow(2, 23))
	{
		Data_Qb -= pow(2, 24);
	}
	Qb = (float)Data_Qb * LTU_ATT7022E_AdjustParam_Reg.K;
	if(Data_Qc > pow(2, 23))
	{
		Data_Qc -= pow(2, 24);
	}
	Qc = (float)Data_Qc * LTU_ATT7022E_AdjustParam_Reg.K;
	if(Data_Qt > pow(2, 23))
	{
		Data_Qt -= pow(2, 24);
	}
	Qt = (float)Data_Qt * 2 * LTU_ATT7022E_AdjustParam_Reg.K;
	if(Data_Sa > pow(2, 23))
	{
		Data_Sa -= pow(2, 24);
	}
	Sa = (float)Data_Sa * LTU_ATT7022E_AdjustParam_Reg.K;
	if(Data_Sb > pow(2, 23))
	{
		Data_Sb -= pow(2, 24);
	}
	Sb = (float)Data_Sb * LTU_ATT7022E_AdjustParam_Reg.K;
	if(Data_Sc > pow(2, 23))
	{
		Data_Sc -= pow(2, 24);
	}
	Sc = (float)Data_Sc * LTU_ATT7022E_AdjustParam_Reg.K;
	if(Data_St > pow(2, 23))
	{
		Data_St -= pow(2, 24);
	}
	St = (float)Data_St * 2 * LTU_ATT7022E_AdjustParam_Reg.K;
	if(Data_Pfa > pow(2, 23))
	{
		Data_Pfa -= pow(2, 24);
	}
	Pfa = (float)Data_Pfa / pow(2, 23);
	if(Data_Pfb > pow(2, 23))
	{
		Data_Pfb -= pow(2, 24);
	}
	Pfb = (float)Data_Pfb / pow(2, 23);
	if(Data_Pfc > pow(2, 23))
	{
		Data_Pfc -= pow(2, 24);
	}
	Pfc = (float)Data_Pfc / pow(2, 23);
	if(Data_Pft > pow(2, 23))
	{
		Data_Pft -= pow(2, 24);
	}
	Pft = (float)Data_Pft / pow(2, 23);

#ifdef LTU_TRANSFORMS_RATE
	//printf("\n before LTU_Transformer_Rate.Numerator=%d, Denominator=%d, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f\n", LTU_Transformer_Rate.Numerator, LTU_Transformer_Rate.Denominator, Pa, Pb, Pc, Pt, Qa, Qb, Qc,Qt, Sa, Sb, Sc, St, Pfa, Pfb, Pfc, Pft);

	Pa = (float)(LTU_Transformer_Rate.Numerator / LTU_Transformer_Rate.Denominator)*Pa;
	Pb = (float)(LTU_Transformer_Rate.Numerator / LTU_Transformer_Rate.Denominator)*Pb;
	Pc = (float)(LTU_Transformer_Rate.Numerator / LTU_Transformer_Rate.Denominator)*Pc;
	Pt = (float)(LTU_Transformer_Rate.Numerator / LTU_Transformer_Rate.Denominator)*Pt;
	Qa = (float)(LTU_Transformer_Rate.Numerator / LTU_Transformer_Rate.Denominator)*Qa;
	Qb = (float)(LTU_Transformer_Rate.Numerator / LTU_Transformer_Rate.Denominator)*Qb;
	Qc = (float)(LTU_Transformer_Rate.Numerator / LTU_Transformer_Rate.Denominator)*Qc;
	Qt = (float)(LTU_Transformer_Rate.Numerator / LTU_Transformer_Rate.Denominator)*Qt;
	Sa = (float)(LTU_Transformer_Rate.Numerator / LTU_Transformer_Rate.Denominator)*Sa;
	Sb = (float)(LTU_Transformer_Rate.Numerator / LTU_Transformer_Rate.Denominator)*Sb;
	Sc = (float)(LTU_Transformer_Rate.Numerator / LTU_Transformer_Rate.Denominator)*Sc;
	St = (float)(LTU_Transformer_Rate.Numerator / LTU_Transformer_Rate.Denominator)*St;
	
	//printf("\n after LTU_Transformer_Rate.Numerator=%d, Denominator=%d, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f\n", LTU_Transformer_Rate.Numerator, LTU_Transformer_Rate.Denominator, Pa, Pb, Pc, Pt, Qa, Qb, Qc,Qt, Sa, Sb, Sc, St, Pfa, Pfb, Pfc, Pft);
#endif
	//�����ȫ�ֱ�����
	LTU_Data.ATT7022E.PA[Num_ATT7022E]  = Pa;
	LTU_Data.ATT7022E.PB[Num_ATT7022E]  = Pb;
	LTU_Data.ATT7022E.PC[Num_ATT7022E]  = Pc;
	LTU_Data.ATT7022E.PT[Num_ATT7022E]  = Pt;
	LTU_Data.ATT7022E.QA[Num_ATT7022E]  = Qa;
	LTU_Data.ATT7022E.QB[Num_ATT7022E]  = Qb;
	LTU_Data.ATT7022E.QC[Num_ATT7022E]  = Qc;
	LTU_Data.ATT7022E.QT[Num_ATT7022E]  = Qt;
	LTU_Data.ATT7022E.SA[Num_ATT7022E]  = Sa;
	LTU_Data.ATT7022E.SB[Num_ATT7022E]  = Sb;
	LTU_Data.ATT7022E.SC[Num_ATT7022E]  = Sc;
	LTU_Data.ATT7022E.ST[Num_ATT7022E]  = St;
	LTU_Data.ATT7022E.PfA[Num_ATT7022E] = Pfa;
	LTU_Data.ATT7022E.PfB[Num_ATT7022E] = Pfb;
	LTU_Data.ATT7022E.PfC[Num_ATT7022E] = Pfc;
	LTU_Data.ATT7022E.PfT[Num_ATT7022E] = Pft;
	//printf("\nNum_ATT7022E=%d %6.4f, %6.4f, %6.4f, %6.4f\n", Num_ATT7022E, LTU_Data.ATT7022E.PA[Num_ATT7022E], LTU_Data.ATT7022E.PB[Num_ATT7022E], LTU_Data.ATT7022E.PC[Num_ATT7022E], LTU_Data.ATT7022E.PT[Num_ATT7022E]);
}



/*
//��ȡ��ǰ˲ʱ����
void ATT7022E_Get_Power(uint8_t Num_ATT7022E)
{
	extern Struct_LTU_ATT7022E_Param_Reg LTU_ATT7022E_AdjustParam_Reg;
	extern Struct_LTU_Data                 LTU_Data;
	
	//�洢�ӼĴ�����ȡ����ԭʼ����
	int Data_Pa  = 0;
	int Data_Pb  = 0;
	int Data_Pc  = 0;
	int Data_Pt  = 0;
	int Data_Qa  = 0;
	int Data_Qb  = 0;
	int Data_Qc  = 0;
	int Data_Qt  = 0;
	int Data_Sa  = 0;
	int Data_Sb  = 0;
	int Data_Sc  = 0;
	int Data_St  = 0;
	int Data_Pfa = 0;
	int Data_Pfb = 0;
	int Data_Pfc = 0;
	int Data_Pft = 0;
	
	//���ڱ궨�Ƿ���
	uint8_t Flag_Negative = 0;
	
	//�洢ת���ɹ��ʵ�λ�Ƶ�����
	float Pa  = 0.0f;
	float Pb  = 0.0f;
	float Pc  = 0.0f;
	float Pt  = 0.0f;
	float Qa  = 0.0f;
	float Qb  = 0.0f;
	float Qc  = 0.0f;
	float Qt  = 0.0f;
	float Sa  = 0.0f;
	float Sb  = 0.0f;
	float Sc  = 0.0f;
	float St  = 0.0f;
	float Pfa = 0.0f;
	float Pfb = 0.0f;
	float Pfc = 0.0f;
	float Pft = 0.0f;
	
	//�ӹ��ʼĴ����ж�ȡ��ԭʼֵ
	Data_Pa  = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rPa);
	Data_Pb  = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rPb);
	Data_Pc  = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rPc);
	Data_Pt  = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rPt);
	Data_Qa  = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rQa);
	Data_Qb  = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rQb);
	Data_Qc  = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rQc);
	Data_Qt  = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rQt);
	Data_Sa  = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rSa);
	Data_Sb  = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rSb);
	Data_Sc  = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rSc);
	Data_St  = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rSt);
	Data_Pfa = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rPfa);
	Data_Pfb = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rPfb);
	Data_Pfc = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rPfc);
	Data_Pft = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rPft);
	
	//���ͨ�ŷ����˹���
	if(ATT7022E_Check_Result_Get(Num_ATT7022E) == 0)
	{
		printf("��ȡ��������ʱ����ͨ�Ź���\r\n");
		return;
	}
	
	//����ɹ��ʵ�λֵ
	if(Data_Pa > pow(2, 23))
	{
		Data_Pa -= pow(2, 24);
		Flag_Negative = 1;
	}
	Pa = (float)Data_Pa * LTU_ATT7022E_AdjustParam_Reg.K;
	if(Flag_Negative)
	{
		Pa = -Pa;
		Flag_Negative = 0;
	}
	
	if(Data_Pb > pow(2, 23))
	{
		Data_Pb -= pow(2, 24);
		Flag_Negative = 1;
	}
	Pb = (float)Data_Pb * LTU_ATT7022E_AdjustParam_Reg.K;
	if(Flag_Negative)
	{
		Pb = -Pb;
		Flag_Negative = 0;
	}
	
	if(Data_Pc > pow(2, 23))
	{
		Data_Pc -= pow(2, 24);
		Flag_Negative = 1;
	}
	Pc = (float)Data_Pc * LTU_ATT7022E_AdjustParam_Reg.K;
	if(Flag_Negative)
	{
		Pc = -Pc;
		Flag_Negative = 0;
	}
	
	if(Data_Pt > pow(2, 23))
	{
		Data_Pt -= pow(2, 24);
		Flag_Negative = 1;
	}
	Pt = (float)Data_Pt * 2 * LTU_ATT7022E_AdjustParam_Reg.K;
	if(Flag_Negative)
	{
		Pt = -Pt;
		Flag_Negative = 0;
	}
	
	if(Data_Qa > pow(2, 23))
	{
		Data_Qa -= pow(2, 24);
		Flag_Negative = 1;
	}
	Qa = (float)Data_Qa * LTU_ATT7022E_AdjustParam_Reg.K;
	if(Flag_Negative)
	{
		Qa = -Qa;
		Flag_Negative = 0;
	}
	
	if(Data_Qb > pow(2, 23))
	{
		Data_Qb -= pow(2, 24);
		Flag_Negative = 1;
	}
	Qb = (float)Data_Qb * LTU_ATT7022E_AdjustParam_Reg.K;
	if(Flag_Negative)
	{
		Qb = -Qb;
		Flag_Negative = 0;
	}
	
	if(Data_Qc > pow(2, 23))
	{
		Data_Qc -= pow(2, 24);
		Flag_Negative = 1;
	}
	Qc = (float)Data_Qc * LTU_ATT7022E_AdjustParam_Reg.K;
	if(Flag_Negative)
	{
		Qc = -Qc;
		Flag_Negative = 0;
	}
	
	if(Data_Qt > pow(2, 23))
	{
		Data_Qt -= pow(2, 24);
		Flag_Negative = 1;
	}
	Qt = (float)Data_Qt * 2 * LTU_ATT7022E_AdjustParam_Reg.K;
	if(Flag_Negative)
	{
		Qt = -Qt;
		Flag_Negative = 0;
	}
	
	if(Data_Sa > pow(2, 23))
	{
		Data_Sa -= pow(2, 24);
		Flag_Negative = 1;
	}
	Sa = (float)Data_Sa * LTU_ATT7022E_AdjustParam_Reg.K;
	if(Flag_Negative)
	{
		Sa = -Sa;
		Flag_Negative = 0;
	}
	
	if(Data_Sb > pow(2, 23))
	{
		Data_Sb -= pow(2, 24);
		Flag_Negative = 1;
	}
	Sb = (float)Data_Sb * LTU_ATT7022E_AdjustParam_Reg.K;
	if(Flag_Negative)
	{
		Sb = -Sb;
		Flag_Negative = 0;
	}
	
	if(Data_Sc > pow(2, 23))
	{
		Data_Sc -= pow(2, 24);
		Flag_Negative = 1;
	}
	Sc = (float)Data_Sc * LTU_ATT7022E_AdjustParam_Reg.K;
	if(Flag_Negative)
	{
		Sc = -Sc;
		Flag_Negative = 0;
	}
	
	if(Data_St > pow(2, 23))
	{
		Data_St -= pow(2, 24);
		Flag_Negative = 1;
	}
	St = (float)Data_St * 2 * LTU_ATT7022E_AdjustParam_Reg.K;
	if(Flag_Negative)
	{
		St = -St;
		Flag_Negative = 0;
	}
	
	if(Data_Pfa > pow(2, 23))
	{
		Data_Pfa -= pow(2, 24);
		Flag_Negative = 1;
	}
	Pfa = (float)Data_Pfa / pow(2, 23);
	if(Flag_Negative)
	{
		Pfa = -Pfa;
		Flag_Negative = 0;
	}
	
	if(Data_Pfb > pow(2, 23))
	{
		Data_Pfb -= pow(2, 24);
		Flag_Negative = 1;
	}
	Pfb = (float)Data_Pfb / pow(2, 23);
	if(Flag_Negative)
	{
		Pfb = -Pfb;
		Flag_Negative = 0;
	}
	
	if(Data_Pfc > pow(2, 23))
	{
		Data_Pfc -= pow(2, 24);
		Flag_Negative = 1;
	}
	Pfc = (float)Data_Pfc / pow(2, 23);
	if(Flag_Negative)
	{
		Pfc = -Pfc;
		Flag_Negative = 0;
	}
	
	if(Data_Pft > pow(2, 23))
	{
		Data_Pft -= pow(2, 24);
		Flag_Negative = 1;
	}
	Pft = (float)Data_Pft / pow(2, 23);
	if(Flag_Negative)
	{
		Pft = -Pft;
		Flag_Negative = 0;
	}
	
	//�����ȫ�ֱ�����
	LTU_Data.ATT7022E.PA[Num_ATT7022E]  = Pa;
	LTU_Data.ATT7022E.PB[Num_ATT7022E]  = Pb;
	LTU_Data.ATT7022E.PC[Num_ATT7022E]  = Pc;
	LTU_Data.ATT7022E.PT[Num_ATT7022E]  = Pt;
	LTU_Data.ATT7022E.QA[Num_ATT7022E]  = Qa;
	LTU_Data.ATT7022E.QB[Num_ATT7022E]  = Qb;
	LTU_Data.ATT7022E.QC[Num_ATT7022E]  = Qc;
	LTU_Data.ATT7022E.QT[Num_ATT7022E]  = Qt;
	LTU_Data.ATT7022E.SA[Num_ATT7022E]  = Sa;
	LTU_Data.ATT7022E.SB[Num_ATT7022E]  = Sb;
	LTU_Data.ATT7022E.SC[Num_ATT7022E]  = Sc;
	LTU_Data.ATT7022E.ST[Num_ATT7022E]  = St;
	LTU_Data.ATT7022E.PfA[Num_ATT7022E] = Pfa;
	LTU_Data.ATT7022E.PfB[Num_ATT7022E] = Pfb;
	LTU_Data.ATT7022E.PfC[Num_ATT7022E] = Pfc;
	LTU_Data.ATT7022E.PfT[Num_ATT7022E] = Pft;
}
*/



//��ȡ��ѹƵ��
void ATT7022E_Get_Frequency(uint8_t Num_ATT7022E)
{
	extern Struct_LTU_Data LTU_Data;
	
	int   Data_Freq = 0;
	float Freq      = 0.0f;
	
	//��Ƶ�ʼĴ����л�ȡ��ԭʼֵ
	Data_Freq = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rFreq);
	
	if(ATT7022E_Check_Result_Get(Num_ATT7022E) == 0)
	{
		printf("��ȡ������Ƶ�ʷ���ͨ�Ź���\r\n");
		return;
	}
	
	//����ɹ��ʵ�λ��
	Freq = (float)Data_Freq / pow(2, 13);
	
	//���ABC��������һ�෢�����࣬��ô˵��Ƶ���жϲ�׼ȷ��ֱ�ӹ��㼴��
	if((LTU_Data.Event.EVA[Num_ATT7022E] & ACSAMPLE_EVENT_NO_PHSREGPQ) ||
	   (LTU_Data.Event.EVB[Num_ATT7022E] & ACSAMPLE_EVENT_NO_PHSREGPQ) ||
	   (LTU_Data.Event.EVC[Num_ATT7022E] & ACSAMPLE_EVENT_NO_PHSREGPQ))
	{
		Freq = 0.0f;
	}
	
	//�����ȫ�ֱ�����
	LTU_Data.ATT7022E.Frequency[Num_ATT7022E] = Freq;
}



//��ȡ����
void ATT7022E_Get_Energy(uint8_t Num_ATT7022E)
{
	extern Struct_LTU_ATT7022E_Param_Reg LTU_ATT7022E_AdjustParam_Reg;
	extern Struct_LTU_Data               LTU_Data;
	
	//�ӼĴ����ж�ȡ������ԭʼֵ
	int32 Data_EPa;
	int32 Data_EPb;
	int32 Data_EPc;
	int32 Data_EPt;
	int32 Data_EQa;
	int32 Data_EQb;
	int32 Data_EQc;
	int32 Data_EQt;
	int32 Data_ESa;
	int32 Data_ESb;
	int32 Data_ESc;
	int32 Data_ESt;
	int32 Data_PDFlag;
	
	//ת��Ϊ���ʵ�λ�Ƶ�ֵ
	float EPa;
	float EPb;
	float EPc;
	float EPt;
	float EQa;
	float EQb;
	float EQc;
	float EQt;
	float ESa;
	float ESb;
	float ESc;
	float ESt;
	
	//�ӼĴ������л�ȡ�����ܵ�ԭʼֵ
	Data_EPa    = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rEpa);
	Data_EPb    = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rEpb);
	Data_EPc    = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rEpc);
	Data_EPt    = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rEpt);
	Data_EQa    = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rEqa);
	Data_EQb    = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rEqb);
	Data_EQc    = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rEqc);
	Data_EQt    = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rEqt);
	Data_ESa    = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rEsa);
	Data_ESb    = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rEsb);
	Data_ESc    = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rEsc);
	Data_ESt    = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rEst);
	Data_PDFlag = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rPFlag);  //��ȡ���ʷ���
	
	//ת���ɹ��ʵ�λ�Ƶ�ֵ��kWh��
	EPa = (float)Data_EPa / LTU_ATT7022E_AdjustParam_Reg._EC;
	EPb = (float)Data_EPb / LTU_ATT7022E_AdjustParam_Reg._EC;
	EPc = (float)Data_EPc / LTU_ATT7022E_AdjustParam_Reg._EC;
	EPt = (float)Data_EPt / LTU_ATT7022E_AdjustParam_Reg._EC;
	EQa = (float)Data_EQa / LTU_ATT7022E_AdjustParam_Reg._EC;
	EQb = (float)Data_EQb / LTU_ATT7022E_AdjustParam_Reg._EC;
	EQc = (float)Data_EQc / LTU_ATT7022E_AdjustParam_Reg._EC;
	EQt = (float)Data_EQt / LTU_ATT7022E_AdjustParam_Reg._EC;
	ESa = (float)Data_ESa / LTU_ATT7022E_AdjustParam_Reg._EC;
	ESb = (float)Data_ESb / LTU_ATT7022E_AdjustParam_Reg._EC;
	ESc = (float)Data_ESc / LTU_ATT7022E_AdjustParam_Reg._EC;
	ESt = (float)Data_ESt / LTU_ATT7022E_AdjustParam_Reg._EC;

#ifdef LTU_TRANSFORMS_RATE
	EPa = (float)(LTU_Transformer_Rate.Numerator / LTU_Transformer_Rate.Denominator)*EPa;
	EPb = (float)(LTU_Transformer_Rate.Numerator / LTU_Transformer_Rate.Denominator)*EPb;
	EPc = (float)(LTU_Transformer_Rate.Numerator / LTU_Transformer_Rate.Denominator)*EPc;
	EPt = (float)(LTU_Transformer_Rate.Numerator / LTU_Transformer_Rate.Denominator)*EPt;
	EQa = (float)(LTU_Transformer_Rate.Numerator / LTU_Transformer_Rate.Denominator)*EQa;
	EQb = (float)(LTU_Transformer_Rate.Numerator / LTU_Transformer_Rate.Denominator)*EQb;
	EQc = (float)(LTU_Transformer_Rate.Numerator / LTU_Transformer_Rate.Denominator)*EQc;
	EQt = (float)(LTU_Transformer_Rate.Numerator / LTU_Transformer_Rate.Denominator)*EQt;
	ESa = (float)(LTU_Transformer_Rate.Numerator / LTU_Transformer_Rate.Denominator)*ESa;
	ESb = (float)(LTU_Transformer_Rate.Numerator / LTU_Transformer_Rate.Denominator)*ESb;
	ESc = (float)(LTU_Transformer_Rate.Numerator / LTU_Transformer_Rate.Denominator)*ESc;
	ESt = (float)(LTU_Transformer_Rate.Numerator / LTU_Transformer_Rate.Denominator)*ESt;
#endif
	
	//���������й��ܵ��ܡ������й��ܵ���
	if(Data_PDFlag & BIT(3))  //�жϵ�ǰ�����������Ƿ���
	{
		LTU_Data.ATT7022E.EPTR[Num_ATT7022E] += EPt;  //�����й��ܵ���
	}
	else
	{
		LTU_Data.ATT7022E.EPTF[Num_ATT7022E] += EPt;  //�����й��ܵ���
	}
	
	//�����������й��޹��������ȫ�ֱ�����
	switch(Data_PDFlag & 0x88)
	{
		//һ�������޹��ܵ���
		case 0x00:
		LTU_Data.ATT7022E.Eq1_P[Num_ATT7022E] += EPt;
		LTU_Data.ATT7022E.Eq1_Q[Num_ATT7022E] += EQt;
		break;
		
		//���������޹��ܵ���
		case 0x08:
		LTU_Data.ATT7022E.Eq2_P[Num_ATT7022E] += EPt;
		LTU_Data.ATT7022E.Eq2_Q[Num_ATT7022E] += EQt;
		break;
		
		//���������޹��ܵ���
		case 0x88:
		LTU_Data.ATT7022E.Eq3_P[Num_ATT7022E] += EPt;
		LTU_Data.ATT7022E.Eq3_Q[Num_ATT7022E] += EQt;
		break;
		
		//���������޹��ܵ���
		case 0x80:
		LTU_Data.ATT7022E.Eq4_P[Num_ATT7022E] += EPt;
		LTU_Data.ATT7022E.Eq4_Q[Num_ATT7022E] += EQt;
		break;
	}
	
	//�����ȫ�ֱ�����
	LTU_Data.ATT7022E.EPA[Num_ATT7022E] += EPa;
	LTU_Data.ATT7022E.EPB[Num_ATT7022E] += EPb;
	LTU_Data.ATT7022E.EPC[Num_ATT7022E] += EPc;
	LTU_Data.ATT7022E.EPT[Num_ATT7022E] += EPt;
	LTU_Data.ATT7022E.EQA[Num_ATT7022E] += EQa;
	LTU_Data.ATT7022E.EQB[Num_ATT7022E] += EQb;
	LTU_Data.ATT7022E.EQC[Num_ATT7022E] += EQc;
	LTU_Data.ATT7022E.EQT[Num_ATT7022E] += EQt;
	LTU_Data.ATT7022E.ESA[Num_ATT7022E] += ESa;
	LTU_Data.ATT7022E.ESB[Num_ATT7022E] += ESb;
	LTU_Data.ATT7022E.ESC[Num_ATT7022E] += ESc;
	LTU_Data.ATT7022E.EST[Num_ATT7022E] += ESt;
}



//�쳣������
void ATT7022E_Error_Handler(uint8_t Num_ATT7022E)
{
	printf("ATT7022E�����쳣�������λ��......\r\n");
	
	//�����λ
	ATT7022E_Reset_Soft(Num_ATT7022E);

	//���´�EEPROM��װ��У�����
	ATT7022E_Load_AdjustParam_From_EEPROM(Num_ATT7022E);
	
	//�����¼���ʼ��Ϊ0��ͨ�Ž����ʼ��Ϊ��ȷ
	ATT7022E_Check_Error_Set(Num_ATT7022E, 0);
	ATT7022E_Check_Result_Set(Num_ATT7022E, 1);
	
	printf("ATT7022E�����λ����\r\n");
}



//�����¼������������ؽ��ֵ
void ATT7022E_ACSample_Event(uint8_t Num)
{
	extern Struct_LTU_Data LTU_Data;
	float  Umax     = 0.0f;
	float  Umin     = 0.0f;
	float  Uaverage = 0.0f;
	float  Imax     = 0.0f;
	float  Imin     = 0.0f;
	float  Iaverage = 0.0f;
	
	static float YIA[ATT7022E_NUM_TOTAL] = {0.0f};
	static float YIB[ATT7022E_NUM_TOTAL] = {0.0f};
	static float YIC[ATT7022E_NUM_TOTAL] = {0.0f};
	
	//�ҳ���ѹ���ֵ
	if(LTU_Data.ATT7022E.UA[Num] > LTU_Data.ATT7022E.UB[Num])
	{
		Umax = LTU_Data.ATT7022E.UA[Num];
	}
	else
	{
		Umax = LTU_Data.ATT7022E.UB[Num];
	}
	if(LTU_Data.ATT7022E.UC[Num] > Umax)
	{
		Umax = LTU_Data.ATT7022E.UC[Num];
	}
	
	//��ѹƽ��ֵ
	Uaverage = (LTU_Data.ATT7022E.UA[Num] + LTU_Data.ATT7022E.UB[Num] + LTU_Data.ATT7022E.UC[Num]) / 3.0f;
	
	//�ҳ���ѹ��Сֵ
	if(LTU_Data.ATT7022E.UA[Num] < LTU_Data.ATT7022E.UB[Num])
	{
		Umin = LTU_Data.ATT7022E.UA[Num];
	}
	else
	{
		Umin = LTU_Data.ATT7022E.UB[Num];
	}
	if(LTU_Data.ATT7022E.UC[Num] < Umin)
	{
		Umin = LTU_Data.ATT7022E.UC[Num];
	}
	
	//�ҳ��������ֵ
	if(LTU_Data.ATT7022E.IA[Num] > LTU_Data.ATT7022E.IB[Num])
	{
		Imax = LTU_Data.ATT7022E.IA[Num];
	}
	else
	{
		Imax = LTU_Data.ATT7022E.IB[Num];
	}
	if(LTU_Data.ATT7022E.IC[Num] > Imax)
	{
		Imax = LTU_Data.ATT7022E.IC[Num];
	}
	
	//�ҳ�������Сֵ
	if(LTU_Data.ATT7022E.IA[Num] < LTU_Data.ATT7022E.IB[Num])
	{
		Imin = LTU_Data.ATT7022E.IA[Num];
	}
	else
	{
		Imin = LTU_Data.ATT7022E.IB[Num];
	}
	if(LTU_Data.ATT7022E.IC[Num] < Imin)
	{
		Imin = LTU_Data.ATT7022E.IC[Num];
	}
	
	//����ƽ��ֵ
	Iaverage = (LTU_Data.ATT7022E.IA[Num] + LTU_Data.ATT7022E.IB[Num] + LTU_Data.ATT7022E.IC[Num]) / 3.0f;
	
	//�����¼�
	
	//ʧѹ------------------------------------------------------------------
	
	//�ж�A���Ƿ�ʧѹ
	
	//���A��û�з���ʧѹ�¼�����ô����ʧѹ���
	if((LTU_Data.Event.EVA[Num] & ACSAMPLE_EVENT_NO_VOLTAGE) == 0)
	{
		//���A�����������ʼ����������A���ѹС���趨��ʧѹ��ֵ��˵����ǰ������ʧѹ�¼�
		if((LTU_Data.ATT7022E.UA[Num] < THRESHOLD_EVENT_NO_VOLTAGE_U) &&
		   (LTU_Data.ATT7022E.IA[Num] > THRESHOLD_EVENT_NO_VOLTAGE_I))
		{
			//��ر�־������1����������ʧѹ�¼�
			Flag_A_No_Voltage[Num] = 1;
			
			//��ʱ�����Ƿ񳬹���ֵ
			if(Count_A_No_Voltage[Num] >= TIME_EVENT_NO_VOLTAGE)
			{
				//�����־�����ͼ�ʱ����
				Flag_A_No_Voltage[Num]  = 0;
				Count_A_No_Voltage[Num] = 0;
				
				//A�෢��ʧѹ�¼�
				LTU_Data.Event.EVA[Num] |= ACSAMPLE_EVENT_NO_VOLTAGE;
			}
		}
		else
		{
			Flag_A_No_Voltage[Num]  = 0;
			Count_A_No_Voltage[Num] = 0;
		}
	}
	//���A���Ѿ�����ʧѹ�¼�
	else
	{
		//���A�����������ʼ����������A���ѹС���趨��ʧѹ��ֵ��˵����ǰ������ʧѹ�¼�
		if((LTU_Data.ATT7022E.UA[Num] < THRESHOLD_EVENT_NO_VOLTAGE_U) &&
		   (LTU_Data.ATT7022E.IA[Num] > THRESHOLD_EVENT_NO_VOLTAGE_I))
		{
			Flag_A_No_Voltage[Num]  = 0;
			Count_A_No_Voltage[Num] = 0;
		}
		//��ǰû�з���ʧѹ�¼�
		else
		{
			//��ر�־������2������û�з���ʧѹ�¼�
			Flag_A_No_Voltage[Num] = 2;
			
			//��ʱ�����Ƿ񳬹���ֵ
			if(Count_A_No_Voltage[Num] >= TIME_EVENT_NO_VOLTAGE)
			{
				//�����־�����ͼ�ʱ����
				Flag_A_No_Voltage[Num]  = 0;
				Count_A_No_Voltage[Num] = 0;
				
				//A��û�з���ʧѹ�¼�����ʧѹ�¼��лָ���
				LTU_Data.Event.EVA[Num] &= ~ACSAMPLE_EVENT_NO_VOLTAGE;
			}
		}
	}
	
	//�ж�B���Ƿ�ʧѹ
	
	if((LTU_Data.Event.EVB[Num] & ACSAMPLE_EVENT_NO_VOLTAGE) == 0)
	{
		if((LTU_Data.ATT7022E.UB[Num] < THRESHOLD_EVENT_NO_VOLTAGE_U) &&
		   (LTU_Data.ATT7022E.IB[Num] > THRESHOLD_EVENT_NO_VOLTAGE_I))
		{
			Flag_B_No_Voltage[Num] = 1;
			
			if(Count_B_No_Voltage[Num] >= TIME_EVENT_NO_VOLTAGE)
			{
				Flag_B_No_Voltage[Num]  = 0;
				Count_B_No_Voltage[Num] = 0;
				
				LTU_Data.Event.EVB[Num] |= ACSAMPLE_EVENT_NO_VOLTAGE;
			}
		}
		else
		{
			Flag_B_No_Voltage[Num]  = 0;
			Count_B_No_Voltage[Num] = 0;
		}
	}
	else
	{
		if((LTU_Data.ATT7022E.UB[Num] < THRESHOLD_EVENT_NO_VOLTAGE_U) &&
		   (LTU_Data.ATT7022E.IB[Num] > THRESHOLD_EVENT_NO_VOLTAGE_I))
		{
			Flag_B_No_Voltage[Num]  = 0;
			Count_B_No_Voltage[Num] = 0;
		}
		else
		{
			Flag_B_No_Voltage[Num] = 2;
			
			if(Count_B_No_Voltage[Num] >= TIME_EVENT_NO_VOLTAGE)
			{
				Flag_B_No_Voltage[Num]  = 0;
				Count_B_No_Voltage[Num] = 0;
				
				LTU_Data.Event.EVB[Num] &= ~ACSAMPLE_EVENT_NO_VOLTAGE;
			}
		}
	}
	
	//�ж�C���Ƿ�ʧѹ
	
	if((LTU_Data.Event.EVC[Num] & ACSAMPLE_EVENT_NO_VOLTAGE) == 0)
	{
		if((LTU_Data.ATT7022E.UC[Num] < THRESHOLD_EVENT_NO_VOLTAGE_U) &&
		   (LTU_Data.ATT7022E.IC[Num] > THRESHOLD_EVENT_NO_VOLTAGE_I))
		{
			Flag_C_No_Voltage[Num] = 1;
			
			if(Count_C_No_Voltage[Num] >= TIME_EVENT_NO_VOLTAGE)
			{
				Flag_C_No_Voltage[Num]  = 0;
				Count_C_No_Voltage[Num] = 0;
				
				LTU_Data.Event.EVC[Num] |= ACSAMPLE_EVENT_NO_VOLTAGE;
			}
		}
		else
		{
			Flag_C_No_Voltage[Num]  = 0;
			Count_C_No_Voltage[Num] = 0;
		}
	}
	else
	{
		if((LTU_Data.ATT7022E.UC[Num] < THRESHOLD_EVENT_NO_VOLTAGE_U) &&
		   (LTU_Data.ATT7022E.IC[Num] > THRESHOLD_EVENT_NO_VOLTAGE_I))
		{
			Flag_C_No_Voltage[Num]  = 0;
			Count_C_No_Voltage[Num] = 0;
		}
		else
		{
			Flag_C_No_Voltage[Num] = 2;
			
			if(Count_C_No_Voltage[Num] >= TIME_EVENT_NO_VOLTAGE)
			{
				Flag_C_No_Voltage[Num]  = 0;
				Count_C_No_Voltage[Num] = 0;
				
				LTU_Data.Event.EVC[Num] &= ~ACSAMPLE_EVENT_NO_VOLTAGE;
			}
		}
	}
	
	//Ƿѹ------------------------------------------------------------------
	
	//�ж�A���Ƿ�Ƿѹ
	
	//���A��û�з���Ƿѹ�¼�����ô����Ƿѹ���
	if((LTU_Data.Event.EVA[Num] & ACSAMPLE_EVENT_LOW_VOLTAGE) == 0)
	{
		//���A���ѹС���趨��Ƿѹ��ѹ��ֵ��������ʧѹ��ѹ��ֵ�����ҵ��������趨��Ƿѹ������ֵ��˵����ǰ������Ƿѹ�¼�
		if((LTU_Data.ATT7022E.UA[Num] < THRESHOLD_EVENT_LOW_VOLTAGE_U) &&
		   (LTU_Data.ATT7022E.UA[Num] > THRESHOLD_EVENT_NO_VOLTAGE_U)  &&
		   (LTU_Data.ATT7022E.IA[Num] > THRESHOLD_EVENT_LOW_VOLTAGE_I))
		{
			//��ر�־������1����������Ƿѹ�¼�
			Flag_A_Low_Voltage[Num] = 1;
			
			//��ʱ�����Ƿ񳬹���ֵ
			if(Count_A_Low_Voltage[Num] >= TIME_EVENT_LOW_VOLTAGE)
			{
				//�����־�����ͼ�ʱ����
				Flag_A_Low_Voltage[Num]  = 0;
				Count_A_Low_Voltage[Num] = 0;
				
				//A�෢��Ƿѹ�¼�
				LTU_Data.Event.EVA[Num] |= ACSAMPLE_EVENT_LOW_VOLTAGE;
			}
		}
		else
		{
			Flag_A_Low_Voltage[Num]  = 0;
			Count_A_Low_Voltage[Num] = 0;
		}
	}
	//���A���Ѿ�����ʧѹ�¼�
	else
	{
		//���A���ѹС���趨��Ƿѹ��ѹ��ֵ�����ҵ��������趨��Ƿѹ������ֵ��˵����ǰ������Ƿѹ�¼�
		if((LTU_Data.ATT7022E.UA[Num] < THRESHOLD_EVENT_LOW_VOLTAGE_U) &&
		   (LTU_Data.ATT7022E.UA[Num] > THRESHOLD_EVENT_NO_VOLTAGE_U)  &&
		   (LTU_Data.ATT7022E.IA[Num] > THRESHOLD_EVENT_LOW_VOLTAGE_I))
		{
			Flag_A_Low_Voltage[Num]  = 0;
			Count_A_Low_Voltage[Num] = 0;
		}
		else
		{
			//��ر�־������2������û�з���Ƿѹ�¼�
			Flag_A_Low_Voltage[Num] = 2;
			
			//��ʱ�����Ƿ񳬹���ֵ
			if(Count_A_Low_Voltage[Num] >= TIME_EVENT_LOW_VOLTAGE)
			{
				//�����־�����ͼ�ʱ����
				Flag_A_Low_Voltage[Num]  = 0;
				Count_A_Low_Voltage[Num] = 0;
				
				//A��û�з���Ƿѹ�¼�����Ƿѹ�¼��лָ���
				LTU_Data.Event.EVA[Num] &= ~ACSAMPLE_EVENT_LOW_VOLTAGE;
			}
		}
	}
	
	//�ж�B���Ƿ�Ƿѹ
	
	if((LTU_Data.Event.EVB[Num] & ACSAMPLE_EVENT_LOW_VOLTAGE) == 0)
	{
		if((LTU_Data.ATT7022E.UB[Num] < THRESHOLD_EVENT_LOW_VOLTAGE_U) &&
		   (LTU_Data.ATT7022E.UB[Num] > THRESHOLD_EVENT_NO_VOLTAGE_U)  &&
		   (LTU_Data.ATT7022E.IB[Num] > THRESHOLD_EVENT_LOW_VOLTAGE_I))
		{
			Flag_B_Low_Voltage[Num] = 1;
			
			if(Count_B_Low_Voltage[Num] >= TIME_EVENT_LOW_VOLTAGE)
			{
				Flag_B_Low_Voltage[Num]  = 0;
				Count_B_Low_Voltage[Num] = 0;
				
				LTU_Data.Event.EVB[Num] |= ACSAMPLE_EVENT_LOW_VOLTAGE;
			}
		}
		else
		{
			Flag_B_Low_Voltage[Num]  = 0;
			Count_B_Low_Voltage[Num] = 0;
		}
	}
	else
	{
		if((LTU_Data.ATT7022E.UB[Num] < THRESHOLD_EVENT_LOW_VOLTAGE_U) &&
		   (LTU_Data.ATT7022E.UB[Num] > THRESHOLD_EVENT_NO_VOLTAGE_U)  &&
		   (LTU_Data.ATT7022E.IB[Num] > THRESHOLD_EVENT_LOW_VOLTAGE_I))
		{
			Flag_B_Low_Voltage[Num]  = 0;
			Count_B_Low_Voltage[Num] = 0;
		}
		else
		{
			Flag_B_Low_Voltage[Num] = 2;
			
			if(Count_B_Low_Voltage[Num] >= TIME_EVENT_LOW_VOLTAGE)
			{
				Flag_B_Low_Voltage[Num]  = 0;
				Count_B_Low_Voltage[Num] = 0;
				
				LTU_Data.Event.EVB[Num] &= ~ACSAMPLE_EVENT_LOW_VOLTAGE;
			}
		}
	}
	
	//�ж�C���Ƿ�Ƿѹ
	
	if((LTU_Data.Event.EVC[Num] & ACSAMPLE_EVENT_LOW_VOLTAGE) == 0)
	{
		if((LTU_Data.ATT7022E.UC[Num] < THRESHOLD_EVENT_LOW_VOLTAGE_U) &&
		   (LTU_Data.ATT7022E.UC[Num] > THRESHOLD_EVENT_NO_VOLTAGE_U)  &&
		   (LTU_Data.ATT7022E.IC[Num] > THRESHOLD_EVENT_LOW_VOLTAGE_I))
		{
			Flag_C_Low_Voltage[Num] = 1;
			
			if(Count_C_Low_Voltage[Num] >= TIME_EVENT_LOW_VOLTAGE)
			{
				Flag_C_Low_Voltage[Num]  = 0;
				Count_C_Low_Voltage[Num] = 0;
				
				LTU_Data.Event.EVC[Num] |= ACSAMPLE_EVENT_LOW_VOLTAGE;
			}
		}
		else
		{
			Flag_C_Low_Voltage[Num]  = 0;
			Count_C_Low_Voltage[Num] = 0;
		}
	}
	else
	{
		if((LTU_Data.ATT7022E.UC[Num] < THRESHOLD_EVENT_LOW_VOLTAGE_U) &&
		   (LTU_Data.ATT7022E.UC[Num] > THRESHOLD_EVENT_NO_VOLTAGE_U)  &&
		   (LTU_Data.ATT7022E.IC[Num] > THRESHOLD_EVENT_LOW_VOLTAGE_I))
		{
			Flag_C_Low_Voltage[Num]  = 0;
			Count_C_Low_Voltage[Num] = 0;
		}
		else
		{
			Flag_C_Low_Voltage[Num] = 2;
			
			if(Count_C_Low_Voltage[Num] >= TIME_EVENT_LOW_VOLTAGE)
			{
				Flag_C_Low_Voltage[Num]  = 0;
				Count_C_Low_Voltage[Num] = 0;
				
				LTU_Data.Event.EVC[Num] &= ~ACSAMPLE_EVENT_LOW_VOLTAGE;
			}
		}
	}
	
	//��ѹ------------------------------------------------------------------
	
	//�ж�A���Ƿ��ѹ
	
	//���A��û�з�����ѹ�¼�����ô���й�ѹ���
	if((LTU_Data.Event.EVA[Num] & ACSAMPLE_EVENT_OVER_VOLTAGE) == 0)
	{
		//���A���ѹ�����趨�Ĺ�ѹ��ֵ��˵����ǰ�����˹�ѹ�¼�
		if(LTU_Data.ATT7022E.UA[Num] > THRESHOLD_EVENT_OVER_VOLTAGE_U)
		{
			//��ر�־������1����������Ƿѹ�¼�
			Flag_A_Over_Voltage[Num] = 1;
			
			//��ʱ�����Ƿ񳬹���ֵ
			if(Count_A_Over_Voltage[Num] >= TIME_EVENT_OVER_VOLTAGE)
			{
				//�����־�����ͼ�ʱ����
				Flag_A_Over_Voltage[Num]  = 0;
				Count_A_Over_Voltage[Num] = 0;
				
				//A�෢����ѹ�¼�
				LTU_Data.Event.EVA[Num] |= ACSAMPLE_EVENT_OVER_VOLTAGE;
			}
		}
		else
		{
			Flag_A_Over_Voltage[Num]  = 0;
			Count_A_Over_Voltage[Num] = 0;
		}
	}
	//���A���Ѿ�������ѹ�¼�
	else
	{
		//���A���ѹ�����趨�Ĺ�ѹ��ֵ��˵����ǰ�����˹�ѹ�¼�
		if(LTU_Data.ATT7022E.UA[Num] > THRESHOLD_EVENT_OVER_VOLTAGE_U)
		{
			Flag_A_Over_Voltage[Num]  = 0;
			Count_A_Over_Voltage[Num] = 0;
		}
		//û�з�����ѹ�¼�
		else
		{
			//��ر�־������2������û�з�����ѹ�¼�
			Flag_A_Over_Voltage[Num] = 2;
			
			//��ʱ�����Ƿ񳬹���ֵ
			if(Count_A_Over_Voltage[Num] >= TIME_EVENT_OVER_VOLTAGE)
			{
				//�����־�����ͼ�ʱ����
				Flag_A_Over_Voltage[Num]  = 0;
				Count_A_Over_Voltage[Num] = 0;
				
				//A��û�з�����ѹ�¼����ӹ�ѹ�¼��лָ���
				LTU_Data.Event.EVA[Num] &= ~ACSAMPLE_EVENT_OVER_VOLTAGE;
			}
		}
	}
	
	//�ж�B���Ƿ��ѹ
	
	if((LTU_Data.Event.EVB[Num] & ACSAMPLE_EVENT_OVER_VOLTAGE) == 0)
	{
		if(LTU_Data.ATT7022E.UB[Num] > THRESHOLD_EVENT_OVER_VOLTAGE_U)
		{
			Flag_B_Over_Voltage[Num] = 1;
			
			if(Count_B_Over_Voltage[Num] >= TIME_EVENT_OVER_VOLTAGE)
			{
				Flag_B_Over_Voltage[Num]  = 0;
				Count_B_Over_Voltage[Num] = 0;
				
				LTU_Data.Event.EVB[Num] |= ACSAMPLE_EVENT_OVER_VOLTAGE;
			}
		}
		else
		{
			Flag_B_Over_Voltage[Num]  = 0;
			Count_B_Over_Voltage[Num] = 0;
		}
	}
	else
	{
		if(LTU_Data.ATT7022E.UB[Num] > THRESHOLD_EVENT_OVER_VOLTAGE_U)
		{
			Flag_B_Over_Voltage[Num]  = 0;
			Count_B_Over_Voltage[Num] = 0;
		}
		else
		{
			Flag_B_Over_Voltage[Num] = 2;
			
			if(Count_B_Over_Voltage[Num] >= TIME_EVENT_OVER_VOLTAGE)
			{
				Flag_B_Over_Voltage[Num]  = 0;
				Count_B_Over_Voltage[Num] = 0;
				
				LTU_Data.Event.EVB[Num] &= ~ACSAMPLE_EVENT_OVER_VOLTAGE;
			}
		}
	}
	
	//�ж�C���Ƿ��ѹ
	
	if((LTU_Data.Event.EVC[Num] & ACSAMPLE_EVENT_OVER_VOLTAGE) == 0)
	{
		if(LTU_Data.ATT7022E.UC[Num] > THRESHOLD_EVENT_OVER_VOLTAGE_U)
		{
			Flag_C_Over_Voltage[Num] = 1;
			
			if(Count_C_Over_Voltage[Num] >= TIME_EVENT_OVER_VOLTAGE)
			{
				Flag_C_Over_Voltage[Num]  = 0;
				Count_C_Over_Voltage[Num] = 0;
				
				LTU_Data.Event.EVC[Num] |= ACSAMPLE_EVENT_OVER_VOLTAGE;
			}
		}
		else
		{
			Flag_C_Over_Voltage[Num]  = 0;
			Count_C_Over_Voltage[Num] = 0;
		}
	}
	else
	{
		if(LTU_Data.ATT7022E.UC[Num] > THRESHOLD_EVENT_OVER_VOLTAGE_U)
		{
			Flag_C_Over_Voltage[Num]  = 0;
			Count_C_Over_Voltage[Num] = 0;
		}
		else
		{
			Flag_C_Over_Voltage[Num] = 2;
			
			if(Count_C_Over_Voltage[Num] >= TIME_EVENT_OVER_VOLTAGE)
			{
				Flag_C_Over_Voltage[Num]  = 0;
				Count_C_Over_Voltage[Num] = 0;
				
				LTU_Data.Event.EVC[Num] &= ~ACSAMPLE_EVENT_OVER_VOLTAGE;
			}
		}
	}
	
	//����------------------------------------------------------------------
		
	//�ж�A���Ƿ����
	
	//���A��û�з��������¼�����ô���ж�����
	if((LTU_Data.Event.EVA[Num] & ACSAMPLE_EVENT_NO_PHSREGPQ) == 0)
	{
		//���A���ѹС�ڶ�Ӧ�Ķ�����ֵ�����Ҹ��ɵ���С����������
		if((LTU_Data.ATT7022E.UA[Num] < THRESHOLD_EVENT_NO_PHSREGPQ_U) &&
		   (LTU_Data.ATT7022E.IA[Num] < THRESHOLD_EVENT_NO_PHSREGPQ_I))
		{
			//��ر�־������1���������������¼�
			Flag_A_No_PhSregpq[Num] = 1;
			
			//��ʱ�����Ƿ񳬹���ֵ
			if(Count_A_No_PhSregpq[Num] >= TIME_EVENT_NO_PHSREGPQ)
			{
				//�����־�����ͼ�ʱ����
				Flag_A_No_PhSregpq[Num]  = 0;
				Count_A_No_PhSregpq[Num] = 0;
				
				//A�෢�������¼�
				LTU_Data.Event.EVA[Num] |= ACSAMPLE_EVENT_NO_PHSREGPQ;
			}
		}
		else
		{
			Flag_A_No_PhSregpq[Num]  = 0;
			Count_A_No_PhSregpq[Num] = 0;
		}
	}
	//���A���Ѿ����������¼�
	else
	{
		//���A���ѹ���ڵ����趨�Ķ�����ֵ��˵��û�з��������¼�
		if((LTU_Data.ATT7022E.UA[Num] < THRESHOLD_EVENT_NO_PHSREGPQ_U) &&
		   (LTU_Data.ATT7022E.IA[Num] < THRESHOLD_EVENT_NO_PHSREGPQ_I))
		{
			Flag_A_No_PhSregpq[Num]  = 0;
			Count_A_No_PhSregpq[Num] = 0;
		}
		else
		{
			//��ر�־������2������û�з��������¼�
			Flag_A_No_PhSregpq[Num] = 2;
			
			//��ʱ�����Ƿ񳬹���ֵ
			if(Count_A_No_PhSregpq[Num] >= TIME_EVENT_NO_PHSREGPQ)
			{
				//�����־�����ͼ�ʱ����
				Flag_A_No_PhSregpq[Num]  = 0;
				Count_A_No_PhSregpq[Num] = 0;
				
				//A��û�з���Ƿѹ�¼�����Ƿѹ�¼��лָ���
				LTU_Data.Event.EVA[Num] &= ~ACSAMPLE_EVENT_NO_PHSREGPQ;
			}
		}
	}
	
	//�ж�B���Ƿ����
	
	if((LTU_Data.Event.EVB[Num] & ACSAMPLE_EVENT_NO_PHSREGPQ) == 0)
	{
		if((LTU_Data.ATT7022E.UB[Num] < THRESHOLD_EVENT_NO_PHSREGPQ_U) &&
		   (LTU_Data.ATT7022E.IB[Num] < THRESHOLD_EVENT_NO_PHSREGPQ_I))
		{
			Flag_B_No_PhSregpq[Num] = 1;
			
			if(Count_B_No_PhSregpq[Num] >= TIME_EVENT_NO_PHSREGPQ)
			{
				Flag_B_No_PhSregpq[Num]  = 0;
				Count_B_No_PhSregpq[Num] = 0;
				
				LTU_Data.Event.EVB[Num] |= ACSAMPLE_EVENT_NO_PHSREGPQ;
			}
		}
		else
		{
			Flag_B_No_PhSregpq[Num]  = 0;
			Count_B_No_PhSregpq[Num] = 0;
		}
	}
	else
	{
		if((LTU_Data.ATT7022E.UB[Num] < THRESHOLD_EVENT_NO_PHSREGPQ_U) &&
		   (LTU_Data.ATT7022E.IB[Num] < THRESHOLD_EVENT_NO_PHSREGPQ_I))
		{
			Flag_B_No_PhSregpq[Num]  = 0;
			Count_B_No_PhSregpq[Num] = 0;
		}
		else
		{
			Flag_B_No_PhSregpq[Num] = 2;
			
			if(Count_B_No_PhSregpq[Num] >= TIME_EVENT_NO_PHSREGPQ)
			{
				Flag_B_No_PhSregpq[Num]  = 0;
				Count_B_No_PhSregpq[Num] = 0;
				
				LTU_Data.Event.EVB[Num] &= ~ACSAMPLE_EVENT_NO_PHSREGPQ;
			}
		}
	}
	
	//�ж�C���Ƿ����
	
	if((LTU_Data.Event.EVC[Num] & ACSAMPLE_EVENT_NO_PHSREGPQ) == 0)
	{
		if((LTU_Data.ATT7022E.UC[Num] < THRESHOLD_EVENT_NO_PHSREGPQ_U) &&
		   (LTU_Data.ATT7022E.IC[Num] < THRESHOLD_EVENT_NO_PHSREGPQ_I))
		{
			Flag_C_No_PhSregpq[Num] = 1;
			
			if(Count_C_No_PhSregpq[Num] >= TIME_EVENT_NO_PHSREGPQ)
			{
				Flag_C_No_PhSregpq[Num]  = 0;
				Count_C_No_PhSregpq[Num] = 0;
				
				LTU_Data.Event.EVC[Num] |= ACSAMPLE_EVENT_NO_PHSREGPQ;
			}
		}
		else
		{
			Flag_C_No_PhSregpq[Num]  = 0;
			Count_C_No_PhSregpq[Num] = 0;
		}
	}
	else
	{
		if((LTU_Data.ATT7022E.UB[Num] < THRESHOLD_EVENT_NO_PHSREGPQ_U) &&
		   (LTU_Data.ATT7022E.IB[Num] < THRESHOLD_EVENT_NO_PHSREGPQ_I))
		{
			Flag_C_No_PhSregpq[Num]  = 0;
			Count_C_No_PhSregpq[Num] = 0;
		}
		else
		{
			Flag_C_No_PhSregpq[Num] = 2;
			
			if(Count_C_No_PhSregpq[Num] >= TIME_EVENT_NO_PHSREGPQ)
			{
				Flag_C_No_PhSregpq[Num]  = 0;
				Count_C_No_PhSregpq[Num] = 0;
				
				LTU_Data.Event.EVC[Num] &= ~ACSAMPLE_EVENT_NO_PHSREGPQ;
			}
		}
	}
	
	//ʧ��------------------------------------------------------------------
	
	//�ж�A���Ƿ�ʧ��
	
	//���A��û�з���ʧ���¼�����ô����ʧ�����
	if((LTU_Data.Event.EVA[Num] & ACSAMPLE_EVENT_NO_CURRENT) == 0)
	{
		//���ABC�����ѹ���ڵ��ܱ�ĵ�ѹ�����Ҹ������С����������������������һ���������5%�����
		if((LTU_Data.ATT7022E.UA[Num] > ATT7022E_START_U) &&
		   (LTU_Data.ATT7022E.UB[Num] > ATT7022E_START_U) &&
		   (LTU_Data.ATT7022E.UC[Num] > ATT7022E_START_U) &&
		   (LTU_Data.ATT7022E.IA[Num] < ATT7022E_START_I) &&
		   ((LTU_Data.ATT7022E.IB[Num] > (0.05f * ATT7022E_RATED_I)) || (LTU_Data.ATT7022E.IC[Num] > (0.05f * ATT7022E_RATED_I))))
		{
			//��ر�־������1����������ʧ���¼�
			Flag_A_No_Current[Num] = 1;
			
			//��ʱ�����Ƿ񳬹���ֵ
			if(Count_A_No_Current[Num] >= TIME_EVENT_NO_CURRENT)
			{
				//�����־�����ͼ�ʱ����
				Flag_A_No_Current[Num]  = 0;
				Count_A_No_Current[Num] = 0;
				
				//A�෢��ʧ���¼�
				LTU_Data.Event.EVA[Num] |= ACSAMPLE_EVENT_NO_CURRENT;
			}
		}
		else
		{
			Flag_A_No_Current[Num]  = 0;
			Count_A_No_Current[Num] = 0;
		}
	}
	//���A���Ѿ�����ʧ���¼�
	else
	{
		//���ABC�����ѹ���ڵ��ܱ�ĵ�ѹ�����Ҹ������С����������������������һ���������5%�����
		if((LTU_Data.ATT7022E.UA[Num] > ATT7022E_START_U) &&
		   (LTU_Data.ATT7022E.UB[Num] > ATT7022E_START_U) &&
		   (LTU_Data.ATT7022E.UC[Num] > ATT7022E_START_U) &&
		   (LTU_Data.ATT7022E.IA[Num] < ATT7022E_START_I)            &&
		   ((LTU_Data.ATT7022E.IB[Num] > (0.05f * ATT7022E_RATED_I)) || (LTU_Data.ATT7022E.IC[Num] > (0.05f * ATT7022E_RATED_I))))
		{
			Flag_A_No_Current[Num]  = 0;
			Count_A_No_Current[Num] = 0;
		}
		else
		{
			//��ر�־������2������û�з���ʧ���¼�
			Flag_A_No_Current[Num] = 2;
			
			//��ʱ�����Ƿ񳬹���ֵ
			if(Count_A_No_Current[Num] >= TIME_EVENT_NO_CURRENT)
			{
				//�����־�����ͼ�ʱ����
				Flag_A_No_Current[Num]  = 0;
				Count_A_No_Current[Num] = 0;
				
				//A��û�з���ʧ���¼�����ʧ���¼��лָ���
				LTU_Data.Event.EVA[Num] &= ~ACSAMPLE_EVENT_NO_CURRENT;
			}
		}
	}
	
	//�ж�B���Ƿ�ʧ��
	
	if((LTU_Data.Event.EVB[Num] & ACSAMPLE_EVENT_NO_CURRENT) == 0)
	{
		if((LTU_Data.ATT7022E.UA[Num] > ATT7022E_START_U) &&
		   (LTU_Data.ATT7022E.UB[Num] > ATT7022E_START_U) &&
		   (LTU_Data.ATT7022E.UC[Num] > ATT7022E_START_U) &&
		   (LTU_Data.ATT7022E.IB[Num] < ATT7022E_START_I)            &&
		   ((LTU_Data.ATT7022E.IA[Num] > (0.05f * ATT7022E_RATED_I)) || (LTU_Data.ATT7022E.IC[Num] > (0.05f * ATT7022E_RATED_I))))
		{
			Flag_B_No_Current[Num] = 1;
			
			if(Count_B_No_Current[Num] >= TIME_EVENT_NO_CURRENT)
			{
				Flag_B_No_Current[Num]  = 0;
				Count_B_No_Current[Num] = 0;
				
				LTU_Data.Event.EVB[Num] |= ACSAMPLE_EVENT_NO_CURRENT;
			}
		}
		else
		{
			Flag_B_No_Current[Num]  = 0;
			Count_B_No_Current[Num] = 0;
		}
	}
	else
	{
		if((LTU_Data.ATT7022E.UA[Num] > ATT7022E_START_U) &&
		   (LTU_Data.ATT7022E.UB[Num] > ATT7022E_START_U) &&
		   (LTU_Data.ATT7022E.UC[Num] > ATT7022E_START_U) &&
		   (LTU_Data.ATT7022E.IB[Num] < ATT7022E_START_I)            &&
		   ((LTU_Data.ATT7022E.IA[Num] > (0.05f * ATT7022E_RATED_I)) || (LTU_Data.ATT7022E.IC[Num] > (0.05f * ATT7022E_RATED_I))))
		{
			Flag_B_No_Current[Num]  = 0;
			Count_B_No_Current[Num] = 0;
		}
		else
		{
			Flag_B_No_Current[Num] = 2;
			
			if(Count_B_No_Current[Num] >= TIME_EVENT_NO_CURRENT)
			{
				Flag_B_No_Current[Num]  = 0;
				Count_B_No_Current[Num] = 0;
				
				LTU_Data.Event.EVB[Num] &= ~ACSAMPLE_EVENT_NO_CURRENT;
			}
		}
	}
	
	//�ж�C���Ƿ�ʧ��
	
	if((LTU_Data.Event.EVC[Num] & ACSAMPLE_EVENT_NO_CURRENT) == 0)
	{
		if((LTU_Data.ATT7022E.UA[Num] > ATT7022E_START_U) &&
		   (LTU_Data.ATT7022E.UB[Num] > ATT7022E_START_U) &&
		   (LTU_Data.ATT7022E.UC[Num] > ATT7022E_START_U) &&
		   (LTU_Data.ATT7022E.IC[Num] < ATT7022E_START_I)            &&
		   ((LTU_Data.ATT7022E.IA[Num] > (0.05f * ATT7022E_RATED_I)) || (LTU_Data.ATT7022E.IB[Num] > (0.05f * ATT7022E_RATED_I))))
		{
			Flag_C_No_Current[Num] = 1;
			
			if(Count_C_No_Current[Num] >= TIME_EVENT_NO_CURRENT)
			{
				Flag_C_No_Current[Num]  = 0;
				Count_C_No_Current[Num] = 0;
				
				LTU_Data.Event.EVC[Num] |= ACSAMPLE_EVENT_NO_CURRENT;
			}
		}
		else
		{
			Flag_C_No_Current[Num]  = 0;
			Count_C_No_Current[Num] = 0;
		}
	}
	else
	{
		if((LTU_Data.ATT7022E.UA[Num] > ATT7022E_START_U) &&
		   (LTU_Data.ATT7022E.UB[Num] > ATT7022E_START_U) &&
		   (LTU_Data.ATT7022E.UC[Num] > ATT7022E_START_U) &&
		   (LTU_Data.ATT7022E.IC[Num] < ATT7022E_START_I)            &&
		   ((LTU_Data.ATT7022E.IA[Num] > (0.05f * ATT7022E_RATED_I)) || (LTU_Data.ATT7022E.IB[Num] > (0.05f * ATT7022E_RATED_I))))
		{
			Flag_C_No_Current[Num]  = 0;
			Count_C_No_Current[Num] = 0;
		}
		else
		{
			Flag_C_No_Current[Num] = 2;
			
			if(Count_C_No_Current[Num] >= TIME_EVENT_NO_CURRENT)
			{
				Flag_C_No_Current[Num]  = 0;
				Count_C_No_Current[Num] = 0;
				
				LTU_Data.Event.EVC[Num] &= ~ACSAMPLE_EVENT_NO_CURRENT;
			}
		}
	}
	
	//����------------------------------------------------------------------
	
	//�ж�A���Ƿ����
	
	//���A��û�з��������¼�����ô���й������
	if((LTU_Data.Event.EVA[Num] & ACSAMPLE_EVENT_OVER_CURRENT) == 0)
	{
		//���ABC�����ѹ������ʼ��ѹ�����Ҹ���������ڶ����
		if((LTU_Data.ATT7022E.UA[Num] > ATT7022E_START_U) &&
		   (LTU_Data.ATT7022E.UB[Num] > ATT7022E_START_U) &&
		   (LTU_Data.ATT7022E.UC[Num] > ATT7022E_START_U) &&
		   (LTU_Data.ATT7022E.IA[Num] > ATT7022E_RATED_I))
		{
			//��ر�־������1���������������¼�
			Flag_A_Over_Current[Num] = 1;
			
			//��ʱ�����Ƿ񳬹���ֵ
			if(Count_A_Over_Current[Num] >= TIME_EVENT_OVER_CURRENT)
			{
				//�����־�����ͼ�ʱ����
				Flag_A_Over_Current[Num]  = 0;
				Count_A_Over_Current[Num] = 0;
				
				//A�෢�������¼�
				LTU_Data.Event.EVA[Num] |= ACSAMPLE_EVENT_OVER_CURRENT;
			}
		}
		else
		{
			Flag_A_Over_Current[Num]  = 0;
			Count_A_Over_Current[Num] = 0;
		}
	}
	//���A���Ѿ����������¼�
	else
	{
		//���ABC�����ѹ������ʼ��ѹ�����Ҹ���������ڶ����
		if((LTU_Data.ATT7022E.UA[Num] > ATT7022E_START_U) &&
		   (LTU_Data.ATT7022E.UB[Num] > ATT7022E_START_U) &&
		   (LTU_Data.ATT7022E.UC[Num] > ATT7022E_START_U) &&
		   (LTU_Data.ATT7022E.IA[Num] > ATT7022E_RATED_I))
		{
			Flag_A_Over_Current[Num]  = 0;
			Count_A_Over_Current[Num] = 0;
		}
		else
		{
			//��ر�־������2������û�з���ʧ���¼�
			Flag_A_Over_Current[Num] = 2;
			
			//��ʱ�����Ƿ񳬹���ֵ
			if(Count_A_Over_Current[Num] >= TIME_EVENT_OVER_CURRENT)
			{
				//�����־�����ͼ�ʱ����
				Flag_A_Over_Current[Num]  = 0;
				Count_A_Over_Current[Num] = 0;
				
				//A��û�з���ʧ���¼�����ʧ���¼��лָ���
				LTU_Data.Event.EVA[Num] &= ~ACSAMPLE_EVENT_OVER_CURRENT;
			}
		}
	}
	
	//�ж�B���Ƿ����
	
	if((LTU_Data.Event.EVB[Num] & ACSAMPLE_EVENT_OVER_CURRENT) == 0)
	{
		if((LTU_Data.ATT7022E.UA[Num] > ATT7022E_START_U) &&
		   (LTU_Data.ATT7022E.UB[Num] > ATT7022E_START_U) &&
		   (LTU_Data.ATT7022E.UC[Num] > ATT7022E_START_U) &&
		   (LTU_Data.ATT7022E.IB[Num] > ATT7022E_RATED_I))
		{
			Flag_B_Over_Current[Num] = 1;
			
			if(Count_B_Over_Current[Num] >= TIME_EVENT_OVER_CURRENT)
			{
				Flag_B_Over_Current[Num]  = 0;
				Count_B_Over_Current[Num] = 0;
				
				LTU_Data.Event.EVB[Num] |= ACSAMPLE_EVENT_OVER_CURRENT;
			}
		}
		else
		{
			Flag_B_Over_Current[Num]  = 0;
			Count_B_Over_Current[Num] = 0;
		}
	}
	else
	{
		if((LTU_Data.ATT7022E.UA[Num] > ATT7022E_START_U) &&
		   (LTU_Data.ATT7022E.UB[Num] > ATT7022E_START_U) &&
		   (LTU_Data.ATT7022E.UC[Num] > ATT7022E_START_U) &&
		   (LTU_Data.ATT7022E.IB[Num] > ATT7022E_RATED_I))
		{
			Flag_B_Over_Current[Num]  = 0;
			Count_B_Over_Current[Num] = 0;
		}
		else
		{
			Flag_B_Over_Current[Num] = 2;
			
			if(Count_B_Over_Current[Num] >= TIME_EVENT_OVER_CURRENT)
			{
				Flag_B_Over_Current[Num]  = 0;
				Count_B_Over_Current[Num] = 0;
				
				LTU_Data.Event.EVB[Num] &= ~ACSAMPLE_EVENT_OVER_CURRENT;
			}
		}
	}
	
	//�ж�C���Ƿ����
	
	if((LTU_Data.Event.EVC[Num] & ACSAMPLE_EVENT_OVER_CURRENT) == 0)
	{
		if((LTU_Data.ATT7022E.UA[Num] > ATT7022E_START_U) &&
		   (LTU_Data.ATT7022E.UB[Num] > ATT7022E_START_U) &&
		   (LTU_Data.ATT7022E.UC[Num] > ATT7022E_START_U) &&
		   (LTU_Data.ATT7022E.IC[Num] > ATT7022E_RATED_I))
		{
			Flag_C_Over_Current[Num] = 1;
			
			if(Count_C_Over_Current[Num] >= TIME_EVENT_OVER_CURRENT)
			{
				Flag_C_Over_Current[Num]  = 0;
				Count_C_Over_Current[Num] = 0;
				
				LTU_Data.Event.EVC[Num] |= ACSAMPLE_EVENT_OVER_CURRENT;
			}
		}
		else
		{
			Flag_C_Over_Current[Num]  = 0;
			Count_C_Over_Current[Num] = 0;
		}
	}
	else
	{
		if((LTU_Data.ATT7022E.UA[Num] > ATT7022E_START_U) &&
		   (LTU_Data.ATT7022E.UB[Num] > ATT7022E_START_U) &&
		   (LTU_Data.ATT7022E.UC[Num] > ATT7022E_START_U) &&
		   (LTU_Data.ATT7022E.IC[Num] > ATT7022E_RATED_I))
		{
			Flag_C_Over_Current[Num]  = 0;
			Count_C_Over_Current[Num] = 0;
		}
		else
		{
			Flag_C_Over_Current[Num] = 2;
			
			if(Count_C_Over_Current[Num] >= TIME_EVENT_OVER_CURRENT)
			{
				Flag_C_Over_Current[Num]  = 0;
				Count_C_Over_Current[Num] = 0;
				
				LTU_Data.Event.EVC[Num] &= ~ACSAMPLE_EVENT_OVER_CURRENT;
			}
		}
	}
	
	//�����¼�
	
	//ȫʧѹ----------------------------------------------------------------
	
	//���û�з���ȫʧѹ�¼�
	if((LTU_Data.Event.EV[Num] & ACSAMPLE_EVENT_ALL_NO_VOLTAGE) == 0)
	{
		//���ABC�����ѹ��������ʼ��ѹ�����Ҹ��ɵ���������5%�Ķ������˵������ȫʧѹ�¼�
		if((LTU_Data.ATT7022E.UA[Num] < ATT7022E_START_U) &&
		   (LTU_Data.ATT7022E.UB[Num] < ATT7022E_START_U) &&
		   (LTU_Data.ATT7022E.UC[Num] < ATT7022E_START_U) &&
		   (LTU_Data.ATT7022E.IA[Num] > (0.05f * ATT7022E_RATED_I)) &&
		   (LTU_Data.ATT7022E.IB[Num] > (0.05f * ATT7022E_RATED_I)) &&
		   (LTU_Data.ATT7022E.IC[Num] > (0.05f * ATT7022E_RATED_I)))
		{
			//��ر�־������1���������������¼�
			Flag_ABC_All_No_Voltage[Num] = 1;
			
			//��ʱ�����Ƿ񳬹���ֵ
			if(Count_ABC_All_No_Voltage[Num] >= TIME_EVENT_ALL_NO_VOLTAGE)
			{
				//�����־�����ͼ�ʱ����
				Flag_ABC_All_No_Voltage[Num]  = 0;
				Count_ABC_All_No_Voltage[Num] = 0;
				
				//A�෢�������¼�
				LTU_Data.Event.EV[Num] |= ACSAMPLE_EVENT_ALL_NO_VOLTAGE;
			}
		}
		else
		{
			Flag_ABC_All_No_Voltage[Num]  = 0;
			Count_ABC_All_No_Voltage[Num] = 0;
		}
	}
	//���A���Ѿ����������¼�
	else
	{
		//���ABC�����ѹ������ʼ��ѹ�����Ҹ���������ڶ����
		if((LTU_Data.ATT7022E.UA[Num] < ATT7022E_START_U) &&
		   (LTU_Data.ATT7022E.UB[Num] < ATT7022E_START_U) &&
		   (LTU_Data.ATT7022E.UC[Num] < ATT7022E_START_U) &&
		   (LTU_Data.ATT7022E.IA[Num] > (0.05f * ATT7022E_RATED_I)) &&
		   (LTU_Data.ATT7022E.IB[Num] > (0.05f * ATT7022E_RATED_I)) &&
		   (LTU_Data.ATT7022E.IC[Num] > (0.05f * ATT7022E_RATED_I)))
		{
			Flag_ABC_All_No_Voltage[Num]  = 0;
			Count_ABC_All_No_Voltage[Num] = 0;
		}
		else
		{
			//��ر�־������2������û�з���ʧ���¼�
			Flag_ABC_All_No_Voltage[Num] = 2;
			
			//��ʱ�����Ƿ񳬹���ֵ
			if(Count_ABC_All_No_Voltage[Num] >= TIME_EVENT_ALL_NO_VOLTAGE)
			{
				//�����־�����ͼ�ʱ����
				Flag_ABC_All_No_Voltage[Num]  = 0;
				Count_ABC_All_No_Voltage[Num] = 0;
				
				//A��û�з���ʧ���¼�����ʧ���¼��лָ���
				LTU_Data.Event.EV[Num] &= ~ACSAMPLE_EVENT_ALL_NO_VOLTAGE;
			}
		}
	}
	
	//��ѹ������------------------------------------------------------------
	
	//���������һ��û�з��������¼�
	if(((LTU_Data.Event.EVA[Num] & ACSAMPLE_EVENT_NO_PHSREGPQ) == 0) ||
	   ((LTU_Data.Event.EVB[Num] & ACSAMPLE_EVENT_NO_PHSREGPQ) == 0) ||
	   ((LTU_Data.Event.EVC[Num] & ACSAMPLE_EVENT_NO_PHSREGPQ) == 0))
	{
		//���û�з�����ѹ�������¼�
		if((LTU_Data.Event.EV[Num] & ACSAMPLE_EVENT_VOLTAGE_PHSREGPQ_ERR) == 0)
		{
			//��������ѹƫ����ԵĽǶȹ���
			if((abs((int32)(LTU_Data.ATT7022E.YUA[Num] - 0))   > THRESHOLD_EVENT_VOLTAGE_PHSREGPQ_ERR) ||
			   (abs((int32)(LTU_Data.ATT7022E.YUB[Num] - 120)) > THRESHOLD_EVENT_VOLTAGE_PHSREGPQ_ERR) ||
			   (abs((int32)(LTU_Data.ATT7022E.YUC[Num] - 240)) > THRESHOLD_EVENT_VOLTAGE_PHSREGPQ_ERR))
			{
				//��ر�־������1������������ѹ�������¼�
				Flag_ABC_Voltage_PhSregpq_Err[Num] = 1;
				
				//��ʱ�����Ƿ񳬹���ֵ
				if(Count_ABC_Voltage_PhSregpq_Err[Num] >= TIME_EVENT_VOLTAGE_PHSREGPQ_ERR)
				{
					//�����־�����ͼ�ʱ����
					Flag_ABC_Voltage_PhSregpq_Err[Num]  = 0;
					Count_ABC_Voltage_PhSregpq_Err[Num] = 0;
					
					//������ѹ�������¼�
					LTU_Data.Event.EV[Num] |= ACSAMPLE_EVENT_VOLTAGE_PHSREGPQ_ERR;
				}
			}
			//���ABC�����ѹ������ȷ
			else
			{
				Flag_ABC_Voltage_PhSregpq_Err[Num]  = 0;
				Count_ABC_Voltage_PhSregpq_Err[Num] = 0;
			}
		}
		//���A���Ѿ�������ѹ�������¼�
		else
		{
			//���ABC�����ѹ����������
			if((LTU_Data.ATT7022E.YUA[Num] > LTU_Data.ATT7022E.YUB[Num]) ||
			   (LTU_Data.ATT7022E.YUA[Num] > LTU_Data.ATT7022E.YUC[Num]) ||
			   (LTU_Data.ATT7022E.YUB[Num] > LTU_Data.ATT7022E.YUC[Num]))
			{
				Flag_ABC_Voltage_PhSregpq_Err[Num]  = 0;
				Count_ABC_Voltage_PhSregpq_Err[Num] = 0;
			}
			//���ABC�����ѹ������ȷ
			else
			{
				//��ر�־������2������û�з�����ѹ�������¼�
				Flag_ABC_Voltage_PhSregpq_Err[Num] = 2;
				
				//��ʱ�����Ƿ񳬹���ֵ
				if(Count_ABC_Voltage_PhSregpq_Err[Num] >= TIME_EVENT_VOLTAGE_PHSREGPQ_ERR)
				{
					//�����־�����ͼ�ʱ����
					Flag_ABC_Voltage_PhSregpq_Err[Num]  = 0;
					Count_ABC_Voltage_PhSregpq_Err[Num] = 0;
					
					//A��û�з�����ѹ�������¼����ӵ�ѹ�������¼��лָ���
					LTU_Data.Event.EV[Num] &= ~ACSAMPLE_EVENT_VOLTAGE_PHSREGPQ_ERR;
				}
			}
		}
	}
	
	//����������------------------------------------------------------------
	
	//��ȡ�������
	YIA[Num] = LTU_Data.ATT7022E.YUA[Num] + LTU_Data.ATT7022E.PhSregpqA[Num];
	YIB[Num] = LTU_Data.ATT7022E.YUB[Num] + LTU_Data.ATT7022E.PhSregpqB[Num];
	YIC[Num] = LTU_Data.ATT7022E.YUC[Num] + LTU_Data.ATT7022E.PhSregpqC[Num];
	
	//���������һ��û�з��������¼�
	if(((LTU_Data.Event.EVA[Num] & ACSAMPLE_EVENT_NO_PHSREGPQ) == 0) ||
	   ((LTU_Data.Event.EVB[Num] & ACSAMPLE_EVENT_NO_PHSREGPQ) == 0) ||
	   ((LTU_Data.Event.EVC[Num] & ACSAMPLE_EVENT_NO_PHSREGPQ) == 0))
	{
		//���û�з��������������¼�
		if((LTU_Data.Event.EV[Num] & ACSAMPLE_EVENT_CURRENT_PHSREGPQ_ERR) == 0)
		{
			//���ABC�����������������
			if((YIA[Num] > YIB[Num]) ||
			   (YIA[Num] > YIC[Num]) ||
			   (YIB[Num] > YIC[Num]))
			{
				//��ر�־������1���������������������¼�
				Flag_ABC_Current_PhSregpq_Err[Num] = 1;
				
				//��ʱ�����Ƿ񳬹���ֵ
				if(Count_ABC_Current_PhSregpq_Err[Num] >= TIME_EVENT_CURRENT_PHSREGPQ_ERR)
				{
					//�����־�����ͼ�ʱ����
					Flag_ABC_Current_PhSregpq_Err[Num]  = 0;
					Count_ABC_Current_PhSregpq_Err[Num] = 0;
					
					//���������������¼�
					LTU_Data.Event.EV[Num] |= ACSAMPLE_EVENT_CURRENT_PHSREGPQ_ERR;
				}
			}
			//���ABC�������������ȷ
			else
			{
				Flag_ABC_Current_PhSregpq_Err[Num]  = 0;
				Count_ABC_Current_PhSregpq_Err[Num] = 0;
			}
		}
		//���ABC�����Ѿ����������������¼�
		else
		{
			//���ABC�����������������
			if((YIA[Num] > YIB[Num]) ||
			   (YIA[Num] > YIC[Num]) ||
			   (YIB[Num] > YIC[Num]))
			{
				Flag_ABC_Current_PhSregpq_Err[Num]  = 0;
				Count_ABC_Current_PhSregpq_Err[Num] = 0;
			}
			//���ABC�������������ȷ
			else
			{
				//��ر�־������2������û�з��������������¼�
				Flag_ABC_Current_PhSregpq_Err[Num] = 2;
				
				//��ʱ�����Ƿ񳬹���ֵ
				if(Count_ABC_Current_PhSregpq_Err[Num] >= TIME_EVENT_CURRENT_PHSREGPQ_ERR)
				{
					//�����־�����ͼ�ʱ����
					Flag_ABC_Current_PhSregpq_Err[Num]  = 0;
					Count_ABC_Current_PhSregpq_Err[Num] = 0;
					
					//A��û�з��������������¼����ӵ����������¼��лָ���
					LTU_Data.Event.EV[Num] &= ~ACSAMPLE_EVENT_CURRENT_PHSREGPQ_ERR;
				}
			}
		}
	}
	
	//������ƽ��------------------------------------------------------------
		
	//���û�з���������ƽ���¼�
	if((LTU_Data.Event.EV[Num] & ACSAMPLE_EVENT_CURRENT_UNBALANCE) == 0)
	{
		//���������һ���������5%����������Ҳ�ƽ���ʴ�����ֵ
		if(((LTU_Data.ATT7022E.IA[Num] > (0.05f * ATT7022E_RATED_I)) || (LTU_Data.ATT7022E.IB[Num] > (0.05f * ATT7022E_RATED_I)) || (LTU_Data.ATT7022E.IC[Num] > (0.05f * ATT7022E_RATED_I))) &&
		   (((Imax - Imin) / Iaverage) > THRESHOLD_EVENT_CURRENT_UNBALANCE))
		{
			//��ر�־������1����������������ƽ���¼�
			Flag_ABC_Current_UnBalance[Num] = 1;
			
			//��ʱ�����Ƿ񳬹���ֵ
			if(Count_ABC_Current_UnBalance[Num] >= TIME_EVENT_CURRENT_UNBALANCE)
			{
				//�����־�����ͼ�ʱ����
				Flag_ABC_Current_UnBalance[Num]  = 0;
				Count_ABC_Current_UnBalance[Num] = 0;
				
				//����������ƽ���¼�
				LTU_Data.Event.EV[Num] |= ACSAMPLE_EVENT_CURRENT_UNBALANCE;
			}
		}
		//���ABC�������ƽ��
		else
		{
			Flag_ABC_Current_UnBalance[Num]  = 0;
			Count_ABC_Current_UnBalance[Num] = 0;
		}
	}
	//���ABC�����Ѿ�����������ƽ���¼�
	else
	{
		//���������һ���������5%����������Ҳ�ƽ���ʴ�����ֵ��˵������������ƽ���¼�
		if(((LTU_Data.ATT7022E.IA[Num] > (0.05f * ATT7022E_RATED_I)) || (LTU_Data.ATT7022E.IB[Num] > (0.05f * ATT7022E_RATED_I)) || (LTU_Data.ATT7022E.IC[Num] > (0.05f * ATT7022E_RATED_I))) &&
		   (((Imax - Imin) / Iaverage) > THRESHOLD_EVENT_CURRENT_UNBALANCE))
		{
			Flag_ABC_Current_UnBalance[Num]  = 0;
			Count_ABC_Current_UnBalance[Num] = 0;
		}
		//���ABC�������ƽ��
		else
		{
			//��ر�־������2������û�з���������ƽ���¼�
			Flag_ABC_Current_UnBalance[Num] = 2;
			
			//��ʱ�����Ƿ񳬹���ֵ
			if(Count_ABC_Current_UnBalance[Num] >= TIME_EVENT_CURRENT_UNBALANCE)
			{
				//�����־�����ͼ�ʱ����
				Flag_ABC_Current_UnBalance[Num]  = 0;
				Count_ABC_Current_UnBalance[Num] = 0;
				
				//A��û�з���������ƽ���¼����ӵ�����ƽ���¼��лָ���
				LTU_Data.Event.EV[Num] &= ~ACSAMPLE_EVENT_CURRENT_UNBALANCE;
			}
		}
	}
	
	//��ѹ��ƽ��------------------------------------------------------------
		
	//���û�з�����ѹ��ƽ���¼�
	if((LTU_Data.Event.EV[Num] & ACSAMPLE_EVENT_VOLTAGE_UNBALANCE) == 0)
	{
		//���������һ���ѹ����������ѹ�����Ҳ�ƽ���ʴ�����ֵ
		if(((LTU_Data.ATT7022E.UA[Num] > ATT7022E_START_U) || (LTU_Data.ATT7022E.UB[Num] > ATT7022E_START_U) || (LTU_Data.ATT7022E.UC[Num] > ATT7022E_START_U)) &&
		   (((Umax - Umin) / Uaverage) > THRESHOLD_EVENT_VOLTAGE_UNBALANCE))
		{
			//��ر�־������1������������ѹ��ƽ���¼�
			Flag_ABC_Voltage_UnBalance[Num] = 1;
			
			//��ʱ�����Ƿ񳬹���ֵ
			if(Count_ABC_Voltage_UnBalance[Num] >= TIME_EVENT_VOLTAGE_UNBALANCE)
			{
				//�����־�����ͼ�ʱ����
				Flag_ABC_Voltage_UnBalance[Num]  = 0;
				Count_ABC_Voltage_UnBalance[Num] = 0;
				
				//������ѹ��ƽ���¼�
				LTU_Data.Event.EV[Num] |= ACSAMPLE_EVENT_VOLTAGE_UNBALANCE;
			}
		}
		//���ABC�����ѹƽ��
		else
		{
			Flag_ABC_Voltage_UnBalance[Num]  = 0;
			Count_ABC_Voltage_UnBalance[Num] = 0;
		}
	}
	//���ABC�����Ѿ�������ѹ��ƽ���¼�
	else
	{
		//���������һ���ѹ����������ѹ�����Ҳ�ƽ���ʴ�����ֵ
		if(((LTU_Data.ATT7022E.UA[Num] > ATT7022E_START_U) || (LTU_Data.ATT7022E.UB[Num] > ATT7022E_START_U) || (LTU_Data.ATT7022E.UC[Num] > ATT7022E_START_U)) &&
		   (((Umax - Umin) / Uaverage) > THRESHOLD_EVENT_VOLTAGE_UNBALANCE))
		{
			Flag_ABC_Voltage_UnBalance[Num]  = 0;
			Count_ABC_Voltage_UnBalance[Num] = 0;
		}
		//���ABC�����ѹƽ��
		else
		{
			//��ر�־������2������û�з�����ѹ��ƽ���¼�
			Flag_ABC_Voltage_UnBalance[Num] = 2;
			
			//��ʱ�����Ƿ񳬹���ֵ
			if(Count_ABC_Voltage_UnBalance[Num] >= TIME_EVENT_VOLTAGE_UNBALANCE)
			{
				//�����־�����ͼ�ʱ����
				Flag_ABC_Voltage_UnBalance[Num]  = 0;
				Count_ABC_Voltage_UnBalance[Num] = 0;
				
				//A��û�з�����ѹ��ƽ���¼����ӵ�ѹ��ƽ���¼��лָ���
				LTU_Data.Event.EV[Num] &= ~ACSAMPLE_EVENT_VOLTAGE_UNBALANCE;
			}
		}
	}
}



void ATT7022E_ACSample_Event_Timer_1000ms(void)
{
	uint8_t i = 0;
	
	for(i = 0; i < ATT7022E_NUM_TOTAL; i++)
	{
		//ʧѹ�¼���ʱ
		if(Flag_A_No_Voltage[i])
		{
			Count_A_No_Voltage[i]++;
		}
		if(Flag_B_No_Voltage[i])
		{
			Count_B_No_Voltage[i]++;
		}
		if(Flag_C_No_Voltage[i])
		{
			Count_C_No_Voltage[i]++;
		}
		
		//Ƿѹ�¼���ʱ
		if(Flag_A_Low_Voltage[i])
		{
			Count_A_Low_Voltage[i]++;
		}
		if(Flag_B_Low_Voltage[i])
		{
			Count_B_Low_Voltage[i]++;
		}
		if(Flag_C_Low_Voltage[i])
		{
			Count_C_Low_Voltage[i]++;
		}
		
		//��ѹ�¼���ʱ
		if(Flag_A_Over_Voltage[i])
		{
			Count_A_Over_Voltage[i]++;
		}
		if(Flag_B_Over_Voltage[i])
		{
			Count_B_Over_Voltage[i]++;
		}
		if(Flag_C_Over_Voltage[i])
		{
			Count_C_Over_Voltage[i]++;
		}
		
		//�����¼�
		if(Flag_A_No_PhSregpq[i])
		{
			Count_A_No_PhSregpq[i]++;
		}
		if(Flag_B_No_PhSregpq[i])
		{
			Count_B_No_PhSregpq[i]++;
		}
		if(Flag_C_No_PhSregpq[i])
		{
			Count_C_No_PhSregpq[i]++;
		}
		
		//ʧ���¼�
		if(Flag_A_No_Current[i])
		{
			Count_A_No_Current[i]++;
		}
		if(Flag_B_No_Current[i])
		{
			Count_B_No_Current[i]++;
		}
		if(Flag_C_No_Current[i])
		{
			Count_C_No_Current[i]++;
		}
		
		//�����¼�
		if(Flag_A_Over_Current[i])
		{
			Count_A_Over_Current[i]++;
		}
		if(Flag_B_Over_Current[i])
		{
			Count_B_Over_Current[i]++;
		}
		if(Flag_C_Over_Current[i])
		{
			Count_C_Over_Current[i]++;
		}
		
		//ȫʧѹ
		if(Flag_ABC_All_No_Voltage[i])
		{
			Count_ABC_All_No_Voltage[i]++;
		}
		
		//��ѹ������
		if(Flag_ABC_Voltage_PhSregpq_Err[i])
		{
			Count_ABC_Voltage_PhSregpq_Err[i]++;
		}
		
		//����������
		if(Flag_ABC_Current_PhSregpq_Err[i])
		{
			Count_ABC_Current_PhSregpq_Err[i]++;
		}
		
		//������ƽ��
		if(Flag_ABC_Current_UnBalance[i])
		{
			Count_ABC_Current_UnBalance[i]++;
		}
		
		//��ѹ��ƽ��
		if(Flag_ABC_Voltage_UnBalance[i])
		{
			Count_ABC_Voltage_UnBalance[i]++;
		}
	}
}
#endif
