#ifndef _TA_MODULE_H_
#define _TA_MODULE_H_



#define MAC_SIZE 					4

#define CIRCUIT_NORMAL				0//正常状态
#define CIRCUIT_SHORTCUT			1//短路
#define CIRCUIT_BREAK				2//开路
#define CIRCUIT_SEC_SEPARATE		4//二次分流


typedef struct TAG_FormatCode
{
    u8  IVcode:1;   //1ʱ�� ESAM ʹ���ⲿ IV����0��ʱ��ESAM ʹ���ڲ� IV��
    u8  DirFlag:1;  //��1�� ʱ�� ��������վ�·������´���ʶΪ��0�� ʱ�� �������ն��·�
    u8  EncryFlag:1;//��1��ʱ�� 645 ֡���������� DATA Ϊ���ģ������ı�ʶΪ��0��ʱ�� 645 ֡���������� DATA Ϊ����
    u8  Reserve:5;
}__attribute__((packed))sTAG_FormatCode;

typedef struct Format_TA_Status
{
    sTAG_FormatCode FormatTag;
    u8  Phase_A_ststus;
    u8  Phase_B_ststus;
    u8  Phase_C_ststus;
    u8  Mac[MAC_SIZE];
}__attribute__((packed))sFormat_TA_Status;


sFormat_TA_Status GetTaStatus(void);
void TaModuleInit(void);


#endif
