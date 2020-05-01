#ifndef ATT7022E_H
#define ATT7022E_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#if 0
#include "stm32f10x.h"
#include "Define.h"
#include "Delay.h"
#include "Att7022eDefine.h"
#include "EEPROM.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Tools.h"
#endif
#include "Sgd_types.h"
#if 0
//片选引脚
#define ATT7022E_SPI_PORT    GPIOB
#define ATT7022E_SPI_PINS    (GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15)

#define ATT7022E_SPI_CS1_PORT    GPIOC
#define ATT7022E_SPI_CS1_PIN     GPIO_Pin_6 

#define ATT7022E_SPI_CS2_PORT    GPIOC
#define ATT7022E_SPI_CS2_PIN     GPIO_Pin_7

#define ATT7022E_SPI_CS3_PORT    GPIOC
#define ATT7022E_SPI_CS3_PIN     GPIO_Pin_8

#define ATT7022E_SPI_CS4_PORT    GPIOC
#define ATT7022E_SPI_CS4_PIN     GPIO_Pin_9

#define ATT7022E_SPI_CS5_PORT    GPIOA
#define ATT7022E_SPI_CS5_PIN     GPIO_Pin_8 

//五路ATT7022E的片选引脚操作
#define ATT7022E_SPI_CS0_L()    GPIO_ResetBits(ATT7022E_SPI_CS1_PORT, ATT7022E_SPI_CS1_PIN)  //CS1
#define ATT7022E_SPI_CS0_H()    GPIO_SetBits(ATT7022E_SPI_CS1_PORT, ATT7022E_SPI_CS1_PIN);

#define ATT7022E_SPI_CS1_L()    GPIO_ResetBits(ATT7022E_SPI_CS2_PORT, ATT7022E_SPI_CS2_PIN)  //CS2
#define ATT7022E_SPI_CS1_H()    GPIO_SetBits(ATT7022E_SPI_CS2_PORT, ATT7022E_SPI_CS2_PIN);

#define ATT7022E_SPI_CS2_L()    GPIO_ResetBits(ATT7022E_SPI_CS3_PORT, ATT7022E_SPI_CS3_PIN)  //CS3
#define ATT7022E_SPI_CS2_H()    GPIO_SetBits(ATT7022E_SPI_CS3_PORT, ATT7022E_SPI_CS3_PIN);

#define ATT7022E_SPI_CS3_L()    GPIO_ResetBits(ATT7022E_SPI_CS4_PORT, ATT7022E_SPI_CS4_PIN)  //CS4
#define ATT7022E_SPI_CS3_H()    GPIO_SetBits(ATT7022E_SPI_CS4_PORT, ATT7022E_SPI_CS4_PIN);

#define ATT7022E_SPI_CS4_L()    GPIO_ResetBits(ATT7022E_SPI_CS5_PORT, ATT7022E_SPI_CS5_PIN)  //CS5
#define ATT7022E_SPI_CS4_H()    GPIO_SetBits(ATT7022E_SPI_CS5_PORT, ATT7022E_SPI_CS5_PIN);



//校准寄存器
#define ATT_ADDR_ModeCfg      0x01  //模式相关控制
#define ATT_ADDR_PGACtrl      0x02  //ADC增益配置
#define ATT_ADDR_EMUCfg       0x03  //EMU单元配置 

#define ATT_ADDR_Pga          0x04  //A相有功功率增益 
#define ATT_ADDR_Pgb          0x05  //B相有功功率增益 
#define ATT_ADDR_Pgc          0x06  //C相有功功率增益 
#define ATT_ADDR_Qga          0x07  //A相无功功率增益
#define ATT_ADDR_Qgb          0x08  //B相无功功率增益
#define ATT_ADDR_Qgc          0x09  //C相无功功率增益
#define ATT_ADDR_Sga          0x0a  //A相视在功率增益 
#define ATT_ADDR_Sgb          0x0b  //B相视在功率增益 
#define ATT_ADDR_Sgc          0x0c  //C相视在功率增益 

#define ATT_ADDR_Pha0 	      0x0d  //A相相位校正0
#define ATT_ADDR_Phb0 	      0x0e  //B相相位校正0
#define ATT_ADDR_Phc0 	      0x0f  //C相相位校正0
#define ATT_ADDR_Pha1         0x10  //A相相位校正1
#define ATT_ADDR_Phb1         0x11  //B相相位校正1
#define ATT_ADDR_Phc1         0x12  //C相相位校正1

#define ATT_ADDR_Posa         0x13  //A相有功功率offset校正
#define ATT_ADDR_Posb         0x14  //B相有功功率offset校正
#define ATT_ADDR_Posc         0x15  //C相有功功率offset校正

