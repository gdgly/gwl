#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "gpio.h"



/**************************************
****gpio��ʼ��
****num:IO�ں�
****direction������ 0-input 1-output
�����������أ�0 -�ɹ� -1 -ʧ��
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
****gpio���
****num:IO�ں�
****val�����ֵ 0-Low 1-Hig
�����������أ�0 -�ɹ� -1 -ʧ��
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
****gpio����
****num:IO�ں�
�����������أ�0,1 ����ֵ�� -1 ʧ��
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
//	��������: getMagnetStatusValue
//	��������: ��ȡDRV5033״̬        
//	����˵��:
//	int	[in]	gpio �˿ں�
//	�� �� ֵ: 
//	0 : ���ų�����
//	1 : Ĭ��ֵ
//	����Ϊ������  
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

