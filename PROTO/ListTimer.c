#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include "ListTimer.h"


sList ListTimer;


///*************************
// * @brief 
// * @param 
// * @param 
// * @return
// *************************/
//sList *GetListTimer(void)
//{
//	return &ListTimer;
//}

/**
 * @brief init_List_Timer
 * @param lt timer list
 * initialize list timer, head and tail are NULL
*/
void init_List_Timer(void)
{
	ListInit(&ListTimer);
}
/*************************
 * @brief 
 * @param 
 * @param 
 * @return
 *************************/
static int load_timer(UTIL_TIME *ut)
{	
	if(!ut) return -1;
	
	if(ListLength(&ListTimer) > MAX_TIMER_NUM)
	{
		return -1;
	}
	if(0 == ListInsertTail(&ListTimer,&ut->Node))
	{
		return 0;
	}
	
    return -1;
}

/*************************
 * @brief 
 * @param 
 * @param 
 * @return
 *************************/

int add_timer(int timeout,int period,void (*timeout_callback)(void * data),void *data,UTIL_TIME **out_ut)
{
	if(timeout_callback == NULL) return -1;
	UTIL_TIME *ut = (UTIL_TIME *)malloc(sizeof(UTIL_TIME));
	if(NULL == ut)
	{
		printf("[timer]timer malloc error!\n");
		return -1;
	}
    ut->timeout_callback = timeout_callback;
    ut->out_time = time(NULL) + timeout;
    ut->persist = period;	
	ut->cdata = data;
	if(out_ut)
	{
		*out_ut = ut;
	}
	if(!load_timer(ut))
	{
		printf("[timer]add timer success:0x%x\n",ut);
		return 0;
	}
	printf("[timer]add timer fail\n");
	return -1;
}
/**
 * @brief del_timer
 * @param lt
 * @param ut
 * @return
 * 
 */
int del_timer(UTIL_TIME **ut)
{
	UTIL_TIME *data = *ut;

	if(!ListRemove(&ListTimer,NULL,&data->Node))
	{
		printf("[timer]delete timer\n");
		free(data);
		data = NULL;
		return 0;
	}
    return -1;
}

/**
 * @brief adjust_timer
 * @param lt
 * @param ut
 * @param _time
 * @return
 * if a timer be deleted or addition time to a timer, adjust timer list
 */
static int adjust_timer(UTIL_TIME * ut)
{
    if(!ut)
	{
        return -1;
    }
	ListRemove(&ListTimer,NULL,&ut->Node);
    ut->out_time = time(NULL) + ut->persist;
    //Can be optimized , insert after this Node.
    if(!load_timer(ut))//reinsert it
    {
        return 0;
    }
    return -1;
}
static int Excecute(void *Node1,void *extra)
{
	if(!Node1) return -1;

	UTIL_TIME *Timer = (UTIL_TIME*)Node1;
	time_t now = time(NULL);
	
	if(Timer->out_time <= now)
	{		
		if(Timer->timeout_callback)
		{
			Timer->timeout_callback(Timer->cdata);
		}
		if(Timer->persist)
		{
			adjust_timer(Timer);
		}
		else
		{
			del_timer(&Timer);
		}
		return 0;
	}
	return -1;
}
/**
 * @brief tick
 * @param lt
 * Timer list tick, depend on callback function, adjust timer.
 */
void TimerTick(void)
{
	ListTraverse(&ListTimer, Excecute, NULL);
}

static int TimerListClear(void *node,void *extra)
{
	if(!node) return -1;

	free(node);
	return 0;
}
/**
 * @brief destroy_list_Timer
 * @param lt
 * destroy timer list
 */
void destroy_list_Timer(void)
{
	ListClear(&ListTimer,TimerListClear,NULL);
}






























