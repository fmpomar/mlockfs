
#include <stdlib.h>
#include <stdio.h>

typedef struct LinkedListNode {
	struct LinkedListNode* next;
	void* data;
} LinkedListNode;

typedef struct LinkedList {
	LinkedListNode* first;
} LinkedList;

LinkedListNode* linkedListNodeCreate(void* data, LinkedListNode* next) {
	LinkedListNode* node = (LinkedListNode*)malloc(sizeof(LinkedListNode));
	node->data = data;
	node->next = next;
	return node;
}

typedef int (*LinkedListFilter)(void*,void*);
typedef void (*LinkedListCallback)(void*,void*);

void linkedListNodeDestroy(LinkedListNode* node) {
	free(node);
}

LinkedList* linkedListCreate() {
	LinkedList* new = (LinkedList*)malloc(sizeof(LinkedList));
	new->first = NULL;
	return new;
}

void linkedListPop(LinkedList* list) {
	LinkedListNode* next;
	if (list->first) {
		next = list->first->next;
		linkedListNodeDestroy(list->first);
		list->first = next;
	}
}

void linkedListPush(LinkedList* list, void* data) {
	list->first = linkedListNodeCreate(data, list->first);
}

int linkedListEmpty(LinkedList* list) {
	return (!(list->first));
}

void linkedListDestroy(LinkedList* list) {
	while (!linkedListEmpty(list))
		linkedListPop(list);
	free(list);
}

LinkedListNode* linkedListFilterRec(LinkedListNode * node, LinkedListFilter filter, void* filterData) {
	LinkedListNode * next;
	if (!node) return NULL;
	next = node->next;
	if (filter(node->data, filterData)) {
		linkedListNodeDestroy(node);
		return linkedListFilterRec(next, filter, filterData);
	} else {
		node->next = linkedListFilterRec(node->next, filter, filterData);
		return node;
	}
}

void linkedListFilter(LinkedList* list, LinkedListFilter filter, void* filterData) {
	list->first = linkedListFilterRec(list->first, filter, filterData);
}


void linkedListIterate(LinkedList* list, LinkedListCallback callback, void* callbackData) {
	LinkedListNode* node = list->first;
	while (node) {
		callback(node->data, callbackData);
		node = node->next;
	}
}

void printCallback(void* element, void* data) {
	printf("%s\n", (char*)element);
}

int removeFilter(void* element, void* data) {
	return (strcmp((char*)element,(char*)data)==0);
}

int main(int argc, char const *argv[])
{
	LinkedList* myLinkedList = linkedListCreate();

	linkedListIterate(myLinkedList, printCallback, NULL);
	linkedListPop(myLinkedList);

	linkedListPush(myLinkedList, "poronga");
	linkedListPush(myLinkedList, "chota");
	linkedListPush(myLinkedList, "verga");
	linkedListIterate(myLinkedList, printCallback, NULL);

	printf("removing\n");

	linkedListFilter(myLinkedList, removeFilter, "chotax");

	printf("removed\n");

	linkedListIterate(myLinkedList, printCallback, NULL);

	return 0;
}