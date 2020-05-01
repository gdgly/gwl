#include "ATT7022E.h"

#if 0
#define Dummy_Byte    0xFF  //无效数据

static uint8_t Flag_ATT7022E_Check_Result[ATT7022E_NUM_TOTAL] = {1, 1, 1, 1, 1,};  //判断最后一次的SPI通信是否正确
static uint8_t Flag_ATT7022E_Check_Error[ATT7022E_NUM_TOTAL]  = {0, 0, 0, 0, 0,};  //记录是否发生了错误事件

//用于交采事件
static uint8_t Flag_A_No_Voltage[ATT7022E_NUM_TOTAL]              = {0};  //A相失压标志
static uint8_t Count_A_No_Voltage[ATT7022E_NUM_TOTAL]             = {0};  //A相失压计时
static uint8_t Flag_B_No_Voltage[ATT7022E_NUM_TOTAL]              = {0};  //B相失压标志
static uint8_t Count_B_No_Voltage[ATT7022E_NUM_TOTAL]             = {0};  //B相失压计时
static uint8_t Flag_C_No_Voltage[ATT7022E_NUM_TOTAL]              = {0};  //C相失压标志
static uint8_t Count_C_No_Voltage[ATT7022E_NUM_TOTAL]             = {0};  //C相失压计时
static uint8_t Flag_A_Low_Voltage[ATT7022E_NUM_TOTAL]             = {0};  //A相欠压标志
static uint8_t Count_A_Low_Voltage[ATT7022E_NUM_TOTAL]            = {0};  //A相欠压计时
static uint8_t Flag_B_Low_Voltage[ATT7022E_NUM_TOTAL]             = {0};  //B相欠压标志
static uint8_t Count_B_Low_Voltage[ATT7022E_NUM_TOTAL]            = {0};  //B相欠压计时
static uint8_t Flag_C_Low_Voltage[ATT7022E_NUM_TOTAL]             = {0};  //C相欠压标志
static uint8_t Count_C_Low_Voltage[ATT7022E_NUM_TOTAL]            = {0};  //C相欠压计时
static uint8_t Flag_A_Over_Voltage[ATT7022E_NUM_TOTAL]            = {0};  //A相过压标志
static uint8_t Count_A_Over_Voltage[ATT7022E_NUM_TOTAL]           = {0};  //A相过压计时
static uint8_t Flag_B_Over_Voltage[ATT7022E_NUM_TOTAL]            = {0};  //B相过压标志
static uint8_t Count_B_Over_Voltage[ATT7022E_NUM_TOTAL]           = {0};  //B相过压计时
static uint8_t Flag_C_Over_Voltage[ATT7022E_NUM_TOTAL]            = {0};  //C相过压标志
static uint8_t Count_C_Over_Voltage[ATT7022E_NUM_TOTAL]           = {0};  //C相过压计时
static uint8_t Flag_A_No_PhSregpq[ATT7022E_NUM_TOTAL]             = {0};  //A相断相标志
static uint8_t Count_A_No_PhSregpq[ATT7022E_NUM_TOTAL]            = {0};  //A相断相计时
static uint8_t Flag_B_No_PhSregpq[ATT7022E_NUM_TOTAL]             = {0};  //B相断相标志
static uint8_t Count_B_No_PhSregpq[ATT7022E_NUM_TOTAL]            = {0};  //B相断相计时
static uint8_t Flag_C_No_PhSregpq[ATT7022E_NUM_TOTAL]             = {0};  //C相断相标志
static uint8_t Count_C_No_PhSregpq[ATT7022E_NUM_TOTAL]            = {0};  //C相断相计时
static uint8_t Flag_A_No_Current[ATT7022E_NUM_TOTAL]              = {0};  //A相失流标志
static uint8_t Count_A_No_Current[ATT7022E_NUM_TOTAL]             = {0};  //A相失流计时
static uint8_t Flag_B_No_Current[ATT7022E_NUM_TOTAL]              = {0};  //B相失流标志
static uint8_t Count_B_No_Current[ATT7022E_NUM_TOTAL]             = {0};  //B相失流计时
static uint8_t Flag_C_No_Current[ATT7022E_NUM_TOTAL]              = {0};  //C相失流标志
static uint8_t Count_C_No_Current[ATT7022E_NUM_TOTAL]             = {0};  //C相失流计时
static uint8_t Flag_A_Over_Current[ATT7022E_NUM_TOTAL]            = {0};  //A相过流标志
static uint8_t Count_A_Over_Current[ATT7022E_NUM_TOTAL]           = {0};  //A相过流计时
static uint8_t Flag_B_Over_Current[ATT7022E_NUM_TOTAL]            = {0};  //B相过流标志
static uint8_t Count_B_Over_Current[ATT7022E_NUM_TOTAL]           = {0};  //B相过流计时
static uint8_t Flag_C_Over_Current[ATT7022E_NUM_TOTAL]            = {0};  //C相过流标志
static uint8_t Count_C_Over_Current[ATT7022E_NUM_TOTAL]           = {0};  //C相过流计时
static uint8_t Flag_ABC_All_No_Voltage[ATT7022E_NUM_TOTAL]        = {0};  //ABC相全失压标志
static uint8_t Count_ABC_All_No_Voltage[ATT7022E_NUM_TOTAL]       = {0};  //ABC相全失压计时
static uint8_t Flag_ABC_Voltage_PhSregpq_Err[ATT7022E_NUM_TOTAL]  = {0};  //ABC相电压逆相序标志
static uint8_t Count_ABC_Voltage_PhSregpq_Err[ATT7022E_NUM_TOTAL] = {0};  //ABC相电压逆相序计时
static uint8_t Flag_ABC_Current_PhSregpq_Err[ATT7022E_NUM_TOTAL]  = {0};  //ABC相电流逆相序标志
static uint8_t Count_ABC_Current_PhSregpq_Err[ATT7022E_NUM_TOTAL] = {0};  //ABC相电流逆相序计时
static uint8_t Flag_ABC_Current_UnBalance[ATT7022E_NUM_TOTAL]     = {0};  //ABC相电流不平衡标志
static uint8_t Count_ABC_Current_UnBalance[ATT7022E_NUM_TOTAL]    = {0};  //ABC相电流不平衡计时
static uint8_t Flag_ABC_Voltage_UnBalance[ATT7022E_NUM_TOTAL]     = {0};  //ABC相电流不平衡标志
static uint8_t Count_ABC_Voltage_UnBalance[ATT7022E_NUM_TOTAL]    = {0};  //ABC相电流不平衡计时

#ifdef	LTU_TRANSFORMS_RATE
extern Struct_LTU_Transformer      LTU_Transformer_Rate;             //互感器变比
#endif



void ATT7022E_Init(void)
{
	printf("初始化ATT7022E引脚\r\n");
	
	//定义一个用于初始化SPI外设的结构体变量
	SPI_InitTypeDef  SPI_InitStructure;
	
	//定义一个用于初始化GPIO引脚的结构体变量
	GPIO_InitTypeDef GPIO_InitStructure;

	//使能PORT_B、PORT_C、SPI外设的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC , ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

	//初始化SPI2外设对应的引脚，初始化为复用推挽输出
	GPIO_InitStructure.GPIO_Pin   = ATT7022E_SPI_PINS;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ATT7022E_SPI_PORT, &GPIO_InitStructure);

	//初始化ATT7022E_1的片选引脚
	GPIO_InitStructure.GPIO_Pin   = ATT7022E_SPI_CS1_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ATT7022E_SPI_CS1_PORT, &GPIO_InitStructure);

	//初始化ATT7022E_2的片选引脚
	GPIO_InitStructure.GPIO_Pin   = ATT7022E_SPI_CS2_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ATT7022E_SPI_CS2_PORT, &GPIO_InitStructure);

	//初始化ATT7022E_3的片选引脚
	GPIO_InitStructure.GPIO_Pin   = ATT7022E_SPI_CS3_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ATT7022E_SPI_CS3_PORT, &GPIO_InitStructure);

	//初始化ATT7022E_4的片选引脚
	GPIO_InitStructure.GPIO_Pin   = ATT7022E_SPI_CS4_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ATT7022E_SPI_CS4_PORT, &GPIO_InitStructure);

	//初始化ATT7022E_5的片选引脚
	GPIO_InitStructure.GPIO_Pin   = ATT7022E_SPI_CS5_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ATT7022E_SPI_CS5_PORT, &GPIO_InitStructure);

	//拉高CS引脚，不进行通信
	ATT7022E_SPI_CS0_H();
	ATT7022E_SPI_CS1_H();
	ATT7022E_SPI_CS2_H();
	ATT7022E_SPI_CS3_H();
	ATT7022E_SPI_CS4_H();

	//初始化SPI2外设
	SPI_InitStructure.SPI_Direction         = SPI_Direction_2Lines_FullDuplex;  //双线双向全双工
	SPI_InitStructure.SPI_Mode              = SPI_Mode_Master;                  //主机模式
	SPI_InitStructure.SPI_DataSize          = SPI_DataSize_8b;                  //8个数据位
	SPI_InitStructure.SPI_CPOL              = SPI_CPOL_Low;                     //SCLK空闲时为低电平
	SPI_InitStructure.SPI_CPHA              = SPI_CPHA_2Edge;                   //SCLK的第二个跳变沿读取或者传输数据
	SPI_InitStructure.SPI_NSS               = SPI_NSS_Soft;                     //软件控制片选引脚
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;         //SCLK的时钟频率设为fPCLK的16分频
	SPI_InitStructure.SPI_FirstBit          = SPI_FirstBit_MSB;                 //高位先传
	SPI_InitStructure.SPI_CRCPolynomial     = 7;                                //用于校验，此项不管
	SPI_Init(SPI2, &SPI_InitStructure);

	//使能SPI2外设
	SPI_Cmd(SPI2, ENABLE);
}



static uint8_t SPI2_SendByte(uint8_t byte)
{
	//等待SPI2外设的发送缓冲区为空
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);

	//通过SPI2外设把数据发送出去
	SPI_I2S_SendData(SPI2, byte);

	//等待SPI2外设的接收缓冲区不为空（即等待SPI2外设接收到了1个字节数据）
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
	
	//返回SPI2外设接收到的数据
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

	//发送需要读取数据的地址出去
	SPI2_SendByte(SPIADDR);

	//读取高字节数据
	//因为双线双向全双工模式，发送的同时也能接收数据，所以这里发送一个无意义数据，用于在发送过程中接收数据
	Data_8bit_H = SPI2_SendByte(Dummy_Byte);

	//读取中字节数据
	Data_8bit_M = SPI2_SendByte(Dummy_Byte);

	//读取低字节数据
	Data_8bit_L = SPI2_SendByte(Dummy_Byte);

	//把接收到的数据组合成一个完整数据
	Data_24bit = (Data_8bit_H << 16) | (Data_8bit_M << 8) | Data_8bit_L;

	return Data_24bit;
}



static void ATT7022E_Write3Bytes(uint8_t SPIADDR, uint32 SPIDATA)
{
	//发送写入的寄存器地址
	SPI2_SendByte(SPIADDR | 0x80);

	//发送高8位
	SPI2_SendByte((SPIDATA & 0xFF0000) >> 16);
	//发送中8位
	SPI2_SendByte((SPIDATA & 0xFF00) >> 8);
	//发送低8位
	SPI2_SendByte(SPIDATA & 0xFF);
}



uint32 ATT7022E_ReadID(void)
{
	uint32 Data_24bit  = 0;
	uint32 Data_8bit_H = 0;
	uint32 Data_8bit_M = 0;
	uint32 Data_8bit_L = 0;

	//发送ID寄存器的地址
	SPI2_SendByte(0x00);

	//读取高8位
	Data_8bit_H = SPI2_SendByte(Dummy_Byte);

	//读取中8位
	Data_8bit_M = SPI2_SendByte(Dummy_Byte);

	//读取低8位
	Data_8bit_L = SPI2_SendByte(Dummy_Byte);

	//组合数据
	Data_24bit = (Data_8bit_H << 16) | (Data_8bit_M << 8) | Data_8bit_L;

	return Data_24bit;
}



//校验数据通信的可靠性
static bool ATT7022E_Check_Result(uint8_t Num_ATT7022E)
{
	uint32 Reg_BackUp            = 0;  //记录备份寄存器里的数据
	uint8_t  CheckSum_Reg_BackUp   = 0;  //记录备份寄存器里的数据的校验和
	uint32 Reg_CheckSum          = 0;  //记录通信校验和寄存器里的数据
	uint8_t  CheckSum_Reg_CheckSum = 0;  //记录通信校验和寄存的数据的校验和
	uint32 Reg_Device            = 0;  //记录设备寄存器的值
	
	//从ATT7022E的备份寄存器当中获取到上一次SPI通信的数据
	ATT7022E_SPI_CS_Selete(Num_ATT7022E, 0);
	Reg_BackUp = ATT7022E_Read3Bytes(0x2D);
	ATT7022E_SPI_CS_Selete(Num_ATT7022E, 1);
	
	//计算高字节、中字节、低字节的校验和
	CheckSum_Reg_BackUp = ((Reg_BackUp & 0xFF0000) >> 16) + ((Reg_BackUp & 0x00FF00) >> 8) + ((Reg_BackUp & 0x0000FF) >> 0);

	//从ATT7022E的通讯校验和寄存器中获取到数据
	ATT7022E_SPI_CS_Selete(Num_ATT7022E, 0);
	Reg_CheckSum = ATT7022E_Read3Bytes(0x2E);
	ATT7022E_SPI_CS_Selete(Num_ATT7022E, 1);
	
	//注: 每一次的SPI通信，ATT7022E都会把命令字节（寄存器地址字节）、数据的高字节、中字节、低字节等四个字节相加，存进通信校验和寄存器当中
	//通信校验和寄存器的高字节存储命令，中字节、低字节共同存储校验和
	//此处即是把通信校验和寄存器的值减去命令字节的值，得出数据校验和的值
	CheckSum_Reg_CheckSum = (Reg_CheckSum & 0x00FFFF) - ((Reg_CheckSum & 0xFF0000) >> 16);

	//如果计算出来的校验和和通信校验和寄存器里的值不相等，说明通信发生了错误
	if (CheckSum_Reg_CheckSum != CheckSum_Reg_BackUp)
	{
		printf("ATT7022E: SPI通信校验和发生错误\r\n");
		
		//读取出0x00地址的寄存器数据
		ATT7022E_SPI_CS_Selete(Num_ATT7022E, 0);
		Reg_Device = ATT7022E_Read3Bytes(0x00);
		ATT7022E_SPI_CS_Selete(Num_ATT7022E, 1);
		
		//判断当前ATT7022E处于何种状态
		switch (Reg_Device)
		{
			//当前是校表模式，校表模式不需要进行校验
			case 0x00AAAA:
			printf("校表模式 不需校验\r\n");
			return true;
			
			//当前是计量模式，通信确实发生了错误
			case 0x7122A0:
			printf("计量模式 校验错误\r\n");
			return false;
			
			//其他
			default:
			printf("读取 0x00 寄存器错误\r\n");
			return false;
		}
	}
	
	return true;
};



