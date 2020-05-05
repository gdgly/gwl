#ifndef __MRS_FW_PROTO376_1_H_
#define __MRS_FW_PROTO376_1_H_
#include "Sgd_types.h"


#define FRAME_MAX_SIZE 512

#define	FRAME_START_13761	0x68
#define	FRAME_END_13761		0x16

#define	PROTOCOL_13761_BUF_SIZE			1200
#define	PROTOCOL_13761_CTR_SEQ_LEN		8		//������+��ַ��+AFN+SEQ
#define	PROTOCOL_13761_HEAD_LEN			6
#define	PROTOCOL_13761_TAIL_LEN			2
#define	FRAME_13761_DIR_DOWN			0		//��վ���б���
#define	FRAME_13761_DIR_UP				1		//�ն����б���
#define	FRAME_13761_PRM_START_STATION	1		//֡������������վ
#define	FRAME_13761_PRM_ACK_STATION		0		//֡�������ԴӶ�վ

#define	PROTOCOL_13761_MAX__DATA_LEN	PROTOCOL_13761_BUF_SIZE-PROTOCOL_13761_CTR_SEQ_LEN
#define	PROTOCOL_13761_MIN_FRAME_LEN	20		//֡��С����

//��Ӧ��·���ƹ�����
#define LINK_CTRL_UP_CONFIRM			0
#define LINK_CTRL_UP_RES_USERDATA		8
#define LINK_CTRL_UP_RES_NODATA			9
#define LINK_CTRL_UP_RES_LINKSTAT		11

//��Լ��ʶ
#define PROTOCOL_13761_VALID			2

//Ӧ�ò㹦����    //
#define AFN_00_CONFIRM				0x00	//ȷ��/����
#define AFN_01_RESET				0x01	//��λ����
#define AFN_02_LINK_TEST			0x02	//��·�ӿڼ��
#define AFN_03_RELAYSTATION			0x03	//�м�վ����
#define AFN_04_SETUP_PARM			0x04	//���ò���
#define AFN_05_CONTROL				0x05	//��������
#define AFN_06_AUTHENTICATION		0x06	//�����֤����ԿЭ��
#define AFN_08_LEVEL_CHECK			0x08	//���󱻼����ն������ϱ�
#define AFN_09_TMCONFIG             0x09	//�����ն����ü���Ϣ
#define AFN_0A_INQUIRY_PARM			0x0A	//��ѯ����
#define AFN_0B_TASK_DATA			0x0B	//������������
#define AFN_0C_REALTIME_DATA		0x0C	//����1������(ʵʱ����)
#define AFN_0D_HISTORY_DATA			0x0D	//����2������(��ʷ����)
#define AFN_0E_EVENT_DATA			0x0E	//����3������(�¼�����)
#define AFN_0F_FILE_TRANSFER		0x0F	//�ļ�����
#define AFN_10_DATA_TRANSMIT		0x10	//����ת��

#define	MAX_13761_DI_NUM		10

//Ӧ������
#define	ACK_ALL_OK		1
#define	ACK_ALL_ERR		2
#define	ACK_PART		3

#define	ACK_ERR_OK		0
#define	ACK_ERR_OT		1
#define	ACK_ERR_ADDR	2

#define	NULL_BCD		0xEE

#define BCD2INT(x)      (u32)((x)/16 * 10 + ((x) % 16))


/******************���ݸ�ʽ*******************/
typedef struct
{
    u8 sec;
	u8 min;
	u8 hour;
	u8 day;
	u8 month:5;
	u8 week:3;
	u8 year;
}__attribute__((packed)) SJGS_01;//���ݸ�ʽ01

typedef struct
{
    u32 SFW:4,    //ʮ��λ
        GW:4;     //��λ
    u32 SW:4,     //ʮλ
        BW:3,     //��λ
        S:1;      
}__attribute__((packed)) SJGS_05;//���ݸ�ʽ05

typedef struct
{
    u32 BFW:4,    //�ٷ�λ
        SFW:4;    //ʮ��λ 
    u32 GW:4,     //��λ
        SW:3,     //ʮλ
        S:1;      
}__attribute__((packed)) SJGS_06;//���ݸ�ʽ06

typedef struct
{
    u32 SFW:4,    //ʮ��λ
        GW:4;     //��λ
    u32 SW:4,     //ʮλ
        BW:4;     //��λ     
}__attribute__((packed)) SJGS_07;//���ݸ�ʽ07

typedef struct
{
    u32 GW:4,    //��λ
          SW:4;     //ʮλ
    u32 BW:4,     //��λ
          QW:4;     //ǧλ     
}__attribute__((packed)) SJGS_08;//���ݸ�ʽ08

typedef struct
{
    u32 WFW:4,    //���λ
        QFW:4;    //ǧ��λ
    u32 BFW:4,    //�ٷ�λ
        SFW:4;    //ʮ��λ
    u32 GW:4,     //��λ
        SW:3,     //ʮλ
        S:1;
}__attribute__((packed)) SJGS_09;//���ݸ�ʽ09

