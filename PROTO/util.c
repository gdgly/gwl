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

//������ĸ��ֽں͵��ֽ�λ��ȫ���Ե�������
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

//���645Э��
void Protocol_645_Unpack(unsigned char *P_String, unsigned char Length)
{
	unsigned int i = 0;
	
	//��һ������ַ��ߵ��ֽڷ�ת���ָ�����
	Array_Mirror(P_String + 1, 6);
	
	//�ڶ������������ȥ0x33
	for(i = 0; i < P_String[9]; i++)
	{
		P_String[10 + i] -= 0x33;
	}
}

//���645Э��
void Protocol_645_Pack(unsigned char *P_String, unsigned char Length)
{
	unsigned int	i = 0;
	
	//��һ������ַ��ߵ��ֽڷ�ת��С�˴���
	Array_Mirror(P_String + 1, 6);
	
	//�ڶ����������������ֽ�+0x33
	for(i = 0; i < P_String[9]; i++)
	{
		P_String[10 + i] += 0x33;
	}
}

//����У���
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
//	��������: BitReverse
//	��������: ������Ԫ��ȡ��        
//	����˵��:
//	pbuf [in]
//  pbufrev [out]
//	�� �� ֵ: 
//	0 : �ɹ�
//	����Ϊ������   
//	����Ԫ��ȡ��,����1 2 3 -> fe fd fc
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
// ��������: mrsToolsCalcCheckSum2
// ��������: ����У���
// ����˵��:
//   unsigned char* [IN] ��������ʼ��ַ
//   int   [IN] �����Ƴ���
// �� �� ֵ:
//    ����У���룬
//       ע�⣺mrsToolsCalcCheckSum2�Բ���������������,�����ؼ����0��
//    ��ȷ��У���Ҳ�п��ܷ���0!!!
// ����Ҫ��:
// ���þ���:
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
// ��������: mrsToolsCalcCheckSum
// ��������: ����У���
// ����˵��:
//   HI_PBYTE [IN] ��������ʼ��ַ
//   HI_U16   [IN] �����Ƴ���
// �� �� ֵ:
//    ����У���룬
//       ע�⣺mrsToolCalcCheckSum�Բ���������������,�����ؼ����0��
//    ��ȷ��У���Ҳ�п��ܷ���0!!!
// ����Ҫ��:
// ���þ���:
// ��    ��: niesongsong/kf62735 [2011-12-13]
//*****************************************************************************
unsigned char mrsToolsCalcCheckSum(unsigned char* ptr, int len)
{
	unsigned char checksum = 0;
    //return (unsigned char)mrsToolsCalcCheckSum2(ptr, len);
    checksum = (unsigned char)mrsToolsCalcCheckSum2(ptr, len);
	return checksum;
}


//*****************************************************************************
// ��������: mrsToolsStreamFindByte
// ��������: �Ӷ�����������Ŀ���ַ�
// ����˵��:
//   HI_PBYTE   [IN] ����ʼ
//   HI_U16     [IN] ������
//   HI_U8      [IN] Ŀ���ַ�
// �� �� ֵ:
//    ����ҵ��򷵻������еĵ�ַ�����򷵻ؿ�
// ����Ҫ��: ��
// ���þ���: ��
// ��    ��: niesongsong/kf62735 [2011-12-13]
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
// ��������: mrsToolsCheck645Frame
// ��������: ���645֡�Ϸ���
// ����˵��:
//   HI_PBYTE [IN] ������ַ
//   HI_U16 [IN] ���泤��
// �� �� ֵ:
//    �ɹ�����HI_ERR_SUCCESS,ʧ�ܷ���HI_ERR_BAD_DATA
// ����Ҫ��: ��
// ���þ���: ��
// ��    ��: niesongsong/kf62735 [2012-01-05]
//*****************************************************************************
int mrsToolsCheck645Frame(unsigned char* ptr, int len)
{
    unsigned char checkSum;

    //֡�����
    if ((!ptr) ||
        (len > MRS_PROTO645_DATAGRAM_LEN_MAX) ||
        (len < MRS_PROTO645_DATAGRAM_HEADER_LEN_MAX))
    {
		printf("Frame len check\n");
        return -1;
    }

    //֡��־���
    if ((ptr[0] != 0x68) || (ptr[7] != 0x68) || (ptr[len-1] != 0x16))
    {		
		printf("Frame flag check\n");
        return -1;
    }

    //У��ͼ��
    checkSum = mrsToolsCalcCheckSum(ptr,(len-2));
	printf("checkSum=0x%x, ptr[len-2]=0x%x\n", checkSum, ptr[len-2]);
    if (checkSum != ptr[len - 2])
    {
		printf("Frame checksum check, 0x%x, 0x%x\n", len, ptr[len-2]);
        return -1;
    }

    return 0;
}


