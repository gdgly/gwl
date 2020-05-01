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
//Ƭѡ����
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

//��·ATT7022E��Ƭѡ���Ų���
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



//У׼�Ĵ���
#define ATT_ADDR_ModeCfg      0x01  //ģʽ��ؿ���
#define ATT_ADDR_PGACtrl      0x02  //ADC��������
#define ATT_ADDR_EMUCfg       0x03  //EMU��Ԫ���� 

#define ATT_ADDR_Pga          0x04  //A���й��������� 
#define ATT_ADDR_Pgb          0x05  //B���й��������� 
#define ATT_ADDR_Pgc          0x06  //C���й��������� 
#define ATT_ADDR_Qga          0x07  //A���޹���������
#define ATT_ADDR_Qgb          0x08  //B���޹���������
#define ATT_ADDR_Qgc          0x09  //C���޹���������
#define ATT_ADDR_Sga          0x0a  //A�����ڹ������� 
#define ATT_ADDR_Sgb          0x0b  //B�����ڹ������� 
#define ATT_ADDR_Sgc          0x0c  //C�����ڹ������� 

#define ATT_ADDR_Pha0 	      0x0d  //A����λУ��0
#define ATT_ADDR_Phb0 	      0x0e  //B����λУ��0
#define ATT_ADDR_Phc0 	      0x0f  //C����λУ��0
#define ATT_ADDR_Pha1         0x10  //A����λУ��1
#define ATT_ADDR_Phb1         0x11  //B����λУ��1
#define ATT_ADDR_Phc1         0x12  //C����λУ��1

#define ATT_ADDR_Posa         0x13  //A���й�����offsetУ��
#define ATT_ADDR_Posb         0x14  //B���й�����offsetУ��
#define ATT_ADDR_Posc         0x15  //C���й�����offsetУ��

#define ATT_ADDR_Qph          0x16  //�޹���λУ��

#define ATT_ADDR_Uga          0x17  //A���ѹ����
#define ATT_ADDR_Ugb          0x18  //B���ѹ����
#define ATT_ADDR_Ugc          0x19  //C���ѹ����
#define ATT_ADDR_Iga          0x1a  //A���������
#define ATT_ADDR_Igb          0x1b  //B���������
#define ATT_ADDR_Igc          0x1c  //C���������

#define ATT_ADDR_Istart       0x1d  //�𶯵�����ֵ����
#define ATT_ADDR_HFconst      0x1e  //��Ƶ�����������
#define ATT_ADDR_Ufail        0x1f  //ʧѹ��ֵ����

#define ATT_ADDR_Ign          0x20

#define ATT_ADDR_Qosa         0x21  //A���޹�����offsetУ�� 
#define ATT_ADDR_Qosb         0x22  //B���޹�����offsetУ�� 
#define ATT_ADDR_Qosc         0x23  //C���޹�����offsetУ�� 
#define ATT_ADDR_Uosa         0x24  //A���ѹ��ЧֵoffsetУ�� 
#define ATT_ADDR_Uosb         0x25  //B���ѹ��ЧֵoffsetУ�� 
#define ATT_ADDR_Uosc         0x26  //C���ѹ��ЧֵoffsetУ�� 
#define ATT_ADDR_Iosa         0x27  //A�������ЧֵoffsetУ�� 
#define ATT_ADDR_Iosb         0x28  //B�������ЧֵoffsetУ�� 
#define ATT_ADDR_Iosc         0x29  //C�������ЧֵoffsetУ�� 

#define ATT_ADDR_adc_Ua       0x2a  //A���ѹͨ��ADC offsetУ��
#define ATT_ADDR_adc_Ub       0x2b  //B���ѹͨ��ADC offsetУ��
#define ATT_ADDR_adc_Uc       0x2c  //C���ѹͨ��ADC offsetУ��
#define ATT_ADDR_adc_Ia       0x2d  //A�����ͨ��ADC offsetУ��
#define ATT_ADDR_adc_Ib       0x2e  //B�����ͨ��ADC offsetУ��
#define ATT_ADDR_adc_Ic       0x2f  //C�����ͨ��ADC offsetУ��

