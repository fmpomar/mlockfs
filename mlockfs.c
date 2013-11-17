#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

static const char *basepath = "/tu_vieja";

static int getattr(const char *path, struct stat *stbuf) {
	int res = 0;
	memset(stbuf, 0, sizeof(struct stat));
	if (strcmp(path, "/") == 0) {
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
	} else if (strcmp(path, hello_path) == 0) {
		stbuf->st_mode = S_IFREG | 0444;
		stbuf->st_nlink = 1;
		stbuf->st_size = strlen(hello_str);
	} else
		res = -ENOENT;
	return res;
}

static int readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
	(void) offset;
	(void) fi;
	if (strcmp(path, "/") != 0)
		return -ENOENT;
	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);
	filler(buf, basepath + 1, NULL, 0);
	return 0;
}

static int open(const char *path, struct fuse_file_info *fi) {
	if (strcmp(path, basepath) != 0)
		return -ENOENT;
	if ((fi->flags & 3) != O_RDONLY)
		return -EACCES;
	return 0;
}

static int read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
	size_t len;
	(void) fi;
	if (strcmp(path, basepath) != 0)
		return -ENOENT;
	return 0;
}

static int write(void /*hay q ver q meirda va aca*/) {
	return 0;
}

static struct fuse_operations opers = {
	.getattr        = getattr,
	.readdir        = readdir,
	.open           = open,
	.read           = read,
	.write          = write
};

int main(int argc, char *argv[]) {
	return fuse_main(argc, argv, &opers, NULL);
}