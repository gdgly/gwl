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