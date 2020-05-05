#ifndef QUEUE_H
#define QUEUE_H

#include "Sgd_types.h"

#define QUEUEMAXSIZE 15			//消息队列节点个数
#define DATASIZE     256		//数据域的长度


//消息队列结构体
typedef struct
{
  int datalen;				    //数据长度
  u8 data[DATASIZE];  			//数据域，最大为QUEUEMAXSIZE
  u8 seq;						//帧序号
  u8 report_sucesss_flag;       //上报成功标识
  u8 report_count;              //上报次数
}Eventdata;


//消息队列结构体
typedef struct
{
  Eventdata data[QUEUEMAXSIZE];  //数据域，最大为QUEUEMAXSIZE
  int front;                     //指向队列头
  int rear;                      //指向队尾后一个位置
  int count;					 //元素的个数
}EventQueue;


//队列初始化
void sequeue_init();


//出队,失败返回1， 成功返回 0  
int sequeue_output(EventQueue *q, Eventdata *d);    


//入队 ，成功时返回0，失败时返回1
int sequeue_input(EventQueue *q, Eventdata d);   

extern EventQueue *queue;		//消息队列头

#endif
