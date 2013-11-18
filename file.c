#include "file.h"
#include <stdlib.h>
#include <string.h>


File* fileCreate() {
	File* newFile = malloc(sizeof(File));
	if(!newFile) return NULL;
	newFile->contents = NULL;
	newFile->size = 0;
	return newFile;
}

void fileDestroy(File* file) {
	if (!file) return ;
	memset(file->contents, 0, file->size);
	free(file->contents);
	memset(file, 0, sizeof(File));
	free(file);
}


int fileRead(File* file, void* buffer, size_t size, off_t offset) {
	int readLength;
	if (!file) return 0;
	if (!file->contents) return 0;
	if (offset < file->size) {
		readLength = size;
		if (readLength + offset > file->size)
			readLength = file->size - offset;
		memcpy(buffer, file->contents + offset, readLength);
		return readLength;
	} else {
		return 0;
	}
}

int fileWrite(File* file, const void* buffer, size_t size, off_t offset) {
	if (!file) return 0;
	if (offset + size > file->size)
		fileResize(file, offset+size);
	memcpy(file->contents + offset, buffer, size);
	return size;
}

int fileResize(File* file, size_t size) {
	if (!file) return 0;
	if((file->size - size) > 0) memset((file->contents)+size, 0, file->size - size);
	file->contents = realloc(file->contents, size);
	if (size < 1)
		file->contents = NULL;
	file->size = size;
	return 1;
}

size_t fileGetSize(File* file) {
	if (!file) return 0;
	return file->size;
}


