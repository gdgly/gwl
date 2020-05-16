/*
 * Real Time Clock Driver Test/Example Program
 *
 * Compile with:
 *  gcc -s -Wall -Wstrict-prototypes rtctest.c -o rtctest
 *
 * Copyright (C) 1996, Paul Gortmaker.
 *
 * Released under the GNU General Public License, version 2,
 * included herein by reference.
 *
 */

#include <stdio.h>
#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>
#include <signal.h>
#include <string.h>
#include "rtc.h"


int setup_time(struct rtc_time* rtc_tm)
{
	int retval = 0, i = 0;
	const int WRITE_TIMES = 5;


	int fd = open("/dev/rtc0", O_RDWR);
	if(fd < 0)
	{
		printf("open /dev/rtc0 failed");
		return -1;
	}

	rtc_tm->tm_year -= 1900;
	rtc_tm->tm_mon -= 1;
	
	for(i=0; i<WRITE_TIMES; i++)
	{
		if(0 == ioctl(fd, RTC_SET_TIME, rtc_tm))
		{
			retval = 0;
			break;
		}
	}
	
	if (i == WRITE_TIMES) 
	{
		printf("ioctl RTC_SET_TIME  faild!!!\n");
		retval = -1;
	}
	
	close(fd);
	return retval;
}

int read_time(struct rtc_time *rtc_tm)
{
	int retval = 0, i = 0;
	const int READ_TIMES = 5;
	
	int fd = open("/dev/rtc0", O_RDWR);
	if(fd < 0)
	{
		printf("open /dev/rtc0 failed");
		return -1;
	}
	
	/*read current time*/
	for(i = 0; i < READ_TIMES; i++)
	{
		if(0 == ioctl(fd, RTC_RD_TIME, rtc_tm))
		{
			retval = 0;
			break;
		}
	}
	
	if (i == READ_TIMES) 
	{
		printf("ioctl RTC_RD_TIME  faild!!!\n");
		close(fd);
		return -1;
	}
	
	rtc_tm->tm_year += 1900;
	rtc_tm->tm_mon += 1;
	
	close(fd);
	return retval;
}