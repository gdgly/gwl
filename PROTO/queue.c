#include "queue.h"
#include "stdlib.h"
#include "string.h"

EventQueue *queue = 0;       //��Ϣ����ͷȫ�ֱ���

//��������
EventQueue *sequeue_create()
{
    EventQueue *q = (EventQueue *)malloc(sizeof(EventQueue));
    q->front = 0;
    q->rear = 0;
	q->count = 0;
    return q;
}

//�ж϶����Ƿ�Ϊ�գ�Ϊ��ʱ����1����Ϊ�շ���0
int sequeue_empty(EventQueue *q)
{
    return q->rear == q->front ? 1 : 0;
}


//�ж϶����Ƿ�Ϊ����Ϊ��ʱ����1����Ϊ������0
int sequeue_full(EventQueue *q)
{
    return (q->rear + 1) % QUEUEMAXSIZE == q->front  ? 1 : 0; 
}


//��� ���ɹ�ʱ����0��ʧ��ʱ����1
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


//����,ʧ�ܷ���1�� �ɹ����� 0  
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


//���г�ʼ��
void sequeue_init()
{
    queue = sequeue_create();
}