void ATT7022E_Write3Bytes_CheckCRC(uint8_t Num_ATT7022E, uint8_t ADDR, uint32 Data_24bit)
{
	while (1)
	{
		//把数据写入对应的寄存器里面
		ATT7022E_SPI_CS_Selete(Num_ATT7022E, 0);
		ATT7022E_Write3Bytes(ADDR, Data_24bit);
		ATT7022E_SPI_CS_Selete(Num_ATT7022E, 1);
		
		//判断通信是否正确，如果通信正确则跳出死循环，如果不正确则10ms延时以后继续通信
		if (ATT7022E_Check_Result(Num_ATT7022E))
		{
			break;
		}
		else
		{
			printf("往ATT7022E的地址0x%2x写入0x%6x时出错，重新写入\r\n", ADDR, Data_24bit);
			Delay_ms(2);
		}
	}
}



int  ATT7022E_Read3Bytes_CheckCRC(uint8_t Num_ATT7022E, uint8_t addr)
{
	//读取对应地址里面的数据
	ATT7022E_SPI_CS_Selete(Num_ATT7022E, 0);
	int Data = ATT7022E_Read3Bytes(addr);
	ATT7022E_SPI_CS_Selete(Num_ATT7022E, 1);
	
	//检查通信结果是否发生错误，如果正确，记为1，如果发生错误，那么记录下此次通信结果正确标志为0
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



//记录下最后一次SPI通信结果是否正确
void ATT7022E_Check_Result_Set(uint8_t Num_ATT7022E, uint8_t Flag)
{
	Flag_ATT7022E_Check_Result[Num_ATT7022E] = Flag;
}



//获取最后一次SPI通信结果是否正确
uint8_t_t ATT7022E_Check_Result_Get(uint8_t Num_ATT7022E)
{
	return Flag_ATT7022E_Check_Result[Num_ATT7022E];
}



//记录是否发生了错误事件
void ATT7022E_Check_Error_Set(uint8_t Num_ATT7022E, uint8_t Flag)
{
	Flag_ATT7022E_Check_Error[Num_ATT7022E] = Flag;
}



//获取是否发生了错误事件
uint8_t_t ATT7022E_Check_Error_Get(uint8_t Num_ATT7022E)
{
	return Flag_ATT7022E_Check_Error[Num_ATT7022E];
}



//软复位
void ATT7022E_Reset_Soft(uint8_t Num_ATT7022E)
{
	//往0xD3内写入0x000000，能够对ATT7022E进行软复位操作
	ATT7022E_SPI_CS_Selete(Num_ATT7022E, 0);
	ATT7022E_Write3Bytes(0xD3, 0x000000);
	ATT7022E_SPI_CS_Selete(Num_ATT7022E, 1);
	
	vTaskDelay(1000);
};



//复位校准寄存器
void ATT7022E_Clear_AdjustParam(uint8_t Num_ATT7022E)
{
	//往0xC3地址写入0x000000，可以复位所有校准寄存器内的值为上电状态
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, 0xC3, 0x000000);
}



//设置是否能够写入校表参数
void ATT7022E_Write_AdjustParam_Set(uint8_t Num_ATT7022E, uint8_t Flag)
{
	if(Flag == 0)
	{
		//往0xC9地址写入*非0x00005A*数据，可以禁止写入校准寄存器
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, 0xC9, 0x000000);
	}
	else
	{
		//往0xC9地址写入0x00005A，可以使能写入校准寄存器
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, 0xC9, 0x00005A);
	}
}



//设置是否读取校准寄存器数据（1: 读取校准寄存器数据  0: 读取计量寄存器数据）
void ATT7022E_Read_AdjustParam_Set(uint8_t Num_ATT7022E, uint8_t Flag)
{
	if(Flag == 0)
	{
		//往0xC6地址中写入*非0x00005A*数据，那么选择读取计量寄存器的数据
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, 0xC6, 0x000000);
	}
	else
	{
		//往0xC6地址中写入0x00005A，那么选择读取校准寄存器的数据
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, 0xC6, 0x00005A);
	}
}



//计算HFconst值，返回INT型整数
int  ATT7022E_Calculate_HFconst(int _EC, float _Un, float _Ib, float _Vu, float Vi)
{
	int _HFconst = 0;
	
	_HFconst = (int)((2.592 * pow(10, 10) * 1.163 * 1.163 * _Vu * Vi) / (_EC * _Un * _Ib));
	printf("高频脉冲常数: %d\r\n", _HFconst);
	
	return _HFconst;
};



int ATT7022E_Calculate_Istartup(float Ib, float N, float Rate)
{
	float Io       = 0.0;
	int   Istartup = 0.0;
	
	Io       = Ib * N * Rate;
	Istartup = (int)(0.8 * Io * 8192);  //2^13 = 8192
	printf("起始电流: %d\r\n", Istartup);
	
	return Istartup;
}



int ATT7022E_Calculate_Ufail(float Un, float D)
{
	int Ufail = 0.0;
	
	Ufail = (int)(Un * 32 * D);  //2^5 = 32
	printf("失压阈值: %d\r\n", Ufail);
	
	return (int)Ufail;
}



//计算电压有效值Ugain，返回INT
int  ATT7022E_Calculate_Ugain(uint8_t Num_ATT7022E, float Ur, uint8_t_t Ugain_ABC)
{
	int     DataU        = 0.0;  //记录读取到的电压有效值
	float   Urms_Old     = 0.0;  //记录上一次测到的电压有效值
	float   Urms_Now     = 0.0;  //记录当前测到的电压有效值
	float   Urms_Average = 0.0;  //平均有效电压值
	int     Num_Urms     = 0;    //读取到的电压有效值的个数
	float   Sum_Urms     = 0.0;  //记录电压有效值的和
	float   Ugain        = 0.0;  //电压增益校准寄存器的值
	uint8_t ADDR_Ugain   = 0;    //记录电压增益校准地址（A、B、C三相电压增益的哪一相）
	
	switch(Ugain_ABC)
	{
		//A相电压增益校准
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
		printf("传参错误！\r\n");
		return 0;
		break;
	}
	
	printf("电压增益校准开始......\r\n");
		
	printf("电压有效值: ");
	while (1)
	{
		//读取出当前测量到的电压有效值
		DataU = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, ADDR_Ugain);
		if (DataU & 0x00800000L)
		{
			DataU |= 0xff000000L;
		}
		Urms_Now = (float)DataU / 8192; //2^13 = 8192
		printf("%f ", Urms_Now);
		
		//如果这次采集到的电压有效值和上一次的电压有效值相比，超出标准电压值的1%，说明误差太大或者是发生了电压波动，清零并重新采集
		if (abs((int)(Urms_Now - Urms_Old)) > (Ur * 0.01))
		{
			//更新电压有效值，用于下次对比
			Urms_Old = Urms_Now;
			
			//采集到的电压有效值的个数清零，电压有效值的和清零
			Num_Urms = 0;
			Sum_Urms = 0;
		}
		
		//计算采集到的电压有效值的和，用于计算平均电压有效值
		Sum_Urms += Urms_Now;
		
		//计算当前一共采集到了多少个电压有效值，用于计算平均电压有效值
		Num_Urms++;
		if (Num_Urms >= 10)
		{
			break;
		}
		
		//延时
		vTaskDelay(125);
	}
	printf("\r\n");
	
	//计算采集到的电压值的平均数
	Urms_Average = Sum_Urms / Num_Urms;
	printf("平均电压有效值：%f\r\n", Urms_Average);
	
	//根据公式计算电压增益校准寄存器的值
	Ugain = Ur / Urms_Average - 1;
	if (Ugain >= 0)
	{
		Ugain = (Ugain * pow(2, 15));
	}
	else
	{
		Ugain = (pow(2, 16) + Ugain * pow(2, 15));
	}
	printf("电压增益校准值: %f %u\r\n", Ugain, (int)Ugain);
		
	printf("电压增益校准结束\r\n");
		
	return (int)Ugain;
};



//计算电流有效值Igain，返回INT
int  ATT7022E_Calculate_Igain(uint8_t Num_ATT7022E, float Ir, float N, uint8_t_t Igain_ABC)
{
	int     DataI        = 0;    //通过SPI读取到的电流计量寄存器的值
	float   Irms_Now     = 0.0;  //记录当前采集到的电流有效值
	float   Irms_Old     = 0.0;  //记录上次采集到的电流有效值
	float   Irms_Average = 0.0;  //记录平均电流有效值
	int     Num_Irms     = 0;    //记录一共采集到了多少个电流有效值
	float   Sum_Irms     = 0.0;  //记录采集到的电流有效值的和（用于计算平均）
	float   Igain        = 0.0;  //记录
	uint8_t ADDR_Igain   = 0;    //记录电流增益寄存器的地址
	
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
		printf("传参错误！\r\n");
		return 0;
		break;
	}
	
	printf("电流增益校准开始\r\n");
		
	printf("电流有效值: ");
	while (1)
	{
		//读取当前采集到的电流有效值
		DataI = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, ADDR_Igain);
		if (DataI & 0x00800000L)
		{
			DataI |= 0xff000000L;
		}
		Irms_Now = (float)DataI / 8192 / N;  //2^13 = 8192
		printf("%f ", Irms_Now);
		
		//如果当前电流有效值和上次电流有效值相比，差距超过额定电流的1%，那么认为额定电流发生变化或者发生电流波动，需要重新开始读取
		if (abs((int)(Irms_Now - Irms_Old)) > (Ir * 0.01))
		{
			//更新电流有效值
			Irms_Old = Irms_Now;
			
			//清零读取到的电流有效值个数、电流有效值的和
			Num_Irms = 0;
			Sum_Irms = 0;
		}
		
		//计算当前采集到的电流有效值的和
		Sum_Irms += Irms_Now;
		
		//计算当前采集到的电流有效值的个数，如果大于等于设定的个数，那么跳出死循环
		Num_Irms++;
		if (Num_Irms >= 10)
		{
			break;
		}
		vTaskDelay(125);
	}
	printf("\r\n");

	//计算平均电流有效值
	Irms_Average = Sum_Irms / Num_Irms;
	printf("平均电流有效值: %f\r\n", Irms_Average);
	
	//计算电流增益校准
	Igain = Ir / Irms_Average - 1;
	if (Igain >= 0)
	{
		Igain = (Igain * pow(2, 15));
	}
	else
	{
		Igain = ((1 << 16) + Igain * pow(2, 15));
	}
	printf("电流增益校准值: %u\r\n", (int)Igain);
	
	printf("电流增益校准结束");
	
	return (int)Igain;
}



