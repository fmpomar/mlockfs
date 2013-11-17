#define FUSE_USE_VERSION  26
 
#include <fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include "tree.h"

Link* root;
static const char *mlockfs_str[2] = {"Hello World!\n","Hola Mundo!!\n"};
static const char *mlockfs_path = "/mlockfs";
int str = 0;



static int mlockfs_getattr(const char *path, struct stat *stbuf)
{
    INode* inode = getNodeByPath(root, path);
    if (!inode) return -ENOENT;
    memcpy(stbuf, &(inode->stat), sizeof(inode->stat));
    return 0;
}



static int mlockfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                         off_t offset, struct fuse_file_info *fi)
{

    INode* directoryNode;
    Directory* directory;
    Link* directoryEntry;
    LinkedListIter iter;

    directoryNode = getNodeByPath(root, path);

    if (!isDirectory(directoryNode))
      return -ENOENT;

    directory = (Directory*) directoryNode->payload;

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);

    iter = linkedListIter(directory->links);

    while (iter) {
      directoryEntry = (Link*) linkedListIterData(iter);
      filler(buf, directoryEntry->name, NULL, 0);
      iter = linkedListIterNext(iter);
    }

    return 0;

}

static int mlockfs_open(const char *path, struct fuse_file_info * ffi)
{
    INode* node = getNodeByPath(root, path);

    if(!node) return -ENOENT;

    switch (ffi->flags & O_ACCMODE) {
        case O_WRONLY:
        case O_RDONLY:
        case O_RDWR:
    }

    if((fi->flags & 3) != O_RDONLY)
        return -EACCES;

    return 0;
}

static int mlockfs_rmdir(const char *path) {
    INode* directoryNode = getNodeByPath(root, path);
    INode* parentNode;
    char* name;
    Directory * directory;
    if (!directoryNode) return -ENOENT;
    if (!isDirectory(directoryNode)) return -ENOTDIR;
    directory = ((Directory*)(directoryNode->payload));
    if (!linkedListEmpty(directory->links)) return -ENOTEMPTY;
    parentNode = getParentNodeByPath(root, path);
    if (!parentNode) return -EACCES;
    name = getBasename(path);
    unlinkINode(parentNode, name);
    return 0;

}

static int mlockfs_unlink(const char * path) {
    INode* fileNode = getNodeByPath(root, path);
    INode* parentNode;
    char* name;
    if (!fileNode) return -ENOENT;
    if (isDirectory(fileNode)) return -EPERM;
    parentNode = getParentNodeByPath(root, path);
    if (!parentNode) return -EACCES;
    name = getBasename(path);
    unlinkINode(parentNode, name);
    return 0;
}

static int mlockfs_create(const char * path, mode_t mode, struct fuse_file_info * ffi) {

}

static int mlockfs_read(const char *path, char *buf, size_t size, off_t offset,
                      struct fuse_file_info *fi)
{
    size_t len;
    (void) fi;
    
    str++;
    if (str > 1) str = 0;
    if(strcmp(path, mlockfs_path) != 0)
        return -ENOENT;

    len = strlen(mlockfs_str[str]);
    if (offset < len) {
        if (offset + size > len)
            size = len - offset;
        memcpy(buf, mlockfs_str[str] + offset, size);
    } else
        size = 0;

    return size;
}

static int mlockfs_mkdir(const char * path, mode_t mode) {
    INode* parentNode; 
    char* name;

    parentNode = getParentNodeByPath(root, path);

    if (!parentNode) return -EACCES;
    if (!isDirectory(parentNode)) return -EACCES;

    name = getBasename(path);
    createINode(parentNode, name, mode | S_IFDIR);

    free(name);

    return 0;
}


static struct fuse_operations mlockfs_oper = {
    .getattr   = mlockfs_getattr,
    .readdir = mlockfs_readdir,
    .open   = mlockfs_open,
    .read   = mlockfs_read,
    .mkdir  = mlockfs_mkdir,
    .rmdir  = mlockfs_rmdir,
    .unlink = mlockfs_unlink,
    .create = mlockfs_create
};

void* printFoldr(void* result, void* current, void* data) {
  Link* currentLink = (Link*) current;
  printf("%s\n", currentLink->name);
  return NULL;
}

int main(int argc, char *argv[])
{
    root = createRoot();
    addDummyFile(root->inode, "test1");
    addDummyFile(root->inode, "test2");
    addDummyFile(addDummyDir(root->inode, "folder"), "file");

    return fuse_main(argc, argv, &mlockfs_oper, NULL);
}
