#ifndef LIST_H
#define LIST_H


#include "Sgd_types.h"









typedef struct _LIST
{
    struct _LIST *prev;
    struct _LIST *next;
}sList;











int ListInit(sList *list);
bool mrsSrvIsListEmpty(sList *list);
int ListLength(sList * list);
int ListInsertHead(sList *list,sList *node);
int ListInsertTail(sList *list,sList *node);
int ListRemove(sList *list,int (*move_node)(void *),sList *node);
int ListTraverse(sList *list, int (*visit)(void *,void *), void * extra);
int ListClear(sList *list,int (*free_node)(void *,void *),void *cb);

















#endif

