#include "queue.h"

//Very simple queue implementation
queue* queueInit() {
	queue* q = (queue*)malloc(sizeof(queue));
	q->data = listInit();
	return q;
}
void queuePush(queue* q, void *item) {
	listInsertTail(q->data, item);
}
void* queuePeek(queue* q) {
	struct node_t* listhead = listGetHead(q->data);
	if (listhead == NULL) return NULL;
	return listhead->data;
}
void* queuePop(queue* q) {
	void* top_data = queuePeek(q);
	listDeleteNode(q->data, top_data);
	return top_data;
}
int queueSize(queue* q) {
	return listSize(q->data);
}
void queueFree(queue* q) {
	listFree(q->data);
	free(q);
}
void queuePrint(queue* q) {
	listPrint(q->data);
}