typedef struct
{
    u32 GW:4,     //��λ
        SW:4;     //ʮλ
    u32 BW:4,     //��λ
        QW:4;     //ǧλ 
    u32 WW:4,     //��λ
        SWW:4;    //ʮ��λ
}__attribute__((packed)) SJGS_10;//���ݸ�ʽ10

typedef struct
{
    u32 BFW:4,    //�ٷ�λ
        SFW:4;    //ʮ��λ
    u32 GW:4,     //��λ
        SW:4;     //ʮλ
    u32 BW:4,     //��λ
        QW:4;     //ǧλ
    u32 WW:4,     //��λ
        SWW:4;    //ʮ��λ      
}__attribute__((packed)) SJGS_11;//���ݸ�ʽ11

typedef struct
{
    u32 GW:4,     //��λ
        SW:4;     //ʮλ
    u32 BW:4,     //��λ
        QW:4;     //ǧλ
    u32 WW:4,     //��λ
        SWW:4;    //ʮ��λ
    u32 BWW:4,    //����λ
        QWW:4;    //ǧ��λ
    u32 YW:4,     //��λ
        SYW:4;    //ʮ��λ
    u32 BYW:4,    //����λ
        QYW:4;    //ǧ��λ      
}__attribute__((packed)) SJGS_12;//���ݸ�ʽ12

typedef struct
{
    u32 WFW:4,    //���λ
        QFW:4;    //ǧ��λ
    u32 BFW:4,    //�ٷ�λ
        SFW:4;    //ʮ��λ
    u32 GW:4,     //��λ
        SW:4;     //ʮλ
    u32 BW:4,     //��λ
        QW:4;     //ǧλ
    u32 WW:4,     //��λ
        SWW:4;    //ʮ��λ      
}__attribute__((packed)) SJGS_14;//���ݸ�ʽ14

typedef struct
{
    u32 FGW:4,    //�ָ�λ
        FSW:4;    //��ʮλ
    u32 SGW:4,    //ʱ��λ
        SSW:4;    //ʱʮλ
    u32 RGW:4,    //�ո�λ
        RSW:4;    //��ʮλ
    u32 YGW:4,    //�¸�λ
        YSW:4;    //��ʮλ 
    u32 NGW:4,    //���λ
        NSW:4;    //��ʮλ
}__attribute__((packed)) SJGS_15;//���ݸ�ʽ15

typedef struct
{
    u32 FGW:4,    //�ָ�λ
        FSW:4;    //��ʮλ
    u32 SGW:4,    //ʱ��λ
        SSW:4;    //ʱʮλ
    u32 RGW:4,    //�ո�λ
        RSW:4;    //��ʮλ
    u32 YGW:4,    //�¸�λ
        YSW:4;    //��ʮλ 
}__attribute__((packed)) SJGS_17;//���ݸ�ʽ17

typedef struct 
{
	u32 RGW:4,    //�ո�λ
        RSW:4;	//��ʮλ
	u32 YGW:4,	//�¸�λ
	    YSW:4;	//��ʮλ
	u32 NGW:4,	//���λ
	    NSW:4;	//��ʮλ
}__attribute__((packed)) SJGS_20;//���ݸ�ʽ20

typedef struct
{
    u32 YGW:4,    //�¸�λ
        YSW:4;    //��ʮλ 
    u32 NGW:4,    //���λ
        NSW:4;    //��ʮλ
}__attribute__((packed)) SJGS_21;//���ݸ�ʽ21

typedef struct
{
    u32 SFW:4,    //ʮ��λ
        GW:4;     //��λ
}__attribute__((packed)) SJGS_22;//���ݸ�ʽ22

typedef struct
{
    u32 WFW:4,    //���λ
        QFW:4;    //ǧ��λ
    u32 BFW:4,    //�ٷ�λ
        SFW:4;    //ʮ��λ
    u32 GW:4,     //��λ
        SW:4;     //ʮλ
}__attribute__((packed)) SJGS_23;//���ݸ�ʽ23

typedef struct
{
    u32 QFW:4,    //ǧ��λ
        BFW:4;    //�ٷ�λ
    u32 SFW:4,    //ʮ��λ
        GW:4;     //��λ
    u32 SW:4,     //ʮλ
        BW:3,     //��λ 
        S:1;
}__attribute__((packed)) SJGS_25;//���ݸ�ʽ25

typedef struct
{
    u32 GW:4,     //��λ
        SW:4;     //ʮλ
    u32 BW:4,     //��λ
        QW:4;     //ǧλ 
    u32 WW:4,     //��λ
        SWW:4;    //ʮ��λ
    u32 BWW:4,    //����λ
        QWW:4;    //ǧ��λ
}__attribute__((packed)) SJGS_27;//���ݸ�ʽ27


typedef struct
{
	u32 unit;		//��λ����
	u32 BFW:4,    //�ٷ�λ
        SFW:4;    //ʮ��λ
    u32 GW:4,     //��λ
        SW:4;     //ʮλ
    u32 BW:4,     //��λ
        QW:4;     //ǧλ 
    u32 WW:4,     //��λ
        SWW:4;    //ʮ��λ  
}__attribute__((packed)) SJGS_29;//���ݸ�ʽ29

