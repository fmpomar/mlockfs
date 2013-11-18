#ifndef __TREE_H
#define __TREE_H

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
int unlinkINode(INode* parentNode, const char* name);

void chownINode(INode* node, uid_t uid, gid_t gid);
void chmodINode(INode* node, mode_t mode);

#include "file.h"
#include "dir.h"

#endif