void ATT7022E_Calculate_PQSgain_PhSregpq_1(uint8_t Num_ATT7022E, float P_Real, float Q_Real, int HFconst, int _EC, uint8_t_t PQgain_PhSregpq_ABC)
{
	extern Struct_LTU_ATT7022E_Param_Reg  LTU_ATT7022E_AdjustParam_Reg;
	
	uint8_t ADDR_P     = 0;    //有功功率寄存器
	uint8_t ADDR_Q     = 0;    //无功功率寄存器
	int32_t DataP      = 0;    //记录从有功功率寄存器中读取到的原始值
	int32_t DataQ      = 0;    //记录从无功功率寄存器中读取到的原始值
	float   P_Now      = 0.0;  //记录当前测量到的有功功率值（单位: 瓦特W）
	float   Q_Now      = 0.0;  //记录当前测量到的无功功率值（单位: 瓦特W）
	float   P_Old      = 0.0;  //记录上次测量到的有功功率值（单位: 瓦特W）
	float   Q_Old      = 0.0;  //记录上次测量到的无功功率值（单位: 瓦特W）
	float   Sum_P      = 0.0;  //记录有功功率的和
	float   Sum_Q      = 0.0;  //记录无功功率的和
	int32_t Num_PQ     = 0;    //记录采集到了多少个有功功率无功功率
	float   K          = 0.0;  //功率参数（详情参考ATT7022E数据手册）
	float   P_Average  = 0.0;  //记录平均有功功率（单位: 瓦特W）
	float   Q_Average  = 0.0;  //记录平均无功功率（单位: 瓦特W）
	float   S_Real     = 0.0;  //标准视在功率
	float   S_Average  = 0.0;  //测得视在功率
	float   Pf_Real    = 0.0;  //标准功率因数
	float   Pf_Average = 0.0;  //测得功率因数
	float   P_Err      = 0.0;  //测得有功功率和标准有功功率之间的误差
	float   Q_Err      = 0.0;  //测得无功功率和标准无功功率之间的误差
	float   Angle_Err  = 0.0;  //角差
	int     PhSregpq   = 0;    //角差校正值
	float   PP         = 0.0;  //记录校正角差后的功率值
	float   Pgain      = 0.0;  //有功功率增益校准值
	int     Pgain_Int  = 0;    //记录需要写入功率增益寄存器里面的值
	
	switch(PQgain_PhSregpq_ABC)
	{
		//校准A相有功功率增益、无功功率增益
		case ATT7022E_ADJUST_PQSgain_PhSregpq_A:
		ADDR_P = 0x01;
		ADDR_Q = 0x05;
		break;
		
		//校准B相有功功率增益、无功功率增益
		case ATT7022E_ADJUST_PQSgain_PhSregpq_B:
		ADDR_P = 0x02;
		ADDR_Q = 0x06;
		break;
		
		//校准C相有功功率增益、无功功率增益
		case ATT7022E_ADJUST_PQSgain_PhSregpq_C:
		ADDR_P = 0x03;
		ADDR_Q = 0x07;
		break;
	}
	
	printf("功率增益校正开始 ...\r\n");
	
	//计算功率参数K
	K = 2.592f * pow(10, 10) / (HFconst * _EC * pow(2, 23));
	printf("功率参数: %f\r\n", K);
	
	while (1)
	{
		//从寄存器中读取出当前测量到的有功功率，再乘以K值转换成瓦特(W)为单位的有功功率有效值
		DataP = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, ADDR_P);
		if(DataP > pow(2, 23))
		{
			DataP -= pow(2, 24);
		}
		P_Now = DataP * K;
		printf("有功功率: %f\r\n", P_Now);
		
		//从寄存器中读取出当前测量到的无功功率，再乘以K值乘以无功增益转换成瓦特(W)为单位的无功功率有效值
		DataQ = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, ADDR_Q);
		if(DataQ > pow(2, 23))
		{
			DataQ -= pow(2, 24);
		}
		Q_Now = DataQ * K * 1.0005;
		printf("无功功率: %f\r\n", Q_Now);
				
		//判断当前测得功率和上次功率相比，变化是否超过1%
		if ((abs((int)(P_Now - P_Old)) > abs((int)(P_Real * 0.01))) || (abs((int)(Q_Now - Q_Old)) > abs((int)(Q_Real * 0.01))))
		{
			//更新有功功率、无功功率
			P_Old = P_Now;
			Q_Old = Q_Now;
			
			//清零测到的有功功率无功功率的个数、有功功率和、无功功率和
			Num_PQ = 0;
			Sum_P  = 0;
			Sum_Q  = 0;
		}
		
		//计算有功功率和、无功功率和、有功功率无功功率个数，用来计算功率的平均值
		Sum_P += P_Now;
		Sum_Q += Q_Now;
		Num_PQ++;
		
		//如果测到的有功功率无功功率个数大于10个，那么跳出死循环
		if (Num_PQ >= 10)
		{
			break;
		}
		
		vTaskDelay(125);
	}
	
	//计算有功功率、无功功率的平均功率值
	P_Average = Sum_P / Num_PQ;
	Q_Average = Sum_Q / Num_PQ;
	printf("平均有功功率: %fW\r\n",   P_Average);
	printf("平均无功功率: %fVar\r\n", Q_Average);
	
	//计算测得有功功率和标准有功功率之间的误差
	P_Err = (P_Average - P_Real) / P_Real;
	Q_Err = (Q_Average - Q_Real) / Q_Real;
	printf("有功功率误差: %f\r\n", P_Err);
	printf("无功功率误差: %f\r\n", Q_Err);
	
	//计算视在功率值
	S_Real    = sqrt((P_Real * P_Real) + (Q_Real * Q_Real));
	S_Average = sqrt((P_Average * P_Average) + (Q_Average * Q_Average));
	printf("标准视在功率: %fW\r\n",   S_Real);
	printf("测得视在功率: %fVar\r\n", S_Average);
	
	//计算标准及测量功率因数
	Pf_Real    = P_Real / S_Real;
	Pf_Average = P_Average / S_Average;
	printf("标准功率因数: %f\r\n", Pf_Real);
	printf("测得功率因数: %f\r\n", Pf_Average);
	
	//计算角差校正值
	Angle_Err = ((P_Real * Q_Average) - (P_Average * Q_Real)) / ((P_Average * P_Real) + (Q_Average * Q_Real));
	if(Angle_Err >= 0)
	{
		PhSregpq = (int)(Angle_Err * pow(2, 15));
	}
	else
	{
		PhSregpq = (int)(pow(2, 16) + (Angle_Err * pow(2, 15)));
	}
	printf("角差: %f\r\n",       Angle_Err);
	printf("角差校正值: %d\r\n", PhSregpq);
	
	//计算校正角差后的功率值
	PP = P_Average + (Q_Average * Angle_Err);
	
	printf("单点校正法\r\n");
	Pgain = P_Real / PP - 1;
	if(Pgain >= 0)
	{
		Pgain_Int = (int)(Pgain * pow(2, 15));
	}
	else
	{
		Pgain_Int = (int)(pow(2, 16) + (Pgain * pow(2, 15)));
	}
	printf("有功功率增益校正原始值:   %f\r\n", Pgain);
	printf("有功功率增益校正寄存器值: %d  0x%x\r\n", Pgain_Int, Pgain_Int);
	
	switch(PQgain_PhSregpq_ABC)
	{
		//校准A相有功功率增益、无功功率增益
		case ATT7022E_ADJUST_PQSgain_PhSregpq_A:
		LTU_ATT7022E_AdjustParam_Reg.Pga  = Pgain_Int;
		LTU_ATT7022E_AdjustParam_Reg.Qga  = Pgain_Int;
		LTU_ATT7022E_AdjustParam_Reg.Sga  = Pgain_Int;
		LTU_ATT7022E_AdjustParam_Reg.Pha0 = PhSregpq;
		LTU_ATT7022E_AdjustParam_Reg.Pha1 = PhSregpq;
		break;
		
		//校准B相有功功率增益、无功功率增益
		case ATT7022E_ADJUST_PQSgain_PhSregpq_B:
		LTU_ATT7022E_AdjustParam_Reg.Pgb  = Pgain_Int;
		LTU_ATT7022E_AdjustParam_Reg.Qgb  = Pgain_Int;
		LTU_ATT7022E_AdjustParam_Reg.Sgb  = Pgain_Int;
		LTU_ATT7022E_AdjustParam_Reg.Phb0 = PhSregpq;
		LTU_ATT7022E_AdjustParam_Reg.Phb1 = PhSregpq;
		break;
		
		//校准C相有功功率增益、无功功率增益
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
	
	uint8_t ADDR_P     = 0;    //有功功率寄存器
	uint8_t ADDR_Q     = 0;    //无功功率寄存器
	int32_t DataP      = 0;    //记录从有功功率寄存器中读取到的原始值
	int32_t DataQ      = 0;    //记录从无功功率寄存器中读取到的原始值
	float   P_Now      = 0.0;  //记录当前测量到的有功功率值（单位: 瓦特W）
	float   Q_Now      = 0.0;  //记录当前测量到的无功功率值（单位: 瓦特W）
	float   P_Old      = 0.0;  //记录上次测量到的有功功率值（单位: 瓦特W）
	float   Q_Old      = 0.0;  //记录上次测量到的无功功率值（单位: 瓦特W）
	float   Sum_P      = 0.0;  //记录有功功率的和
	float   Sum_Q      = 0.0;  //记录无功功率的和
	int32_t Num_PQ     = 0;    //记录采集到了多少个有功功率无功功率
	float   K          = 0.0;  //功率参数（详情参考ATT7022E数据手册）
	float   P_Average  = 0.0;  //记录平均有功功率（单位: 瓦特W）
	float   Q_Average  = 0.0;  //记录平均无功功率（单位: 瓦特W）
	float   S_Real     = 0.0;  //标准视在功率
	float   S_Average  = 0.0;  //测得视在功率
	float   Pf_Real    = 0.0;  //标准功率因数
	float   Pf_Average = 0.0;  //测得功率因数
	float   P_Err      = 0.0;  //测得有功功率和标准有功功率之间的误差
	float   Q_Err      = 0.0;  //测得无功功率和标准无功功率之间的误差
	float   Angle_Err  = 0.0;  //角差
	int     PhSregpq   = 0;    //角差校正值
	float   Pgain      = 0.0;  //有功功率增益校准值
	int     Pgain_Int  = 0;    //记录需要写入功率增益寄存器里面的值
	
	switch(PQgain_PhSregpq_ABC)
	{
		//校准A相有功功率增益、无功功率增益
		case ATT7022E_ADJUST_PQSgain_PhSregpq_A:
		ADDR_P = 0x01;
		ADDR_Q = 0x05;
		break;
		
		//校准B相有功功率增益、无功功率增益
		case ATT7022E_ADJUST_PQSgain_PhSregpq_B:
		ADDR_P = 0x02;
		ADDR_Q = 0x06;
		break;
		
		//校准C相有功功率增益、无功功率增益
		case ATT7022E_ADJUST_PQSgain_PhSregpq_C:
		ADDR_P = 0x03;
		ADDR_Q = 0x07;
		break;
	}
	
	printf("功率增益校正开始 ...\r\n");
	
	//计算功率参数K
	K = 2.592f * pow(10, 10) / (HFconst * _EC * pow(2, 23));
	printf("功率参数: %f\r\n", K);
	
	while (1)
	{
		//从寄存器中读取出当前测量到的有功功率，再乘以K值转换成瓦特(W)为单位的有功功率有效值
		DataP = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, ADDR_P);
		if(DataP > pow(2, 23))
		{
			DataP -= pow(2, 24);
		}
		P_Now = DataP * K;
		printf("有功功率: %f\r\n", P_Now);
		
		//从寄存器中读取出当前测量到的无功功率，再乘以K值乘以无功增益转换成瓦特(W)为单位的无功功率有效值
		DataQ = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, ADDR_Q);
		if(DataQ > pow(2, 23))
		{
			DataQ -= pow(2, 24);
		}
		Q_Now = DataQ * K * 1.0005;
		printf("无功功率: %f\r\n", Q_Now);
				
		//判断当前测得功率和上次功率相比，变化是否超过1%
		if ((abs((int)(P_Now - P_Old)) > abs((int)(P_Real * 0.01))) || (abs((int)(Q_Now - Q_Old)) > abs((int)(Q_Real * 0.01))))
		{
			//更新有功功率、无功功率
			P_Old = P_Now;
			Q_Old = Q_Now;
			
			//清零测到的有功功率无功功率的个数、有功功率和、无功功率和
			Num_PQ = 0;
			Sum_P  = 0;
			Sum_Q  = 0;
		}
		
		//计算有功功率和、无功功率和、有功功率无功功率个数，用来计算功率的平均值
		Sum_P += P_Now;
		Sum_Q += Q_Now;
		Num_PQ++;
		
		//如果测到的有功功率无功功率个数大于10个，那么跳出死循环
		if (Num_PQ >= 10)
		{
			break;
		}
		
		vTaskDelay(125);
	}
	
	//计算有功功率、无功功率的平均功率值
	P_Average = Sum_P / Num_PQ;
	Q_Average = Sum_Q / Num_PQ;
	printf("平均有功功率: %fW\r\n",   P_Average);
	printf("平均无功功率: %fVar\r\n", Q_Average);
	
	//计算测得有功功率和标准有功功率之间的误差
	P_Err = (P_Average - P_Real) / P_Real;
	Q_Err = (Q_Average - Q_Real) / Q_Real;
	printf("有功功率误差: %f\r\n", P_Err);
	printf("无功功率误差: %f\r\n", Q_Err);
	
	//计算视在功率值
	S_Real    = sqrt((P_Real * P_Real) + (Q_Real * Q_Real));
	S_Average = sqrt((P_Average * P_Average) + (Q_Average * Q_Average));
	printf("标准视在功率: %fW\r\n",   S_Real);
	printf("测得视在功率: %fVar\r\n", S_Average);
	
	//计算标准及测量功率因数
	Pf_Real    = P_Real / S_Real;
	Pf_Average = P_Average / S_Average;
	printf("标准功率因数: %f\r\n", Pf_Real);
	printf("测得功率因数: %f\r\n", Pf_Average);
	
	//计算角差校正值
	Angle_Err = (Pf_Average - Pf_Real) / Pf_Real;
	if(Angle_Err >= 0)
	{
		PhSregpq = (int)(Angle_Err * pow(2, 15));
	}
	else
	{
		PhSregpq = (int)(pow(2, 16) + (Angle_Err * pow(2, 15)));
	}
	printf("角差: %f\r\n",       Angle_Err);
	printf("角差校正值: %d\r\n", PhSregpq);
	
	//计算功率增益校正值
	Pgain = S_Real / S_Average - 1;
	if(Pgain >= 0)
	{
		Pgain_Int = (int)(Pgain * pow(2, 15));
	}
	else
	{
		Pgain_Int = (int)(pow(2, 16) + (Pgain * pow(2, 15)));
	}
	printf("有功功率增益校正原始值:   %f\r\n", Pgain);
	printf("有功功率增益校正寄存器值: %d  0x%x\r\n", Pgain_Int, Pgain_Int);
	
	switch(PQgain_PhSregpq_ABC)
	{
		//校准A相有功功率增益、无功功率增益
		case ATT7022E_ADJUST_PQSgain_PhSregpq_A:
		LTU_ATT7022E_AdjustParam_Reg.Pga  = Pgain_Int;
		LTU_ATT7022E_AdjustParam_Reg.Qga  = Pgain_Int;
		LTU_ATT7022E_AdjustParam_Reg.Sga  = Pgain_Int;
		LTU_ATT7022E_AdjustParam_Reg.Pha0 = PhSregpq;
		LTU_ATT7022E_AdjustParam_Reg.Pha1 = PhSregpq;
		break;
		
		//校准B相有功功率增益、无功功率增益
		case ATT7022E_ADJUST_PQSgain_PhSregpq_B:
		LTU_ATT7022E_AdjustParam_Reg.Pgb  = Pgain_Int;
		LTU_ATT7022E_AdjustParam_Reg.Qgb  = Pgain_Int;
		LTU_ATT7022E_AdjustParam_Reg.Sgb  = Pgain_Int;
		LTU_ATT7022E_AdjustParam_Reg.Phb0 = PhSregpq;
		LTU_ATT7022E_AdjustParam_Reg.Phb1 = PhSregpq;
		break;
		
		//校准C相有功功率增益、无功功率增益
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
	
	uint8_t ADDR_P    = 0;    //有功功率寄存器
	int32 Ugain     = 0;    //电压增益校准值
	int32 Igain     = 0;    //电流增益校准值
	int32 DataP     = 0;    //记录从有功功率寄存器中读取到的原始值
	float P_Now     = 0.0;  //记录当前测量到的有功功率值（单位: 瓦特W）
	float P_Old     = 0.0;  //记录上次测量到的有功功率值（单位: 瓦特W）
	float Sum_P     = 0.0;  //记录有功功率的和
	int32 Num_P     = 0;    //记录采集到了多少个有功功率无功功率
	float K         = 0.0;  //功率参数（详情参考ATT7022E数据手册）
	float P_Average = 0.0;  //记录平均有功功率（单位: 瓦特W）
	float P_Err     = 0.0;  //测得有功功率和标准有功功率之间的误差
	float Angle_Err = 0.0;  //角差
	int32 PhSregpq  = 0;    //角差校正值
	float Pgain     = 0.0;  //有功功率增益校准值
	int32 Pgain_Int = 0;    //记录需要写入功率增益寄存器里面的值
	
	//校正电压增益值
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
	
	//校正电流增益值
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
	
	//计算功率增益校正系数
	Pgain = ((Ugain + 1) * (Igain + 1) * HFconst * _EC * pow(2, 20) / (2.592 * pow(10, 10) * N)) - 1;
	if(Pgain >= 0)
	{
		Pgain_Int = (int32)(Pgain * pow(2, 15));
	}
	else
	{
		Pgain_Int = (int32)(pow(2, 16) + (Pgain * pow(2, 15)));
	}
	
	//写入功率增益校正系数
	switch(PQgain_PhSregpq_ABC)
	{
		//校准A相有功功率增益、无功功率增益
		case ATT7022E_ADJUST_PQSgain_PhSregpq_A:
		LTU_ATT7022E_AdjustParam_Reg.Pga = Pgain_Int;
		LTU_ATT7022E_AdjustParam_Reg.Qga = Pgain_Int;
		LTU_ATT7022E_AdjustParam_Reg.Sga = Pgain_Int;
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Pga, LTU_ATT7022E_AdjustParam_Reg.Pga);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Qga, LTU_ATT7022E_AdjustParam_Reg.Qga);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Sga, LTU_ATT7022E_AdjustParam_Reg.Sga);
		break;
		
		//校准B相有功功率增益、无功功率增益
		case ATT7022E_ADJUST_PQSgain_PhSregpq_B:
		LTU_ATT7022E_AdjustParam_Reg.Pgb = Pgain_Int;
		LTU_ATT7022E_AdjustParam_Reg.Qgb = Pgain_Int;
		LTU_ATT7022E_AdjustParam_Reg.Sgb = Pgain_Int;
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Pgb, LTU_ATT7022E_AdjustParam_Reg.Pgb);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Qgb, LTU_ATT7022E_AdjustParam_Reg.Qgb);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Sgb, LTU_ATT7022E_AdjustParam_Reg.Sgb);
		break;
		
		//校准C相有功功率增益、无功功率增益
		case ATT7022E_ADJUST_PQSgain_PhSregpq_C:
		LTU_ATT7022E_AdjustParam_Reg.Pgc = Pgain_Int;
		LTU_ATT7022E_AdjustParam_Reg.Qgc = Pgain_Int;
		LTU_ATT7022E_AdjustParam_Reg.Sgc = Pgain_Int;
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Pgc, LTU_ATT7022E_AdjustParam_Reg.Pgc);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Qgc, LTU_ATT7022E_AdjustParam_Reg.Qgc);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Sgc, LTU_ATT7022E_AdjustParam_Reg.Sgc);
		break;
	}
	
	//等待1.2s
	vTaskDelay(1200);

	//计算出测量到的功率值
	switch(PQgain_PhSregpq_ABC)
	{
		//校准A相有功功率增益、无功功率增益
		case ATT7022E_ADJUST_PQSgain_PhSregpq_A:
		ADDR_P = rPa;
		break;
		
		//校准B相有功功率增益、无功功率增益
		case ATT7022E_ADJUST_PQSgain_PhSregpq_B:
		ADDR_P = rPb;
		break;
		
		//校准C相有功功率增益、无功功率增益
		case ATT7022E_ADJUST_PQSgain_PhSregpq_C:
		ADDR_P = rPc;
		break;
	}
	
	//计算功率参数K
	K = 2.592f * pow(10, 10) / (HFconst * _EC * pow(2, 23));
	printf("功率参数: %f\r\n", K);
	
	while (1)
	{
		//从寄存器中读取出当前测量到的有功功率，再乘以K值转换成瓦特(W)为单位的有功功率有效值
		DataP = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, ADDR_P);
		if(DataP > pow(2, 23))
		{
			DataP -= pow(2, 24);
		}
		P_Now = DataP * K;
		printf("有功功率: %f\r\n", P_Now);

		//判断当前测得功率和上次功率相比，变化是否超过1%
		if (abs((int)(P_Now - P_Old)) > abs((int)(P_Real * 0.01)))
		{
			//更新有功功率、无功功率
			P_Old = P_Now;
			
			//清零测到的有功功率无功功率的个数、有功功率和、无功功率和
			Num_P = 0;
			Sum_P = 0;
		}
		
		//计算有功功率和、无功功率和、有功功率无功功率个数，用来计算功率的平均值
		Sum_P += P_Now;
		Num_P++;
		
		//如果测到的有功功率无功功率个数大于10个，那么跳出死循环
		if (Num_P >= 10)
		{
			break;
		}
		
		vTaskDelay(125);
	}
	
	//计算有功功率的平均值
	P_Average = Sum_P / Num_P;
	printf("平均有功功率: %fW\r\n",   P_Average);
	
	//计算角差
	P_Err = (P_Average - P_Real) / P_Real;
	printf("有功功率误差: %f\r\n", P_Err);
	
	//计算角差校正系数
	Angle_Err = -P_Err / 1.732;
	if(Angle_Err >= 0)
	{
		PhSregpq = (int32)(Angle_Err * pow(2, 15));
	}
	else
	{
		PhSregpq = (int32)(pow(2, 16) + (Angle_Err * pow(2, 15)));
	}
	printf("角差校正值: %d  0x%X\r\n", PhSregpq, PhSregpq);
	
	
	switch(PQgain_PhSregpq_ABC)
	{
		//校准A相有功功率增益、无功功率增益
		case ATT7022E_ADJUST_PQSgain_PhSregpq_A:
		LTU_ATT7022E_AdjustParam_Reg.Pha0 = PhSregpq;
		LTU_ATT7022E_AdjustParam_Reg.Pha1 = PhSregpq;
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Pha0, LTU_ATT7022E_AdjustParam_Reg.Pha0);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Pha1, LTU_ATT7022E_AdjustParam_Reg.Pha1);
		break;
		
		//校准B相有功功率增益、无功功率增益
		case ATT7022E_ADJUST_PQSgain_PhSregpq_B:
		LTU_ATT7022E_AdjustParam_Reg.Phb0 = PhSregpq;
		LTU_ATT7022E_AdjustParam_Reg.Phb1 = PhSregpq;
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Phb0, LTU_ATT7022E_AdjustParam_Reg.Phb0);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Phb1, LTU_ATT7022E_AdjustParam_Reg.Phb1);
		break;
		
		//校准C相有功功率增益、无功功率增益
		case ATT7022E_ADJUST_PQSgain_PhSregpq_C:
		LTU_ATT7022E_AdjustParam_Reg.Phc0 = PhSregpq;
		LTU_ATT7022E_AdjustParam_Reg.Phc1 = PhSregpq;
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Phc0, LTU_ATT7022E_AdjustParam_Reg.Phc0);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Phc1, LTU_ATT7022E_AdjustParam_Reg.Phc1);
		break;
	}
}



