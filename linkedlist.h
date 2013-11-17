

#ifndef __LINKEDLIST_H
#define __LINKEDLIST_H

typedef struct LinkedListNode {
	struct LinkedListNode* next;
	void* data;
} LinkedListNode;

typedef struct LinkedList {
	LinkedListNode* first;
} LinkedList;

typedef int (*LinkedListFilter)(void*,void*);
typedef void (*LinkedListCallback)(void*,void*);

LinkedList* linkedListCreate();
void linkedListDestroy(LinkedList* list);

int linkedListEmpty(LinkedList* list);

void linkedListPop(LinkedList* list);
void linkedListPush(LinkedList* list, void* data);

void linkedListFilter(LinkedList* list, LinkedListFilter filter, void* filterData);
void* linkedListGetFirst(LinkedList* list, LinkedListFilter filter, void* filterData);
void linkedListIterate(LinkedList* list, LinkedListCallback callback, void* callbackData);

#endif
