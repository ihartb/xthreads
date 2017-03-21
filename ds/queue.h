#ifndef __queue_h__
#define __queue_h__

#include "list.h"

typedef struct queue_t {
	list* data;
} queue;

queue* queueInit();
void queuePush(queue* q, void *item);
void* queuePeek(queue* q);
void* queuePop(queue* q);
int queueSize(queue* q);
void queueFree(queue* q);
void queuePrint(queue* q);


#endif
