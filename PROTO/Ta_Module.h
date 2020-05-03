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



//645协议--数据域标识
const uint8_t TAG_FILE_TRAMSMIT[] = {0x0F, 0x00, 0x00, 0x01};  		//文件传输指令          
const uint8_t TAG_PROGRAM_SWITCH[] = {0x0F, 0x00, 0x00, 0x02};  		//程序切换            
const uint8_t TAG_ID_AUTHEN_FIRST[] = {0x04, 0x00, 0x0F, 0x40}; 	 	//身份认证第一帧下行  
const uint8_t TAG_ID_AUTHEN_SECOND[] = {0x04, 0x00, 0x0F, 0x41}; 	 	//身份认证第二帧下行  
const uint8_t TAG_GET_SECURITY_INFO[] = {0x04, 0x00, 0x0F, 0x42}; 	//获取TA专用模块安全信
const uint8_t TAG_KEY_UPDATE[] = {0x04, 0x00, 0x0F, 0x43};  			//密钥更新              
const uint8_t TAG_GET_BAUDRATE[] = {0x04, 0x00, 0x0F, 0x44};  		//获取波特率特征字下行帧
const uint8_t TAG_SET_BAUDRATE[] = {0x04, 0x00, 0x0F, 0x45};  		//设置波特率特征字下行帧
const uint8_t TAG_GET_TA_TRANSFORMER[] = {0x04, 0x00, 0x0F, 0x46};  	//获取TA变比参数下行
const uint8_t TAG_SET_TA_TRANSFORMER[] = {0x04, 0x00, 0x0F, 0x47};  	//设置TA变比参数下行
const uint8_t TAG_GET_TA_STATUS[] = {0x07, 0x00, 0x00, 0x00};  		//获取TA专用模块状态下行
const uint8_t TAG_GET_TA_WORKING[] = {0x07, 0x00, 0x00, 0x01};  		//获取TA专用模块工况信
const uint8_t TAG_GET_REAL_DATA[] = {0x07, 0x00, 0x00, 0x03};  		//获取TA专用模块实时测量
const uint8_t TAG_GET_GUIDE_PARAM[] = {0x07, 0x00, 0x00, 0x04};  		//获取TA专用模块调教参

#define MAC_SIZE 4


enum  DIRECTION
{
    DOWN_DIRECTION = 0,
    UP_DIRECTION = 1
};


typedef struct TAG_FormatCode
{
    u8  IVcode:1;   //1时， ESAM 使用外部 IV；“0”时，ESAM 使用内部 IV。
    u8  DirFlag:1;  //“1” 时， 数据由主站下发；当下传标识为“0” 时， 数据由终端下发
    u8  EncryFlag:1;//“1”时， 645 帧中数据区的 DATA 为密文；当密文标识为“0”时， 645 帧中数据区的 DATA 为明文
    u8  Reserve:5;
}__attribute__((packed))sTAG_FormatCode;


//窃电情况定义： 00H， CT 处在正常模式； 01H， CT 短路； 02H， CT 开路
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
    u8  Impedance_Mode[4];//XXXXXX.XX Ω
    u8  Impedance_Angle[2];//XXX.X °
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
    u8 Temperature[2];//XXX.X ℃
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
    u16 oherErrFlag:1;//其他错误
    u16 noRequestFlag:1;//无 请 求数据
    u16 needAuthenFlag:1;//未 进 行身 份 认证
    u16 MacErrFlag:1;   //MAC 计算 或 验证错误
    u16 authenFailFlag:1;//身 份 认证失败
    u16 validDataFlag:1;//数 据 无效
    u16 KeyUpdateFailFlag:1;//密 钥 更新失败
    u16 reserve:1;
    u16 flowFailFlag:8;//流程失败信息标识 双向身份认证： 参考“6.2.2 F101： 获取 TA 专用模块安全信息”。其他流程：保留，直接写成 0x00；
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