typedef struct
{
	u32 unit;		//��λ����
	u32 WFW:4,    //���λ
        QFW:4;    //ǧ��λ  
	u32 BFW:4,    //�ٷ�λ
        SFW:4;    //ʮ��λ
    u32 GW:4,     //��λ
        SW:4;     //ʮλ
    u32 BW:4,     //��λ
        QW:4;     //ǧλ 
}__attribute__((packed)) SJGS_30;//���ݸ�ʽ30

typedef struct
{
	u32 BFW:4,    //�ٷ�λ
        SFW:4;    //ʮ��λ
    u32 GW:4,     //��λ
        SW:4;     //ʮλ
    u32 BW:4,     //��λ
        QW:4;     //ǧλ 
}__attribute__((packed)) SJGS_31;//���ݸ�ʽ31

typedef struct
{
    u32 MGW:4,     //���λ
        MSW:4;     //��ʮλ
    u32 FGW:4,     //�ָ�λ
        FSW:4;     //��ʮλ
    u32 SGW:4,     //ʱ��λ
        SSW:4;     //ʱʮλ
    u32 RGW:4,     //�ո�λ
        RSW:4;     //��ʮλ
    u32 YGW:4,     //�¸�λ
        YSW:4;     //��ʮλ
    u32 NGW:4,     //���λ
        NSW:4;     //��ʮλ
	u32 NBW:4,     //���λ
	    NQW:4;     //��ǧλ
}__attribute__((packed)) SJGS_32;//���ݸ�ʽ32


typedef struct
{
    u32 switchstate    :2,     //����״̬
        BatteryVoltage :1,     //��ص�ѹ 0������1��Ƿѹ
        reservebit     :5;
	u32 reservebyte;           //����
}__attribute__((packed)) SJGS_ST;//���ݸ�ʽST

/*******************END************************/


typedef enum{
	ERR_COMMTIMEOUT = 1,	// ͨ�ų�ʱ
	ERR_INVALID_DATAUNIT,	// ��Ч���ݵ�Ԫ
	ERR_LEN,				// ���ȴ�
	ERR_VERIFY,				// У�����
	ERR_NO_INFO,			// ��Ϣ�಻����
	ERR_FORMAT,				// ��ʽ����
}GDW376_1_ERR_TYPE;


//LINE144
typedef struct stFrame13761LenSType
{
	uint16_t m_pid : 2;
	uint16_t m_len : 14;
}stFrame13761LenSType;

typedef union ULCLenUnType
{ 
	stFrame13761LenSType len_s;
	uint16_t value;
}unLenUnType;
	
typedef struct stFrame13761Len
{
	unLenUnType len_un;
}stFrame13761Len;
//LINE157

//LINE158
typedef struct stFrame13761ActSType
{
	uint8_t code : 4;
	uint8_t fcv : 1;
	uint8_t fcb : 1;
	uint8_t prm : 1;
	uint8_t dir : 1;
}stFrame13761ActSType;

typedef struct stFrame13761RespSType
{
	uint8_t code : 4;				//������
	uint8_t rsv : 1;				//֡������Чλ
	uint8_t acd : 1;				//�������λ
	uint8_t prm : 1;				//������־
	uint8_t dir : 1;				//���䷽��
}stFrame13761RespSType;		

typedef union unFrame13761CtrlType{
	stFrame13761ActSType act_s;
	stFrame13761RespSType resp_s;
	uint8_t value;
}unFrame13761CtrlType;
//LINE179

typedef union stFrame13761CtrlType
{
	unFrame13761CtrlType control_un;
}stFrame13761CtrlType;

//LINE184
typedef struct stFrame13761MaSType
{
	uint8_t group : 1;
	uint8_t msta : 7;
}stFrame13761MaSType;

typedef union 
{
	stFrame13761MaSType ma_s;
	uint8_t value;
}unFrame13761M_Ma_unType;	


typedef struct 
{
	uint16_t districtCode;
	uint16_t termAddr;
	unFrame13761M_Ma_unType mainStn_TermGrp_addr;
}__attribute__((packed)) stFrame13761AddrType;
//LINE202

//204
typedef struct stFrame13761AppSeqType
{
	uint8_t seq : 4;
	uint8_t con : 1;
	uint8_t fin : 1;
	uint8_t fir : 1;
	uint8_t tpv : 1;
}stFrame13761AppSeqType;
typedef struct stFrame13761UserDataType
{
	uint8_t afn;
	stFrame13761AppSeqType appseq;
	uint8_t data[0];
}stFrame13761UserDataType;
//217

//֡ͷ
typedef struct stFrame13761Head
{
	uint8_t header1;					// 1
	stFrame13761Len len;				// 2
	stFrame13761Len len1;				// 2
	uint8_t header2;					// 1
	stFrame13761CtrlType control;		// 1
	stFrame13761AddrType addr;			// 5
	stFrame13761UserDataType userdata;	// 1+1+data 
}__attribute__((packed)) stFrame13761Head;



//֡β
typedef struct stFrame13761Tail
{
	uint8_t check_sum;
	uint8_t tail;
}stFrame13761Tail;

