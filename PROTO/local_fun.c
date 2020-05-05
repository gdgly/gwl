#include <stdio.h>
#include <arpa/inet.h>
#include "para.h"
#include "file.h"
#include "local_fun.h"


void LocalSetTermIpInfo(void *data)
{
	char *pTmp;
	struct in_addr InData;
	sTermSysInfo *para = GetCiSysInfo();
	
	
	if(para->CiIpAndPort.termip32u)
	{
		printf("[set_sys_ip]%d:%d:%d:%d\n",para->CiIpAndPort.termip[0],
										   para->CiIpAndPort.termip[1],
										   para->CiIpAndPort.termip[2],
										   para->CiIpAndPort.termip[3]);
		pTmp = NULL;
		InData.s_addr = para->CiIpAndPort.termip32u;
		pTmp = inet_ntoa(InData);
		printf("%s\n",pTmp);
		if(pTmp)
		{
			if(saveConfPara(TERM_IP_CONF_FILE,"address",pTmp,' '))
			{
				return;
			}
		}
	}
	if(para->CiIpAndPort.netmask32u)
	{
		printf("[set_sys_mask]%d:%d:%d:%d\n",para->CiIpAndPort.netmask[0],
										     para->CiIpAndPort.netmask[1],
										     para->CiIpAndPort.netmask[2],
										     para->CiIpAndPort.netmask[3]);
		pTmp = NULL;
		InData.s_addr = para->CiIpAndPort.netmask32u;
		pTmp = inet_ntoa(InData);
		printf("%s",pTmp);
		if(saveConfPara(TERM_IP_CONF_FILE,"netmask",pTmp,' '))
		{
			return;
		}
	}
	system("sync");
	system("/etc/init.d/S40network restart");	
}





















































