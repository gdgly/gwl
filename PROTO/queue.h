#ifndef QUEUE_H
#define QUEUE_H

#include "Sgd_types.h"

#define QUEUEMAXSIZE 15			//��Ϣ���нڵ����
#define DATASIZE     256		//������ĳ���


//��Ϣ���нṹ��
typedef struct
{
  int datalen;				    //���ݳ���
  u8 data[DATASIZE];  			//���������ΪQUEUEMAXSIZE
  u8 seq;						//֡���
  u8 report_sucesss_flag;       //�ϱ��ɹ���ʶ
  u8 report_count;              //�ϱ�����
}Eventdata;


//��Ϣ���нṹ��
typedef struct
{
  Eventdata data[QUEUEMAXSIZE];  //���������ΪQUEUEMAXSIZE
  int front;                     //ָ�����ͷ
  int rear;                      //ָ���β��һ��λ��
  int count;					 //Ԫ�صĸ���
}EventQueue;


//���г�ʼ��
void sequeue_init();


//����,ʧ�ܷ���1�� �ɹ����� 0  
int sequeue_output(EventQueue *q, Eventdata *d);    


//��� ���ɹ�ʱ����0��ʧ��ʱ����1
int sequeue_input(EventQueue *q, Eventdata d);   

extern EventQueue *queue;		//��Ϣ����ͷ

#endif
