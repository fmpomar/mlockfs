#ifndef __FILE_H
#define __FILE_H

#include "tree.h"
#include <stdlib.h>

typedef struct File {
	void* contents;
	size_t size;
} File;

File* fileCreate();
void fileDestroy(File* file);

int fileRead(File* file, void* buffer, size_t size, off_t offset);
int fileWrite(File* file, const void* buffer, size_t size, off_t offset);
int fileResize(File* file, size_t size);
size_t fileGetSize(File* file);

#endif