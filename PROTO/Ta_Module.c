#include <stdlib.h> 
#include <stdio.h> 
#include <errno.h> 
#include <string.h> 
#include <sys/types.h> 
#include "Sgd_types.h"
#include "GetTerminalESAMData.h"
#include "ListTimer.h"
#include "Ta_Module.h"




#define GET_TA_STATUS_CNT			3//获取状态失败重发次数


int fd_ta=-1;
sFormat_TA_Status TaStatus;




static void TaModuleCallBack(void * data)
{		
	int i;

	memset((u8 *)&TaStatus,0,sizeof(TaStatus));
	for(i=0;i<GET_TA_STATUS_CNT;i++)
	{
		if(0==GetCTModuleStatus((u8 *)&TaStatus.FormatTag,&TaStatus.Phase_A_ststus,TaStatus.Mac))
		{
			printf("[ta_status]:\n");
			printhexdata(&TaStatus, sizeof(TaStatus));
			break;
		}
		printf("get TA status failled,restart=%d",i);
	}
}


void TaModuleInit(void)
{	
	add_timer(2, 10, TaModuleCallBack, NULL, NULL);
}

sFormat_TA_Status GetTaStatus(void)
{
	return TaStatus;
}