#define ATT_ADDR_EMUIE        0x30  //�ж�ʹ�� 
#define ATT_ADDR_ModuleCfg    0x31  //��·ģ�����üĴ��� 
#define ATT_ADDR_ACG          0x32  //ȫͨ�����棬����Vref���¶�У�� 
#define ATT_ADDR_HFdouble     0x33  //���峣���ӱ�ѡ��
#define ATT_ADDR_Fgain        0x34  //��������У��
#define ATT_ADDR_PinCfg       0x35  //����pin����������ѡ����� 
#define ATT_ADDR_Pstart       0x36  //�𶯹������üĴ��� 
#define ATT_ADDR_Iregion      0x37  //��λ�����������üĴ��� 



//�Ƿ���¶ȴ�����
#define TURN_ON_TEMPERATURE_SENSOR 0

//��ѹ����У׼
#define ATT7022E_ADJUST_Ugain_A     1  //У׼A���ѹ����
#define ATT7022E_ADJUST_Ugain_B     2  //У׼B���ѹ����
#define ATT7022E_ADJUST_Ugain_C     3  //У׼C���ѹ����
//��������У׼
#define ATT7022E_ADJUST_Igain_A     1  //У׼A���������
#define ATT7022E_ADJUST_Igain_B     2  //У׼B���������
#define ATT7022E_ADJUST_Igain_C     3  //У׼C���������
//��������У׼���ǲ�У׼
#define ATT7022E_ADJUST_PQSgain_PhSregpq_A    1  //У׼A���й��޹��������桢�ǲ�
#define ATT7022E_ADJUST_PQSgain_PhSregpq_B    2  //У׼B���й��޹��������桢�ǲ�
#define ATT7022E_ADJUST_PQSgain_PhSregpq_C    3  //У׼C���й��޹��������桢�ǲ�
//����ƫ��У׼
#define ATT7022E_ADJUST_PQoffset_A    1  //У׼A�๦��ƫ��
#define ATT7022E_ADJUST_PQoffset_B    2  //У׼B�๦��ƫ��
#define ATT7022E_ADJUST_PQoffset_C    3  //У׼C�๦��ƫ��



//��ʼ��
extern void ATT7022E_Init(void);

//��ȡIDֵ��У��ʱ��ID������ʱ��ID������ͬ��
extern uint32 ATT7022E_ReadID(void);

//��¼���һ�ε�ͨ��״����API
extern void  ATT7022E_Check_Result_Set(uint8 Num_ATT7022E, uint8 Flag);  //��¼���һ�ε�SPIͨ�Ž���Ƿ���ȷ
extern uint8 ATT7022E_Check_Result_Get(uint8 Num_ATT7022E);              //��ȡ���һ�ε�SPIͨ�Ž���Ƿ���ȷ
extern void  ATT7022E_Check_Error_Set(uint8 Num_ATT7022E, uint8 Flag);   //��¼�Ƿ��������¼�
extern uint8 ATT7022E_Check_Error_Get(uint8 Num_ATT7022E);               //��ȡ�Ƿ��������¼�

//ATT7022E��ͨ��API������У��ʹ���
extern void  ATT7022E_Write3Bytes_CheckCRC(uint8 Num_ATT7022E, uint8 ADDR, uint32 Data_24bit);  //д��24λ���ݣ�������CRCУ���ж��Ƿ�ͨ����ȷ
extern int32 ATT7022E_Read3Bytes_CheckCRC(uint8 Num_ATT7022E, uint8 ADDR);                      //��ȡ24λ���ݣ�������CRCУ���ж��Ƿ�ͨ����ȷ

