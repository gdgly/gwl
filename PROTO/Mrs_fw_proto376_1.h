#ifndef __MRS_FW_PROTO376_1_H_
#define __MRS_FW_PROTO376_1_H_
#include "Sgd_types.h"


#define FRAME_MAX_SIZE 512

#define	FRAME_START_13761	0x68
#define	FRAME_END_13761		0x16

#define	PROTOCOL_13761_BUF_SIZE			1200
#define	PROTOCOL_13761_CTR_SEQ_LEN		8		//控制域+地址域+AFN+SEQ
#define	PROTOCOL_13761_HEAD_LEN			6
#define	PROTOCOL_13761_TAIL_LEN			2
#define	FRAME_13761_DIR_DOWN			0		//主站下行报文
#define	FRAME_13761_DIR_UP				1		//终端上行报文
#define	FRAME_13761_PRM_START_STATION	1		//帧报文来自启动站
#define	FRAME_13761_PRM_ACK_STATION		0		//帧报文来自从动站

#define	PROTOCOL_13761_MAX__DATA_LEN	PROTOCOL_13761_BUF_SIZE-PROTOCOL_13761_CTR_SEQ_LEN
#define	PROTOCOL_13761_MIN_FRAME_LEN	20		//帧最小长度

//响应链路控制功能码
#define LINK_CTRL_UP_CONFIRM			0
#define LINK_CTRL_UP_RES_USERDATA		8
#define LINK_CTRL_UP_RES_NODATA			9
#define LINK_CTRL_UP_RES_LINKSTAT		11

//规约标识
#define PROTOCOL_13761_VALID			2

//应用层功能码    //
#define AFN_00_CONFIRM				0x00	//确认/否认
#define AFN_01_RESET				0x01	//复位命令
#define AFN_02_LINK_TEST			0x02	//链路接口检测
#define AFN_03_RELAYSTATION			0x03	//中继站命令
#define AFN_04_SETUP_PARM			0x04	//设置参数
#define AFN_05_CONTROL				0x05	//控制命令
#define AFN_06_AUTHENTICATION		0x06	//身份认证及密钥协商
#define AFN_08_LEVEL_CHECK			0x08	//请求被级联终端主动上报
#define AFN_09_TMCONFIG             0x09	//请求终端配置及信息
#define AFN_0A_INQUIRY_PARM			0x0A	//查询参数
#define AFN_0B_TASK_DATA			0x0B	//请求任务数据
#define AFN_0C_REALTIME_DATA		0x0C	//请求1类数据(实时数据)
#define AFN_0D_HISTORY_DATA			0x0D	//请求2类数据(历史数据)
#define AFN_0E_EVENT_DATA			0x0E	//请求3类数据(事件数据)
#define AFN_0F_FILE_TRANSFER		0x0F	//文件传输
#define AFN_10_DATA_TRANSMIT		0x10	//数据转发

#define	MAX_13761_DI_NUM		10

//应答类型
#define	ACK_ALL_OK		1
#define	ACK_ALL_ERR		2
#define	ACK_PART		3

#define	ACK_ERR_OK		0
#define	ACK_ERR_OT		1
#define	ACK_ERR_ADDR	2

#define	NULL_BCD		0xEE


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


