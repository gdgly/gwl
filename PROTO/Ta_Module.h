#ifndef _TA_MODULE_H_
#define _TA_MODULE_H_

#ifdef __cplusplus
extern "C" {
#endif

#pragma once

#ifndef  __GNUC__
#pragma pack(push,1)
#define __attribute__(x)
#endif

#define START_CODE1 0x68
#define START_CODE2 START_CODE1
#define END_CODE    0x16
#define CONTROL_CODE_645_EX 0x03



//645Э��--�������ʶ
const uint8_t TAG_FILE_TRAMSMIT[] = {0x0F, 0x00, 0x00, 0x01};  		//�ļ�����ָ��          
const uint8_t TAG_PROGRAM_SWITCH[] = {0x0F, 0x00, 0x00, 0x02};  		//�����л�            
const uint8_t TAG_ID_AUTHEN_FIRST[] = {0x04, 0x00, 0x0F, 0x40}; 	 	//�����֤��һ֡����  
const uint8_t TAG_ID_AUTHEN_SECOND[] = {0x04, 0x00, 0x0F, 0x41}; 	 	//�����֤�ڶ�֡����  
const uint8_t TAG_GET_SECURITY_INFO[] = {0x04, 0x00, 0x0F, 0x42}; 	//��ȡTAר��ģ�鰲ȫ��
const uint8_t TAG_KEY_UPDATE[] = {0x04, 0x00, 0x0F, 0x43};  			//��Կ����              
const uint8_t TAG_GET_BAUDRATE[] = {0x04, 0x00, 0x0F, 0x44};  		//��ȡ����������������֡
const uint8_t TAG_SET_BAUDRATE[] = {0x04, 0x00, 0x0F, 0x45};  		//���ò���������������֡
const uint8_t TAG_GET_TA_TRANSFORMER[] = {0x04, 0x00, 0x0F, 0x46};  	//��ȡTA��Ȳ�������
const uint8_t TAG_SET_TA_TRANSFORMER[] = {0x04, 0x00, 0x0F, 0x47};  	//����TA��Ȳ�������
const uint8_t TAG_GET_TA_STATUS[] = {0x07, 0x00, 0x00, 0x00};  		//��ȡTAר��ģ��״̬����
const uint8_t TAG_GET_TA_WORKING[] = {0x07, 0x00, 0x00, 0x01};  		//��ȡTAר��ģ�鹤����
const uint8_t TAG_GET_REAL_DATA[] = {0x07, 0x00, 0x00, 0x03};  		//��ȡTAר��ģ��ʵʱ����
const uint8_t TAG_GET_GUIDE_PARAM[] = {0x07, 0x00, 0x00, 0x04};  		//��ȡTAר��ģ����̲�

#define MAC_SIZE 4


enum  DIRECTION
{
    DOWN_DIRECTION = 0,
    UP_DIRECTION = 1
};


typedef struct TAG_FormatCode
{
    u8  IVcode:1;   //1ʱ�� ESAM ʹ���ⲿ IV����0��ʱ��ESAM ʹ���ڲ� IV��
    u8  DirFlag:1;  //��1�� ʱ�� ��������վ�·������´���ʶΪ��0�� ʱ�� �������ն��·�
    u8  EncryFlag:1;//��1��ʱ�� 645 ֡���������� DATA Ϊ���ģ������ı�ʶΪ��0��ʱ�� 645 ֡���������� DATA Ϊ����
    u8  Reserve:5;
}__attribute__((packed))sTAG_FormatCode;


//�Ե�������壺 00H�� CT ��������ģʽ�� 01H�� CT ��·�� 02H�� CT ��·
typedef struct Format_TA_Status
{
    sTAG_FormatCode FormatTag;
    u8  Phase_A_ststus;
    u8  Phase_B_ststus;
    u8  Phase_C_ststus;
    u8  Mac[MAC_SIZE];
}__attribute__((packed))sFormat_TA_Status;


typedef struct TAG_TA_Frequence
{//XXX.XXX kHz
    u8  Freq_A_Value[3];
    u8  Freq_B_Value[3];
    u8  Freq_C_Value[3];
}__attribute__((packed))sTAG_TA_Frequence;


typedef struct TAG_TA_Currents
{//XXX.XXX A 
    u8  Currents_A[3];
    u8  Currents_B[3];
    u8  Currents_C[3];
}__attribute__((packed))sTAG_TA_Currents;


typedef struct TAG_TA_Misc
{
    u8  Impedance_Mode[4];//XXXXXX.XX ��
    u8  Impedance_Angle[2];//XXX.X ��
    u8  Impedance_Freq[2];//XXX.X  kHz
}__attribute__((packed))sTAG_TA_Misc;


typedef struct Format_TA_Real_Data
{
    sTAG_FormatCode FormatTag;
    u8  Phase_A_ststus;
    u8  Phase_B_ststus;
    u8  Phase_C_ststus;
    sTAG_TA_Frequence FreqInfo_Max;
    sTAG_TA_Frequence FreqInfo_Min;
    sTAG_TA_Currents CurrentsInfo;
    u8 Temperature[2];//XXX.X ��
    sTAG_TA_Misc Phase_A_First;
    sTAG_TA_Misc Phase_B_First;
    sTAG_TA_Misc Phase_C_First;
    sTAG_TA_Misc Phase_A_Second;
    sTAG_TA_Misc Phase_B_Second;
    sTAG_TA_Misc Phase_C_Second;
    sTAG_TA_Misc Phase_A_Three;
    sTAG_TA_Misc Phase_B_Three;
    sTAG_TA_Misc Phase_C_Three;
    u8  Mac[MAC_SIZE];
}__attribute__((packed))sFormat_TA_Real_Data;






typedef struct TAG_SERR_INFO
{
    u16 oherErrFlag:1;//��������
    u16 noRequestFlag:1;//�� �� ������
    u16 needAuthenFlag:1;//δ �� ���� �� ��֤
    u16 MacErrFlag:1;   //MAC ���� �� ��֤����
    u16 authenFailFlag:1;//�� �� ��֤ʧ��
    u16 validDataFlag:1;//�� �� ��Ч
    u16 KeyUpdateFailFlag:1;//�� Կ ����ʧ��
    u16 reserve:1;
    u16 flowFailFlag:8;//����ʧ����Ϣ��ʶ ˫�������֤�� �ο���6.2.2 F101�� ��ȡ TA ר��ģ�鰲ȫ��Ϣ�����������̣�������ֱ��д�� 0x00��
}__attribute__((packed))sTAG_SERR_INFO;



typedef struct ControlCode_645
{
    u8  functionCode:5;
    u8  followUpFlag:1;
    u8  slaveRespondFlag:1;
    u8  direction:1;
}__attribute__((packed))sControlCode_645;


typedef struct DataArea_645
{
    u8  DI[4];
    u8  opCode[4]; 
    u8  data[1];
}__attribute__((packed))sDataArea_645;




typedef struct ProHead_645_ex
{
    u8  startCode1;
    u8  addr[6];
    u8  startCode2;
    sControlCode_645  sCtrCode;
    u8  dataLen;
    sDataArea_645 dataArea;
}__attribute__((packed))sProHead_645_ex;

typedef struct ProEnd_645
{
    u8  sumCrc;
    u8  endCode;
}__attribute__((packed))sProEnd_645;





#ifndef  __GNUC__
#pragma pack(pop)
#endif

#ifdef __cplusplus
}
#endif

#endif
