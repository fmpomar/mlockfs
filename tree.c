#include "tree.h"
#include "linkedlist.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include <pthread.h>

#include "dir.h"
#include "file.h"

LinkedList* parsePath(const char* path) {
	char* token;
	char* string = strdup(path);
	char* freePtr = string;
	LinkedList* nameList = linkedListCreate();

	while ((token = strsep(&string,"/")) != NULL) {
		if (strlen(token) > 0)
			linkedListPush(nameList, strdup(token));
	}

	free(freePtr);
	return nameList;
}

void* freeNameListItem(void* item, void* data) {
	free(item);
	return NULL;
}

void freeNameList(LinkedList* nameList) {
	linkedListMap(nameList, freeNameListItem, NULL);
	linkedListDestroy(nameList);
}

inline int isDirectory(INode* inode) {
	return S_ISDIR(inode->stat.st_mode);
}

inline int isRegular(INode* inode) {
	return S_ISREG(inode->stat.st_mode);
}

void* linkFoldr(void* result, void* current, void* data) {
	Link* resultLink = (Link*) result;
	char* currentName = (char*) current;
	INode* resultNode;
	if (!resultLink || !currentName) return NULL;
	resultNode = resultLink->inode;

	if (isDirectory(resultNode)) {
		return directoryLinkGet((Directory*) resultNode->payload, currentName);
	} else {
		return NULL; // A file cannot have any subdirs!
	}
}

INode* getNodeByNameList(Link* root, LinkedList* nameList) {
	Link* result = linkedListFoldR(nameList, linkFoldr, root, NULL);
	if (result)
		return result->inode;
	else
		return NULL;
}

INode* getNodeByPath(Link* root, const char* path) {
	LinkedList* nameList = parsePath(path);
	INode* result = getNodeByNameList(root, nameList);
	freeNameList(nameList);
	return result;
}

INode* getParentNodeByPath(Link* root, const char* path) {
	LinkedList* nameList = parsePath(path);
	INode* result;
	if (linkedListEmpty(nameList)) {
		freeNameList(nameList);
		return NULL;
	}
	linkedListPop(nameList);
	result = getNodeByNameList(root, nameList);
	freeNameList(nameList);
	return result;
}

char* getBasename(const char* path) {
	LinkedList* nameList = parsePath(path);
	char* basename = strdup(linkedListPop(nameList));
	freeNameList(nameList);
	return basename;
}

Link* createRoot() {
	Link* root = malloc(sizeof(Link));
	INode* rootNode = malloc(sizeof(INode));
	Directory* rootDirectory = directoryCreate();
	struct stat * nodeStat = &(rootNode->stat);

	root->name = "";
	root->inode = rootNode;
	memset(&(rootNode->stat), 0, sizeof(struct stat));

	nodeStat->st_mode = S_IFDIR | 0755;
	nodeStat->st_nlink = 2;

	nodeStat->st_atime = time(NULL);
	nodeStat->st_mtime = time(NULL);
	nodeStat->st_ctime = time(NULL);

	nodeStat->st_uid = getuid();
	nodeStat->st_gid = getgid();

	rootNode->payload = rootDirectory;
	return root;
}

void chownINode(INode* node, uid_t uid, gid_t gid) {
	if (!node) return;
	if (gid != (-1))
		node->stat.st_gid = gid;
	if (uid != (-1))
		node->stat.st_uid = uid;
}

void chmodINode(INode* node, mode_t mode) {
	if (!node) return;
	node->stat.st_mode = mode;
}

void utimensINode(INode* node, const struct timespec tv[]) {
	if (!node) return;
	node->stat.st_atim = tv[0];
	node->stat.st_mtim = tv[1];
}

INode* createINode(INode* parentNode, const char* name, mode_t mode) {
	INode* newNode;
	struct stat * nodeStat;

	if (!isDirectory(parentNode)) return NULL;
	if (!name) return NULL;

	if (isDirectory(parentNode)) {
		newNode = malloc(sizeof(INode));
		nodeStat = &(newNode->stat);

		memcpy(nodeStat, &(parentNode->stat), sizeof(struct stat));

		nodeStat->st_mode = mode;
		nodeStat->st_atime = time(NULL);
		nodeStat->st_mtime = time(NULL);
		nodeStat->st_ctime = time(NULL);
		nodeStat->st_nlink = 0;

		switch (mode & S_IFMT) {
			case S_IFDIR:
				newNode->payload = directoryCreate();
				break;
			case S_IFREG:
				newNode->payload = fileCreate();
				break;
			default:
				free(newNode);
				return NULL;
				break;
		}
		
		linkINode(parentNode, name, newNode);
		return newNode;

	} 

	return NULL;
}

void destroyINode(INode* node) {
	if (node) {
		switch (node->stat.st_mode & S_IFMT) {
			case S_IFDIR:
				directoryDestroy((Directory*)node->payload);
				break;
			case S_IFREG:
				fileDestroy((File*)node->payload);
				break;
			default:
				break;
		}
		free(node);
	}
}


int linkINode(INode* parentNode, const char* name, INode* node) {
	if (!name) return 0;

	if (parentNode && node && isDirectory(parentNode)) {
		node->stat.st_nlink++;
		directoryLinkAdd((Directory*)parentNode->payload, name, node);
		return 1;
	}

	return 0;
}

int unlinkINode(INode* parentNode, const char* name) {
	Link* link;
	if (isDirectory(parentNode)) {
		link = directoryLinkGet((Directory*)parentNode->payload, name);
		if (link) {
			link->inode->stat.st_nlink--;
			if (!link->inode->stat.st_nlink)
				destroyINode(link->inode);
			directoryLinkRemove((Directory*)parentNode->payload, name);
		}
		return 1;
	}
	return 0;
}

INode* addDummyDir(INode* inode, char*name) {
	Directory* dir = (Directory*) inode->payload;
	Link* dummyLink;
	INode* dummyNode;
	Directory* dummyDir;

	printf("add dummy dir\n");
	if (isDirectory(inode)) {
		dummyLink = malloc(sizeof(Link));
		dummyNode = malloc(sizeof(INode));
		dummyDir = malloc(sizeof(Directory));
		dummyLink->name = strdup(name);
		dummyLink->inode = dummyNode;
		memset(&(dummyNode->stat), 0, sizeof(struct stat));
		(dummyNode->stat).st_mode =  S_IFDIR | 0755;
		(dummyNode->stat).st_nlink = 1;
		dummyNode->payload = dummyDir;
		dummyDir->links = linkedListCreate();
		linkedListPush(dir->links, dummyLink);
		printf("added\n");
		return dummyNode;
	} 

	return NULL;
}

void addDummyFile(INode* inode, char*name) {
	Directory* dir = (Directory*) inode->payload;
	Link* dummyLink;
	INode* dummyNode;
	File* dummyFile;

	printf("add dummy file\n");
	if (inode && isDirectory(inode)) {
		dummyLink = malloc(sizeof(Link));
		dummyNode = malloc(sizeof(INode));
		dummyFile = malloc(sizeof(File));
		dummyLink->name = strdup(name);
		dummyLink->inode = dummyNode;
		memset(&(dummyNode->stat), 0, sizeof(struct stat));
		(dummyNode->stat).st_mode =  S_IFREG | 0755;
		(dummyNode->stat).st_nlink = 1;
		dummyNode->payload = dummyFile;
		dummyFile->contents = strdup("BONGIORNO\n");
		dummyFile->size = strlen((char*)dummyFile->contents);
		linkedListPush(dir->links, dummyLink);
		printf("added\n");
	} 
}