//存储校表参数进EEPROM当中
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



//判断ATT7022E的当前状态
void ATT7022E_Check_Status(uint8_t Num_ATT7022E)
{
	//外部定义的校表参数的全局变量
	extern Struct_LTU_ATT7022E_Param_Reg LTU_ATT7022E_AdjustParam_Reg;
	
	int Device_ID;
	
	//读取芯片的Device ID
	Device_ID = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, 0x00);
	//printf("ATT7022E的Device ID: 0x%06x\r\n", Device_ID);
	
	if(Device_ID == 0x7122A0)
	{
		//printf("ATT7022E处于启动状态，判断是否已经校表\r\n");
		
		//选择读取校准寄存器的值，禁止读取计量寄存器的值
		ATT7022E_Read_AdjustParam_Set(Num_ATT7022E, 1);
		
		if (ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, 0x1E) != LTU_ATT7022E_AdjustParam_Reg.HFconst)
		{
			//如果EEPROM是空的，说明没有进行过校表，将会直接以上电复位的默认值进行测量
			if(EEPROM_Check_Empty_Get())
			{
				//printf("ATT7022E没有校表，直接以默认值进行测量");
			}
			//如果EEPROM存在校表数据，说明校表失败
			else
			{
				ATT7022E_Check_Error_Set(Num_ATT7022E, 1);
				//printf("ATT7022E校表失败，需要重新校表\r\n");
			}
		}
		else
		{
			//printf("ATT7022E校表成功，运行......\r\n");
		}

		//选择读取计量寄存器的值，禁止读取校准寄存器的值
		ATT7022E_Read_AdjustParam_Set(Num_ATT7022E, 0);
	}
	else
	{
		//printf("ATT7022E状态错误\r\n");
	}
	
	//printf("\r\n");
};



