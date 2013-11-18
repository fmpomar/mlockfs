#include "dir.h"
#include <stdlib.h>
#include <string.h>

Directory* directoryCreate() {
	Directory* newDirectory = malloc(sizeof(Directory));
	newDirectory->links = linkedListCreate();
	return newDirectory;
}

void directoryDestroy(Directory* directory) {
	linkedListDestroy(directory->links);
	free(directory);
}

LinkedList* directoryGetLinks(Directory* directory) {
	return directory->links;
}


void directoryLinkAdd(Directory* directory, const char* name, INode* node) {
	Link* newLink;
	if (!directory) return;
	newLink = malloc(sizeof(Link));
	newLink->inode = node;
	newLink->name = strdup(name);
	linkedListPush(directory->links, newLink);
}

int removeLinkFilter(void* current, void* toRemove) {
	char* toRemoveName = (char*)toRemove;
	Link* currentLink = (Link*) current;
	if (!strcmp(currentLink->name, toRemoveName)) {
		free(currentLink->name);
		free(currentLink);
		return 1;
	}
	return 0;
}

void directoryLinkRemove(Directory* directory, const char* name) {
	char* tmpName;
	if (!directory) return;
	tmpName = strdup(name);
	linkedListFilter(directory->links, removeLinkFilter, tmpName);
	free(tmpName);
}

int linkFilter(void* current, void* data) {
	Link* currentLink = (Link*) current;
	char* nameToFind = (char*) data;
	return (strcmp(nameToFind, currentLink->name) == 0);
}

Link* directoryLinkGet(Directory* directory, const char* name) {
	char* tmpName;
	Link* link;
	if (!directory) return NULL;
	tmpName = strdup(name);
	link = linkedListGetFirst(directory->links, linkFilter, tmpName);
	free(tmpName);
	return link;
}