//���ݸ�ʽ


//A.16	���ݸ�ʽ16
//���ʱ��
typedef struct StDataFormat16
{
	uint8_t second; //BCD
	uint8_t minute; //BCD
	uint8_t hour;   //BCD
	uint8_t day;    //BCD
}StDataFormat16;


//DA��DT
typedef struct stDA	//Pn
{
	uint8_t DA1;
	uint8_t DA2;
}stDA;

typedef struct stDT	//Fn
{
	uint8_t DT1;
	uint8_t DT2;
}stDT;

typedef struct stDataDI
{
	stDA  DA;
	stDT  DT;
}stDataDI;

typedef struct AppAuxEC               	//�¼�������EC�����У�
{
	uint8_t AppAuxEC1;           		//��Ҫ�¼�������EC1
	uint8_t AppAuxEC2;          	 	//һ���¼�������EC2
}AppAuxEC;


//������Ϣ�������Ϣ��֤���ֶ�PW���¼�������EC��ʱ���ǩTp
typedef struct AppAuxPwd				//������Ϣ��
{
	uint8_t  appAuxPw[16];				//��Ϣ��֤���ֶ�PW�����У�
}AppAuxPwd;

typedef struct AppAuxTp	                //������Ϣʱ���ǩTp
{
	uint8_t       AppAuxTpPFC;      	//����֡֡��ż�����PFC
	StDataFormat16 AppAuxTpDate;		//����֡����ʱ�꣨���ʱ�գ�
	uint8_t       AppAuxTpDelay;   		//�����ʹ�����ʱʱ��,����,0��ʾ������ʱ�ж�
}AppAuxTp;

typedef struct AppAuxUp					//������Ϣ��
{
	AppAuxEC appAuxEC;					//�¼�������EC�����У�
	AppAuxTp appAuxTp;					//������Ϣʱ���ǩTp
}AppAuxUp;

typedef struct AppAuxDown
{
	AppAuxPwd appAuxPwd;
	AppAuxTp  appAuxTp;					//������Ϣʱ���ǩTp
}AppAuxDown;




typedef  struct st_IP
{
	uint8_t IP_s1;
	uint8_t IP_s2;
	uint8_t IP_s3;
	uint8_t IP_s4;
}st_IP;

typedef  union U_IP
{
	uint32_t IP;
	st_IP st_IP;
}U_IP;



	


//BCD��
typedef struct stTime
{
	u8 sec;
	u8 min;
	u8 hour;
	u8 day;
	u8 month:5;
	u8 week:3;
	u8 year;
}stTime;

//F3�ն�����ʱ��
typedef struct stTermTime
{
	stTime time;//����ʱ��
}stTermTime;





/*------------------------------------------------
F9	�ն��¼���¼��������	p0 
�¼���¼��Ч��־λ	BS64	8
�¼���Ҫ�Եȼ���־λ	BS64	8
------------------------------------------------*/
typedef struct  TermEventRecordSet
{
	u8 CS_F9_EventRecordValidFlag[8];
	u8 CS_F9_EventRatingFlag[8];
}TermEventRecordSet;











// ʱ�����ڸ�ʽ����
typedef struct
{
	u32 year	:6;	// 0-63		since 2000
	u32 month	:4;	// 1-12
	u32 day		:5;	// 1-31
	u32 hour	:5;	// 0-23
	u32 minute	:6;	// 0-59
	u32 second	:6;	// 0-59
} DateTime;


//YJW 
typedef struct //��λ����
{
    u32   DA[2];              //pn
    u32   DT[2];              //fn 
    SJGS_01  termCurDate;
}__attribute__((packed)) DATA_AFN0C_F2;			//F2���ն����ڲ�ѯ


typedef struct
{
    u32 DA[2];                //pn
    u32 DT[2];                //fn
    u32 Dayvalue;
    u32 Monthvalue;    
}__attribute__((packed))DATA_AFN0C_F10;			//F10��ͨ������






/*****************************��������*******************************/

typedef struct ST_OVERTIME
{
	uint16_t over_time:12;
	uint16_t over_times:2;
	uint16_t rev:2;
}ST_OVERTIME;

typedef union U_OVERTIME
{
	uint16_t over_time_times;
	ST_OVERTIME st_over_time_times;
}U_OVERTIME;

//F1�ն�����ͨ�Ų���
typedef struct stTermUpCommuPara
{
	uint8_t RTS;
	uint8_t delay_time;
	U_OVERTIME u_over_time_times;
	uint8_t confirm;
	uint8_t heart_beats;//min
}__attribute__((packed))stTermUpCommuPara;



