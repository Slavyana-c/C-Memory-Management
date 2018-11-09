#include <sys/types.h>
#include <unistd.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>

#include "memory_management.h"

#define MIN_BLOCK_SIZE 4096.0

typedef struct block
{
	size_t size;
	struct block *next;
	struct block *prev;
	int free;
} Block;

Block *head = NULL;

// Requests memory and returns a new block
Block *requestMemory(size_t size)
{
	Block *newBlock = (Block *)sbrk(0);
	printf("Heap top: %ld \n", (long)newBlock);

	// Request memory of size that is a multiple of 4096
	size_t requestSize = ceil(
							 (size + sizeof(Block)) / MIN_BLOCK_SIZE) *
						 MIN_BLOCK_SIZE;
	void *request = (Block *)sbrk(requestSize);

	printf("Size: %ld, Block: %ld, Requesting %ld memory\n", size, sizeof(Block), requestSize);
	printf("New heap top: %ld \n", (long)sbrk(0));

	// Command failure
	if (request == (void *)-1)
	{
		return NULL;
		printf("Request failed\n");
	}

	newBlock->size = requestSize - sizeof(Block);
	newBlock->next = NULL;
	newBlock->free = 1;
	return newBlock;
}

// Returns a free block with enough memory, if any
Block *findFreeBlock(size_t size)
{
	Block *currentBlock = head;
	Block *bestBlock = NULL;
	int sizeDifference = INT_MAX;
	int minDifference = INT_MAX;

	while (currentBlock != NULL)
	{
		if (!currentBlock->free || currentBlock->size < size)
		{
			printf("Block checked, not free\n");
		}
		else
		{
			printf("Found free block %ld of size %ld\n", (long)currentBlock,
				   (long)currentBlock->size);
			sizeDifference = currentBlock->size - size;
			if (sizeDifference < minDifference)
			{
				printf("New best block!- minimum difference: %d\n",
					   sizeDifference);
				minDifference = sizeDifference;
				bestBlock = currentBlock;
			}
		}
		currentBlock = currentBlock->next;
	}
	return bestBlock;
}

// Splits block if possible
void split(Block *bigBlock, size_t size)
{
	Block *new = (void *)((void *)bigBlock + size + sizeof(Block));
	new->size = (bigBlock->size) - size - sizeof(Block);
	new->free = 1;
	new->prev = bigBlock;
	new->next = bigBlock->next;
	bigBlock->size = size;
	bigBlock->free = 0;
	bigBlock->next = new;

	printf("Split successful: Block1: %ld, Block2: %ld\n", bigBlock->size,
		   new->size);
}

// Returns a pointer to the free space
void *_malloc(size_t size)
{
	Block *block;

	if ((int)size <= 0)
	{
		printf("Invalid size\n");
		return NULL;
	}

	if (head == NULL)
	{ // First call.
		printf("Empty list\n");
		block = requestMemory(size);
		head = block;
	}

	printf("Searching for available block\n");
	block = findFreeBlock(size);

	// Failed to find free block.
	if (block == NULL)
	{
		printf("No free block found\n");
		block = requestMemory(size);
		if (!block)
		{
			return NULL;
		}
	}

	// Found free block
	if (block->size - size > sizeof(Block))
	{
		printf("Needs splitting\n");
		split(block, size);
		block->free = 0;
	}
	else
	{
		block->free = 0;
		//printf("Formula: %d\n", block->size - size - sizeof(Block));
		if ((int)(block->size - size - sizeof(Block)) < 0)
		{
			printf("Not perfect size, but can't be split\n");
		}
		else
		{
			printf("Perfect size\n");
		}
	}
	return ((Block *)((void *)block + sizeof(Block)));
}

// Merges blocks when possible

Block *merge(Block *freeBlock)
{

	Block *prevBlock = freeBlock->prev;
	Block *nextBlock = freeBlock->next;
	int merged = 1;

	if (prevBlock != NULL && prevBlock->free)
	{
		printf("Previous block is free, size -> %zu\n", prevBlock->size);
		prevBlock->size += freeBlock->size + sizeof(Block);
		prevBlock->next = freeBlock->next;
		if(freeBlock->next != NULL) {
			freeBlock->next->prev = prevBlock;
		}

		freeBlock = prevBlock;
		nextBlock = freeBlock->next;
		printf("Successfully merged\n");
		printf("New block of size %zu\n", freeBlock->size);
	}

	if (nextBlock != NULL && nextBlock->free)
	{
		printf("Next block is free, size -> %zu\n", nextBlock->size);
		freeBlock->size += nextBlock->size + sizeof(Block);
		freeBlock->next = nextBlock->next;
		if(nextBlock->next != NULL) {
					nextBlock->next->prev = freeBlock;
		}


		printf("Successfully merged\n");
		printf("New block of size %zu\n", freeBlock->size);
	}

	printf("Successfully merged\n");
	printf("New block of size %zu\n", freeBlock->size);

	return freeBlock;
}

// Returns memory back to the OS
void returnMemory(Block* block) {
	void *endOfBlock = (void*) block + sizeof(Block) + block->size;
	printf("Block ends at %ld, top is %ld\n", (long) endOfBlock,
			(long) sbrk(0));

	if (endOfBlock == sbrk(0)) {
	
		if(block->prev != NULL) {
			block->prev->next = NULL;
		}
		if (head == block) {
			head = block->next;
			printf("This block was head\n");
		}

		int requestSize = -1 * (block->size + sizeof(Block));
		printf("Requesting: %d\n", requestSize);
		void *request = (Block *) sbrk(requestSize);
		// Command failure
		if (request == (void*) -1) {
			printf("Request failed\n");
			return;
		} else {
			printf("new %ld\n", (long) sbrk(0));
		}
	}
}

// Frees the block of memory
void _free(void *ptr)
{

	printf("%ld\n", (long)ptr);
	Block *block = (Block *)(ptr - sizeof(Block));
	block->free = 1;
	printf("Freed %ld, size %zu\n", (long)block, block->size);
	Block *mergedBlock = merge(block);
	//merge();
	returnMemory(mergedBlock);
}

// Prints List
void printList() {
	Block *current = head;
	int i = 1;
	while (current != NULL) {
		printf("Block%d: %ld -> size: %zu \n", i, (long) current,
				current->size);
		current = current->next;
		i++;
	}
}

int main()
{


	return 0;
}
