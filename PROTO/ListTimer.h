#ifndef LIST_TIMER_H
#define LIST_TIMER_H

#include "list.h"


#define MAX_TIMER_NUM				600

typedef struct util_time
{
	sList Node;
    void * cdata;         //
    //timeout value
    time_t out_time;       //
    int persist;      //
    void (*timeout_callback)(void * data); //
}UTIL_TIME;

//Timer list
//定时器链表头
typedef struct
{
//    struct TimerOP timer_op;
    UTIL_TIME * head;
    UTIL_TIME * tail;
}LIST_TIMER;


struct TimerOP
{
    int (*add)(LIST_TIMER *lt, UTIL_TIME *ut);
    int (*del)(LIST_TIMER *lt, UTIL_TIME **ut);
    int (*adjust)(LIST_TIMER *lt, UTIL_TIME *ut, time_t _time);
    void (*_tick)(LIST_TIMER * lt);
};



//timer list operator, all function only handle list and free timer, not free clientdata
//LIST_TIMER *GetListTimer(void);
int add_timer(int timeout,int period,void (*timeout_callback)(void * data),void *data,UTIL_TIME **out_ut);
int del_timer(UTIL_TIME **ut);
void TimerTick(void);
void init_List_Timer(void);
void destroy_list_Timer(void);
#endif

