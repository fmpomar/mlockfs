#define FUSE_USE_VERSION  26
 
#include <fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include "tree.h"
#include <sys/mman.h>

Link* root;

static int mlockfs_getattr(const char *path, struct stat *stbuf)
{
    INode* inode = getNodeByPath(root, path);
    if (!inode) return -ENOENT;
    memcpy(stbuf, &(inode->stat), sizeof(inode->stat));
    if (isRegular(inode))
      stbuf->st_size = fileGetSize((File*)inode->payload);
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
    free(name);
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
    free(name);
    return 0;
}

static int mlockfs_create(const char * path, mode_t mode, struct fuse_file_info * ffi) {
    INode* parentNode; 
    char* name;

    parentNode = getParentNodeByPath(root, path);

    if (!parentNode) return -EACCES;
    if (!isDirectory(parentNode)) return -EACCES;

    name = getBasename(path);
    createINode(parentNode, name, mode);

    free(name);

    return 0;
}

static int mlockfs_truncate(const char * path, off_t size) 
{
    INode* node = getNodeByPath(root, path);

    if (!node) return -EACCES;
    if (isDirectory(node)) return -EISDIR;
    if (size < 0) return -EINVAL;
    if (!isRegular(node)) return -EACCES;

    fileResize((File*)node->payload, size);

    return 0;
}

static int mlockfs_write(const char *path, const char *buf, size_t size, off_t offset,
                      struct fuse_file_info *fi)
{
    INode* node = getNodeByPath(root, path);

    if (!node) return -EACCES;
    if (isDirectory(node)) return -EISDIR;
    if (!isRegular(node)) return -EACCES;

    return fileWrite((File*)node->payload, buf, size, offset);


}

static int mlockfs_read(const char *path, char *buf, size_t size, off_t offset,
                      struct fuse_file_info *fi)
{
    INode* node = getNodeByPath(root, path);

    if (!node) return -EACCES;
    if (isDirectory(node)) return -EISDIR;
    if (!isRegular(node)) return -EACCES;

    return fileRead((File*)node->payload, buf, size, offset);
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

static int mlockfs_chmod(const char* path, mode_t mode) {

    INode* node = getNodeByPath(root, path);
    if (!node) return -EACCES;
    chmodINode(node, mode);

    return 0;

}

static int mlockfs_chown(const char* path, uid_t uid, gid_t gid) {

    INode* node = getNodeByPath(root, path);
    if (!node) return -EACCES;

    chownINode(node, uid, gid);

    return 0;

}

static int mlockfs_utimens(const char* path, const struct timespec tv[]) {
    INode* node = getNodeByPath(root, path);
    if (!node) return -EACCES;

    utimensINode(node, tv);

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
    .write  = mlockfs_write,
    .truncate = mlockfs_truncate,
    .create = mlockfs_create,
    .chmod = mlockfs_chmod,
    .chown = mlockfs_chown,
    .utimens = mlockfs_utimens
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

    // Esto le da el nombre a nuestro FS.
    mlockall(MCL_CURRENT|MCL_FUTURE);

    return fuse_main(argc, argv, &mlockfs_oper, NULL);
}
