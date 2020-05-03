#include "queue.h"
#include "stdlib.h"
#include "string.h"

EventQueue *queue = 0;       //消息队列头全局变量

//创建队列
EventQueue *sequeue_create()
{
    EventQueue *q = (EventQueue *)malloc(sizeof(EventQueue));
    q->front = 0;
    q->rear = 0;
	q->count = 0;
    return q;
}

//判断队列是否为空，为空时返回1，不为空返回0
int sequeue_empty(EventQueue *q)
{
    return q->rear == q->front ? 1 : 0;
}


//判断队列是否为满，为满时返回1，不为满返回0
int sequeue_full(EventQueue *q)
{
    return (q->rear + 1) % QUEUEMAXSIZE == q->front  ? 1 : 0; 
}


//入队 ，成功时返回0，失败时返回1
int sequeue_input(EventQueue *q, Eventdata d)
{
    if(sequeue_full(q) == 1)
    {
            //printf("sequeue is full.\n");
            return 1;
    }	
	memset(&q->data[q->rear], 0, sizeof(q->data[q->rear]));
    q->data[q->rear] = d;
    q->rear = (q->rear + 1) % QUEUEMAXSIZE;	
	q->count++;
    return 0;
}


//出队,失败返回1， 成功返回 0  
int sequeue_output(EventQueue *q, Eventdata *d)
{
    if(sequeue_empty(q) == 1)
    {
            //printf("sequeue is empty.\n");
            return 1;
    }

    *d = q->data[q->front];
    q->front = (q->front + 1) % QUEUEMAXSIZE;
	q->count--;
    return 0;
}


//队列初始化
void sequeue_init()
{
    queue = sequeue_create();
}
