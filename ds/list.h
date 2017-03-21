#ifndef __list_h__
#define __list_h__
//wrapper for linked list. make it easier to use
#include "linkedlist.h"
typedef struct list_t {
	struct node_t* head;
} list;

//wrapper level functions
struct node_t* listGetHead(list *l);

//public functions from linkedlist.h
list* listInit();
void listPrint(list *l);
bool listContains(list* l, void* data);
void listInsertHead(list* l, void* data);
void listInsertTail(list* l, void* data);
void listDeleteNode(list* l, void* data);
int listSize(list* l);
void listFree(list* l);
#endif
