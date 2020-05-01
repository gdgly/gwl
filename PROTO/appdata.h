#ifndef __APP_DATA_H_
#define __APP_DATA_H_
#pragma pack(1) 


#if 0

/******************数据格式*******************/
typedef struct
{
    u32 MGW:4,     //秒个位
        MSW:4;     //秒十位
    u32 FGW:4,     //分个位
        FSW:4;     //分十位
    u32 SGW:4,     //时个位
        SSW:4;     //时十位
    u32 RGW:4,     //日个位
        RSW:4;     //日十位
    u32 YGW:4,     //月个位
        YSW:4;     //月十位
    u32 NGW:4,     //年个位
        NSW:4;     //年十位
}__attribute__((packed)) SJGS_01;//数据格式01

typedef struct
{
    u32 SFW:4,    //十分位
        GW:4;     //个位
    u32 SW:4,     //十位
        BW:3,     //百位
        S:1;      
}__attribute__((packed)) SJGS_05;//数据格式05

typedef struct
{
    u32 BFW:4,    //百分位
        SFW:4;    //十分位 
    u32 GW:4,     //个位
        SW:3,     //十位
        S:1;      
}__attribute__((packed)) SJGS_06;//数据格式06

typedef struct
{
    u32 SFW:4,    //十分位
        GW:4;     //个位
    u32 SW:4,     //十位
        BW:4;     //百位     
}__attribute__((packed)) SJGS_07;//数据格式07

typedef struct
{
    u32 GW:4,    //个位
          SW:4;     //十位
    u32 BW:4,     //百位
          QW:4;     //千位     
}__attribute__((packed)) SJGS_08;//数据格式08

typedef struct
{
    u32 WFW:4,    //万分位
        QFW:4;    //千分位
    u32 BFW:4,    //百分位
        SFW:4;    //十分位
    u32 GW:4,     //个位
        SW:3,     //十位
        S:1;
}__attribute__((packed)) SJGS_09;//数据格式09

typedef struct
{
    u32 GW:4,     //个位
        SW:4;     //十位
    u32 BW:4,     //百位
        QW:4;     //千位 
    u32 WW:4,     //万位
        SWW:4;    //十万位
}__attribute__((packed)) SJGS_10;//数据格式10

typedef struct
{
    u32 BFW:4,    //百分位
        SFW:4;    //十分位
    u32 GW:4,     //个位
        SW:4;     //十位
    u32 BW:4,     //百位
        QW:4;     //千位
    u32 WW:4,     //万位
        SWW:4;    //十万位      
}__attribute__((packed)) SJGS_11;//数据格式11

typedef struct
{
    u32 GW:4,     //个位
        SW:4;     //十位
    u32 BW:4,     //百位
        QW:4;     //千位
    u32 WW:4,     //万位
        SWW:4;    //十万位
    u32 BWW:4,    //百万位
        QWW:4;    //千万位
    u32 YW:4,     //亿位
        SYW:4;    //十亿位
    u32 BYW:4,    //百亿位
        QYW:4;    //千亿位      
}__attribute__((packed)) SJGS_12;//数据格式12

typedef struct
{
    u32 WFW:4,    //万分位
        QFW:4;    //千分位
    u32 BFW:4,    //百分位
        SFW:4;    //十分位
    u32 GW:4,     //个位
        SW:4;     //十位
    u32 BW:4,     //百位
        QW:4;     //千位
    u32 WW:4,     //万位
        SWW:4;    //十万位      
}__attribute__((packed)) SJGS_14;//数据格式14

typedef struct
{
    u32 FGW:4,    //分个位
        FSW:4;    //分十位
    u32 SGW:4,    //时个位
        SSW:4;    //时十位
    u32 RGW:4,    //日个位
        RSW:4;    //日十位
    u32 YGW:4,    //月个位
        YSW:4;    //月十位 
    u32 NGW:4,    //年个位
        NSW:4;    //年十位
}__attribute__((packed)) SJGS_15;//数据格式15

typedef struct
{
    u32 FGW:4,    //分个位
        FSW:4;    //分十位
    u32 SGW:4,    //时个位
        SSW:4;    //时十位
    u32 RGW:4,    //日个位
        RSW:4;    //日十位
    u32 YGW:4,    //月个位
        YSW:4;    //月十位 
}__attribute__((packed)) SJGS_17;//数据格式17

typedef struct 
{
	u32 RGW:4,    //日个位
        RSW:4;	//日十位
	u32 YGW:4,	//月个位
	    YSW:4;	//月十位
	u32 NGW:4,	//年个位
	    NSW:4;	//年十位
}__attribute__((packed)) SJGS_20;//数据格式20

typedef struct
{
    u32 YGW:4,    //月个位
        YSW:4;    //月十位 
    u32 NGW:4,    //年个位
        NSW:4;    //年十位
}__attribute__((packed)) SJGS_21;//数据格式21

typedef struct
{
    u32 SFW:4,    //十分位
        GW:4;     //个位
}__attribute__((packed)) SJGS_22;//数据格式22

typedef struct
{
    u32 WFW:4,    //万分位
        QFW:4;    //千分位
    u32 BFW:4,    //百分位
        SFW:4;    //十分位
    u32 GW:4,     //个位
        SW:4;     //十位
}__attribute__((packed)) SJGS_23;//数据格式23

