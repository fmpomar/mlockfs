
#include "linkedlist.h"
#include <stdlib.h>

/*
	TAIL RECURSION IS ITS OWN REWARD
*/

LinkedListNode* linkedListNodeCreate(void* data, LinkedListNode* next) {
	LinkedListNode* node = (LinkedListNode*)malloc(sizeof(LinkedListNode));
	node->data = data;
	node->next = next;
	return node;
}

void linkedListNodeDestroy(LinkedListNode* node) {
	memset(node, 0, sizeof(LinkedListNode));
	free(node);
}

LinkedList* linkedListCreate() {
	LinkedList* new = (LinkedList*)malloc(sizeof(LinkedList));
	new->first = NULL;
	return new;
}

void* linkedListPop(LinkedList* list) {
	LinkedListNode* next;
	void* data;
	if (list->first) {
		next = list->first->next;
		data = list->first->data;
		linkedListNodeDestroy(list->first);
		list->first = next;
	}
	return data;
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
	memset(list, 0, sizeof(LinkedList));
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

void* linkedListGetFirst(LinkedList* list, LinkedListFilter filter, void* filterData) {
	LinkedListNode* node = list->first;
	while (node) {
		if (filter(node->data, filterData))
			return node->data;
		node = node->next;
	}
	return NULL;
}

void linkedListMap(LinkedList* list, LinkedListMapper mapper, void* mapperData) {
	LinkedListNode* node = list->first;
	while (node) {
		node->data = mapper(node->data, mapperData);
		node = node->next;
	}
}

void* linkedListFoldRRec(LinkedListNode* node, LinkedListFolder folder, void* zero, void* folderData) {
	if (!node) return zero;
	return folder(linkedListFoldRRec(node->next, folder, zero, folderData), node->data, folderData);
}

void* linkedListFoldR(LinkedList* list, LinkedListFolder folder, void* zero, void* folderData) {
	return linkedListFoldRRec(list->first, folder, zero, folderData);
}

void* linkedListFoldL(LinkedList* list, LinkedListFolder folder, void* zero, void* folderData) {
	LinkedListNode* node = list->first;
	void* result = zero;
	while (node) {
		result = folder(result, node->data, folderData);
		node = node->next;
	}
	return result;
}

LinkedListIter linkedListIter(LinkedList * list) {
	return list->first;
}

void* linkedListIterData(LinkedListIter iter) {
	return iter->data;
}

LinkedListIter linkedListIterNext(LinkedListIter iter) {
	return iter->next;
}