//F2�ն˵���λ��
typedef struct
{
    u16 longitude_S_BFW:4,		//ʮ��λ		��
        longitude_S_SFW:4,		//�ٷ�λ
        longitude_S_GW:4,		//ʮλ
       	longitude_S_SW:4;		//��λ
       	
    u8  longitude_B_GW:4,		//ʮλ		��
        longitude_B_SW:4;		//��λ
        
    u16 longitude_D_GW:4,		//��λ		��
        longitude_D_SW:4,		//ʮλ
        longitude_D_BW:4,		//��λ
        longitude_D_BLW:4;		//����

	u16 latitude_S_BFW:4,		//�ٷ�λ		��
        latitude_S_SFW:4,		//ʮ��λ
        latitude_S_GW:4,		//��λ
       	latitude_S_SW:4;		//ʮλ
       	
    u8  latitude_B_GW:4,		//��λ		��
        latitude_B_SW:4;		//ʮλ
        
    u16 latitude_D_GW:4,		//��λ		��
        latitude_D_SW:4,		//ʮλ
        latitude_D_BW:4,		//����
        latitude_D_BLW:4;		//����	
}__attribute__((packed))stTermLocation;


//F3��վIP��ַ�Ͷ˿�
typedef struct
{
	U_IP u_IP_main;
	uint16_t Port_main;
	
	U_IP u_IP_bak;
	uint16_t Port_bak;
	
	uint8_t APN[16];	
}__attribute__((packed))stMainStationIPPort;


//AFN04_F7;//F7:�ն�IP��ַ�Ͷ˿�
typedef struct
{
    //�ն�IP
    union
    {
        u8   termip[4];
        u32  termip32u;
    };                    

	//��������
    union
    {
        u8   netmask[4];
        u32  netmask32u;
    };                   	

	//����
    union
    {
        u8   gateway[4];
        u32  gateway32u;
    };                   
    u8   proxytype;             //��������  //a)�������ͣ���ֵ��Χ0��3�����α�ʾ����ʹ�ô���httpconnect����socks4����socks5����
    union
    {
        u8   proxyserverip[4];
        u32  proxyserverip32u;
    };             

	//���������IP��ַ
    u16  proxyserverport;         			//����������˿�
    u8   proxyserverconnectway;  			//������������ӷ�ʽ //������������ӷ�ʽ����ֵ��Χ0��1�����α�ʾ��������֤����Ҫ�û���/����
    u8 user_len;//�û�������
    u8 user[20];//�û���
    u8 pw_len;//���볤��
    u8 pw[20];//����;ascll
    u16 listen_port;//�����˿�
}__attribute__((packed)) DATA_AFN04_F7;//F7:�ն�IP��ַ�Ͷ˿�

//AFN04_F8
typedef struct
{
    uint8_t ConnStartMode:2;
    uint8_t rev2:2;
    uint8_t mode:2;
    uint8_t rev1:1;
    uint8_t TcpUdp:1;
}sTermMode;
typedef union
{
    sTermMode bMode;
    uint8_t mode;
}uTermMode;
typedef struct
{
	uTermMode TermMode;
    uint16_t ReconnInterval;//s
    uint8_t MaxRetryCnt;
    uint8_t MaxIdleTime;//min
    uint8_t TimeSpan[3];//every bit indicates one hour of allow sign in flag
}__attribute__((packed))sTermWorkWaysUpComm;

#define MAX_USERLEN     20
#define MAX_PASSWDLEN   20
typedef struct
{
    DATA_AFN04_F7   data;
    u8   userlen;
    u8   user[MAX_USERLEN];
    u8   passwdlen;
    u8   passwd[MAX_PASSWDLEN];
    u8   listenport;
    u8   updatetime;    
}__attribute__((packed)) buf_AFN04_F7;//F7���ն�IP��ַ�Ͷ˿�





//F89�ն��߼���ַ
typedef struct
{
	u16 districtCode;							//��������
	u16 termAddr;								//�ն˵�ַ
	//u8 a3;
}MODLE_ADDR_STRU;


//F153������·ʹ�ܲ���
typedef struct
{
    unsigned char   phase_a_curr;			//������·ʹ�ܲ���A
    unsigned char   phase_b_curr;			//������·ʹ�ܲ���B
    unsigned char   phase_c_curr;			//������·ʹ�ܲ���C
    unsigned char   mac[4];				    //mac
}__attribute__((packed)) current_circuit_param_stru;

//F154TA��Ȳ���
typedef struct
{
    unsigned char format;
	unsigned short phase_a_trans;	//A���������������
	unsigned short phase_b_trans;	//B���������������
	unsigned short phase_c_trans;	//C���������������
	unsigned char mac[4];		    //mac
}__attribute__((packed)) current_transformer_rate_stru;	


typedef struct
{
	unsigned char VirUser[32];
	unsigned char VirPasswd[32];
}sVirtualNet;


//Ѳ�����豸ϵͳ��Ϣ
typedef struct
{
	stTermLocation 		Location;				//λ��
	stMainStationIPPort MainStationIPPort;		//��վip/port	
	MODLE_ADDR_STRU 	usModleAddr;			//�ն��߼���ַ
	stTermUpCommuPara  	UpCommuPara; 			//����ͨ�Ų���
}CIRCUIT_SYSTEM_INFO_STRU;  


typedef struct
{
	CIRCUIT_SYSTEM_INFO_STRU parm;
	u8 enable_flag;
}NV_CHARGE_SYSTEM_INFO;


