#ifndef __APP_DATA_H_
#define __APP_DATA_H_
#pragma pack(1) 


#if 0

/******************���ݸ�ʽ*******************/
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



typedef struct
{
	u16 districtCode;							//��������
	u16 termAddr;								//�ն˵�ַ
	u8 a3;
}MODLE_ADDR_STRU;


//F2�ն˵���λ��
typedef struct
{
    u16 longitude_S_BFW:4,//ʮ��λ		��
        longitude_S_SFW:4,//�ٷ�λ
        longitude_S_GW:4,//ʮλ
       	longitude_S_SW:4;//��λ
       	
    u8  longitude_B_GW:4,//ʮλ		��
        longitude_B_SW:4;//��λ
        
    u16 longitude_D_GW:4,//ʮλ		��
        longitude_D_SW:4,//��λ
        longitude_D_BW:4,//����
        longitude_D_BLW:4;//��λ

	u16 latitude_S_SFW:4,//ʮ��λ		��
        latitude_S_BFW:4,//�ٷ�λ
        latitude_S_GW:4,//ʮλ
       	latitude_S_SW:4;//��λ
       	
    u8  latitude_B_GW:4,//ʮλ		��
        latitude_B_SW:4;//��λ
        
    u16 latitude_D_GW:4,//ʮλ		��
        latitude_D_SW:4,//��λ
        latitude_D_BW:4,//����
        latitude_D_BLW:4;//��λ
	
	//u8 longitude[5];		//����,�룬�֣���(F�Ķ��壺F=0�� ��ʾ������γ��F=l,��ʾ��������γ��)
	//u8 latitude[5];			//γ��,�룬�֣���(F�Ķ��壺F=0�� ��ʾ������γ��F=l,��ʾ��������γ��)
}__attribute__((packed))stTermLocation;

typedef struct
{
	stTermLocation 		Location;				//λ��
	stMainStationIPPort MainStationIPPort;		//��վip/port	
	MODLE_ADDR_STRU 	usModleAddr;			//�ն˵�ַ
	stTermUpCommuPara  	UpCommuPara; 			//����ͨ�Ų���
#if 0
	stTermVerInfo		VerInfo;			//�ն˰汾��Ϣ
	stTermFlow	        Flow;			//����
	stTermEc			Ec;			//
	stTermEvt			Evt;			//
	
	stChagingPortSwitch   ChagingPortSwitch;
	stChagingPortParm   ChagingPortParm;

	stMastModeVer  MastModeVer;

	stSlaveMacHi  SlaveMacHi;
	stChargeCalibration ChargeCalibration;
	stChargeStatus chargeStatus;
	stChargeCriticalValue ChargeCriticalValue;
	stDeviceFaultStatus DeviceFaultStatus;
#endif
}CIRCUIT_SYSTEM_INFO_STRU;  //Ѳ�����豸ϵͳ��Ϣ

typedef struct
{
	CIRCUIT_SYSTEM_INFO_STRU parm;
	u8 enable_flag;
}NV_CHARGE_SYSTEM_INFO;


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
}__attribute__((packed)) DATA_AFN0C_F2;//F2���ն����ڲ�ѯ


typedef struct
{
    u32 DA[2];                //pn
    u32 DT[2];                //fn
    u32 Dayvalue;
    u32 Monthvalue;    
}__attribute__((packed))DATA_AFN0C_F10;//F10��ͨ������







#pragma pack()
#endif

#endif
