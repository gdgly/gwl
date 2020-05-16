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

	if ((0 == srcSize) || (NULL == destData))
	{
		return -1;
	}
	
	for(i=0; i<srcSize; i++)
	{
		destData[i] = ~srcData[i];
	}
	return 0;
}

//*****************************************************************************
// 函数名称: mrsToolsCalcCheckSum2
// 功能描述: 计算校验和
// 参数说明:
//   unsigned char* [IN] 二进制起始地址
//   int   [IN] 二进制长度
// 返 回 值:
//    返回校验码，
//       注意：mrsToolsCalcCheckSum2对参数错误不做特殊标记,将返回检验和0，
//    正确的校验和也有可能返回0!!!
// 调用要求:
// 调用举例:
//*****************************************************************************
unsigned char mrsToolsCalcCheckSum2(unsigned char* ptr, int len)
{
    unsigned char CheckSum = 0;
#if 0//debug_info
	int i = 0;
	printf("total(%d):", len);
	for (i = 0; i < len ; i++)
	{
		printf("%02X ", ptr[i]);
	}
	printf("\n");
#endif
    if ((ptr != NULL) && (len > 0))
    {
        int i = 0;
        for(i = 0; i < len; i++)
        {
            CheckSum += ptr[i];
        }
    }
	printf("mrsToolsCalcCheckSum2 CheckSum=0x%x\n", CheckSum);
    return CheckSum;
}


//*****************************************************************************
// 函数名称: mrsToolsCalcCheckSum
// 功能描述: 计算校验和
// 参数说明:
//   HI_PBYTE [IN] 二进制起始地址
//   HI_U16   [IN] 二进制长度
// 返 回 值:
//    返回校验码，
//       注意：mrsToolCalcCheckSum对参数错误不做特殊标记,将返回检验和0，
//    正确的校验和也有可能返回0!!!
// 调用要求:
// 调用举例:
// 作    者: niesongsong/kf62735 [2011-12-13]
//*****************************************************************************
unsigned char mrsToolsCalcCheckSum(unsigned char* ptr, int len)
{
	unsigned char checksum = 0;
    //return (unsigned char)mrsToolsCalcCheckSum2(ptr, len);
    checksum = (unsigned char)mrsToolsCalcCheckSum2(ptr, len);
	return checksum;
}


//*****************************************************************************
// 函数名称: mrsToolsStreamFindByte
// 功能描述: 从二进制流查找目标字符
// 参数说明:
//   HI_PBYTE   [IN] 流起始
//   HI_U16     [IN] 流长度
//   HI_U8      [IN] 目标字符
// 返 回 值:
//    如果找到则返回在流中的地址，否则返回空
// 调用要求: 无
// 调用举例: 无
// 作    者: niesongsong/kf62735 [2011-12-13]
//*****************************************************************************
unsigned char* mrsToolsStreamFindByte(unsigned char* ptr, int len, int v)
{
    int i = 0;

    if (ptr != NULL && len < MRS_STREAM_MAX_LEN)
    {
        for (i = 0; i < len; i++)
        {
            if (ptr[i] == v)
            {
                return ptr + i;
            }
        }
    }

    return NULL;
}

//*****************************************************************************
// 函数名称: mrsToolsCheck645Frame
// 功能描述: 检测645帧合法性
// 参数说明:
//   HI_PBYTE [IN] 缓存起址
//   HI_U16 [IN] 缓存长度
// 返 回 值:
//    成功返回HI_ERR_SUCCESS,失败返回HI_ERR_BAD_DATA
// 调用要求: 无
// 调用举例: 无
// 作    者: niesongsong/kf62735 [2012-01-05]
//*****************************************************************************
int mrsToolsCheck645Frame(unsigned char* ptr, int len)
{
    unsigned char checkSum;

    //帧长检测
    if ((!ptr) ||
        (len > MRS_PROTO645_DATAGRAM_LEN_MAX) ||
        (len < MRS_PROTO645_DATAGRAM_HEADER_LEN_MAX))
    {
		printf("Frame len check\n");
        return -1;
    }

    //帧标志检查
    if ((ptr[0] != 0x68) || (ptr[7] != 0x68) || (ptr[len-1] != 0x16))
    {		
		printf("Frame flag check\n");
        return -1;
    }

    //校验和检查
    checkSum = mrsToolsCalcCheckSum(ptr,(len-2));
	printf("checkSum=0x%x, ptr[len-2]=0x%x\n", checkSum, ptr[len-2]);
    if (checkSum != ptr[len - 2])
    {
		printf("Frame checksum check, 0x%x, 0x%x\n", len, ptr[len-2]);
        return -1;
    }

    return 0;
}