#define ATT_ADDR_Qph          0x16  //无功相位校正

#define ATT_ADDR_Uga          0x17  //A相电压增益
#define ATT_ADDR_Ugb          0x18  //B相电压增益
#define ATT_ADDR_Ugc          0x19  //C相电压增益
#define ATT_ADDR_Iga          0x1a  //A相电流增益
#define ATT_ADDR_Igb          0x1b  //B相电流增益
#define ATT_ADDR_Igc          0x1c  //C相电流增益

#define ATT_ADDR_Istart       0x1d  //起动电流阈值设置
#define ATT_ADDR_HFconst      0x1e  //高频脉冲输出设置
#define ATT_ADDR_Ufail        0x1f  //失压阈值设置

#define ATT_ADDR_Ign          0x20

#define ATT_ADDR_Qosa         0x21  //A相无功功率offset校正 
#define ATT_ADDR_Qosb         0x22  //B相无功功率offset校正 
#define ATT_ADDR_Qosc         0x23  //C相无功功率offset校正 
#define ATT_ADDR_Uosa         0x24  //A相电压有效值offset校正 
#define ATT_ADDR_Uosb         0x25  //B相电压有效值offset校正 
#define ATT_ADDR_Uosc         0x26  //C相电压有效值offset校正 
#define ATT_ADDR_Iosa         0x27  //A相电流有效值offset校正 
#define ATT_ADDR_Iosb         0x28  //B相电流有效值offset校正 
#define ATT_ADDR_Iosc         0x29  //C相电流有效值offset校正 

#define ATT_ADDR_adc_Ua       0x2a  //A相电压通道ADC offset校正
#define ATT_ADDR_adc_Ub       0x2b  //B相电压通道ADC offset校正
#define ATT_ADDR_adc_Uc       0x2c  //C相电压通道ADC offset校正
#define ATT_ADDR_adc_Ia       0x2d  //A相电流通道ADC offset校正
#define ATT_ADDR_adc_Ib       0x2e  //B相电流通道ADC offset校正
#define ATT_ADDR_adc_Ic       0x2f  //C相电流通道ADC offset校正

#define ATT_ADDR_EMUIE        0x30  //中断使能 
#define ATT_ADDR_ModuleCfg    0x31  //电路模块配置寄存器 
#define ATT_ADDR_ACG          0x32  //全通道增益，用于Vref的温度校正 
#define ATT_ADDR_HFdouble     0x33  //脉冲常数加倍选择
#define ATT_ADDR_Fgain        0x34  //基波增益校正
#define ATT_ADDR_PinCfg       0x35  //数字pin上下拉电阻选择控制 
#define ATT_ADDR_Pstart       0x36  //起动功率设置寄存器 
#define ATT_ADDR_Iregion      0x37  //相位补偿区域设置寄存器 



//是否打开温度传感器
#define TURN_ON_TEMPERATURE_SENSOR 0

//电压增益校准
#define ATT7022E_ADJUST_Ugain_A     1  //校准A相电压增益
#define ATT7022E_ADJUST_Ugain_B     2  //校准B相电压增益
#define ATT7022E_ADJUST_Ugain_C     3  //校准C相电压增益
//电流增益校准
#define ATT7022E_ADJUST_Igain_A     1  //校准A相电流增益
#define ATT7022E_ADJUST_Igain_B     2  //校准B相电流增益
#define ATT7022E_ADJUST_Igain_C     3  //校准C相电流增益
//功率增益校准、角差校准
#define ATT7022E_ADJUST_PQSgain_PhSregpq_A    1  //校准A相有功无功功率增益、角差
#define ATT7022E_ADJUST_PQSgain_PhSregpq_B    2  //校准B相有功无功功率增益、角差
#define ATT7022E_ADJUST_PQSgain_PhSregpq_C    3  //校准C相有功无功功率增益、角差
//功率偏移校准
#define ATT7022E_ADJUST_PQoffset_A    1  //校准A相功率偏移
#define ATT7022E_ADJUST_PQoffset_B    2  //校准B相功率偏移
#define ATT7022E_ADJUST_PQoffset_C    3  //校准C相功率偏移



//初始化
extern void ATT7022E_Init(void);

//读取ID值（校表时的ID、计量时的ID都不相同）
extern uint32 ATT7022E_ReadID(void);

//记录最近一次的通信状况的API
extern void  ATT7022E_Check_Result_Set(uint8 Num_ATT7022E, uint8 Flag);  //记录最后一次的SPI通信结果是否正确
extern uint8 ATT7022E_Check_Result_Get(uint8 Num_ATT7022E);              //获取最后一次的SPI通信结果是否正确
extern void  ATT7022E_Check_Error_Set(uint8 Num_ATT7022E, uint8 Flag);   //记录是否发生错误事件
extern uint8 ATT7022E_Check_Error_Get(uint8 Num_ATT7022E);               //获取是否发生错误事件

