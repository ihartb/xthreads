#include "linkedlist.h"
#include "stdio.h"
#include "stdlib.h"

//Create a new node, return it
struct node_t* createHead(void* data) {
	struct node_t* p = (struct node_t*)malloc(sizeof(struct node_t));
	p->data = data;
	p->next = NULL;
	return p;
}

//Print the list.
//MUST HAVE #define DEBUG 1
//OR IT WONT RUN
void printList(struct node_t* head) {
	struct node_t* r = head;
	while(r != NULL) {
	//	if (DEBUG) printf("llnode[addr=%p, dataptr=%p]\n", r, r->data);
		r=r->next;
	}
	return;
}

//Check if the list contains a node with data
bool containsNode(struct node_t* head, void* data) {
	struct node_t* ptr = head;
	while( ptr != NULL) {
		if (ptr->data == data) return true;
		ptr=ptr->next;
	}
	return false;
}

//Insert a new head of the list
struct node_t* insertHead(struct node_t* head, void* data) {
	struct node_t* temp = (struct node_t*)malloc(sizeof(struct node_t));
	temp->data = data;
	temp->next = head;
	return temp;
}

//Insert a  new node at the tail
struct node_t* insertTail(struct node_t* head, void* data) {
	struct node_t* newNode = (struct node_t*)malloc(sizeof(struct node_t));
	if (newNode == NULL)
		exit(0);

	newNode->data = data;
	newNode->next = NULL;

	if (head == NULL) {
		head = newNode;
		return head;
	}

	if (head->next == NULL) {
		head->next = newNode;
	} else {
		struct node_t* cur = head;
		while(true) {
			if (cur->next == NULL) {
				cur->next = newNode;
				break;
			}
			cur = cur->next;
		}
	}
	return head;
}

//Delete a node with data = data. We only use pointers as data so it's okay
struct node_t* deleteNode(struct node_t* head, void* data) {
	struct node_t* temp=head;
	struct node_t* prev;
	while(temp != NULL) {
		if (temp->data==data) {
			if (temp==head) {
				head=temp->next;
				return head;
			} else {
				prev->next=temp->next;
			}
			free(temp);
			return head;
		} else {
			prev=temp;
			temp=temp->next;
		}
	}
	return head;
}

//Return the size of the list
int sizeList(struct node_t* head) {
	struct node_t* ptr = head;
	int s = 0;
	while(ptr != NULL) {
		s++;
		ptr=ptr->next;
	}
	return s;
}

//Free the list
void freeList(struct node_t* head) {
	struct node_t* ptr = head;
	if (ptr == NULL) return;
	while(ptr->next != NULL) {
		struct node_t* next = ptr->next;
		free(ptr);
		ptr = next;
	}
	free(ptr);
}