typedef struct
{
    u32 QFW:4,    //千分位
        BFW:4;    //百分位
    u32 SFW:4,    //十分位
        GW:4;     //个位
    u32 SW:4,     //十位
        BW:3,     //百位 
        S:1;
}__attribute__((packed)) SJGS_25;//数据格式25

typedef struct
{
    u32 GW:4,     //个位
        SW:4;     //十位
    u32 BW:4,     //百位
        QW:4;     //千位 
    u32 WW:4,     //万位
        SWW:4;    //十万位
    u32 BWW:4,    //百万位
        QWW:4;    //千万位
}__attribute__((packed)) SJGS_27;//数据格式27


typedef struct
{
	u32 unit;		//单位代号
	u32 BFW:4,    //百分位
        SFW:4;    //十分位
    u32 GW:4,     //个位
        SW:4;     //十位
    u32 BW:4,     //百位
        QW:4;     //千位 
    u32 WW:4,     //万位
        SWW:4;    //十万位  
}__attribute__((packed)) SJGS_29;//数据格式29

typedef struct
{
	u32 unit;		//单位代号
	u32 WFW:4,    //万分位
        QFW:4;    //千分位  
	u32 BFW:4,    //百分位
        SFW:4;    //十分位
    u32 GW:4,     //个位
        SW:4;     //十位
    u32 BW:4,     //百位
        QW:4;     //千位 
}__attribute__((packed)) SJGS_30;//数据格式30

typedef struct
{
	u32 BFW:4,    //百分位
        SFW:4;    //十分位
    u32 GW:4,     //个位
        SW:4;     //十位
    u32 BW:4,     //百位
        QW:4;     //千位 
}__attribute__((packed)) SJGS_31;//数据格式31

typedef struct
{
    u32 MGW:4,     //秒个位
        MSW:4;     //秒十位
    u32 FGW:4,     //分个位
        FSW:4;     //分十位
    u32 SGW:4,     //时个位
        SSW:4;     //时十位
    u32 RGW:4,     //日个位
        RSW:4;     //日十位
    u32 YGW:4,     //月个位
        YSW:4;     //月十位
    u32 NGW:4,     //年个位
        NSW:4;     //年十位
	u32 NBW:4,     //年百位
	    NQW:4;     //年千位
}__attribute__((packed)) SJGS_32;//数据格式32


typedef struct
{
    u32 switchstate    :2,     //阀门状态
        BatteryVoltage :1,     //电池电压 0：正常1：欠压
        reservebit     :5;
	u32 reservebyte;           //保留
}__attribute__((packed)) SJGS_ST;//数据格式ST

/*******************END************************/



typedef struct
{
	u16 districtCode;							//行政区码
	u16 termAddr;								//终端地址
	u8 a3;
}MODLE_ADDR_STRU;


//F2终端地理位置
typedef struct
{
    u16 longitude_S_BFW:4,//十分位		秒
        longitude_S_SFW:4,//百分位
        longitude_S_GW:4,//十位
       	longitude_S_SW:4;//个位
       	
    u8  longitude_B_GW:4,//十位		分
        longitude_B_SW:4;//个位
        
    u16 longitude_D_GW:4,//十位		度
        longitude_D_SW:4,//个位
        longitude_D_BW:4,//保留
        longitude_D_BLW:4;//百位

	u16 latitude_S_SFW:4,//十分位		秒
        latitude_S_BFW:4,//百分位
        latitude_S_GW:4,//十位
       	latitude_S_SW:4;//个位
       	
    u8  latitude_B_GW:4,//十位		分
        latitude_B_SW:4;//个位
        
    u16 latitude_D_GW:4,//十位		度
        latitude_D_SW:4,//个位
        latitude_D_BW:4,//保留
        latitude_D_BLW:4;//百位
	
	//u8 longitude[5];		//经度,秒，分，度(F的定义：F=0， 表示东经或北纬；F=l,表示西经或南纬。)
	//u8 latitude[5];			//纬度,秒，分，度(F的定义：F=0， 表示东经或北纬；F=l,表示西经或南纬。)
}__attribute__((packed))stTermLocation;

typedef struct
{
	stTermLocation 		Location;				//位置
	stMainStationIPPort MainStationIPPort;		//主站ip/port	
	MODLE_ADDR_STRU 	usModleAddr;			//终端地址
	stTermUpCommuPara  	UpCommuPara; 			//上行通信参数
#if 0
	stTermVerInfo		VerInfo;			//终端版本信息
	stTermFlow	        Flow;			//流量
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
}CIRCUIT_SYSTEM_INFO_STRU;  //巡检仪设备系统信息

typedef struct
{
	CIRCUIT_SYSTEM_INFO_STRU parm;
	u8 enable_flag;
}NV_CHARGE_SYSTEM_INFO;


// 时间日期格式定义
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
typedef struct //低位在先
{
    u32   DA[2];              //pn
    u32   DT[2];              //fn 
    SJGS_01  termCurDate;
}__attribute__((packed)) DATA_AFN0C_F2;//F2：终端日期查询


typedef struct
{
    u32 DA[2];                //pn
    u32 DT[2];                //fn
    u32 Dayvalue;
    u32 Monthvalue;    
}__attribute__((packed))DATA_AFN0C_F10;//F10：通信流量







#pragma pack()
#endif

#endif