//ATT7022E的通信API，有做校验和处理
extern void  ATT7022E_Write3Bytes_CheckCRC(uint8 Num_ATT7022E, uint8 ADDR, uint32 Data_24bit);  //写入24位数据，并进行CRC校验判断是否通信正确
extern int32 ATT7022E_Read3Bytes_CheckCRC(uint8 Num_ATT7022E, uint8 ADDR);                      //读取24位数据，并进行CRC校验判断是否通信正确

//功能性API
extern void ATT7022E_Reset_Soft(uint8 Num_ATT7022E);                         //进行软复位操作
extern void ATT7022E_Clear_AdjustParam(uint8 Num_ATT7022E);                  //把所有校准寄存器的值都恢复成上电状态（复位校准寄存器内的值）
extern void ATT7022E_Write_AdjustParam_Set(uint8 Num_ATT7022E, uint8 Flag);  //设置是否能够写入校准参数
extern void ATT7022E_Read_AdjustParam_Set(uint8 Num_ATT7022E, uint8 Flag);   //设置是否读取校准寄存器参数（1: 读取校准寄存器数据  0: 读取计量寄存器数据）

//校准计算函数
extern int  ATT7022E_Calculate_HFconst(int _EC, float _Un, float _Ib, float _Vu, float Vi);      //计算高频脉冲常数
extern int  ATT7022E_Calculate_Istartup(float Ib, float N, float Rate);                          //计算起始电流
extern int  ATT7022E_Calculate_Ufail(float Un, float D);                                         //计算失压电压阈值
extern int  ATT7022E_Calculate_Ugain(uint8 Num_ATT7022E, float Ur, uint8_t Ugain_ABC);           //计算电压增益校准
extern int  ATT7022E_Calculate_Igain(uint8 Num_ATT7022E, float Ir, float N, uint8_t Igain_ABC);  //计算电流增益校准
extern void ATT7022E_Calculate_PQSgain_PhSregpq_1(uint8 Num_ATT7022E, float P_Real, float Q_Real, int HFconst, int _EC, uint8_t PQgain_PhSregpq_ABC);
extern void ATT7022E_Calculate_PQSgain_PhSregpq_2(uint8 Num_ATT7022E, float P_Real, float Q_Real, int HFconst, int _EC, uint8_t PQgain_PhSregpq_ABC);
extern void ATT7022E_Calculate_Ugain_Igain_PQSgain_PhSregpq_1_Extra(uint8 Num_ATT7022E, float P_Real, int HFconst, int _EC, uint8 PQgain_PhSregpq_ABC, float U_Real, uint8 Ugain_ABC, float I_Real, float N, uint8 Igain_ABC);

//存储校表参数进EEPROM当中
extern void ATT7022E_Write_AdjustParam_To_EEPROM(uint8 Num_ATT7022E, uint8 Length, uint8 *P_AdjustParam);   //写入
extern void ATT7022E_Read_AdjustParam_From_EEPROM(uint8 Num_ATT7022E, uint8 Length, uint8 *P_AdjustParam);  //读取

//功能性API
extern void ATT7022E_Check_Status(uint8 Num_ATT7022E);    //获取ATT7022E的当前状态
extern void ATT7022E_Adjust_2(uint8 Num_ATT7022E, uint8 *P_Param_Uint8, uint16 Length);
extern void ATT7022E_Load_AdjustParam_From_EEPROM(uint8 Num_ATT7022E);  //把EEPROM中的校表参数写入ATT7022E当中
extern void ATT7022E_Get_Voltage(uint8 Num_ATT7022E);       //获取电压
extern void ATT7022E_Get_Current(uint8 Num_ATT7022E);       //获取电流
extern void ATT7022E_Get_PhSregpq(uint8 Num_ATT7022e);      //获取相角
extern void ATT7022E_Get_Power(uint8 Num_ATT7022E);         //获取功率
extern void ATT7022E_Get_Frequency(uint8 Num_ATT7022E);     //获取频率
extern void ATT7022E_Get_Energy(uint8 Num_ATT7022E);        //获取电能
extern void ATT7022E_Error_Handler(uint8 Num_ATT7022E);     //异常处理
extern void ATT7022E_ACSample_Event(uint8 Num_ATT7022E);    //交采事件

extern void ATT7022E_ACSample_Event_Timer_1000ms(void);  //放在定时周期为1000ms的中断函数内运行
#endif
#endif
