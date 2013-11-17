#ifndef __TREE_H
#define __TREE_H

#include "linkedlist.h"
#include <sys/stat.h>

typedef enum {
	ELEMENT_DIR=0, ELEMENT_FILE
} ElementType;

typedef struct INode {
	struct stat stat;
	void* payload;
} INode;

typedef struct Link {
	char* name;
	INode * inode;
} Link;

typedef struct File {
	void* content;
} File;

typedef struct Directory {
	LinkedList* links;
} Directory;


INode* getNodeByPath(Link* root, const char* path);
INode* getParentNodeByPath(Link* root, const char* path);
char* getBasename(const char* path);

Link* createRoot();
void addDummyFile(INode* inode, char*name);;
INode* addDummyDir(INode* inode, char*name);;

inline int isDirectory(INode* inode);
inline int isRegular(INode* inode);

int linkINode(INode* parentNode, const char* name, INode* node);
INode* createINode(INode* parentNode, const char* name, mode_t mode);
int unlinkINode(INode* parentNode, char* name);

#endif