//AFN00�ķ���֡������
typedef struct stError
{
	u8 afn;				//Ҫ��ȷ�ϵı��ĵ� AFN	
	u8 Di[4];			//��ȷ�ϵ����ݵ�Ԫ��ʶ 1  
	u8 err;				//������ϴ���  0��ȷ��1�������󡢱��ַ�ظ�
}stError;



//��·��ͨ�Żظ�
typedef struct stUpflag
{
	u8 signAck;			//��¼�ظ�
	u8 heatAck;			//�����ظ�
}stUpflag;



#if 0
#define CMDMSGNUM  10
#define CHARGET_TEST_NUM 3

typedef struct stControlCMD
{
	volatile u8 cmd;//0���У�1������磬2���ӳ�磬3ֹͣ���
	volatile u8 srcpn;
	
	volatile u8 dst;
	volatile u8 dstpn;
	
	volatile u16 srcFrmLen;
	u8 srcFrm[100];
	//stChargerACK ChargAck;
	volatile u16 acklen;
	u8 ackBuf[50];
	volatile u32 timeout;
	
}stControlCMD;

typedef struct
{
	volatile u8 cmd;//0���У�1������磬2���ӳ�磬3ֹͣ���
	volatile u8 srcpn;
	
	volatile u8 dst;
	volatile u8 dstpn;
	
	volatile u16 srcFrmLen;
	u8 srcFrm[60];
	//stChargerACK ChargAck;
	volatile u16 acklen;
	u8 ackBuf[50];
	volatile u32 timeout;
	
}stConChargeCMD;


typedef struct{
	volatile u8 flag;
	volatile u8  repeatUpgradeCount;
	volatile u16 update_size;
	volatile u32 total_size;
	u8 softVer[4];
	volatile u8 crc_error;
	volatile u8 crc_error_cnt;
}stRepeatUpgrade;


typedef struct stControlMsg
{
	volatile u8 cnt;//��Ϣ����������
	stControlCMD ControlCMD[CMDMSGNUM];
}stControlMsg;

typedef struct {
	volatile u8 cnt;
	stConChargeCMD ControlCMD[CHARGET_TEST_NUM];

}stChargeTest;


#define GPRSMSGNUM 20

typedef struct stMsg
{
	u8 len;//��Ϣ����������
	u8 data[100];
}stMsg;


#define ENDCHARGEMSGNUM 10

typedef struct stMsg1
{
	u16 len;
	u16 report_sucesss_flag;
	u8 seq;
	u8 data[60];
}stMsg1;

typedef struct stGprsMsg
{
	u8 cnt;							 //��Ϣ����������
	stMsg msg[GPRSMSGNUM];
}stGprsMsg;

//
typedef struct stEndChargeMsg{
	u8 cnt;							 //��Ϣ����������
	stMsg1 msg[ENDCHARGEMSGNUM];
	u16 deviceFaultStatus;			 //���ڼ�¼�豸����״̬
}stEndChargeMsg;
#endif







/***************************�¼�***************************/


//������Ҫ�¼�ָ��
typedef struct
{
    u8 Pm;                //��ʼָ��
    u8 Pn;                //����ָ��
}__attribute__((packed))DATA_AFN0E_F1;


//����һ���¼�ָ��
typedef struct
{
    u8 Pm;                //��ʼָ��
    u8 Pn;                //����ָ��
}__attribute__((packed))DATA_AFN0E_F2;

//F3���¼�����ERC�����¼�
typedef struct
{
    u8 ERC;                //�¼�����ERC
    u8 Num;                //������������¼�������
}__attribute__((packed))DATA_AFN0E_F3;

typedef struct
{
	u8 DA[2];				//pn
	u8 DT[2];				//fn
	u8 EventNum;			//�¼�����
}__attribute__((packed))EventReqReply;


//AFN0EӦ��������ͷ��-�����¼�
typedef struct  
{
	u8 DA[2];				//pn
	u8 DT[2];				//fn
	u8 EC1;
	u8 EC2;
	u8 Pm;
	u8 Pn;
}__attribute__((packed))EventsBlockHead;



//ERC1�����ݳ�ʼ���Ͱ汾����¼�
typedef struct
{
    u8 ERCcode;          //�¼�����ERC
    u8 Le;               //�¼�����
    SJGS_15 time;        //���ʱ��
    u8  CSH:1,           //��ʼ��
        VER:1,           //�汾
        Reserv:6;
    u8  VerOld[4];       //���ǰ�汾
    u8  VerNew[4];       //�����汾
}__attribute__((packed))stru_sjjl_ERC1;

typedef struct
{
    u8 DA[2];            //pn
    u8 DT[2];            //fn
    u8 ECcount;          //�¼�����
    stru_sjjl_ERC1 sjjlerc1[15];      
}__attribute__((packed))DATA_AFN0E_F3_ERC1;



//ERC2��������ʧ�¼�
typedef struct
{
    u8 ERCcode;          //�¼�����ERC
    u8 Le;               //�¼�����
    SJGS_15 time;        //����ʱ��		 		
	u8	LoseTerminal:1,	 //�¼���־	
		LosePot:1,
		Reserv1:6;
}__attribute__((packed))stru_sjjl_ERC2;     


