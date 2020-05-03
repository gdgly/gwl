#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <pthread.h>
#include "ListTimer.h"


LIST_TIMER ListTimer;


//void* timer_worker(void *para)
//{	
//	while(1)
//	{
//	    tick();
//	    usleep(1000);		
//	}
//}
LIST_TIMER *GetListTimer(void)
{
	return &ListTimer;
}

/**
 * @brief init_List_Timer
 * @param lt timer list
 * initialize list timer, head and tail are NULL
 *  初始化定时器
 */
void init_List_Timer(void)
{
//	int ret = 0;
//	thread_t timer_thread;

		
    ListTimer.head = NULL;
    ListTimer.tail = NULL;

//	ret = pthread_create(&timer_thread,NULL,timer_worker,NULL);
//	if(ret) printf("create timer thread failled!\n");
}
int load_timer(UTIL_TIME *ut)
{	
	printf("[load timer]\n");
    if (ut)
    {
        if (!ListTimer.head && !ListTimer.tail)
        {
            ListTimer.head = ut;
            ListTimer.tail = ut;
            ut->prev = NULL;
            ut->next = NULL;
            return 0;
        }
        else if (ListTimer.head && ListTimer.tail)
        {
            UTIL_TIME * temp = ListTimer.head;
            UTIL_TIME * tempbak = NULL;
            while(temp)
            {
                if((temp->out_time) > (ut->out_time))
                {
                    if(ListTimer.head == temp)
                    {
                        ut->next = temp;
                        temp->prev = ut;
                        ListTimer.head = ut;
                        ut->prev = NULL;
                        return 0;
                    }
                }
                tempbak = temp;
                temp = temp->next;
            }
            tempbak->next = ut;
            ut->prev = tempbak;
            ut->next = NULL;
            ListTimer.tail = ut;
            return 0;
        }
        return -1;
    }
    return -1;
}

/**
 * @brief add_timer
 * @param lt
 * @param ut
 * @return
 * 添加一个定时器 并跟定时器排序
 */
int add_timer(int timeout,int period,void (*timeout_callback)(void * data),void *data,UTIL_TIME **out_ut)
{
	if(timeout_callback == NULL) return -1;
	UTIL_TIME *ut = (UTIL_TIME *)malloc(sizeof(UTIL_TIME));
	if(NULL == ut)
	{
		printf("timer malloc error!\n");
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
		printf("[add timer success]\n");
		return 0;
	}
	printf("[add timer fail]\n");
	return -1;
}


/**
 * @brief del_timer
 * @param lt
 * @param ut
 * @return
 * 由相应定时器的位置删除定时器
 */
int del_timer(UTIL_TIME **ut)
{
    if(ut && *ut)
    {
        if(ListTimer.head)
        {
            UTIL_TIME *temp = *ut;
            if((temp == ListTimer.head) && (temp != ListTimer.tail)) //头
            {
                ListTimer.head = temp->next;
                temp->next->prev = NULL;
            }
            else if((temp == ListTimer.tail) && (temp != ListTimer.head)) //尾
            {
                temp->prev->next = NULL;
                ListTimer.tail = temp->prev;
            }
            else if((temp == ListTimer.tail) && (temp == ListTimer.head)) //只有一个定时器
            {
                ListTimer.head = NULL;
                ListTimer.tail = NULL;
            }
            else
            {
                temp->next->prev = temp->prev;
                temp->prev->next = temp->next;
            }
            (*ut)->cdata = NULL;
            (*ut)->next = NULL;
            (*ut)->prev = NULL;
            (*ut)->timeout_callback = NULL;
            free(*ut);
            *ut = NULL;
            ut = NULL;
            return 0;
        }
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
 *  移除指针并插入
 */
int adjust_timer(UTIL_TIME * ut, time_t _time)
{
    if(!ut||!_time)
	{
        return 0;
    }
    ut->out_time = time(NULL) + _time;
    if(!ut->prev && !ut->next)
    {
        return 0;   //only have single Node
    }
	else if (ut->prev && !ut->next) 
	{
        ut->prev->next = NULL;   //if ut is tail Node, remove it.
        ut->prev = NULL;
    }
	else if (!ut->prev && ut->next) 
	{
        ListTimer.head = ut->next;    //if ut is head Node
        ut->next->prev = NULL;
        ut->next = NULL;
        ut->prev = NULL;
    }
	else
	{
        ut->next->prev = ut->prev; //ut is middle
        ut->prev->next = ut->next;
        ut->next = NULL;
        ut->prev = NULL;
    }
    //  Can be optimized , insert after this Node.
    if(!load_timer(ut))          //reinsert it
    {
        return 0;
    }
    return -1;
}

/**
 * @brief tick
 * @param lt
 * Timer list tick, depend on callback function, adjust timer.
 */
void tick(void)
{
    time_t now = time(NULL);
    UTIL_TIME *temp = ListTimer.head;
    UTIL_TIME *tempbak = temp;
    while(temp)
    {
        tempbak = temp;
        temp = temp->next;
        if(tempbak->out_time <= now)    //检查时间
        {
            tempbak->timeout_callback(tempbak->cdata);
            if(!(tempbak->persist))   
                del_timer(&tempbak);
            else
			{
                //persist time 重新调整
                adjust_timer(tempbak, tempbak->persist);
            }
        }
		else
		{
            break;
        }
    }
//	printf("[tick]\n");
}

/**
 * @brief destroy_list_Timer
 * @param lt
 * destroy timer list
 */
void destroy_list_Timer(void)
{
    UTIL_TIME * temp = ListTimer.head;
    UTIL_TIME * tempnext = NULL;
    while(temp)
	{
        tempnext = temp->next;
        temp->cdata = NULL;
        temp->next = NULL;
        temp->prev = NULL;
        temp->timeout_callback = NULL;
        free(temp);
        temp = tempnext;
    }
    ListTimer.head = NULL;
    ListTimer.tail = NULL;
}






























