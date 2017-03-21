#ifndef __linkedlist_h__
#define __linkedlist_h__
/*
 * DO NOT INCLUDE THIS FILE!!!
 * INCLUDE AND USE list.h
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//Linked List Node declaration
typedef struct node_t {
	void* data;
	struct node_t *next;
} node ;

//Public functions
struct node_t* createHead(void* data);
void printList(struct node_t* head);
bool containsNode(struct node_t* head, void* data);
//returns new head
struct node_t* insertHead(struct node_t* head, void* data);
struct node_t* insertTail(struct node_t* head, void* data);
struct node_t* deleteNode(struct node_t* head, void* data);

int sizeList(struct node_t* head);
void freeList(struct node_t* head);

#endif
