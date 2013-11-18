#include "file.h"
#include <stdlib.h>
#include <string.h>


File* fileCreate() {
	File* newFile = malloc(sizeof(File));
	newFile->contents = NULL;
	newFile->size = 0;
	return newFile;
}

void fileDestroy(File* file) {
	free(file->contents);
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


