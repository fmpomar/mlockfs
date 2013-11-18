#include "file.h"
#include <stdlib.h>
#include <string.h>

#define BLOCK_SIZE 4096 //(32*1024*1024)
#define ROUND_BLOCK(x) (((x/BLOCK_SIZE)+1)*BLOCK_SIZE)

File* fileCreate() {
	File* newFile = malloc(sizeof(File));
	if(!newFile) return NULL;
	newFile->contents = NULL;
	newFile->size = 0;
	return newFile;
}

void fileDestroy(File* file) {
	if (!file) return;
	fileResize(file, 0);
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
	
	if(file->size > size)
		memset((file->contents)+size, 0, file->size - size);

	if (size > 0 && (ROUND_BLOCK(size) != ROUND_BLOCK(file->size) || file->size == 0)) {
		//printf("realloc: %d\n", ROUND_BLOCK(size));
		file->contents = realloc(file->contents, ROUND_BLOCK(size));
	} else if (size == 0 && file->size > 0) {
		free(file->contents);
		file->contents = NULL;
	}

	file->size = size;
	return 1;
}

size_t fileGetSize(File* file) {
	if (!file) return 0;
	return file->size;
}


