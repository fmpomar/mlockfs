

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
typedef void* (*LinkedListMapper)(void*,void*);
typedef void* (*LinkedListFolder) (void*, void*, void*);

typedef LinkedListNode* LinkedListIter;

LinkedList* linkedListCreate();
void linkedListDestroy(LinkedList* list);

int linkedListEmpty(LinkedList* list);

void* linkedListPop(LinkedList* list);
void linkedListPush(LinkedList* list, void* data);

void linkedListFilter(LinkedList* list, LinkedListFilter filter, void* filterData);
void* linkedListGetFirst(LinkedList* list, LinkedListFilter filter, void* filterData);

void linkedListMap(LinkedList* list, LinkedListMapper mapper, void* mapperData);

void* linkedListFoldR(LinkedList* list, LinkedListFolder folder, void* zero, void* folderData);
void* linkedListFoldL(LinkedList* list, LinkedListFolder folder, void* zero, void* folderData);

LinkedListIter linkedListIter(LinkedList * list);
void* linkedListIterData(LinkedListIter iter);
LinkedListIter linkedListIterNext(LinkedListIter iter);

#endif
