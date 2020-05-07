

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"




int ListInit(sList *list)
{
	list->prev = list;
	list->next = list;
	return 0;
}

bool mrsSrvIsListEmpty(sList *list)
{
    if (!list) return true;

    return (bool)((list->next == list) && (list->prev == list));
}

int ListLength(sList * list)
{
    sList *it = NULL;
    int len = 0;
    
    it = list->next;
	while(it != list)
	{
		if(it == NULL) break;
		len++;
		it = it->next;
	}
    return len;
}


int ListInsertHead(sList *list,sList *node)
{
	if(0 == ListLength(list))
		ListInit(list);
    node->next = list->next;
    node->prev = list;

    node->next->prev = node;
    node->prev->next = node;

    return 0;	
}

int ListInsertTail(sList *list,sList *node)
{
	if(0 == ListLength(list))
		ListInit(list);	
    node->next = list;
    node->prev = list->prev;

    node->next->prev = node;
    node->prev->next = node;

    return 0;
}

int ListRemove(sList *list,int (*move_node)(void *),sList *node)
{
	if(!node) return -1;
	
    sList *it;


    for(it = list->next; it != list; it = it->next) 
    {
		if(!it) return -1;
		
        if(node == it) 
        {
			if(move_node)
			{
				if(0==move_node((void *)node))
				{
					break;
				}
			}
			else
			{
				break;
			}
        }
    }

    //没有找到该结点
    if (it == list) 
	{
		printf("[ListDebug]not file list node\n");
		return -1;
    }

    node->next->prev = node->prev;
    node->prev->next = node->next;

    return 0;
}

int ListTraverse(sList *list, int (*visit)(void *,void *), void * extra)
{
	if(!visit) return -1;
    sList * it = NULL;

    for (it = list->next; it != list; it = it->next) 
    {
		if(!it) 
		{
			return -1;
		}
        // 如果返回HI_TRUE，跳出循环
        if(!visit((void *)it,extra))
        {
           return 0;
        }
    }
    return -1;
}

int ListClear(sList *list,int (*free_node)(void *,void *),void *cb)
{
	if(!free_node) return -1;
	
    sList *next = NULL;
	sList *it = NULL;

	
    for (it = list->next; it != list;) 
    {
        next = it->next;
		free_node((void *)it,cb);
        it = next;
    }

    list->next = list;
    list->prev = list;

    return 0;
}






