typedef enum{
	ERR_COMMTIMEOUT = 1,	// 通信超时
	ERR_INVALID_DATAUNIT,	// 无效数据单元
	ERR_LEN,				// 长度错
	ERR_VERIFY,				// 校验错误
	ERR_NO_INFO,			// 信息类不存在
	ERR_FORMAT,				// 格式错误
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
	uint8_t code : 4;				//功能码
	uint8_t rsv : 1;				//帧计数有效位
	uint8_t acd : 1;				//请求访问位
	uint8_t prm : 1;				//启动标志
	uint8_t dir : 1;				//传输方向
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

typedef union unFrame13761M_Ma_unType
{
	stFrame13761MaSType ma_s;
	uint8_t value;
}unFrame13761M_Ma_unType;	

#pragma pack(1)//取消字节对齐，按1字节对齐
typedef struct stFrame13761AddrType
{
	uint16_t districtCode;
	uint16_t termAddr;
	unFrame13761M_Ma_unType mainStn_TermGrp_addr;
}stFrame13761AddrType;
#pragma pack()
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

//帧头
#pragma pack(1)//取消字节对齐，按1字节对齐
typedef struct stFrame13761Head
{
	uint8_t header1;					// 1
	stFrame13761Len len;				// 2
	stFrame13761Len len1;				// 2
	uint8_t header2;					// 1
	stFrame13761CtrlType control;		// 1
	stFrame13761AddrType addr;			// 5
	stFrame13761UserDataType userdata;	// 1+1+data 
}stFrame13761Head;
#pragma pack()



//帧尾
typedef struct stFrame13761Tail
{
	uint8_t check_sum;
	uint8_t tail;
}stFrame13761Tail;

//数据格式


//A.16	数据格式16
//秒分时日
typedef struct StDataFormat16
{
	uint8_t second; //BCD
	uint8_t minute; //BCD
	uint8_t hour;   //BCD
	uint8_t day;    //BCD
}StDataFormat16;


//DA、DT
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

typedef struct AppAuxEC               	//事件计数器EC（上行）
{
	uint8_t AppAuxEC1;           		//重要事件计数器EC1
	uint8_t AppAuxEC2;          	 	//一般事件计数器EC2
}AppAuxEC;

typedef struct AppAuxPwd				//附加信息域
{
	uint8_t  appAuxPw[16];				//消息认证码字段PW（下行）
}AppAuxPwd;

typedef struct AppAuxTp	                //附加信息时间标签Tp
{
	uint8_t       AppAuxTpPFC;      	//启动帧帧序号计数器PFC
	StDataFormat16 AppAuxTpDate;		//启动帧发送时标（秒分时日）
	uint8_t       AppAuxTpDelay;   		//允许发送传输延时时间,分钟,0表示不做超时判断
}AppAuxTp;

typedef struct AppAuxUp					//附加信息域
{
	AppAuxEC appAuxEC;					//事件计数器EC（上行）
	AppAuxTp appAuxTp;					//附加信息时间标签Tp
}AppAuxUp;

typedef struct AppAuxDown
{
	AppAuxPwd appAuxPwd;
	AppAuxTp  appAuxTp;					//附加信息时间标签Tp
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
#pragma pack(1) 


#pragma pack() 



	


//BCD码
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

//F3终端日历时钟
typedef struct stTermTime
{
	stTime time;//日历时钟
}stTermTime;





/*------------------------------------------------
F9	终端事件记录配置设置	p0 
事件记录有效标志位	BS64	8
事件重要性等级标志位	BS64	8
------------------------------------------------*/
typedef struct  TermEventRecordSet
{
	u8 CS_F9_EventRecordValidFlag[8];
	u8 CS_F9_EventRatingFlag[8];
}TermEventRecordSet;











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






/*****************************参数设置*******************************/

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

//F1终端上行通信参数
typedef struct stTermUpCommuPara
{
	uint8_t RTS;
	uint8_t delay_time;
	U_OVERTIME u_over_time_times;
	uint8_t confirm;
	uint8_t heart_beats;//min
}__attribute__((packed))stTermUpCommuPara;



//F2终端地理位置
typedef struct
{
    u16 longitude_S_BFW:4,		//十分位		秒
        longitude_S_SFW:4,		//百分位
        longitude_S_GW:4,		//十位
       	longitude_S_SW:4;		//个位
       	
    u8  longitude_B_GW:4,		//十位		分
        longitude_B_SW:4;		//个位
        
    u16 longitude_D_GW:4,		//十位		度
        longitude_D_SW:4,		//个位
        longitude_D_BW:4,		//保留
        longitude_D_BLW:4;		//百位

	u16 latitude_S_SFW:4,		//十分位		秒
        latitude_S_BFW:4,		//百分位
        latitude_S_GW:4,		//十位
       	latitude_S_SW:4;		//个位
       	
    u8  latitude_B_GW:4,		//十位		分
        latitude_B_SW:4;		//个位
        
    u16 latitude_D_GW:4,		//十位		度
        latitude_D_SW:4,		//个位
        latitude_D_BW:4,		//保留
        latitude_D_BLW:4;		//百位
	
	//u8 longitude[5];				//经度,秒，分，度(F的定义：F=0， 表示东经或北纬；F=l,表示西经或南纬。)
	//u8 latitude[5];				//纬度,秒，分，度(F的定义：F=0， 表示东经或北纬；F=l,表示西经或南纬。)
}__attribute__((packed))stTermLocation;


//F3主站IP地址和端口
typedef struct
{
	U_IP u_IP_main;
	uint16_t Port_main;
	
	U_IP u_IP_bak;
	uint16_t Port_bak;
	
	uint8_t APN[16];	
}stMainStationIPPort;


//AFN04_F7;//F7:终端IP地址和端口
typedef struct
{
    u8   DA[2];              //pn
    u8   DT[2];              //fn 
    //终端IP
    union
    {
        u8   termip[4];
        u32  termip32u;
    };                    

	//子网掩码
    union
    {
        u8   netmask[4];
        u32  netmask32u;
    };                   	

	//网关
    union
    {
        u8   gateway[4];
        u32  gateway32u;
    };                   
    u8   proxytype;             //代理类型  //a)代理类型：数值范围0～3，依次表示：不使用代理、httpconnect代理、socks4代理、socks5代理。
    union
    {
        u8   proxyserverip[4];
        u32  proxyserverip32u;
    };             

	//代理服务器IP地址
    u16  proxyserverport;         			//代理服务器端口
    u8   proxyserverconnectway;  			//代理服务器连接方式 //代理服务器连接方式：数值范围0～1，依次表示：无需验证、需要用户名/密码
    u8 user_len;//用户名长度
    u8 user[20];//用户名
    u8 pw_len;//密码长度
    u8 pw[20];//密码;ascll
    u16 listen_port;//侦听端口
}__attribute__((packed)) DATA_AFN04_F7;//F7:终端IP地址和端口

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
}__attribute__((packed)) buf_AFN04_F7;//F7：终端IP地址和端口





//F89终端逻辑地址
typedef struct
{
	u16 districtCode;							//行政区码
	u16 termAddr;								//终端地址
	//u8 a3;
}MODLE_ADDR_STRU;


//F153电流回路使能参数
typedef struct
{
    unsigned char   phase_a_curr;			//电流回路使能参数A
    unsigned char   phase_b_curr;			//电流回路使能参数B
    unsigned char   phase_c_curr;			//电流回路使能参数C
    unsigned char   mac[4];				    //mac
}__attribute__((packed)) current_circuit_param_stru;

//F154TA变比参数
typedef struct
{
    unsigned char format;
	unsigned short phase_a_trans;	//A相电流互感器倍率
	unsigned short phase_b_trans;	//B相电流互感器倍率
	unsigned short phase_c_trans;	//C相电流互感器倍率
	unsigned char mac[4];		    //mac
}__attribute__((packed)) current_transformer_rate_stru;	



//巡检仪设备系统信息
typedef struct
{
	stTermLocation 		Location;				//位置
	stMainStationIPPort MainStationIPPort;		//主站ip/port	
	MODLE_ADDR_STRU 	usModleAddr;			//终端逻辑地址
	stTermUpCommuPara  	UpCommuPara; 			//上行通信参数
}CIRCUIT_SYSTEM_INFO_STRU;  


typedef struct
{
	CIRCUIT_SYSTEM_INFO_STRU parm;
	u8 enable_flag;
}NV_CHARGE_SYSTEM_INFO;














/***************************事件***************************/


//请求重要事件指针
typedef struct
{
    u8 Pm;                //起始指针
    u8 Pn;                //结束指针
}__attribute__((packed))DATA_AFN0E_F1;


//请求一般事件指针
typedef struct
{
    u8 Pm;                //起始指针
    u8 Pn;                //结束指针
}__attribute__((packed))DATA_AFN0E_F2;

//F3按事件代码ERC请求事件
typedef struct
{
    u8 ERC;                //事件代码ERC
    u8 Num;                //请求最近发生事件的数量
}__attribute__((packed))DATA_AFN0E_F3;

typedef struct
{
	//u8 DA[2];				//pn
	//u8 DT[2];				//fn
	u8 EventNum;	//事件数量
}__attribute__((packed))EventReqReply;


//AFN0E应答数据体头部-请求事件
typedef struct  
{
	//u8 DA[2];				//pn
	//u8 DT[2];				//fn
	u8 EC1;
	u8 EC2;
	u8 Pm;
	u8 Pn;
}__attribute__((packed))EventsBlockHead;






//ERC1：数据初始化和版本变更事件
typedef struct
{
    u8 ERCcode;          //事件代码ERC
    u8 Le;               //事件长度
    SJGS_15 time;        //变更时间
    u8  CSH:1,           //初始化
        VER:1;           //版本
    u8  VerOld[4];       //变更前版本
    u8  VerNew[4];       //变更后版本
}__attribute__((packed))stru_sjjl_ERC1;

typedef struct
{
    u8 DA[2];            //pn
    u8 DT[2];            //fn
    u8 ECcount;          //事件计数
    stru_sjjl_ERC1 sjjlerc1[15];      
}__attribute__((packed))DATA_AFN0E_F3_ERC1;



//ERC2：参数丢失事件
typedef struct
{
    u8 ERCcode;          //事件代码ERC
    u8 Le;               //事件长度
    SJGS_15 time;        //发生时间
	u8 Flag;			 //事件标志					
}__attribute__((packed))stru_sjjl_ERC2;     


typedef struct
{
    u8 DA[2];            //pn
    u8 DT[2];            //fn
    u8 ECcount;          //事件计数
    stru_sjjl_ERC2 sjjlerc2[15];      
}__attribute__((packed))DATA_AFN0E_F3_ERC2;


//ERC3：参数变更事件
typedef struct
{
    u8 ERCcode;          //事件代码ERC
    u8 Le;               //事件长度
    SJGS_15 time;        //参数更新时间
    u8  MstAddr;         //启动站地址
    //u8  Num;       	 //数据单元标识符个数
	u8 *uniundata;		 //数据单元标识符												//看怎么定义todo
}__attribute__((packed))stru_sjjl_ERC3;

typedef struct
{
    u8 DA[2];            //pn
    u8 DT[2];            //fn
    u8 ECcount;          //事件计数
    stru_sjjl_ERC3 sjjlerc3[15];      
}__attribute__((packed))DATA_AFN0E_F3_ERC3;



//ERC14：终端停/上电事件
typedef struct
{
    u8 ERCcode;          	//事件代码ERC
    u8 Le;               	//事件长度
    SJGS_15 offtime;        //停电时间
    SJGS_15 ontime;         //上电时间     
}__attribute__((packed))stru_sjjl_ERC14;

typedef struct
{
    u8 DA[2];            //pn
    u8 DT[2];            //fn
    u8 ECcount;          //事件计数
    stru_sjjl_ERC14 sjjlerc14[15];
        
}__attribute__((packed))DATA_AFN0E_F3_ERC14;



//ERC20：消息认证错误事件
typedef struct
{
    u8 ERCcode;          	//事件代码ERC
    u8 Le;               	//事件长度
    SJGS_15 time;           //发生时间
    u8 pw[16];				//消息认证码
    u8 MSA;					//启动站地址
}__attribute__((packed))stru_sjjl_ERC20;    

typedef struct
{
    u8 DA[2];            //pn
    u8 DT[2];            //fn
    u8 ECcount;          //事件计数
    stru_sjjl_ERC20 sjjlerc20[15];    
}__attribute__((packed))DATA_AFN0E_F3_ERC20;



//ERC21：终端故障记录
typedef struct
{

	u8 ERCcode; 		 //事件代码ERC
	u8 Le;			     //事件长度
	SJGS_15 time;		 //发生时间
	u8 Code;			 //故障编码
}__attribute__((packed))stru_sjjl_ERC21;

typedef struct
{
    u8 DA[2];            //pn
    u8 DT[2];            //fn
    u8 ECcount;          //事件计数
    stru_sjjl_ERC21 sjjlerc21[15];
        
}__attribute__((packed))DATA_AFN0E_F3_ERC21;



//ERC32终端与主站通信流量超门限事件记录
typedef struct
{
    u8 ERCcode;          //事件代码ERC
    u8 Le;               //事件长度
    SJGS_15 time;        //发生时间
    u8 dyll;             //当月流量
    u8 llmx;             //流量门限
    
}__attribute__((packed))stru_sjjl_ERC32;

typedef struct
{
    u8 DA[2];            //pn
    u8 DT[2];            //fn
    u8 ECcount;          //事件计数
    stru_sjjl_ERC32 sjjlerc32[15];        
}__attribute__((packed))DATA_AFN0E_F3_ERC32;



//ERC34：CT 异常事件记录
typedef struct
{
	u8 ERCcode;             //事件代码ERC
    u8 Le;                  //事件长度
	SJGS_15 time;           //发生时间
	u8 mp[2];			    //测量点号
	u8 flag;				//异常标识
}__attribute__((packed))stru_sjjl_ERC34;

typedef struct
{
    u8 DA[2];            //pn
    u8 DT[2];            //fn
    u8 ECcount;          //事件计数
    stru_sjjl_ERC34 sjjlerc34[15];        
}__attribute__((packed))DATA_AFN0E_F3_ERC34;



//ERC40：磁场异常事件记录
typedef struct
{
	u8 ERCcode;             //事件代码ERC
    u8 Le;                  //事件长度
	SJGS_15 time;			//发生时间
	u8 mp[2];			    //测量点号
	u8 DeviceType;			//设备类型
	u8 DeviceAddr[6];		//设备地址
	u8 AbnormalType;		//磁场异常类型

}__attribute__((packed))stru_sjjl_ERC40;

typedef struct
{
    u8 DA[2];            //pn
    u8 DT[2];            //fn
    u8 ECcount;          //事件计数
    stru_sjjl_ERC40 sjjlerc40[15];        
}__attribute__((packed))DATA_AFN0E_F3_ERC40;


//ERC41：对时事件记录
typedef struct
{
	u8 ERCcode;                     //事件代码ERC
    u8 Le;                          //事件长度
	u8 mp[2];						//测量点号
	SJGS_01 TimeBeforeAdjust;		//校时前时刻
	SJGS_01 TimeAfterAdjust;		//校时后时刻

}__attribute__((packed))stru_sjjl_ERC41;

typedef struct
{
    u8 DA[2];            //pn
    u8 DT[2];            //fn
    u8 ECcount;          //事件计数
    stru_sjjl_ERC41 sjjlerc41[15];        
}__attribute__((packed))DATA_AFN0E_F3_ERC41;

/*************************事件(end)*************************/










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
