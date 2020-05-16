#include <stdlib.h> 
#include <stdio.h> 
#include <errno.h> 
#include <string.h> 
#include <sys/types.h> 
#include "GetTerminalESAMData.h"
#include "ListTimer.h"
#include "MESamBase.h"
#include "GetTerminalESAMData.h"
#include "Ta_Module.h"




#define GET_TA_STATUS_CNT			3//

sFormat_TA_Status TaStatus;


static int GetInCurrentBypassStatus(u8 *status)
{
	u8 Format = 0;
	u8 TaRealData[256];
	int i,TaRdLen,ret = 0;
	u32 Idata;

	*status = 0;
	memset(TaRealData,0,sizeof(TaRealData));
	ret = GetTAModuleRealTimeMeasureData(&Format,TaRealData,&TaRdLen);
	if(ret)
	{
		printf("Get ta real I data failled\n");
		return ret;
	}
	for(i=0;i<3;i++)
	{
		memcpy((u8*)&Idata,&TaRealData[TA_CURENT_OFFSET+3*i],3);
		if(0==Idata) *status |= (1<<i);
		else *status &=~ (1<<i);
	}
	return ret;	
}


static void TaModuleCallBack(void * data)
{		
	int i,j;
	u8 tmp;

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
	i = 0;
	while(i<GET_TA_STATUS_CNT)
	{
		if(0==GetInCurrentBypassStatus(&tmp))
		{
			u8 *pTmp;

			j=0;
			tmp &= 0x07;
			pTmp = &TaStatus.Phase_A_ststus;

			while(j<3)
			{
				if(tmp&0x01)
				{
					pTmp[j] |= ((tmp<<3)&CIRCUIT_FONT_SHORTCUT);
				}
				else
				{
					pTmp[j] &=~ ((tmp<<3)&CIRCUIT_FONT_SHORTCUT);
				}
				tmp >>= 1;
				j++;
			}
			break;
		}
		i++;
	}
}


int TaModuleInit(void)
{	
	int res = 0;
	int retry = 3;
	//TESAM
	res = MEsamDevOpen();
	if(res != 0)
	{
		printf("MEsamDevOpen failed\n");
		return -1;
	}
	//TA UART
	res = OpenTaUartPort("/dev/ttyS9");
	if(res == -1)
	{
		printf("OpenTaUartPort error\n");
		return -1;
	}
	
	res = ComTaUart_Set(2400, 0, 8, 1, 'e');
	if(res == -1)
	{
		printf("ComTaUart_Set error\n");
		return -1;
	}

	
	while(retry--)
	{
		res = TerminalCtAuthenticate();
		if(res < 0)
		{
			if(retry)
			{
				usleep(100*1000);
				continue;
			}
		}
		else
		{
			break;
		}
	}
	if(res == -1)
	{
		printf("TerminalCtAuthenticate failed\n");
		return -1;
	}
	
	add_timer(2, 10, TaModuleCallBack, NULL, NULL);
}
void AppCloseTaPort(void)
{
	CloseTaUartPort();
}
sFormat_TA_Status GetTaStatus(void)
{
	return TaStatus;
}