//单点校正法: 方案2
void ATT7022E_Adjust_2(uint8_t Num_ATT7022E, uint8_t *P_Param_Uint8, uint16 Length)
{
	extern const uint32 Address_EEPROM_Reg_ATT7022E_Param[ATT7022E_NUM_TOTAL];
	extern const uint32 Length_EEPROM_Reg_ATT7022E_Param[ATT7022E_NUM_TOTAL];
	extern Struct_LTU_ATT7022E_Param_Reg LTU_ATT7022E_AdjustParam_Reg;
	
	//临时记录变量
	int32 Data_Reg = 0; //记录需要吸入校表寄存器的值
	int32 Ugain    = 0; //记录ABC三相电压增益值
	int32 Igain    = 0; //记录ABC三相电流增益值
	uint8_t Flag_Result = 0;  //记录结果
	
	//定义一个记录校表参数的结构体变量
	Struct_ATT7022E_AdjustParam_Standard AdjustParam;
	
	//如果传入参数的缓冲区指针为空，或者是长度不是60个字节，那么校表参数错误，只接退出
	if ((P_Param_Uint8 == NULL) || (Length != 60))
	{
		return;
	}

	memcpy((uint8_t *)&AdjustParam, P_Param_Uint8, 60);

	printf("|-------------------------------------------------|\r\n");
	printf("|              开始校表(方案2): %3d               |\r\n", Num_ATT7022E);
	printf("|-------------------------------------------------|\r\n");
	
	printf("\r\n");
	
	printf("脉冲常数: %d\r\n", AdjustParam._EC);
	printf("额定电压: %f\r\n", AdjustParam.Un);
	printf("额定电流: %f\r\n", AdjustParam.Ib);
	printf("A相电压: %f  电流: %f  有功功率: %f  无功功率: %f\r\n", AdjustParam.UA, AdjustParam.IA, AdjustParam.PA, AdjustParam.QA);
	printf("B相电压: %f  电流: %f  有功功率: %f  无功功率: %f\r\n", AdjustParam.UB, AdjustParam.IB, AdjustParam.PB, AdjustParam.QB);
	printf("C相电压: %f  电流: %f  有功功率: %f  无功功率: %f\r\n", AdjustParam.UC, AdjustParam.IC, AdjustParam.PC, AdjustParam.QC);
	
	printf("\r\n");

	//复位所有校准寄存器内的值
	ATT7022E_Clear_AdjustParam(Num_ATT7022E);
	Delay_us(100);
	
	//使能写入校准寄存器
	ATT7022E_Write_AdjustParam_Set(Num_ATT7022E, 1);
	
	//计算电流系数N值（电流采样电压为50mV时）
	LTU_ATT7022E_AdjustParam_Reg.N   = 60 / AdjustParam.Ib;  //计算电流系数N值
	LTU_ATT7022E_AdjustParam_Reg._EC = AdjustParam._EC;      //计算电表常数EC
	
	//配置模式寄存器
	Data_Reg = 0;
	Data_Reg &= ~BIT(0);   //零线电流I0的ADC通道（0: 关闭  1: 打开）
	Data_Reg |=  BIT(1);   //A相电流Ia的ADC通道（0: 关闭  1: 打开）
	Data_Reg |=  BIT(2);   //A相电压Ua的ADC通道（0: 关闭  1: 打开）
	Data_Reg |=  BIT(3);   //B相电流Ib的ADC通道（0: 关闭  1: 打开）
	Data_Reg |=  BIT(4);   //B相电压Ub的ADC通道（0: 关闭  1: 打开）
	Data_Reg |=  BIT(5);   //C相电流Ic的ADC通道（0: 关闭  1: 打开）
	Data_Reg |=  BIT(6);   //C相电压Uc的ADC通道（0: 关闭  1: 打开）
	Data_Reg |=  BIT(7);   //ADC的Chop功能，能够提高ADC的性能（0: 关闭  1: 打开，推荐打开）
	Data_Reg |=  BIT(8);   //Femu时钟选择921.6kHz（[BIT9:BIT8]  00: 1.8432MHz  01: 921.6kHz  1X: 460.8kHz）
	Data_Reg &= ~BIT(9);
	Data_Reg &= ~BIT(10);  //选择Iref偏置电流，一般兼顾功耗和性能，折中选择7.5uA（[BIT11:BIT10]  00: 5uA  01: 5uA  10: 7.5uA  11: 10uA）
	Data_Reg |=  BIT(11);
	Data_Reg |=  BIT(12);  //有效值和功率的更新速率选择，一般为获取稳定的值，设为慢速（0: 快速28.1Hz  1: 慢速1.76Hz）
	Data_Reg |=  BIT(13);  //选择有效值的稳定时间，一般为获取稳定的值，设为慢速，跳动小（0: 快速  1: 慢速）
	Data_Reg &= ~BIT(14);  //三相三线时，额定电压Ub有效值的数据源选择（0: 内部Ua-Uc  1: Ub通道）
	Data_Reg |=  BIT(15);  //开启Ref的Chop功能，为得到更稳定的Vref，推荐打开（0: 关闭  1: 打开）
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_ModeCfg, Data_Reg);
	//记录在全局变量里面
	LTU_ATT7022E_AdjustParam_Reg.ModeCfg = Data_Reg;
	
	//配置EMU单元
	Data_Reg = 0;
	Data_Reg &= ~BIT(0);   //波形采样频率选择（[BIT2:BIT0]  1XX: 14.4K  011: 7.2K  010: 3.6K  001: 1.8K  000: 0.9K）
	Data_Reg &= ~BIT(1);
	Data_Reg |=  BIT(2);
	Data_Reg &= ~BIT(3);   //视在功率、能量寄存器计数方式（0: PQS方式计量  1: RMS方式计量）
	Data_Reg &= ~BIT(4);   //选择电压过0中断方式（[BIT5:BIT4]  1X: 双向过0中断  01: 负向过0中断  00: 正向过0中断）
	Data_Reg &= ~BIT(5);
	Data_Reg &= ~BIT(6);   //累加方式（0: 三相四线用绝对值累加，三相三线用代数值累加  1: 三相四线用代数值累加，三相三线用绝对值累加）
	Data_Reg |=  BIT(7);   //能量寄存器清零方式（0: 能量寄存器读后不清0  1: 能量寄存器读后请0）
	Data_Reg |=  BIT(8);   //波形缓冲数据源选择（[BIT9:BIT8]  00: ADC数据不经高通滤波器的原始数据  01: ADC数据经高通滤波器且经增益校正  1X: ADC采样数据经基波滤波器）
	Data_Reg &= ~BIT(9);
	Data_Reg &= ~BIT(10);  //基波/谐波计量功能（0: 关闭  1: 开启）
	Data_Reg |=  BIT(11);  //潜动/起动判断依据（0: 选择电流有效值作为潜动/启动判断依据  1: 选择功率作为潜动/启动判断依据，推荐功率）
	Data_Reg |=  BIT(12);  //有功能量CF1通路能量计量功能（0: 关闭  1: 开启）
	Data_Reg |=  BIT(13);  //无功能量CF2通路能量计量功能（0: 关闭  1: 开启）
	Data_Reg |=  BIT(14);  //视在能量CF3通路能量计量功能（0: 关闭  1: 开启）
	Data_Reg |=  BIT(15);  //基波有功能量CF4通路能量计量功能（0: 关闭  1: 开启）
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_EMUCfg, Data_Reg);
	//记录在全局变量里面
	LTU_ATT7022E_AdjustParam_Reg.EMUCfg = Data_Reg;
	
	//模拟模块使能寄存器
	Data_Reg = 0;
	Data_Reg |=  BIT(0);   //第七通道数字高通滤波器（0: 关闭  1: 开启）
	Data_Reg |=  BIT(1);   //电流通道数字高通滤波器（0: 关闭  1: 开启）
	Data_Reg |=  BIT(2);   //电压通道数字高通滤波器（0: 关闭  1: 开启）
	Data_Reg &= ~BIT(3);   //电流通道的罗氏线圈功能（0: 关闭  1: 开启）
	Data_Reg |=  BIT(4);   //TPS功能（0: 关闭  1: 开启）
	Data_Reg |=  BIT(5);   //BOR功能（0: 关闭  1: 开启）
	Data_Reg &= ~BIT(6);   //中断电平（0: 低电平有效  1: 高电平有效）
	Data_Reg &= ~BIT(7);   //温度传感器类型选择信号（0: PN传感器，推荐  1: 电阻传感器）
	Data_Reg &= ~BIT(11);  //SPI速率选择（0: 低速模式  1: 高速模式）
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_ModuleCfg, Data_Reg);
	//记录在全局变量里面
	LTU_ATT7022E_AdjustParam_Reg.ModuleCfg = Data_Reg;

	//设置ADC增益
	Data_Reg = 0;
	Data_Reg &= ~BIT(0);  //零线电流ADC增益（[BIT1:BIT0]  00: 1倍  01: 2倍  10: 8倍  11: 16倍）
	Data_Reg &= ~BIT(1);
	Data_Reg |=  BIT(2);  //A相电流ADC增益（[BIT3:BIT2]  00: 1倍  01: 2倍  10: 8倍  11: 16倍）
	Data_Reg &= ~BIT(3);
	Data_Reg |=  BIT(4);  //B相电流ADC增益（[BIT5:BIT4]  00: 1倍  01: 2倍  10: 8倍  11: 16倍）
	Data_Reg &= ~BIT(5);
	Data_Reg |=  BIT(6);  //C相电流ADC增益（[BIT7:BIT6]  00: 1倍  01: 2倍  10: 8倍  11: 16倍）
	Data_Reg &= ~BIT(7);
	Data_Reg |=  BIT(8);  //三相电压ADC增益（[BIT9:BIT8]  00: 1倍  01: 2倍  10: 8倍  11: 8倍）
	Data_Reg &= ~BIT(9);
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_PGACtrl, LTU_ATT7022E_AdjustParam_Reg.PGACtrl);
	//记录在全局变量里面
	LTU_ATT7022E_AdjustParam_Reg.PGACtrl = Data_Reg;
	
	//校表电流信号: 0.072V
	//校表电压信号: 0.050V
	
	//设置高频脉冲常数
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
	//记录在全局变量里面
	LTU_ATT7022E_AdjustParam_Reg.HFconst = Data_Reg;
	
	//设置起动电流
	Data_Reg = 0;
	Data_Reg = (int32)(0.8 * (AdjustParam.Ib * LTU_ATT7022E_AdjustParam_Reg.N * 0.004) * pow(2, 13));
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Istart, LTU_ATT7022E_AdjustParam_Reg.Istart);
	//记录在全局变量里面
	LTU_ATT7022E_AdjustParam_Reg.Istart = Data_Reg;
	
	//设置失压阈值
	Data_Reg = 0;
	Data_Reg = (int32)(AdjustParam.Un * pow(2, 5) * 0.7);
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Ufail, LTU_ATT7022E_AdjustParam_Reg.Ufail);
	//记录在全局变量里面
	LTU_ATT7022E_AdjustParam_Reg.Ufail = Data_Reg;
	
	//设置起动功率
	Data_Reg = 0;
	Data_Reg = (int32)(0.6 * AdjustParam.Un * AdjustParam.Ib * LTU_ATT7022E_AdjustParam_Reg.HFconst * AdjustParam._EC * 0.004 * pow(2, 23) / (2.592 * pow(10, 10)));
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Pstart, Data_Reg);
	//记录在全局变量里面
	LTU_ATT7022E_AdjustParam_Reg.Pstart = Data_Reg;
	
	//设置相位补偿区域
	//......
	
	//校准A相功率增益补偿寄存器
	printf("A相功率增益校准开始\r\n");
	ATT7022E_Calculate_PQSgain_PhSregpq_1(Num_ATT7022E,          //校准哪个ATT7022E
										  AdjustParam.PA,        //A相标准有功功率
										  AdjustParam.QA,        //A相标准无功功率
										  LTU_ATT7022E_AdjustParam_Reg.HFconst,  //高频脉冲常数寄存器的值
										  AdjustParam._EC,       //电表常数
										  ATT7022E_ADJUST_PQSgain_PhSregpq_A);  //校准A相功率
	printf("A相功率增益校准结束\r\n");
	
	printf("\r\n");
	
	//校准B相功率增益补偿寄存器
	printf("B相功率增益校准开始\r\n");
	ATT7022E_Calculate_PQSgain_PhSregpq_1(Num_ATT7022E,          //校准哪个ATT7022E
										  AdjustParam.PB,        //B相标准有功功率
										  AdjustParam.QB,        //B相标准无功功率
										  LTU_ATT7022E_AdjustParam_Reg.HFconst,  //高频脉冲常数寄存器的值
										  AdjustParam._EC,       //电表常数
										  ATT7022E_ADJUST_PQSgain_PhSregpq_B);  //校准B相功率
	printf("B相功率增益校准结束\r\n");
	
	printf("\r\n");
	
	//校准C相功率增益补偿寄存器
	printf("C相功率增益校准开始\r\n");
	ATT7022E_Calculate_PQSgain_PhSregpq_1(Num_ATT7022E,          //校准哪个ATT7022E
										  AdjustParam.PC,        //C相标准有功功率
										  AdjustParam.QC,        //C相标准无功功率
										  LTU_ATT7022E_AdjustParam_Reg.HFconst,  //高频脉冲常数寄存器的值
										  AdjustParam._EC,       //电表常数
										  ATT7022E_ADJUST_PQSgain_PhSregpq_C);  //校准C相功率
	printf("C相功率增益校准结束\r\n");
	
	printf("\r\n");
	
	//校准A相电压增益
	printf("A相电压增益校正开始......\r\n");
	Ugain = ATT7022E_Calculate_Ugain(Num_ATT7022E, AdjustParam.UA, ATT7022E_ADJUST_Ugain_A);
	if (Ugain >= 0)
	{
		LTU_ATT7022E_AdjustParam_Reg.Uga = Ugain;
		printf("A相电压增益校正成功！\r\n");
	}
	else
	{
		printf("A相电压增益校正失败！\r\n");
	}
	
	printf("\r\n");
	
	//校准B相电压增益
	printf("B相电压增益校正开始......\r\n");
	Ugain = ATT7022E_Calculate_Ugain(Num_ATT7022E, AdjustParam.UB, ATT7022E_ADJUST_Ugain_B);
	if (Ugain >= 0)
	{
		LTU_ATT7022E_AdjustParam_Reg.Ugb = Ugain;
		printf("B相电压增益校正成功！\r\n");
	}
	else
	{
		printf("B相电压增益校正失败！\r\n");
	}
	
	printf("\r\n");
	
	//校准C相电压增益
	printf("C相电压增益校正开始......\r\n");
	Ugain = ATT7022E_Calculate_Ugain(Num_ATT7022E, AdjustParam.UC, ATT7022E_ADJUST_Ugain_C);
	if (Ugain >= 0)
	{
		LTU_ATT7022E_AdjustParam_Reg.Ugc = Ugain;
		printf("C相电压增益校正成功！\r\n");
	}
	else
	{
		printf("C相电压增益校正失败！\r\n");
	}
	
	printf("\r\n");
	
	//校准A相电流增益
	printf("A相电流增益校正开始......\r\n");
	Igain = ATT7022E_Calculate_Igain(Num_ATT7022E, AdjustParam.IA, LTU_ATT7022E_AdjustParam_Reg.N, ATT7022E_ADJUST_Igain_A);
	if (Igain >= 0)
	{
		LTU_ATT7022E_AdjustParam_Reg.Iga = Igain;
		printf("A相电流增益校正成功！\r\n");
	}
	else
	{
		printf("A相电流增益校正失败！\r\n");
	}
	
	printf("\r\n");
	
	//校准B相电流增益
	printf("B相电流增益校正开始......\r\n");
	Igain = ATT7022E_Calculate_Igain(Num_ATT7022E, AdjustParam.IB, LTU_ATT7022E_AdjustParam_Reg.N, ATT7022E_ADJUST_Igain_B);
	if (Igain >= 0)
	{
		LTU_ATT7022E_AdjustParam_Reg.Igb = Igain;
		printf("B相电流增益校正成功！\r\n");
	}
	else
	{
		printf("B相电流增益校正失败！\r\n");
	}
	
	printf("\r\n");
	
	//校准C相电流增益
	printf("C相电流增益校正开始......\r\n");
	Igain = ATT7022E_Calculate_Igain(Num_ATT7022E, AdjustParam.IC, LTU_ATT7022E_AdjustParam_Reg.N, ATT7022E_ADJUST_Igain_C);
	if (Igain >= 0)
	{
		LTU_ATT7022E_AdjustParam_Reg.Igc = Igain;
		printf("C相电流增益校正成功！\r\n");
	}
	else
	{
		printf("C相电流增益校正失败！\r\n");
	}
	
	printf("\r\n");
	
	//把校正后的ABC三相功率增益校准值写入功率增益校准寄存器
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Pga, LTU_ATT7022E_AdjustParam_Reg.Pga);
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Qga, LTU_ATT7022E_AdjustParam_Reg.Qga);
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Sga, LTU_ATT7022E_AdjustParam_Reg.Sga);
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Pgb, LTU_ATT7022E_AdjustParam_Reg.Pgb);
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Qgb, LTU_ATT7022E_AdjustParam_Reg.Qgb);
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Sgb, LTU_ATT7022E_AdjustParam_Reg.Sgb);
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Pgc, LTU_ATT7022E_AdjustParam_Reg.Pgc);
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Qgc, LTU_ATT7022E_AdjustParam_Reg.Qgc);
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Sgc, LTU_ATT7022E_AdjustParam_Reg.Sgc);
	
	//把校正后的相位校正值写入相位校准寄存器
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Pha0, LTU_ATT7022E_AdjustParam_Reg.Pha0);
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Phb0, LTU_ATT7022E_AdjustParam_Reg.Phb0);
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Phc0, LTU_ATT7022E_AdjustParam_Reg.Phc0);
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Pha1, LTU_ATT7022E_AdjustParam_Reg.Pha1);
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Phb1, LTU_ATT7022E_AdjustParam_Reg.Phb1);
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Phc1, LTU_ATT7022E_AdjustParam_Reg.Phc1);
	
	//把校正后的ABC三相电压增益校准值写入电压增益校准寄存器
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Uga, LTU_ATT7022E_AdjustParam_Reg.Uga);
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Ugb, LTU_ATT7022E_AdjustParam_Reg.Ugb);
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Ugc, LTU_ATT7022E_AdjustParam_Reg.Ugc);
	
	//把校正后的ABC三相电流增益校准值写入电流增益校准寄存器
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Iga, LTU_ATT7022E_AdjustParam_Reg.Iga);
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Igb, LTU_ATT7022E_AdjustParam_Reg.Igb);
	ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Igc, LTU_ATT7022E_AdjustParam_Reg.Igc);
	
	//禁止写入校准寄存器，防止误写入
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
	
	//记录功率参数K
	LTU_ATT7022E_AdjustParam_Reg.K = 2.592 * pow(10, 10) / (LTU_ATT7022E_AdjustParam_Reg.HFconst * AdjustParam._EC * pow(2, 23));
	
	//记录校验和
	LTU_ATT7022E_AdjustParam_Reg.CheckSum = GetSum8((uint8_t *)&LTU_ATT7022E_AdjustParam_Reg, sizeof(LTU_ATT7022E_AdjustParam_Reg) - sizeof(LTU_ATT7022E_AdjustParam_Reg.CheckSum));
	printf("计算校验和: 0x%X\r\n", LTU_ATT7022E_AdjustParam_Reg.CheckSum);
	
	printf("\r\n");
	
	//把校表参数写入EEPROM中
	printf("校表数据存入EEPROM\r\n");
	Flag_Result = EEPROM_Write_ATT7022E_Param_Reg(&LTU_ATT7022E_AdjustParam_Reg, Num_ATT7022E);
	if(Flag_Result == 0)
	{
		printf("校表数据存入EEPROM校验出错\r\n");
	}
	printf("\r\n");
	printf("校表数据存储完成\r\n");
	
	//如果EEPROM是空片，那么写入特定字符串，让其变为非空片
	if(EEPROM_Check_Empty_Get())
	{
		EEPROM_WriteString(ADDRESS_EEPROM_REG_EMPTY_CHECK, LENGTH_EEPROM_REG_EMPTY_CHECK, STRING_EEPROM_REG_EMPTY_CHECK);
		//运行一遍空片检测程序
		EEPROM_Check_Empty();
	}
	
	printf("\r\n");
	
	printf("校表结束\r\n");
	
	printf("\r\n");
}



//把EEPROM中的校表参数写入ATT7022E里面去
void ATT7022E_Load_AdjustParam_From_EEPROM(uint8_t Num_ATT7022E)
{
	//外部定义的EEPROM存储交表数据的起始地址数组
	extern const uint32 Address_EEPROM_Reg_ATT7022E_Param[ATT7022E_NUM_TOTAL];
	extern const uint32 Length_EEPROM_Reg_ATT7022E_Param[ATT7022E_NUM_TOTAL];
	extern Struct_LTU_ATT7022E_Param_Reg LTU_ATT7022E_AdjustParam_Reg;
	extern Struct_LTU_Data LTU_Data;
	
	uint8_t  Buffer_AdjustParam[LENGTH_EEPROM_REG_ATT7022E_PARAM_0];  //存储从EEPROM中直接取出的原始数据
	uint8_t Flag_Result = 0;
	
	//初始化相关变量
	memset(Buffer_AdjustParam, 0, LENGTH_EEPROM_REG_ATT7022E_PARAM_0);
	
	//从EEPROM中读取出校表数据
	Flag_Result = EEPROM_Read_ATT7022E_Param_Reg(&LTU_ATT7022E_AdjustParam_Reg, Num_ATT7022E);
	if(Flag_Result)
	{
		printf("从EEPROM中读取校表参数成功\r\n");
		
		//复位所有校准寄存器内的值
		ATT7022E_Clear_AdjustParam(Num_ATT7022E);
		
		//使能写入校准寄存器
		ATT7022E_Write_AdjustParam_Set(Num_ATT7022E, 1);
		
		//校准相关寄存器
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_ModeCfg,   LTU_ATT7022E_AdjustParam_Reg.ModeCfg);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_EMUCfg,    LTU_ATT7022E_AdjustParam_Reg.EMUCfg);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_ModuleCfg, LTU_ATT7022E_AdjustParam_Reg.ModuleCfg);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_PGACtrl,   LTU_ATT7022E_AdjustParam_Reg.PGACtrl);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_HFconst,   LTU_ATT7022E_AdjustParam_Reg.HFconst);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Istart,    LTU_ATT7022E_AdjustParam_Reg.Istart);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Ufail,     LTU_ATT7022E_AdjustParam_Reg.Ufail);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Pstart,    LTU_ATT7022E_AdjustParam_Reg.Pstart);
		
		//校准功率增益
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Pga, LTU_ATT7022E_AdjustParam_Reg.Pga);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Qga, LTU_ATT7022E_AdjustParam_Reg.Qga);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Sga, LTU_ATT7022E_AdjustParam_Reg.Sga);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Pgb, LTU_ATT7022E_AdjustParam_Reg.Pgb);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Qgb, LTU_ATT7022E_AdjustParam_Reg.Qgb);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Sgb, LTU_ATT7022E_AdjustParam_Reg.Sgb);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Pgc, LTU_ATT7022E_AdjustParam_Reg.Pgc);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Qgc, LTU_ATT7022E_AdjustParam_Reg.Qgc);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Sgc, LTU_ATT7022E_AdjustParam_Reg.Sgc);
		
		//校准相角补偿
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Pha0, LTU_ATT7022E_AdjustParam_Reg.Pha0);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Phb0, LTU_ATT7022E_AdjustParam_Reg.Phb0);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Phc0, LTU_ATT7022E_AdjustParam_Reg.Phc0);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Pha1, LTU_ATT7022E_AdjustParam_Reg.Pha1);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Phb1, LTU_ATT7022E_AdjustParam_Reg.Phb1);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Phc1, LTU_ATT7022E_AdjustParam_Reg.Phc1);
		
		//校准电压增益
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Uga, LTU_ATT7022E_AdjustParam_Reg.Uga);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Ugb, LTU_ATT7022E_AdjustParam_Reg.Ugb);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Ugc, LTU_ATT7022E_AdjustParam_Reg.Ugc);
		
		//校准电流增益
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Iga, LTU_ATT7022E_AdjustParam_Reg.Iga);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Igb, LTU_ATT7022E_AdjustParam_Reg.Igb);
		ATT7022E_Write3Bytes_CheckCRC(Num_ATT7022E, ATT_ADDR_Igc, LTU_ATT7022E_AdjustParam_Reg.Igc);
		
		//禁止写入校准寄存器（校准寄存器的写保护）
		ATT7022E_Write_AdjustParam_Set(Num_ATT7022E, 0);
		
		printf("配置相关寄存器\r\n");
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
		printf("已经成功把校表参数装载进ATT7022E中\r\n");
	}
	else
	{
		printf("从EEPROM中获取校表数据时校验失败\r\n");
	}
	printf("\r\n");
}



