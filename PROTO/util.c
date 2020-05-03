#include <time.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include "util.h"

#ifndef WIN32
static int uptime_fd = -1;
#endif

#if	!defined(ACE_WIN32)
long get_sys_uptime_by_sysinfo()
{
	long ret;
	struct sysinfo info;
	if (sysinfo(&info) >= 0)
	{
		ret = info.uptime;
	}
	else
	{
		//MY_ACE_ERROR((LM_ERROR,ACE_TEXT("sysinfo() fails?!\n")));
		printf("sysinfo() fails?!\n");
	}
	return ret;
}
#endif

long get_sys_uptime()
{
#ifndef WIN32
	long ret = -1;
	struct sysinfo info;
	if (sysinfo(&info) >= 0)
	{
		ret = info.uptime;
	}
	else
	{
		//perror("sysinfo() fails?!\n");
	}
	return ret;
#else
	return GetTickCount()/1000;
#endif
}

long get_sys_uptime_ms()
{
	long ret = -1;
#define UPTIME_PATH "/proc/uptime"
	//xxxxx.xx check kernel/fs/proc/uptime.c
	static int fd = -1;
	if (fd < 0)
	{
		fd = open(UPTIME_PATH,O_SYNC | O_RDONLY);
	}
	else
		lseek(fd,0,SEEK_SET);
	if (fd >= 0)
	{
		char buf[64];
		int total = 0;
		int got;
		buf[0] = 0;
		do	
		{
			got = read(fd,buf + total,sizeof(buf) - 1 - total);
			if (got > 0)
			{
				char* space_pos = buf;
				total += got;
				buf[total] = 0;
				while(*space_pos && !isspace(*space_pos))
					++ space_pos;
				if (*space_pos)//read enough(got space)
				{
					unsigned long s = 0;
					//close(fd);fd = -1;
					*space_pos = 0;
					space_pos = buf;
					while(*space_pos && isdigit(*space_pos))
					{
						s = (s << 3) + (s << 1);//*(8+2)
						s += *space_pos - '0';
						++ space_pos;
					}
					ret = s;//seconds
					if (*space_pos != '.' || space_pos[3])//make sure kernel does not change /proc/uptime format
					{
						break;
					}
					//2 digit fraction
					s = (space_pos[1] - '0');
					s = (s << 3) + (s << 1);//*(8+2)
					s += (space_pos[2] - '0');
					return ret*1000 + s*10;
				}
			}
		}while (total < sizeof(buf) - 1 && (got > 0 || errno == EINTR));
	}

	ret = get_sys_uptime();
	ret *= 1000;
	return ret;
}

int mymsleep(unsigned int ms)
{
	//select can do it ,too.
	struct timespec ts;
	ts.tv_sec = ms / 1000;
	ts.tv_nsec = (ms % 1000) * 1000000ul;//200ms
	return	nanosleep(&ts, NULL);
}

unsigned char DEC_To_BCD(unsigned char Data_DEC)
{
	unsigned char Data_BCD = 0;
	
	Data_BCD = ((Data_DEC / 10) << 4) + (Data_DEC % 10);
	
	return Data_BCD;
}

//把数组的高字节和低字节位置全部对调（镜像）
void Array_Mirror(unsigned char* P_Array, int Length)
{
	int i = 0;
	int  Temp = 0;
	int Length_Half = 0;
	
	Length_Half = Length / 2;
	
	for(i = 0; i < Length_Half; i++)
	{
		Temp = *(P_Array + i);
		*(P_Array + i) = *(P_Array + Length - 1 - i);
		*(P_Array + Length - 1 - i) = Temp;
	}
}

//解包645协议
void Protocol_645_Unpack(unsigned char *P_String, unsigned char Length)
{
	unsigned int i = 0;
	
	//第一步，地址域高低字节翻转，恢复正常
	Array_Mirror(P_String + 1, 6);
	
	//第二步，数据域减去0x33
	for(i = 0; i < P_String[9]; i++)
	{
		P_String[10 + i] -= 0x33;
	}
}

//打包645协议
void Protocol_645_Pack(unsigned char *P_String, unsigned char Length)
{
	unsigned int	i = 0;
	
	//第一步，地址域高低字节翻转，小端传送
	Array_Mirror(P_String + 1, 6);
	
	//第二步，数据域所有字节+0x33
	for(i = 0; i < P_String[9]; i++)
	{
		P_String[10 + i] += 0x33;
	}
}

//计算校验和
unsigned char GetSum8(unsigned char *P_Data, unsigned int Length)
{
	unsigned char	Sum = 0;
	unsigned int	i   = 0;
	
	for(i = 0; i < Length; i++)
	{
		Sum += P_Data[i];
	}
	
	return Sum;
}

//*****************************************************************************
//	函数名称: BitReverse
//	功能描述: 对数组元素取反        
//	参数说明:
//	pbuf [in]
//  pbufrev [out]
//	返 回 值: 
//	0 : 成功
//	其它为错误码   
//	数组元素取反,例如1 2 3 -> fe fd fc
//*****************************************************************************
int BitReverse(unsigned char *srcData, unsigned int srcSize, unsigned char *destData)
{
	int i = 0;

	if ((NULL == srcSize) || (NULL == destData))
	{
		return -1;
	}
	
	for(i=0; i<srcSize; i++)
	{
		destData[i] = ~srcData[i];
	}
	return 0;
}

