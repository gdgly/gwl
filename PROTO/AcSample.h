#ifndef __ACSAMPLE_H__
#define __ACSAMPLE_H__

#include "Sgd_types.h"




typedef struct
{	//高字节在前，低字节在后
	float P;			//总有功功率  (单位kW) 换算
	float Pa;			//A相有功功率 (单位kW)
	float Pb;			//B相有功功率 (单位kW)
	float Pc;			//C相有功功率 (单位kW)
	float Q;			//总无功功率  (单位kVar)
	float Qa;			//A相无功功率 (单位kVar)
	float Qb;			//B相无功功率 (单位kVar)
	float Qc;			//C相无功功率 (单位kVar)
	float Factor;		//当前总功率因数  (单位%)
	float FactU;		//当前A相功率因数 (单位%)
	float FactV;		//当前B相功率因数 (单位%)
	float FactW;		//当前C相功率因数 (单位%)
	float U[3];			//A相电压	   (单位V)	//B相电压	   (单位V) //C相电压	   (单位V)
	float I[3];			//A相电流	   (单位A)	//B相电流	   (单位A) //C相电流	   (单位A)
	float IT;			//零序电流	   (单位A)
	float St;			//合相视在功率
	float Sa;			//A相视在功率
	float Sb;			//B相视在功率
	float Sc;			//C相视在功率
	float PhsregA;		//A相相角	   (单位度)
	float PhsregB;		//B相相角	   (单位度)
	float PhsregC;		//C相相角	   (单位度)
	float PH_AtoB;		//UA和UB的角度	   (单位度)
	float PH_AtoC;		//UA和UC的角度	   (单位度)
	float PH_BtoC;		//UA和UC的角度	   (单位度)
	float Freq;		//线频率
}__attribute__((packed))tstMCurDataStruct;








typedef struct
{
    int     FD;
    char    *IP;
    int     PORT;
    int     CONNECT_FLAG;
}com_645_2007_CONNECT_TERM;

typedef enum
{
	GET_JC_DATA = 0,	//获取交采数据任务
						//可扩展
}p645_tasktype;			//任务枚举类型

int com_645_2007_SendFrame(p645_tasktype tasktype645,void *taskpara);							//发送任务
int com_645_2007_recv(com_645_2007_CONNECT_TERM *TERM, char *rcv_buf,int data_len);				//接收数据
int Analyze_protocol_645_2007(u8 *buf, u32 Len, u8 *data, u8 *data_len);	//解析645协议

void BackStage_645_2007(u8 *data,int datalen,int isfirst,int islast);


extern com_645_2007_CONNECT_TERM com_645_2007_HOST;										

#endif
