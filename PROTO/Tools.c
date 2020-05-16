#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"
#include "Tools.h"


int itoa(int num,char* str,int radix)
{
    if((str == NULL)||!radix) return -1;
    
	char index[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	unsigned unum;
	int i=0,j,k;
	if(radix==10&&num<0)
	{
		unum=(unsigned)-num;
		str[i++]='-';
	}
	else unum=(unsigned)num;
	do
	{
		str[i++]=index[unum%(unsigned)radix];

		unum/=radix;
	}while(unum);
	str[i]='\0';

	if(str[0]=='-') k=1;
	else k=0;
	char temp;
	for(j=k;j<=(i-1)/2;j++)
	{
		temp=str[j];

		str[j]=str[i-1+k-j];

		str[i-1+k-j]=temp;
	}
	return 0;
}

/*将整数转化成定长BCD码,并逆序*/
u32 intToBcd(u32 data,u8 *bcd,u8 len)
{
    u32 i = 0;
    u32 remain = (u32)data;
    u32 tail = 0;
    
    if (!bcd) 
    {
        return (u32)-1;
    }

    for (i = 0; i < len; i++)
    {
        tail = remain % 100;
        remain /= 100;
        
        bcd[i] = (u8)(tail / 10 * 16 + tail % 10);
    }
    return 0;
}

u32 Hex2Bcd(u32 hex)
{
    u32 bcd = 0;  
    u32 b = 1;
    
    while(hex)
    {
        bcd += (hex%10)*b;
        hex /= 10;
        b *= 0x10;
    }
    return bcd;
}

u32 Bcd2Hex(u32 bcd)
{
    u32 hex=0;
    u32 b = 1;
    
    while(bcd)
    {
        hex += (bcd%0x10)*b;
        bcd /= 0x10;
        b *= 10;
    }
    return hex;
}
/*************************************************
函 数 名:       
功能描述:         
输    入:           
输    出:      
返    回:                  
*************************************************/
int printhexdata(void *data,int len)
{
	int i;
	unsigned char *pTmp = (unsigned char*)data;
	
	printf("[");
	for(i=0;i<len;i++)
	{
		printf("%02x ",pTmp[i]);
	}
	printf("\b]\n");
	return len;
}
int Str2Hex(u8 *str,u8 *hex)
{
	if(!str || !hex) return -1;

	int i,j,len;
	u8 tmp;

	len = strlen(str);
	for(i=0,j=0;i<len;i++)
	{
		if((str[0]>='0')&&(str[0]<='9'))
		{
			tmp += (str[i] - '0');
		}
		else if((str[0]>='a')&&(str[0]<='f'))
		{
			tmp += str[i] - 'a';
		}
		else if((str[0]>='A')&&(str[0]<='F'))
		{
			tmp += str[i] - 'A';
		}
		else
		{
			return -1;
		}
		if(i%2==0)
		{
			if(i<(len-1))
				tmp *= 0x10;
		}
		else
		{
			hex[j] = tmp;
			j++;
			tmp = 0;			
		}
	}
}

