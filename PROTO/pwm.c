#include <stdio.h>
#include <stdlib.h>

#include "pwm.h"

//pwm0初始化
int pwm0_init()
{
	FILE *fp = NULL;

	if ((fp = fopen("/sys/class/pwm/pwmchip0/export","w")) == NULL)
	{
		printf("Cannot open pwmchip0 export file.\n");
		return -1;
	}
	fprintf(fp, "%d", 0);		//选择pwm0
	fclose(fp);

	if ((fp = fopen("/sys/class/pwm/pwmchip0/pwm0/period", "rb+")) == NULL)
	{
		printf("Cannot open pwm0 period file.\n");
		return -1;
	}
	fprintf(fp, "%d", 26315);	//设置pwm0周期为38k
	fclose(fp);

	if ((fp = fopen("/sys/class/pwm/pwmchip0/pwm0/duty_cycle", "rb+")) == NULL)
	{
		printf("Cannot open pwm0 duty_cycle file.\n");
		return -1;
	}
	fprintf(fp, "%d", 8711);	//设置pwm0占空比1:3
	fclose(fp);

	return 0;
}

//pwm0使能
int pwm0_enable()
{
	FILE *fp = NULL;

	if ((fp = fopen("/sys/class/pwm/pwmchip0/pwm0/enable","w")) == NULL)
	{
		printf("Cannot open pwm0 enable or disable file.\n");
		return -1;
	}
	fprintf(fp, "%d", 1);		//pwm0 使能
	fclose(fp);

	return 0;
}

//pwm0失能
int pwm0_disable()
{
	FILE *fp = NULL;

	if ((fp = fopen("/sys/class/pwm/pwmchip0/pwm0/enable","w")) == NULL)
	{
		printf("Cannot open pwm0 enable or disable file.\n");
		return -1;
	}
	fprintf(fp, "%d", 0);		//pwm0 失能
	fclose(fp);

	return 0;
}

//pwm0关闭
int pwm0_close()
{
	FILE *fp = NULL;

	if ((fp = fopen("/sys/class/pwm/pwmchip0/unexport","w")) == NULL)
	{
		printf("Cannot open pwmchip0 export file.\n");
		return -1;
	}
	fprintf(fp, "%d", 0);		//pwm0
	fclose(fp);

	return 0;
}

