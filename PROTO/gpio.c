#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "gpio.h"



/**************************************
****gpio初始化
****num:IO口号
****direction：方向 0-input 1-output
××××返回：0 -成功 -1 -失败
***************************************/
int gpio_init(int num, int direction)
{
	FILE *fp = NULL;
	char io[]={'A','B','C','D','E','F','G','H','I'};
	char str[256];
	
	if ((fp = fopen("/sys/class/gpio/export","w")) == NULL)
	{
		printf("Cannot open export file.\n");
		return -1;
	}
	fprintf(fp, "%d", num);
	fclose(fp);
	
	sprintf(str,"/sys/class/gpio/gpio%d/direction", num);
	if ((fp = fopen(str, "rb+")) == NULL)
	{
		printf("Cannot open GPIO%C%d direction file.\n", io[num/0x20], num%(0x20));
		return -1;
	}
	if (0 == direction)
	{
		fprintf(fp, "out");
	}
	else
	{
		fprintf(fp, "in");
	}
	fclose(fp);
	
	
	return 0;
}


/**************************************
****gpio输出
****num:IO口号
****val：输出值 0-Low 1-Hig
××××返回：0 -成功 -1 -失败
***************************************/
int gpio_output(int num, int val)
{
	FILE *fp = NULL;
	char io[]={'A','B','C','D','E','F','G','H','I'};
	char str[256];
	
	sprintf(str,"/sys/class/gpio/gpio%d/value", num);
	if ((fp = fopen(str,"w")) == NULL)
	{
		printf("Cannot open GPIO%C%d value file.\n", io[num/0x20], num%(0x20));
		return -1;
	}
	if (0 == val)
	{
		fprintf(fp,"%d",0);
	}
	else
	{
		fprintf(fp,"%d",1);
	}
	fclose(fp);
	return 0;
}


/**************************************
****gpio输入
****num:IO口号
××××返回：0,1 输入值， -1 失败
***************************************/
int gpio_input(int num)
{
	FILE *fp = NULL;
	char io[]={'A','B','C','D','E','F','G','H','I'};
	char str[256];
	
	char buffer[10];
	int value;
	
	sprintf(str,"/sys/class/gpio/gpio%d/value", num);
	if ((fp = fopen(str,"rb")) == NULL)
	{
		printf("Cannot open GPIO%C%d value file.\n", io[num/0x20], num%(0x20));
		return -1;
	}
	
	fread(buffer, sizeof(char), sizeof(buffer) - 1, fp);
	value = atoi(buffer);
	//printf("GPIO%C%d value: %d\n",io[num/0x20],num%(0x20),value);
	fclose(fp);
	return value;
}

//*****************************************************************************
//	函数名称: getMagnetStatusValue
//	功能描述: 获取DRV5033状态        
//	参数说明:
//	int	[in]	gpio 端口号
//	返 回 值: 
//	0 : 被磁场干扰
//	1 : 默认值
//	其它为错误码  
//*****************************************************************************
int getMagnetStatusValue(void)
{
	int rs = -1;
	static int bInitFlag = 1;
	
	if(bInitFlag)
	{
		if(gpio_init(MAGNET, GPIO_IN_PUT) < 0)
		{
			printf("getMagnetStatusValue gpio_init error\n");
			rs = -1;
			return rs;
		}
		bInitFlag = 0;
	}
	rs = gpio_input(MAGNET);
	//printf("getMagnetStatusValue rs(%d)\n", rs);
	return rs;
}

