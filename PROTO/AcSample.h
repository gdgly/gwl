#ifndef __ACSAMPLE_H__
#define __ACSAMPLE_H__

#include "Sgd_types.h"




typedef struct
{	//���ֽ���ǰ�����ֽ��ں�
	float P;			//���й�����  (��λkW) ����
	float Pa;			//A���й����� (��λkW)
	float Pb;			//B���й����� (��λkW)
	float Pc;			//C���й����� (��λkW)
	float Q;			//���޹�����  (��λkVar)
	float Qa;			//A���޹����� (��λkVar)
	float Qb;			//B���޹����� (��λkVar)
	float Qc;			//C���޹����� (��λkVar)
	float Factor;		//��ǰ�ܹ�������  (��λ%)
	float FactU;		//��ǰA�๦������ (��λ%)
	float FactV;		//��ǰB�๦������ (��λ%)
	float FactW;		//��ǰC�๦������ (��λ%)
	float U[3];			//A���ѹ	   (��λV)	//B���ѹ	   (��λV) //C���ѹ	   (��λV)
	float I[3];			//A�����	   (��λA)	//B�����	   (��λA) //C�����	   (��λA)
	float IT;			//�������	   (��λA)
	float St;			//�������ڹ���
	float Sa;			//A�����ڹ���
	float Sb;			//B�����ڹ���
	float Sc;			//C�����ڹ���
	float PhsregA;		//A�����	   (��λ��)
	float PhsregB;		//B�����	   (��λ��)
	float PhsregC;		//C�����	   (��λ��)
	float PH_AtoB;		//UA��UB�ĽǶ�	   (��λ��)
	float PH_AtoC;		//UA��UC�ĽǶ�	   (��λ��)
	float PH_BtoC;		//UA��UC�ĽǶ�	   (��λ��)
	float Freq;		//��Ƶ��
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
	GET_JC_DATA = 0,	//��ȡ������������
						//����չ
}p645_tasktype;			//����ö������

int com_645_2007_SendFrame(p645_tasktype tasktype645,void *taskpara);							//��������
int com_645_2007_recv(com_645_2007_CONNECT_TERM *TERM, char *rcv_buf,int data_len);				//��������
int Analyze_protocol_645_2007(u8 *buf, u32 Len, u8 *data, u8 *data_len);	//����645Э��

void BackStage_645_2007(u8 *data,int datalen,int isfirst,int islast);


extern com_645_2007_CONNECT_TERM com_645_2007_HOST;										

#endif
