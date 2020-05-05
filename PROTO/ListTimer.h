#ifndef LIST_TIMER_H
#define LIST_TIMER_H

typedef struct util_time
{
    struct  util_time * prev; // ˫�����ǰ��
    struct util_time * next; //˫�������
    //Client data point
    void * cdata;         //��������
    //timeout value
    time_t out_time;       //��ʱʱ��
    int persist;      //�Ƿ��Ǽ�ֶ�ʱ
    //if timeout callback
    void (*timeout_callback)(void * data); //��ʱ�ص�����
}UTIL_TIME;

//Timer list
//��ʱ������ͷ
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
LIST_TIMER *GetListTimer(void);
int add_timer(int timeout,int period,void (*timeout_callback)(void * data),void *data,UTIL_TIME **out_ut);
int del_timer(UTIL_TIME **ut);
int adjust_timer(UTIL_TIME * ut, time_t _time);
void tick(void);
void init_List_Timer(void);
void destroy_list_Timer(void);
#endif