//获取当前瞬时电压
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
	
	//从寄存器中获取出相电压的原始值
	Data_Ua   = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rUa);
	Data_Ub   = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rUb);
	Data_Uc   = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rUc);
	Data_YUab = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rYUaUb);
	Data_YUac = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rYUaUc);
	Data_YUbc = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rYUbUc);
	
	//如果通信发生了故障
	if(ATT7022E_Check_Result_Get(Num_ATT7022E) == 0)
	{
		printf("获取相电压时发生通信故障\r\n");
	}
	
	//换算成国际单位值
	Ua = (float)Data_Ua / pow(2, 13);
	Ub = (float)Data_Ub / pow(2, 13);
	Uc = (float)Data_Uc / pow(2, 13);
	YUab = (float)Data_YUab / pow(2, 20) * 180;
	YUac = (float)Data_YUac / pow(2, 20) * 180;
	YUbc = (float)Data_YUbc / pow(2, 20) * 180;
	
	//存放在全局变量中
	LTU_Data.ATT7022E.UA[Num_ATT7022E]  = Ua;
	LTU_Data.ATT7022E.UB[Num_ATT7022E]  = Ub;
	LTU_Data.ATT7022E.UC[Num_ATT7022E]  = Uc;
	LTU_Data.ATT7022E.YUA[Num_ATT7022E] = 0;
	LTU_Data.ATT7022E.YUB[Num_ATT7022E] = YUab;
	LTU_Data.ATT7022E.YUC[Num_ATT7022E] = (YUac + (YUab + YUbc)) / 2;
}



//获取当前瞬时电流
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
	
	//获取电流参数
	N = LTU_ATT7022E_AdjustParam_Reg.N;
	
	//从寄存器中获取出相电流的原始值
	Data_Ia = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rIa);
	Data_Ib = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rIb);
	Data_Ic = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rIc);
	
	//如果通信发生了故障
	if(ATT7022E_Check_Result_Get(Num_ATT7022E) == 0)
	{
		printf("获取相电流时发生通信故障\r\n");
	}
	
	//换算成以安培A为单位
	Ia = (float)(Data_Ia / pow(2, 13)) / N;
	Ib = (float)(Data_Ib / pow(2, 13)) / N;
	Ic = (float)(Data_Ic / pow(2, 13)) / N;

#ifdef LTU_TRANSFORMS_RATE
	//printf("\nLTU_Transformer_Rate.Numerator=%d, Denominator=%d, %.2f, %.2f, %.2f\n", LTU_Transformer_Rate.Numerator, LTU_Transformer_Rate.Denominator, Ia, Ib, Ic);
	Ia = (float)(LTU_Transformer_Rate.Numerator / LTU_Transformer_Rate.Denominator)*Ia;
	Ib = (float)(LTU_Transformer_Rate.Numerator / LTU_Transformer_Rate.Denominator)*Ib;
	Ic = (float)(LTU_Transformer_Rate.Numerator / LTU_Transformer_Rate.Denominator)*Ic;
#endif
	
	//存放在全局变量中
	LTU_Data.ATT7022E.IA[Num_ATT7022E] = Ia;
	LTU_Data.ATT7022E.IB[Num_ATT7022E] = Ib;
	LTU_Data.ATT7022E.IC[Num_ATT7022E] = Ic;
}



//获取当前相角
void ATT7022E_Get_PhSregpq(uint8_t Num_ATT7022E)
{
	extern Struct_LTU_Data LTU_Data;
	
	int   Data_PhSregpq_A = 0;
	int   Data_PhSregpq_B = 0;
	int   Data_PhSregpq_C = 0;
	float PhSregpq_A      = 0.0f;
	float PhSregpq_B      = 0.0f;
	float PhSregpq_C      = 0.0f;
	
	//从相角寄存器中取出原始值
	Data_PhSregpq_A = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rPga);
	Data_PhSregpq_B = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rPgb);
	Data_PhSregpq_C = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rPgc);
	
	//如果通信发生了故障
	if(ATT7022E_Check_Result_Get(Num_ATT7022E) == 0)
	{
		printf("获取相角数据时发生通信故障\r\n");
		return;
	}
	
	//换算成国际单位值（单位: °）
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
	
	//存放在全局变量中
	LTU_Data.ATT7022E.PhSregpqA[Num_ATT7022E] = PhSregpq_A;
	LTU_Data.ATT7022E.PhSregpqB[Num_ATT7022E] = PhSregpq_B;
	LTU_Data.ATT7022E.PhSregpqC[Num_ATT7022E] = PhSregpq_C;
}



/*
//获取当前相角
void ATT7022E_Get_PhSregpq(uint8_t Num_ATT7022E)
{
	extern Struct_LTU_Data LTU_Data;
	
	int   Data_PhSregpq_A = 0;
	int   Data_PhSregpq_B = 0;
	int   Data_PhSregpq_C = 0;
	uint8_t Flag_Negative   = 0;  //用来标定是否为负数
	float PhSregpq_A      = 0.0f;
	float PhSregpq_B      = 0.0f;
	float PhSregpq_C      = 0.0f;
	
	//从相角寄存器中取出原始值
	Data_PhSregpq_A = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rPga);
	Data_PhSregpq_B = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rPgb);
	Data_PhSregpq_C = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rPgc);
	
	//如果通信发生了故障
	if(ATT7022E_Check_Result_Get(Num_ATT7022E) == 0)
	{
		printf("获取相角数据时发生通信故障\r\n");
		return;
	}
	
	//换算成国际单位值（单位: °）
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
	
	//存放在全局变量中
	LTU_Data.ATT7022E.PhSregpqA[Num_ATT7022E] = PhSregpq_A;
	LTU_Data.ATT7022E.PhSregpqB[Num_ATT7022E] = PhSregpq_B;
	LTU_Data.ATT7022E.PhSregpqC[Num_ATT7022E] = PhSregpq_C;
}
*/



