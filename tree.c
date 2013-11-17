#include "tree.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include <pthread.h>

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
	return ((inode->stat).st_mode & S_IFDIR);
}

inline int isRegular(INode* inode) {
	return ((inode->stat).st_mode & S_IFREG);
}

int linkFilter(void* current, void* data) {
	Link* currentLink = (Link*) current;
	char* nameToFind = (char*) data;
	if (!currentLink) {printf("CAGAZO\n"); return 0;}
	printf("%s,%s\n",nameToFind,currentLink->name );
	return (strcmp(nameToFind, currentLink->name) == 0);
}

void* linkFoldr(void* result, void* current, void* data) {
	Link* resultLink = (Link*) result;
	char* currentName = (char*) current;
	INode* resultNode;
	if (!resultLink || !currentName) return NULL;
	resultNode = resultLink->inode;

	if (isDirectory(resultNode)) {
		return linkedListGetFirst(((Directory*)(resultNode->payload))->links, linkFilter, currentName);
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
	if (linkedListEmpty(nameList)) return NULL;
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
	Directory* rootDirectory = malloc(sizeof(Directory));
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
	rootDirectory->links = linkedListCreate();
	return root;
}

INode* createINode(INode* parentNode, const char* name, mode_t mode) {
	INode* newNode;
	struct stat * nodeStat;
	Directory* newDirectory;
	File* newFile;

	if (!isDirectory(parentNode)) return NULL;
	if ((!(mode | S_IFDIR)) && (!(mode | S_IFREG))) return NULL;
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

		if (mode | S_IFDIR) {
			newDirectory = malloc(sizeof(Directory));
			newDirectory->links = linkedListCreate();
			newNode->payload = newDirectory;
		} else if (mode | S_IFREG) {
			newFile = malloc(sizeof(File));
			newNode->payload = newFile;
		}
		
		newNode->payload = newDirectory;

		linkINode(parentNode, name, newNode);
		return newNode;

	} 

	return NULL;
}

int linkINode(INode* parentNode, const char* name, INode* node) {
	Link* newLink;

	if (!name) return 0;

	if (isDirectory(parentNode)) {
		newLink = malloc(sizeof(Link));
		newLink->name = strdup(name);
		newLink->inode = node;
		(node->stat).st_nlink++;
		linkedListPush(((Directory*)(parentNode->payload))->links, newLink);
		return 1;
	}

	return 0;
}

int removeLinkFilter(void* current, void* toRemove) {
	char* toRemoveName = (char*)toRemove;
	Link* currentLink = (Link*) current;
	return (strcmp(currentLink->name, toRemoveName)==0);
}

int unlinkINode(INode* parentNode, char* name) {
	if (isDirectory(parentNode)) {
		linkedListFilter(((Directory*)(parentNode->payload))->links, removeLinkFilter, name);
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
	if (isDirectory(inode)) {
		dummyLink = malloc(sizeof(Link));
		dummyNode = malloc(sizeof(INode));
		dummyFile = malloc(sizeof(File));
		dummyLink->name = strdup(name);
		dummyLink->inode = dummyNode;
		memset(&(dummyNode->stat), 0, sizeof(struct stat));
		(dummyNode->stat).st_mode =  S_IFREG | 0755;
		(dummyNode->stat).st_nlink = 1;
		dummyNode->payload = dummyFile;
		dummyFile->content = "BONGIORNO";
		linkedListPush(dir->links, dummyLink);
		printf("added\n");
	} 
}