typedef struct
{
    u8 DA[2];            //pn
    u8 DT[2];            //fn
    u8 ECcount;          //�¼�����
    stru_sjjl_ERC2 sjjlerc2[15];      
}__attribute__((packed))DATA_AFN0E_F3_ERC2;


//ERC3����������¼�
typedef struct
{
    u8 ERCcode;          //�¼�����ERC
    u8 Le;               //�¼�����
    SJGS_15 time;        //��������ʱ��
    u8  MstAddr;         //����վ��ַ
    //u8  Num;       	 //���ݵ�Ԫ��ʶ������
	u8 *uniundata;		 //���ݵ�Ԫ��ʶ��												//����ô����todo
}__attribute__((packed))stru_sjjl_ERC3;

typedef struct
{
    u8 DA[2];            //pn
    u8 DT[2];            //fn
    u8 ECcount;          //�¼�����
    stru_sjjl_ERC3 sjjlerc3[15];      
}__attribute__((packed))DATA_AFN0E_F3_ERC3;



//ERC14���ն�ͣ/�ϵ��¼�
typedef struct
{
    u8 ERCcode;          	//�¼�����ERC
    u8 Le;               	//�¼�����
    SJGS_15 offtime;        //ͣ��ʱ��
    SJGS_15 ontime;         //�ϵ�ʱ��     
}__attribute__((packed))stru_sjjl_ERC14;

typedef struct
{
    u8 DA[2];            //pn
    u8 DT[2];            //fn
    u8 ECcount;          //�¼�����
    stru_sjjl_ERC14 sjjlerc14[15];
        
}__attribute__((packed))DATA_AFN0E_F3_ERC14;



//ERC20����Ϣ��֤�����¼�
typedef struct
{
    u8 ERCcode;          	//�¼�����ERC
    u8 Le;               	//�¼�����
    SJGS_15 time;           //����ʱ��
    u8 pw[16];				//��Ϣ��֤��
    u8 MSA;					//����վ��ַ
}__attribute__((packed))stru_sjjl_ERC20;    

typedef struct
{
    u8 DA[2];            //pn
    u8 DT[2];            //fn
    u8 ECcount;          //�¼�����
    stru_sjjl_ERC20 sjjlerc20[15];    
}__attribute__((packed))DATA_AFN0E_F3_ERC20;



//ERC21���ն˹��ϼ�¼
typedef struct
{

	u8 ERCcode; 		 //�¼�����ERC
	u8 Le;			     //�¼�����
	SJGS_15 time;		 //����ʱ��
	u8 Code;			 //���ϱ���
}__attribute__((packed))stru_sjjl_ERC21;

typedef struct
{
    u8 DA[2];            //pn
    u8 DT[2];            //fn
    u8 ECcount;          //�¼�����
    stru_sjjl_ERC21 sjjlerc21[15];
        
}__attribute__((packed))DATA_AFN0E_F3_ERC21;



//ERC32�ն�����վͨ�������������¼���¼
typedef struct
{
    u8 ERCcode;          			 //�¼�����ERC
    u8 Le;               			 //�¼�����
    SJGS_15 time;       			 //����ʱ��
    u8 CommFlux[4];             	 //��������
    u8 CommFluxLimit[4];             //��������
    
}__attribute__((packed))stru_sjjl_ERC32;

typedef struct
{
    u8 DA[2];            //pn
    u8 DT[2];            //fn
    u8 ECcount;          //�¼�����
    stru_sjjl_ERC32 sjjlerc32[15];        
}__attribute__((packed))DATA_AFN0E_F3_ERC32;



//ERC34��CT �쳣�¼���¼
typedef struct
{
	u8 ERCcode;             //�¼�����ERC
    u8 Le;                  //�¼�����
	SJGS_15 time;           //����ʱ��
	u8 mp[2];			    //�������
	u8 flag;				//�쳣��ʶ
}__attribute__((packed))stru_sjjl_ERC34;

typedef struct
{
    u8 DA[2];            //pn
    u8 DT[2];            //fn
    u8 ECcount;          //�¼�����
    stru_sjjl_ERC34 sjjlerc34[15];        
}__attribute__((packed))DATA_AFN0E_F3_ERC34;



//ERC40���ų��쳣�¼���¼
typedef struct
{
	u8 ERCcode;             //�¼�����ERC
    u8 Le;                  //�¼�����
	SJGS_15 time;			//����ʱ��
	u8 mp[2];			    //�������
	u8 DeviceType;			//�豸����
	u8 DeviceAddr[6];		//�豸��ַ
	u8 AbnormalType;		//�ų��쳣����

}__attribute__((packed))stru_sjjl_ERC40;

typedef struct
{
    u8 DA[2];            //pn
    u8 DT[2];            //fn
    u8 ECcount;          //�¼�����
    stru_sjjl_ERC40 sjjlerc40[15];        
}__attribute__((packed))DATA_AFN0E_F3_ERC40;