//获取当前瞬时功率
void ATT7022E_Get_Power(uint8_t Num_ATT7022E)
{
	extern Struct_LTU_ATT7022E_Param_Reg LTU_ATT7022E_AdjustParam_Reg;
	extern Struct_LTU_Data                 LTU_Data;
	
	//存储从寄存器中取出的原始数据
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
	
	//存储转换成国际单位制的数据
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
	
	//从功率寄存器中读取出原始值
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
	
	//如果通信发生了故障
	if(ATT7022E_Check_Result_Get(Num_ATT7022E) == 0)
	{
		printf("获取功率数据时发生通信故障\r\n");
		return;
	}
	
	//换算成国际单位值
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
	//存放在全局变量中
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
//获取当前瞬时功率
void ATT7022E_Get_Power(uint8_t Num_ATT7022E)
{
	extern Struct_LTU_ATT7022E_Param_Reg LTU_ATT7022E_AdjustParam_Reg;
	extern Struct_LTU_Data                 LTU_Data;
	
	//存储从寄存器中取出的原始数据
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
	
	//用于标定是否负数
	uint8_t Flag_Negative = 0;
	
	//存储转换成国际单位制的数据
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
	
	//从功率寄存器中读取出原始值
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
	
	//如果通信发生了故障
	if(ATT7022E_Check_Result_Get(Num_ATT7022E) == 0)
	{
		printf("获取功率数据时发生通信故障\r\n");
		return;
	}
	
	//换算成国际单位值
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
	
	//存放在全局变量中
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



//获取电压频率
void ATT7022E_Get_Frequency(uint8_t Num_ATT7022E)
{
	extern Struct_LTU_Data LTU_Data;
	
	int   Data_Freq = 0;
	float Freq      = 0.0f;
	
	//从频率寄存器中获取到原始值
	Data_Freq = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rFreq);
	
	if(ATT7022E_Check_Result_Get(Num_ATT7022E) == 0)
	{
		printf("获取交流电频率发生通信故障\r\n");
		return;
	}
	
	//换算成国际单位制
	Freq = (float)Data_Freq / pow(2, 13);
	
	//如果ABC相中任意一相发生断相，那么说明频率判断不准确，直接归零即可
	if((LTU_Data.Event.EVA[Num_ATT7022E] & ACSAMPLE_EVENT_NO_PHSREGPQ) ||
	   (LTU_Data.Event.EVB[Num_ATT7022E] & ACSAMPLE_EVENT_NO_PHSREGPQ) ||
	   (LTU_Data.Event.EVC[Num_ATT7022E] & ACSAMPLE_EVENT_NO_PHSREGPQ))
	{
		Freq = 0.0f;
	}
	
	//存放在全局变量中
	LTU_Data.ATT7022E.Frequency[Num_ATT7022E] = Freq;
}



//获取电能
void ATT7022E_Get_Energy(uint8_t Num_ATT7022E)
{
	extern Struct_LTU_ATT7022E_Param_Reg LTU_ATT7022E_AdjustParam_Reg;
	extern Struct_LTU_Data               LTU_Data;
	
	//从寄存器中读取出来的原始值
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
	
	//转换为国际单位制的值
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
	
	//从寄存器当中获取出电能的原始值
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
	Data_PDFlag = ATT7022E_Read3Bytes_CheckCRC(Num_ATT7022E, rPFlag);  //读取功率方向
	
	//转换成国际单位制的值（kWh）
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
	
	//计算正向有功总电能、反向有功总电能
	if(Data_PDFlag & BIT(3))  //判断当前功率是正向还是反向
	{
		LTU_Data.ATT7022E.EPTR[Num_ATT7022E] += EPt;  //反向有功总电能
	}
	else
	{
		LTU_Data.ATT7022E.EPTF[Num_ATT7022E] += EPt;  //正向有功总电能
	}
	
	//计算四象限有功无功，存放在全局变量中
	switch(Data_PDFlag & 0x88)
	{
		//一象限有无功总电能
		case 0x00:
		LTU_Data.ATT7022E.Eq1_P[Num_ATT7022E] += EPt;
		LTU_Data.ATT7022E.Eq1_Q[Num_ATT7022E] += EQt;
		break;
		
		//二象限有无功总电能
		case 0x08:
		LTU_Data.ATT7022E.Eq2_P[Num_ATT7022E] += EPt;
		LTU_Data.ATT7022E.Eq2_Q[Num_ATT7022E] += EQt;
		break;
		
		//三相线有无功总电能
		case 0x88:
		LTU_Data.ATT7022E.Eq3_P[Num_ATT7022E] += EPt;
		LTU_Data.ATT7022E.Eq3_Q[Num_ATT7022E] += EQt;
		break;
		
		//四象限有无功总电能
		case 0x80:
		LTU_Data.ATT7022E.Eq4_P[Num_ATT7022E] += EPt;
		LTU_Data.ATT7022E.Eq4_Q[Num_ATT7022E] += EQt;
		break;
	}
	
	//存放在全局变量中
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



//异常错误处理
void ATT7022E_Error_Handler(uint8_t Num_ATT7022E)
{
	printf("ATT7022E发生异常，软件复位中......\r\n");
	
	//软件复位
	ATT7022E_Reset_Soft(Num_ATT7022E);

	//重新从EEPROM中装载校表参数
	ATT7022E_Load_AdjustParam_From_EEPROM(Num_ATT7022E);
	
	//错误事件初始化为0，通信结果初始化为正确
	ATT7022E_Check_Error_Set(Num_ATT7022E, 0);
	ATT7022E_Check_Result_Set(Num_ATT7022E, 1);
	
	printf("ATT7022E软件复位结束\r\n");
}



//交采事件分析，并返回结果值
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
	
	//找出电压最大值
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
	
	//电压平均值
	Uaverage = (LTU_Data.ATT7022E.UA[Num] + LTU_Data.ATT7022E.UB[Num] + LTU_Data.ATT7022E.UC[Num]) / 3.0f;
	
	//找出电压最小值
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
	
	//找出电流最大值
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
	
	//找出电流最小值
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
	
	//电流平均值
	Iaverage = (LTU_Data.ATT7022E.IA[Num] + LTU_Data.ATT7022E.IB[Num] + LTU_Data.ATT7022E.IC[Num]) / 3.0f;
	
	//单独事件
	
	//失压------------------------------------------------------------------
	
	//判断A相是否失压
	
	//如果A相没有发生失压事件，那么进行失压检测
	if((LTU_Data.Event.EVA[Num] & ACSAMPLE_EVENT_NO_VOLTAGE) == 0)
	{
		//如果A相电流大于起始电流，并且A相电压小于设定的失压阈值，说明当前发生了失压事件
		if((LTU_Data.ATT7022E.UA[Num] < THRESHOLD_EVENT_NO_VOLTAGE_U) &&
		   (LTU_Data.ATT7022E.IA[Num] > THRESHOLD_EVENT_NO_VOLTAGE_I))
		{
			//相关标志变量置1，表明发生失压事件
			Flag_A_No_Voltage[Num] = 1;
			
			//计时变量是否超过阈值
			if(Count_A_No_Voltage[Num] >= TIME_EVENT_NO_VOLTAGE)
			{
				//清零标志变量和计时变量
				Flag_A_No_Voltage[Num]  = 0;
				Count_A_No_Voltage[Num] = 0;
				
				//A相发生失压事件
				LTU_Data.Event.EVA[Num] |= ACSAMPLE_EVENT_NO_VOLTAGE;
			}
		}
		else
		{
			Flag_A_No_Voltage[Num]  = 0;
			Count_A_No_Voltage[Num] = 0;
		}
	}
	//如果A相已经发生失压事件
	else
	{
		//如果A相电流大于起始电流，并且A相电压小于设定的失压阈值，说明当前发生了失压事件
		if((LTU_Data.ATT7022E.UA[Num] < THRESHOLD_EVENT_NO_VOLTAGE_U) &&
		   (LTU_Data.ATT7022E.IA[Num] > THRESHOLD_EVENT_NO_VOLTAGE_I))
		{
			Flag_A_No_Voltage[Num]  = 0;
			Count_A_No_Voltage[Num] = 0;
		}
		//当前没有发生失压事件
		else
		{
			//相关标志变量置2，表明没有发生失压事件
			Flag_A_No_Voltage[Num] = 2;
			
			//计时变量是否超过阈值
			if(Count_A_No_Voltage[Num] >= TIME_EVENT_NO_VOLTAGE)
			{
				//清零标志变量和计时变量
				Flag_A_No_Voltage[Num]  = 0;
				Count_A_No_Voltage[Num] = 0;
				
				//A相没有发生失压事件（从失压事件中恢复）
				LTU_Data.Event.EVA[Num] &= ~ACSAMPLE_EVENT_NO_VOLTAGE;
			}
		}
	}
	
	//判断B相是否失压
	
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
	
	//判断C相是否失压
	
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
	
	//欠压------------------------------------------------------------------
	
	//判断A相是否欠压
	
	//如果A相没有发生欠压事件，那么进行欠压检测
	if((LTU_Data.Event.EVA[Num] & ACSAMPLE_EVENT_LOW_VOLTAGE) == 0)
	{
		//如果A相电压小于设定的欠压电压阈值，但大于失压电压阈值，并且电流大于设定的欠压电流阈值，说明当前发生了欠压事件
		if((LTU_Data.ATT7022E.UA[Num] < THRESHOLD_EVENT_LOW_VOLTAGE_U) &&
		   (LTU_Data.ATT7022E.UA[Num] > THRESHOLD_EVENT_NO_VOLTAGE_U)  &&
		   (LTU_Data.ATT7022E.IA[Num] > THRESHOLD_EVENT_LOW_VOLTAGE_I))
		{
			//相关标志变量置1，表明发生欠压事件
			Flag_A_Low_Voltage[Num] = 1;
			
			//计时变量是否超过阈值
			if(Count_A_Low_Voltage[Num] >= TIME_EVENT_LOW_VOLTAGE)
			{
				//清零标志变量和计时变量
				Flag_A_Low_Voltage[Num]  = 0;
				Count_A_Low_Voltage[Num] = 0;
				
				//A相发生欠压事件
				LTU_Data.Event.EVA[Num] |= ACSAMPLE_EVENT_LOW_VOLTAGE;
			}
		}
		else
		{
			Flag_A_Low_Voltage[Num]  = 0;
			Count_A_Low_Voltage[Num] = 0;
		}
	}
	//如果A相已经发生失压事件
	else
	{
		//如果A相电压小于设定的欠压电压阈值，并且电流大于设定的欠压电流阈值，说明当前发生了欠压事件
		if((LTU_Data.ATT7022E.UA[Num] < THRESHOLD_EVENT_LOW_VOLTAGE_U) &&
		   (LTU_Data.ATT7022E.UA[Num] > THRESHOLD_EVENT_NO_VOLTAGE_U)  &&
		   (LTU_Data.ATT7022E.IA[Num] > THRESHOLD_EVENT_LOW_VOLTAGE_I))
		{
			Flag_A_Low_Voltage[Num]  = 0;
			Count_A_Low_Voltage[Num] = 0;
		}
		else
		{
			//相关标志变量置2，表明没有发生欠压事件
			Flag_A_Low_Voltage[Num] = 2;
			
			//计时变量是否超过阈值
			if(Count_A_Low_Voltage[Num] >= TIME_EVENT_LOW_VOLTAGE)
			{
				//清零标志变量和计时变量
				Flag_A_Low_Voltage[Num]  = 0;
				Count_A_Low_Voltage[Num] = 0;
				
				//A相没有发生欠压事件（从欠压事件中恢复）
				LTU_Data.Event.EVA[Num] &= ~ACSAMPLE_EVENT_LOW_VOLTAGE;
			}
		}
	}
	
	//判断B相是否欠压
	
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
	
	//判断C相是否欠压
	
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
	
	//过压------------------------------------------------------------------
	
	//判断A相是否过压
	
	//如果A相没有发生过压事件，那么进行过压检测
	if((LTU_Data.Event.EVA[Num] & ACSAMPLE_EVENT_OVER_VOLTAGE) == 0)
	{
		//如果A相电压大于设定的过压阈值，说明当前发生了过压事件
		if(LTU_Data.ATT7022E.UA[Num] > THRESHOLD_EVENT_OVER_VOLTAGE_U)
		{
			//相关标志变量置1，表明发生欠压事件
			Flag_A_Over_Voltage[Num] = 1;
			
			//计时变量是否超过阈值
			if(Count_A_Over_Voltage[Num] >= TIME_EVENT_OVER_VOLTAGE)
			{
				//清零标志变量和计时变量
				Flag_A_Over_Voltage[Num]  = 0;
				Count_A_Over_Voltage[Num] = 0;
				
				//A相发生过压事件
				LTU_Data.Event.EVA[Num] |= ACSAMPLE_EVENT_OVER_VOLTAGE;
			}
		}
		else
		{
			Flag_A_Over_Voltage[Num]  = 0;
			Count_A_Over_Voltage[Num] = 0;
		}
	}
	//如果A相已经发生过压事件
	else
	{
		//如果A相电压大于设定的过压阈值，说明当前发生了过压事件
		if(LTU_Data.ATT7022E.UA[Num] > THRESHOLD_EVENT_OVER_VOLTAGE_U)
		{
			Flag_A_Over_Voltage[Num]  = 0;
			Count_A_Over_Voltage[Num] = 0;
		}
		//没有发生过压事件
		else
		{
			//相关标志变量置2，表明没有发生过压事件
			Flag_A_Over_Voltage[Num] = 2;
			
			//计时变量是否超过阈值
			if(Count_A_Over_Voltage[Num] >= TIME_EVENT_OVER_VOLTAGE)
			{
				//清零标志变量和计时变量
				Flag_A_Over_Voltage[Num]  = 0;
				Count_A_Over_Voltage[Num] = 0;
				
				//A相没有发生过压事件（从过压事件中恢复）
				LTU_Data.Event.EVA[Num] &= ~ACSAMPLE_EVENT_OVER_VOLTAGE;
			}
		}
	}
	
	//判断B相是否过压
	
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
	
	//判断C相是否过压
	
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
	
	//断相------------------------------------------------------------------
		
	//判断A相是否断相
	
	//如果A相没有发生断相事件，那么进行断相检测
	if((LTU_Data.Event.EVA[Num] & ACSAMPLE_EVENT_NO_PHSREGPQ) == 0)
	{
		//如果A相电压小于对应的断相阈值，并且负荷电流小于启动电流
		if((LTU_Data.ATT7022E.UA[Num] < THRESHOLD_EVENT_NO_PHSREGPQ_U) &&
		   (LTU_Data.ATT7022E.IA[Num] < THRESHOLD_EVENT_NO_PHSREGPQ_I))
		{
			//相关标志变量置1，表明发生断相事件
			Flag_A_No_PhSregpq[Num] = 1;
			
			//计时变量是否超过阈值
			if(Count_A_No_PhSregpq[Num] >= TIME_EVENT_NO_PHSREGPQ)
			{
				//清零标志变量和计时变量
				Flag_A_No_PhSregpq[Num]  = 0;
				Count_A_No_PhSregpq[Num] = 0;
				
				//A相发生断相事件
				LTU_Data.Event.EVA[Num] |= ACSAMPLE_EVENT_NO_PHSREGPQ;
			}
		}
		else
		{
			Flag_A_No_PhSregpq[Num]  = 0;
			Count_A_No_PhSregpq[Num] = 0;
		}
	}
	//如果A相已经发生断相事件
	else
	{
		//如果A相电压大于等于设定的断相阈值，说明没有发生断相事件
		if((LTU_Data.ATT7022E.UA[Num] < THRESHOLD_EVENT_NO_PHSREGPQ_U) &&
		   (LTU_Data.ATT7022E.IA[Num] < THRESHOLD_EVENT_NO_PHSREGPQ_I))
		{
			Flag_A_No_PhSregpq[Num]  = 0;
			Count_A_No_PhSregpq[Num] = 0;
		}
		else
		{
			//相关标志变量置2，表明没有发生断相事件
			Flag_A_No_PhSregpq[Num] = 2;
			
			//计时变量是否超过阈值
			if(Count_A_No_PhSregpq[Num] >= TIME_EVENT_NO_PHSREGPQ)
			{
				//清零标志变量和计时变量
				Flag_A_No_PhSregpq[Num]  = 0;
				Count_A_No_PhSregpq[Num] = 0;
				
				//A相没有发生欠压事件（从欠压事件中恢复）
				LTU_Data.Event.EVA[Num] &= ~ACSAMPLE_EVENT_NO_PHSREGPQ;
			}
		}
	}
	
	//判断B相是否断相
	
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
	
	//判断C相是否断相
	
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
	
	//失流------------------------------------------------------------------
	
	//判断A相是否失流
	
	//如果A相没有发生失流事件，那么进行失流检测
	if((LTU_Data.Event.EVA[Num] & ACSAMPLE_EVENT_NO_CURRENT) == 0)
	{
		//如果ABC三相电压大于电能表的电压，并且该相电流小于启动电流，并且至少有一相电流大于5%额定电流
		if((LTU_Data.ATT7022E.UA[Num] > ATT7022E_START_U) &&
		   (LTU_Data.ATT7022E.UB[Num] > ATT7022E_START_U) &&
		   (LTU_Data.ATT7022E.UC[Num] > ATT7022E_START_U) &&
		   (LTU_Data.ATT7022E.IA[Num] < ATT7022E_START_I) &&
		   ((LTU_Data.ATT7022E.IB[Num] > (0.05f * ATT7022E_RATED_I)) || (LTU_Data.ATT7022E.IC[Num] > (0.05f * ATT7022E_RATED_I))))
		{
			//相关标志变量置1，表明发生失流事件
			Flag_A_No_Current[Num] = 1;
			
			//计时变量是否超过阈值
			if(Count_A_No_Current[Num] >= TIME_EVENT_NO_CURRENT)
			{
				//清零标志变量和计时变量
				Flag_A_No_Current[Num]  = 0;
				Count_A_No_Current[Num] = 0;
				
				//A相发生失流事件
				LTU_Data.Event.EVA[Num] |= ACSAMPLE_EVENT_NO_CURRENT;
			}
		}
		else
		{
			Flag_A_No_Current[Num]  = 0;
			Count_A_No_Current[Num] = 0;
		}
	}
	//如果A相已经发生失流事件
	else
	{
		//如果ABC三相电压大于电能表的电压，并且该相电流小于启动电流，并且至少有一相电流大于5%额定电流
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
			//相关标志变量置2，表明没有发生失流事件
			Flag_A_No_Current[Num] = 2;
			
			//计时变量是否超过阈值
			if(Count_A_No_Current[Num] >= TIME_EVENT_NO_CURRENT)
			{
				//清零标志变量和计时变量
				Flag_A_No_Current[Num]  = 0;
				Count_A_No_Current[Num] = 0;
				
				//A相没有发生失流事件（从失流事件中恢复）
				LTU_Data.Event.EVA[Num] &= ~ACSAMPLE_EVENT_NO_CURRENT;
			}
		}
	}
	
	//判断B相是否失流
	
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
	
	//判断C相是否失流
	
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
	
	//过流------------------------------------------------------------------
	
	//判断A相是否过流
	
	//如果A相没有发生过流事件，那么进行过流检测
	if((LTU_Data.Event.EVA[Num] & ACSAMPLE_EVENT_OVER_CURRENT) == 0)
	{
		//如果ABC三相电压大于起始电压，并且该相电流大于额定电流
		if((LTU_Data.ATT7022E.UA[Num] > ATT7022E_START_U) &&
		   (LTU_Data.ATT7022E.UB[Num] > ATT7022E_START_U) &&
		   (LTU_Data.ATT7022E.UC[Num] > ATT7022E_START_U) &&
		   (LTU_Data.ATT7022E.IA[Num] > ATT7022E_RATED_I))
		{
			//相关标志变量置1，表明发生过流事件
			Flag_A_Over_Current[Num] = 1;
			
			//计时变量是否超过阈值
			if(Count_A_Over_Current[Num] >= TIME_EVENT_OVER_CURRENT)
			{
				//清零标志变量和计时变量
				Flag_A_Over_Current[Num]  = 0;
				Count_A_Over_Current[Num] = 0;
				
				//A相发生过流事件
				LTU_Data.Event.EVA[Num] |= ACSAMPLE_EVENT_OVER_CURRENT;
			}
		}
		else
		{
			Flag_A_Over_Current[Num]  = 0;
			Count_A_Over_Current[Num] = 0;
		}
	}
	//如果A相已经发生过流事件
	else
	{
		//如果ABC三相电压大于起始电压，并且该相电流大于额定电流
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
			//相关标志变量置2，表明没有发生失流事件
			Flag_A_Over_Current[Num] = 2;
			
			//计时变量是否超过阈值
			if(Count_A_Over_Current[Num] >= TIME_EVENT_OVER_CURRENT)
			{
				//清零标志变量和计时变量
				Flag_A_Over_Current[Num]  = 0;
				Count_A_Over_Current[Num] = 0;
				
				//A相没有发生失流事件（从失流事件中恢复）
				LTU_Data.Event.EVA[Num] &= ~ACSAMPLE_EVENT_OVER_CURRENT;
			}
		}
	}
	
	//判断B相是否过流
	
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
	
	//判断C相是否过流
	
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
	
	//公共事件
	
	//全失压----------------------------------------------------------------
	
	//如果没有发生全失压事件
	if((LTU_Data.Event.EV[Num] & ACSAMPLE_EVENT_ALL_NO_VOLTAGE) == 0)
	{
		//如果ABC三相电压都低于起始电压，并且负荷电流都大于5%的额定电流，说明发生全失压事件
		if((LTU_Data.ATT7022E.UA[Num] < ATT7022E_START_U) &&
		   (LTU_Data.ATT7022E.UB[Num] < ATT7022E_START_U) &&
		   (LTU_Data.ATT7022E.UC[Num] < ATT7022E_START_U) &&
		   (LTU_Data.ATT7022E.IA[Num] > (0.05f * ATT7022E_RATED_I)) &&
		   (LTU_Data.ATT7022E.IB[Num] > (0.05f * ATT7022E_RATED_I)) &&
		   (LTU_Data.ATT7022E.IC[Num] > (0.05f * ATT7022E_RATED_I)))
		{
			//相关标志变量置1，表明发生过流事件
			Flag_ABC_All_No_Voltage[Num] = 1;
			
			//计时变量是否超过阈值
			if(Count_ABC_All_No_Voltage[Num] >= TIME_EVENT_ALL_NO_VOLTAGE)
			{
				//清零标志变量和计时变量
				Flag_ABC_All_No_Voltage[Num]  = 0;
				Count_ABC_All_No_Voltage[Num] = 0;
				
				//A相发生过流事件
				LTU_Data.Event.EV[Num] |= ACSAMPLE_EVENT_ALL_NO_VOLTAGE;
			}
		}
		else
		{
			Flag_ABC_All_No_Voltage[Num]  = 0;
			Count_ABC_All_No_Voltage[Num] = 0;
		}
	}
	//如果A相已经发生过流事件
	else
	{
		//如果ABC三相电压大于起始电压，并且该相电流大于额定电流
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
			//相关标志变量置2，表明没有发生失流事件
			Flag_ABC_All_No_Voltage[Num] = 2;
			
			//计时变量是否超过阈值
			if(Count_ABC_All_No_Voltage[Num] >= TIME_EVENT_ALL_NO_VOLTAGE)
			{
				//清零标志变量和计时变量
				Flag_ABC_All_No_Voltage[Num]  = 0;
				Count_ABC_All_No_Voltage[Num] = 0;
				
				//A相没有发生失流事件（从失流事件中恢复）
				LTU_Data.Event.EV[Num] &= ~ACSAMPLE_EVENT_ALL_NO_VOLTAGE;
			}
		}
	}
	
	//电压逆相序------------------------------------------------------------
	
	//如果至少有一相没有发生断相事件
	if(((LTU_Data.Event.EVA[Num] & ACSAMPLE_EVENT_NO_PHSREGPQ) == 0) ||
	   ((LTU_Data.Event.EVB[Num] & ACSAMPLE_EVENT_NO_PHSREGPQ) == 0) ||
	   ((LTU_Data.Event.EVC[Num] & ACSAMPLE_EVENT_NO_PHSREGPQ) == 0))
	{
		//如果没有发生电压逆相序事件
		if((LTU_Data.Event.EV[Num] & ACSAMPLE_EVENT_VOLTAGE_PHSREGPQ_ERR) == 0)
		{
			//如果三相电压偏离各自的角度过大
			if((abs((int32)(LTU_Data.ATT7022E.YUA[Num] - 0))   > THRESHOLD_EVENT_VOLTAGE_PHSREGPQ_ERR) ||
			   (abs((int32)(LTU_Data.ATT7022E.YUB[Num] - 120)) > THRESHOLD_EVENT_VOLTAGE_PHSREGPQ_ERR) ||
			   (abs((int32)(LTU_Data.ATT7022E.YUC[Num] - 240)) > THRESHOLD_EVENT_VOLTAGE_PHSREGPQ_ERR))
			{
				//相关标志变量置1，表明发生电压逆相序事件
				Flag_ABC_Voltage_PhSregpq_Err[Num] = 1;
				
				//计时变量是否超过阈值
				if(Count_ABC_Voltage_PhSregpq_Err[Num] >= TIME_EVENT_VOLTAGE_PHSREGPQ_ERR)
				{
					//清零标志变量和计时变量
					Flag_ABC_Voltage_PhSregpq_Err[Num]  = 0;
					Count_ABC_Voltage_PhSregpq_Err[Num] = 0;
					
					//发生电压逆相序事件
					LTU_Data.Event.EV[Num] |= ACSAMPLE_EVENT_VOLTAGE_PHSREGPQ_ERR;
				}
			}
			//如果ABC三相电压相序正确
			else
			{
				Flag_ABC_Voltage_PhSregpq_Err[Num]  = 0;
				Count_ABC_Voltage_PhSregpq_Err[Num] = 0;
			}
		}
		//如果A相已经发生电压逆相序事件
		else
		{
			//如果ABC三相电压相序发生错误
			if((LTU_Data.ATT7022E.YUA[Num] > LTU_Data.ATT7022E.YUB[Num]) ||
			   (LTU_Data.ATT7022E.YUA[Num] > LTU_Data.ATT7022E.YUC[Num]) ||
			   (LTU_Data.ATT7022E.YUB[Num] > LTU_Data.ATT7022E.YUC[Num]))
			{
				Flag_ABC_Voltage_PhSregpq_Err[Num]  = 0;
				Count_ABC_Voltage_PhSregpq_Err[Num] = 0;
			}
			//如果ABC三相电压相序正确
			else
			{
				//相关标志变量置2，表明没有发生电压逆相序事件
				Flag_ABC_Voltage_PhSregpq_Err[Num] = 2;
				
				//计时变量是否超过阈值
				if(Count_ABC_Voltage_PhSregpq_Err[Num] >= TIME_EVENT_VOLTAGE_PHSREGPQ_ERR)
				{
					//清零标志变量和计时变量
					Flag_ABC_Voltage_PhSregpq_Err[Num]  = 0;
					Count_ABC_Voltage_PhSregpq_Err[Num] = 0;
					
					//A相没有发生电压逆相序事件（从电压逆相序事件中恢复）
					LTU_Data.Event.EV[Num] &= ~ACSAMPLE_EVENT_VOLTAGE_PHSREGPQ_ERR;
				}
			}
		}
	}
	
	//电流逆相序------------------------------------------------------------
	
	//获取电流相角
	YIA[Num] = LTU_Data.ATT7022E.YUA[Num] + LTU_Data.ATT7022E.PhSregpqA[Num];
	YIB[Num] = LTU_Data.ATT7022E.YUB[Num] + LTU_Data.ATT7022E.PhSregpqB[Num];
	YIC[Num] = LTU_Data.ATT7022E.YUC[Num] + LTU_Data.ATT7022E.PhSregpqC[Num];
	
	//如果至少有一相没有发生断相事件
	if(((LTU_Data.Event.EVA[Num] & ACSAMPLE_EVENT_NO_PHSREGPQ) == 0) ||
	   ((LTU_Data.Event.EVB[Num] & ACSAMPLE_EVENT_NO_PHSREGPQ) == 0) ||
	   ((LTU_Data.Event.EVC[Num] & ACSAMPLE_EVENT_NO_PHSREGPQ) == 0))
	{
		//如果没有发生电流逆相序事件
		if((LTU_Data.Event.EV[Num] & ACSAMPLE_EVENT_CURRENT_PHSREGPQ_ERR) == 0)
		{
			//如果ABC三相电流相序发生错误
			if((YIA[Num] > YIB[Num]) ||
			   (YIA[Num] > YIC[Num]) ||
			   (YIB[Num] > YIC[Num]))
			{
				//相关标志变量置1，表明发生电流逆相序事件
				Flag_ABC_Current_PhSregpq_Err[Num] = 1;
				
				//计时变量是否超过阈值
				if(Count_ABC_Current_PhSregpq_Err[Num] >= TIME_EVENT_CURRENT_PHSREGPQ_ERR)
				{
					//清零标志变量和计时变量
					Flag_ABC_Current_PhSregpq_Err[Num]  = 0;
					Count_ABC_Current_PhSregpq_Err[Num] = 0;
					
					//发生电流逆相序事件
					LTU_Data.Event.EV[Num] |= ACSAMPLE_EVENT_CURRENT_PHSREGPQ_ERR;
				}
			}
			//如果ABC三相电流相序正确
			else
			{
				Flag_ABC_Current_PhSregpq_Err[Num]  = 0;
				Count_ABC_Current_PhSregpq_Err[Num] = 0;
			}
		}
		//如果ABC三相已经发生电流逆相序事件
		else
		{
			//如果ABC三相电流相序发生错误
			if((YIA[Num] > YIB[Num]) ||
			   (YIA[Num] > YIC[Num]) ||
			   (YIB[Num] > YIC[Num]))
			{
				Flag_ABC_Current_PhSregpq_Err[Num]  = 0;
				Count_ABC_Current_PhSregpq_Err[Num] = 0;
			}
			//如果ABC三相电流相序正确
			else
			{
				//相关标志变量置2，表明没有发生电流逆相序事件
				Flag_ABC_Current_PhSregpq_Err[Num] = 2;
				
				//计时变量是否超过阈值
				if(Count_ABC_Current_PhSregpq_Err[Num] >= TIME_EVENT_CURRENT_PHSREGPQ_ERR)
				{
					//清零标志变量和计时变量
					Flag_ABC_Current_PhSregpq_Err[Num]  = 0;
					Count_ABC_Current_PhSregpq_Err[Num] = 0;
					
					//A相没有发生电流逆相序事件（从电流逆相序事件中恢复）
					LTU_Data.Event.EV[Num] &= ~ACSAMPLE_EVENT_CURRENT_PHSREGPQ_ERR;
				}
			}
		}
	}
	
	//电流不平衡------------------------------------------------------------
		
	//如果没有发生电流不平衡事件
	if((LTU_Data.Event.EV[Num] & ACSAMPLE_EVENT_CURRENT_UNBALANCE) == 0)
	{
		//如果至少有一相电流大于5%额定电流，并且不平衡率大于阈值
		if(((LTU_Data.ATT7022E.IA[Num] > (0.05f * ATT7022E_RATED_I)) || (LTU_Data.ATT7022E.IB[Num] > (0.05f * ATT7022E_RATED_I)) || (LTU_Data.ATT7022E.IC[Num] > (0.05f * ATT7022E_RATED_I))) &&
		   (((Imax - Imin) / Iaverage) > THRESHOLD_EVENT_CURRENT_UNBALANCE))
		{
			//相关标志变量置1，表明发生电流不平衡事件
			Flag_ABC_Current_UnBalance[Num] = 1;
			
			//计时变量是否超过阈值
			if(Count_ABC_Current_UnBalance[Num] >= TIME_EVENT_CURRENT_UNBALANCE)
			{
				//清零标志变量和计时变量
				Flag_ABC_Current_UnBalance[Num]  = 0;
				Count_ABC_Current_UnBalance[Num] = 0;
				
				//发生电流不平衡事件
				LTU_Data.Event.EV[Num] |= ACSAMPLE_EVENT_CURRENT_UNBALANCE;
			}
		}
		//如果ABC三相电流平衡
		else
		{
			Flag_ABC_Current_UnBalance[Num]  = 0;
			Count_ABC_Current_UnBalance[Num] = 0;
		}
	}
	//如果ABC三相已经发生电流不平衡事件
	else
	{
		//如果至少有一相电流大于5%额定电流，并且不平衡率大于阈值，说明发生电流不平衡事件
		if(((LTU_Data.ATT7022E.IA[Num] > (0.05f * ATT7022E_RATED_I)) || (LTU_Data.ATT7022E.IB[Num] > (0.05f * ATT7022E_RATED_I)) || (LTU_Data.ATT7022E.IC[Num] > (0.05f * ATT7022E_RATED_I))) &&
		   (((Imax - Imin) / Iaverage) > THRESHOLD_EVENT_CURRENT_UNBALANCE))
		{
			Flag_ABC_Current_UnBalance[Num]  = 0;
			Count_ABC_Current_UnBalance[Num] = 0;
		}
		//如果ABC三相电流平衡
		else
		{
			//相关标志变量置2，表明没有发生电流不平衡事件
			Flag_ABC_Current_UnBalance[Num] = 2;
			
			//计时变量是否超过阈值
			if(Count_ABC_Current_UnBalance[Num] >= TIME_EVENT_CURRENT_UNBALANCE)
			{
				//清零标志变量和计时变量
				Flag_ABC_Current_UnBalance[Num]  = 0;
				Count_ABC_Current_UnBalance[Num] = 0;
				
				//A相没有发生电流不平衡事件（从电流不平衡事件中恢复）
				LTU_Data.Event.EV[Num] &= ~ACSAMPLE_EVENT_CURRENT_UNBALANCE;
			}
		}
	}
	
	//电压不平衡------------------------------------------------------------
		
	//如果没有发生电压不平衡事件
	if((LTU_Data.Event.EV[Num] & ACSAMPLE_EVENT_VOLTAGE_UNBALANCE) == 0)
	{
		//如果至少有一相电压大于启动电压，并且不平衡率大于阈值
		if(((LTU_Data.ATT7022E.UA[Num] > ATT7022E_START_U) || (LTU_Data.ATT7022E.UB[Num] > ATT7022E_START_U) || (LTU_Data.ATT7022E.UC[Num] > ATT7022E_START_U)) &&
		   (((Umax - Umin) / Uaverage) > THRESHOLD_EVENT_VOLTAGE_UNBALANCE))
		{
			//相关标志变量置1，表明发生电压不平衡事件
			Flag_ABC_Voltage_UnBalance[Num] = 1;
			
			//计时变量是否超过阈值
			if(Count_ABC_Voltage_UnBalance[Num] >= TIME_EVENT_VOLTAGE_UNBALANCE)
			{
				//清零标志变量和计时变量
				Flag_ABC_Voltage_UnBalance[Num]  = 0;
				Count_ABC_Voltage_UnBalance[Num] = 0;
				
				//发生电压不平衡事件
				LTU_Data.Event.EV[Num] |= ACSAMPLE_EVENT_VOLTAGE_UNBALANCE;
			}
		}
		//如果ABC三相电压平衡
		else
		{
			Flag_ABC_Voltage_UnBalance[Num]  = 0;
			Count_ABC_Voltage_UnBalance[Num] = 0;
		}
	}
	//如果ABC三相已经发生电压不平衡事件
	else
	{
		//如果至少有一相电压大于启动电压，并且不平衡率大于阈值
		if(((LTU_Data.ATT7022E.UA[Num] > ATT7022E_START_U) || (LTU_Data.ATT7022E.UB[Num] > ATT7022E_START_U) || (LTU_Data.ATT7022E.UC[Num] > ATT7022E_START_U)) &&
		   (((Umax - Umin) / Uaverage) > THRESHOLD_EVENT_VOLTAGE_UNBALANCE))
		{
			Flag_ABC_Voltage_UnBalance[Num]  = 0;
			Count_ABC_Voltage_UnBalance[Num] = 0;
		}
		//如果ABC三相电压平衡
		else
		{
			//相关标志变量置2，表明没有发生电压不平衡事件
			Flag_ABC_Voltage_UnBalance[Num] = 2;
			
			//计时变量是否超过阈值
			if(Count_ABC_Voltage_UnBalance[Num] >= TIME_EVENT_VOLTAGE_UNBALANCE)
			{
				//清零标志变量和计时变量
				Flag_ABC_Voltage_UnBalance[Num]  = 0;
				Count_ABC_Voltage_UnBalance[Num] = 0;
				
				//A相没有发生电压不平衡事件（从电压不平衡事件中恢复）
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
		//失压事件计时
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
		
		//欠压事件计时
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
		
		//过压事件计时
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
		
		//断相事件
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
		
		//失流事件
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
		
		//过流事件
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
		
		//全失压
		if(Flag_ABC_All_No_Voltage[i])
		{
			Count_ABC_All_No_Voltage[i]++;
		}
		
		//电压逆相序
		if(Flag_ABC_Voltage_PhSregpq_Err[i])
		{
			Count_ABC_Voltage_PhSregpq_Err[i]++;
		}
		
		//电流逆相序
		if(Flag_ABC_Current_PhSregpq_Err[i])
		{
			Count_ABC_Current_PhSregpq_Err[i]++;
		}
		
		//电流不平衡
		if(Flag_ABC_Current_UnBalance[i])
		{
			Count_ABC_Current_UnBalance[i]++;
		}
		
		//电压不平衡
		if(Flag_ABC_Voltage_UnBalance[i])
		{
			Count_ABC_Voltage_UnBalance[i]++;
		}
	}
}
#endif
