#ifndef __DIR_H
#define __DIR_H

#include "tree.h"
#include "linkedlist.h"

typedef struct Directory {
	LinkedList* links;
} Directory;

Directory* directoryCreate();
void directoryDestroy(Directory* directory);
LinkedList* directoryGetLinks(Directory* directory);
void directoryLinkAdd(Directory* directory, const char*name, INode* node);
void directoryLinkRemove(Directory* directory, const char*name);
Link* directoryLinkGet(Directory* directory, const char* name);

#endif