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
//针对/dev/rtcX下设备操作
////////////////////////////////////////////////////////////

//9260本身自带RTC----序号为0---/dev/rtc0
//9260外扩IIC硬件时钟为1			 /dev/rtc1
//网络表时钟为2								 /conf/rtc2
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

#define	SUCCESS				0x00			//成功
#define COMM_FAIL			0x01			//通讯错误
#define NO_DEVICE			0x02			//设备不存在
#define	FORMAT_FAIL		0x04			//时间格式错误
#define DIFF_ERROR		0x08			//时间超差
#define OPERATE_ERROR	0x10			//操作失败

int SetRtc( struct tm * pTm )
{
	int result = false;
	int i = 0;
	//int err = OPERATE_ERROR;
	if( access("/dev/rtc0", 0) < 0 )
	{
		//err = NO_DEVICE;
		return true;		//没有这个设备，则表示同步该设备时钟正确
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
// 从内核中读取时钟，考虑到硬件时钟信号质量不好，有可能造成时钟跳变，因此需要进行错误处理
// 方式是： 连续读取内核时间若干（默认5次），如果连续几次次的时间相差在允许范围秒内，
// 认为是硬件没有受到干扰，读取到的时钟是可信的。
// 另外，如果某次读取的时钟是非法的，也返回错误
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
  	return false;		//没有这个设备，则表示同步该设备时钟正确 
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
		if( tmp == (time_t) -1 )	//时间无效
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