//������API
extern void ATT7022E_Reset_Soft(uint8 Num_ATT7022E);                         //������λ����
extern void ATT7022E_Clear_AdjustParam(uint8 Num_ATT7022E);                  //������У׼�Ĵ�����ֵ���ָ����ϵ�״̬����λУ׼�Ĵ����ڵ�ֵ��
extern void ATT7022E_Write_AdjustParam_Set(uint8 Num_ATT7022E, uint8 Flag);  //�����Ƿ��ܹ�д��У׼����
extern void ATT7022E_Read_AdjustParam_Set(uint8 Num_ATT7022E, uint8 Flag);   //�����Ƿ��ȡУ׼�Ĵ���������1: ��ȡУ׼�Ĵ�������  0: ��ȡ�����Ĵ������ݣ�

//У׼���㺯��
extern int  ATT7022E_Calculate_HFconst(int _EC, float _Un, float _Ib, float _Vu, float Vi);      //�����Ƶ���峣��
extern int  ATT7022E_Calculate_Istartup(float Ib, float N, float Rate);                          //������ʼ����
extern int  ATT7022E_Calculate_Ufail(float Un, float D);                                         //����ʧѹ��ѹ��ֵ
extern int  ATT7022E_Calculate_Ugain(uint8 Num_ATT7022E, float Ur, uint8_t Ugain_ABC);           //�����ѹ����У׼
extern int  ATT7022E_Calculate_Igain(uint8 Num_ATT7022E, float Ir, float N, uint8_t Igain_ABC);  //�����������У׼
extern void ATT7022E_Calculate_PQSgain_PhSregpq_1(uint8 Num_ATT7022E, float P_Real, float Q_Real, int HFconst, int _EC, uint8_t PQgain_PhSregpq_ABC);
extern void ATT7022E_Calculate_PQSgain_PhSregpq_2(uint8 Num_ATT7022E, float P_Real, float Q_Real, int HFconst, int _EC, uint8_t PQgain_PhSregpq_ABC);
extern void ATT7022E_Calculate_Ugain_Igain_PQSgain_PhSregpq_1_Extra(uint8 Num_ATT7022E, float P_Real, int HFconst, int _EC, uint8 PQgain_PhSregpq_ABC, float U_Real, uint8 Ugain_ABC, float I_Real, float N, uint8 Igain_ABC);

//�洢У�������EEPROM����
extern void ATT7022E_Write_AdjustParam_To_EEPROM(uint8 Num_ATT7022E, uint8 Length, uint8 *P_AdjustParam);   //д��
extern void ATT7022E_Read_AdjustParam_From_EEPROM(uint8 Num_ATT7022E, uint8 Length, uint8 *P_AdjustParam);  //��ȡ

//������API
extern void ATT7022E_Check_Status(uint8 Num_ATT7022E);    //��ȡATT7022E�ĵ�ǰ״̬
extern void ATT7022E_Adjust_2(uint8 Num_ATT7022E, uint8 *P_Param_Uint8, uint16 Length);
extern void ATT7022E_Load_AdjustParam_From_EEPROM(uint8 Num_ATT7022E);  //��EEPROM�е�У�����д��ATT7022E����
extern void ATT7022E_Get_Voltage(uint8 Num_ATT7022E);       //��ȡ��ѹ
extern void ATT7022E_Get_Current(uint8 Num_ATT7022E);       //��ȡ����
extern void ATT7022E_Get_PhSregpq(uint8 Num_ATT7022e);      //��ȡ���
extern void ATT7022E_Get_Power(uint8 Num_ATT7022E);         //��ȡ����
extern void ATT7022E_Get_Frequency(uint8 Num_ATT7022E);     //��ȡƵ��
extern void ATT7022E_Get_Energy(uint8 Num_ATT7022E);        //��ȡ����
extern void ATT7022E_Error_Handler(uint8 Num_ATT7022E);     //�쳣����
extern void ATT7022E_ACSample_Event(uint8 Num_ATT7022E);    //�����¼�

extern void ATT7022E_ACSample_Event_Timer_1000ms(void);  //���ڶ�ʱ����Ϊ1000ms���жϺ���������
#endif
#endif
