#include "linkedlist.c"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef enum {
	DIR=0, MYFILE
} ElementType;

typedef void Element;

typedef struct Node {
	ElementType type;
	Element* element;
} Node;

typedef struct Directory {
	char* name;
	LinkedList* children;
} Directory;

typedef struct File {
	char* name;
	void* content;
} File;

LinkedList* parsePath(const char* path) {
	LinkedList* nameList = linkedListCreate();
	int idx = 0;
	int idy = 0;
	int len = strlen(path);
	while(idx < len){
		idy++;
		while(path[idy] != '/' && path[idy] != '\0'){
			idy++;
		}
		char* name = malloc(sizeof(char)*(idy-idx));
		memcpy(name, path+idx+1, sizeof(char)*(idy-idx-1));
		name[idx-idy] = '\0';
		idx = idy;
		linkedListPush(nameList, name);
	}
	return nameList;
}

Node* getNodeByPath(const char* path) {
	LinkedList* nameList = parsePath(path);
	
}

void* printFoldr(void* result, void* current, void* data) {
	printf("%s\n", (char*)current);
}

int main() {
	LinkedList* myList = parsePath("/tu/vieja/pepe");
	linkedListFoldL(myList, printFoldr, NULL, NULL);
}