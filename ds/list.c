#include "list.h"
/* THIS FILE IS A WRAPPER OF LINKEDLIST.C
*/

//wrapper level functions
struct node_t* listGetHead(list *l) {
	return l->head;
}

//public functions from linkedlist.h
list* listInit() {
	list* l = (list* )malloc(sizeof(list));
	l->head = NULL;
	return l;
}
void listPrint(list *l) {
	printList(l->head);
}
bool listContains(list* l, void* data) {
	return containsNode(l->head, data);
}
void listInsertHead(list* l, void* data) {
	l->head = insertHead(l->head, data);
}
void listInsertTail(list* l, void* data) {
	l->head = insertTail(l->head, data);
}
void listDeleteNode(list* l, void* data) {
	l->head = deleteNode(l->head, data);
}
int listSize(list* l) {
	return sizeList(l->head);
}
void listFree(list* l) {
	freeList(l->head);
}
