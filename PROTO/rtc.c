#include <sys/time.h>
//#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>

#include "rtc.h"

#define DEFAULT_RTC_DEVICE "/dev/rtc1"
//#define	bool	int
#define	true	1
#define false	0


////////////////////////////////////////////////////////////
//Real Time Clock Class 
//���/dev/rtcX���豸����
////////////////////////////////////////////////////////////

//9260�����Դ�RTC----���Ϊ0---/dev/rtc0
//9260����IICӲ��ʱ��Ϊ1			 /dev/rtc1
//�����ʱ��Ϊ2								 /conf/rtc2
#define	CPU_RTC				0
#define IIC_RTC				1
#define NET_RTC				2
#define MAX_RTC_DEV		3
#define MASK_CPU_RTC	(1 << CPU_RTC )
#define MASK_IIC_RTC	(1 << IIC_RTC )
#define MASK_NET_RTC	(1 << NET_RTC )
#define MASK_SYS_RTC	(1 << MAX_RTC_DEV)
#define	MASK_SAVE_SETTIME_REC	 0x80 


#define NO_MASK_CPU_RTC	( ~MASK_CPU_RTC )
#define NO_MASK_IIC_RTC	( ~MASK_IIC_RTC )
#define NO_MASK_NET_RTC	( ~MASK_NET_RTC )
#define NO_MASK_SYS_RTC	( ~MASK_SYS_RTC )

#define	SUCCESS				0x00			//�ɹ�
#define COMM_FAIL			0x01			//ͨѶ����
#define NO_DEVICE			0x02			//�豸������
#define	FORMAT_FAIL		0x04			//ʱ���ʽ����
#define DIFF_ERROR		0x08			//ʱ�䳬��
#define OPERATE_ERROR	0x10			//����ʧ��

int SetRtc( struct tm * pTm )
{
	int result = false;
	int i = 0;
	//int err = OPERATE_ERROR;
	if( access("/dev/rtc0", 0) < 0 )
	{
		//err = NO_DEVICE;
		return true;		//û������豸�����ʾͬ�����豸ʱ����ȷ
	}
	
	int fd = open("/dev/rtc0", O_RDWR );
	if( fd < 0 )
	{
//		err = OPERATE_ERROR;
		return false;
	}
	for(i = 0; i < 3; i ++ ) 
	{
		if( 0 == ioctl(fd, RTC_SET_TIME, pTm ) )
		{
			result = true;
			//err = SUCCESS;
			break;
		}
	}
	close( fd );

	return result;
}

//////////////////////////////////////////////////////
// ���ں��ж�ȡʱ�ӣ����ǵ�Ӳ��ʱ���ź��������ã��п������ʱ�����䣬�����Ҫ���д�����
// ��ʽ�ǣ� ������ȡ�ں�ʱ�����ɣ�Ĭ��5�Σ�������������δε�ʱ�����������Χ���ڣ�
// ��Ϊ��Ӳ��û���ܵ����ţ���ȡ����ʱ���ǿ��ŵġ�
// ���⣬���ĳ�ζ�ȡ��ʱ���ǷǷ��ģ�Ҳ���ش���
/////////////////////////////////////////////////////

int GetRtc( struct tm * pTm )
{
  const int READ_TIMES = 5;

  time_t tmp = 0;
  int i =0;
  //int err = SUCCESS;
  if( access("/dev/rtc0", 0) < 0 )
  {
  	//err = NO_DEVICE;
  	return false;		//û������豸�����ʾͬ�����豸ʱ����ȷ 
  }
	int fd = open("/dev/rtc0", O_RDWR );
  if ( fd < 0 )
  {
  	//err = OPERATE_ERROR;
  	return false;
  }

  int min = INT_MAX; 
  int max = INT_MIN;
	for(i = 0; i < READ_TIMES; i ++ )
	{
		if( ioctl( fd, RTC_RD_TIME, pTm ) < 0 )
		{
			perror("ioctl");
			close( fd );
			//err = OPERATE_ERROR;
			return false;
		}
		//if( !IsValidDt( pTm ) )
		{
			continue;
		}
		tmp = mktime( pTm );
		if( tmp == (time_t) -1 )	//ʱ����Ч
		{
			continue;
		}
		
		if ( min > tmp )
			min = tmp;

		if ( max < tmp )
			max = tmp;
	}
	close( fd );
  if( ( max - min ) < READ_TIMES )
  {
  	 //err = SUCCESS;
  	 return true;
  }
  //err = DIFF_ERROR;
  return false;
} 
