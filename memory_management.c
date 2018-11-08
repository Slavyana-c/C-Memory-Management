#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>

#include "memory_management.h"

#define MIN_BLOCK_SIZE 4096.0

typedef struct block {
	size_t size;
	struct block *next;
	struct block *prev;
	int free;
} Block;

Block *head = NULL;
Block *tail = NULL;

//void requestTest() {
//	printf("Start: %d \n", sbrk(0));
//	// Why Print random address???
//	Block *newBlock = sbrk(1);
//	Block *firstBlock = requestMemory(3000);
//	Block *secondBlock = requestMemory(5000);
//	Block *thirdBlock = requestMemory(1000);
//
//	printf("Address 1: %d, Address 2: %d, Address 3: %d", firstBlock, secondBlock, thirdBlock);
//}

// Ret a new block of memory
Block *requestMemory(size_t size) {
	Block *newBlock = (Block*) sbrk(0);
	printf("Heap top: %d \n", newBlock);

	// Request memory of size that is a multiple of 4096
	size_t requestSize = ceil(
			(size + sizeof(Block)) / MIN_BLOCK_SIZE) * MIN_BLOCK_SIZE;
	void *request = (Block *) sbrk(requestSize);

	printf("Size: %ld, Block: %ld\n", size, sizeof(Block));
	printf("Requesting %ld memory\n", requestSize);
	printf("New heap top: %d \n", sbrk(0));

	// Command failure
	if (request == (void*) -1) {
		return NULL;
		printf("Request failed\n");
	}
	if (tail) { // NULL on first request.
		tail->next = newBlock;
		newBlock->prev = tail;
		tail = newBlock;
	}

	newBlock->size = requestSize;
	newBlock->next = NULL;
	newBlock->free = 1;
	return newBlock;
}

// Returns a free block with enough memory, if any
// Using the "Best Fit" technique
Block *findFreeBlock(size_t size) {
	Block *currentBlock = head;
	Block *bestBlock = NULL;
	int sizeDifference = INT_MAX;
	int minDifference = INT_MAX;

	while (currentBlock != NULL) {
		if (!currentBlock->free || currentBlock->size < size) {
			tail = currentBlock;

			printf("Block checked, not free\n");
		} else {
			printf("Found free block of size %d\n", currentBlock->size);
			sizeDifference = currentBlock->size - size;
			if (sizeDifference < minDifference) {
				printf("New best block!- min diff: %d\n", sizeDifference);
				minDifference = sizeDifference;
				bestBlock = currentBlock;
			}

		}
		currentBlock = currentBlock->next;
	}

	return bestBlock;
}

void * _malloc(size_t size) {
	Block *block;

	if (size <= 0) {
		printf("Invalid size");
		return NULL;
	}

	if (head == NULL) { // First call.
		printf("Empty list\n");
		block = requestMemory(size);
		head = block;
	}

	printf("Searching for available block\n");
	block = findFreeBlock(size);

	// Failed to find free block.
	if (block == NULL) {
		printf("No free block found\n");
//		     block = requestMemory(size);
//		     if (!block)
//		     {
//		         return NULL;
//		     }
	}

	// Found free block
	else {
		// TODO: consider splitting block here.
		printf("Found free block!\n");
		if (block->size > size) {
			printf("Needs splitting\n");
			//split(block, size);
			block->free = 0;
		} else {
			block->free = 0;
			printf("Perfect size\n");
		}
	}
	return block + 1;
}

int main() {

	Block *firstBlock = _malloc(0);

	return 0;
}