//ERC41����ʱ�¼���¼
typedef struct
{
	u8 ERCcode;                     //�¼�����ERC
    u8 Le;                          //�¼�����
	u8 mp[2];						//�������, 0��ʾ�ն�
	SJGS_01 TimeBeforeAdjust;		//Уʱǰʱ��
	SJGS_01 TimeAfterAdjust;		//Уʱ��ʱ��
}__attribute__((packed))stru_sjjl_ERC41;

typedef struct
{
    u8 DA[2];            //pn
    u8 DT[2];            //fn
    u8 ECcount;          //�¼�����
    stru_sjjl_ERC41 sjjlerc41[15];        
}__attribute__((packed))DATA_AFN0E_F3_ERC41;

/*************************�¼�(end)*************************/





/***********AFN09�ն���Ϣ�����ṹ��****************************/
//F1�ն˰汾��Ϣ
typedef struct stTermVerInfo
{
	u8 factry_code[4];				//���̴���
	u8 term_number[8];				//�豸���
	u8 soft_ver[4];					//����汾
	SJGS_20 release_time;			//��������,bcd���գ��£���				A.20
	u8 capacity_info[11];			//����������Ϣ��
	u8 Protocol_version[4];			//Э��汾��
	u8 hardware_ver[4];				//Ӳ���汾��
	SJGS_20 hardware_release_time;	//��������,bcd���գ��£���				A.20
}stTermVerInfo;


//F9Զ��ͨ��ģ��汾��Ϣ
typedef struct RemoteVerInfo
{
	u8 factry_code[4];				//���̴���
	u8 module_type[8];				//ģ���ͺ�
	u8 soft_ver[4];					//����汾
	SJGS_20 release_time;			//��������,bcd���գ��£���				A.20
	u8 hardware_ver[4];				//Ӳ���汾��
	SJGS_20 hardware_release_time;	//��������,bcd���գ��£���				A.20
	u8 ICCID[20];					//SIM��ICCID
}RemoteVerInfo;




/****************************�ļ�����************************/

#define START_AND_PROCESS_FRAME  	0x00
#define END_FRAME  					0x02

#define FIST_LEN_FRAME				16
#define AFTER_LEN_FRAME 			64
#define TIMEOUT_CNT                 100

typedef struct{
	volatile u8 flag;
	volatile u8  repeatUpgradeCount;
	volatile u16 update_size;
	volatile u32 total_size;
	u8 softVer[4];
	volatile u8 crc_error;
	volatile u8 crc_error_cnt;
}stRepeatUpgrade;

typedef struct
{
	volatile u8 flag;// 1�����ļ�������0������
	volatile u8 uncopyflag;//0:�Ѿ�������1δ����
	volatile u8 runTestflag;//1�����У�����δ����
	volatile u16 updatasize;//��λ:k
	volatile u32 total_size;
	u8  fileHead[16];
}NV_UPDATA_FILE_FLAG;


typedef struct stFlieTransmit
{
	u8 Flag;//�ļ���ʶ//0--7,0:��������ļ�
	u8 mode;//�ļ�����//00H ��ʼ/�м� 01H ��β
	u8 cmd;//0--2	//�ļ�ָ��	0:376.1(ҪӦ��,����վ����֡ǰ����Ӧ��) 1:FTP 2:���ַ����?(����Ӧ��)
	u16 Num;//�ܶ���
	u32 DI;              //��i�α�ʾ��0----Num-1
	u16 dataLen;		//��i�εĳ���LF
	u8 data[0];			//��LF��Ӧ����������
}stFlieTransmit;




uint8_t CheckSum13761(uint8_t *pBuf, uint16_t size);
u8 GDW376_1_CalSum(u8* pBuf, u16 len);
s32 Protocl13761ParseData(u8 *pRecvData, u16 nRecvLen, int fd);
int sendLinkPacktoMainStaion(int stauts, unsigned char *ucDatagram, int *len);
AppAuxPwd* getAppAuxPwdDown(uint8_t *pBuf, uint16_t len);
AppAuxTp* getAppAuxDownTp(uint8_t *pBuf, uint16_t len);
AppAuxEC* getAppAuxUpEC(uint8_t *pBuf, uint16_t len);
AppAuxTp* getAppAuxUpTp(uint8_t *pBuf, uint16_t len);
uint8_t getFn(stDT pDT, uint8_t *Fn);
uint8_t setFn(uint8_t Fn, stDT *pDT);
uint8_t getPn(stDA pDA, uint16_t *Pn);
uint8_t setPn(uint16_t Pn, stDA *pDA);
uint8_t sendAcktoMainStation(uint8_t type, uint8_t *frame, uint16_t frame_len, uint8_t *di_res, uint8_t di_len, int fd);
uint8_t sendAckFrametoMainStation(uint8_t function, uint8_t *frame, uint16_t frame_len, uint8_t *ACKdata, uint16_t ack_data_len, int fd);
s32 Protocl13761SendData2(u8 *pSendData, u16 nSendLen, int fd);
u32 intToBcd(u32 data,u8 *bcd,u8 len);


int getTermUpCommuPara(stTermUpCommuPara *TermUpCommuPara);
int SetTermUpCommuPara(stTermUpCommuPara *TermUpCommuPara);
#endif
