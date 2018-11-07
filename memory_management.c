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

// Ret a new block of memory
Block *requestMemory(size_t size) {
  Block *newBlock = (Block*)sbrk(0);
  printf("Heap top: %d \n", newBlock);

  // Request memory of size that is a multiple of 4096
  size_t requestSize = ceil((size + sizeof(Block)) / MIN_BLOCK_SIZE) * MIN_BLOCK_SIZE;
  void *request = (Block *)sbrk(requestSize);

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
     newBlock->prev=tail;
     tail = newBlock;
   }

   newBlock->size = requestSize;
   newBlock->next = NULL;
   newBlock->free = 0;
   return newBlock;
 }

// Returns a free block with enough memory, if any
// Using the "Best Fit" technique
Block *findFreeBlock(size_t size) {
	 Block *currentBlock = head;
	 Block *bestBlock = NULL;
	 int sizeDifference = INT_MAX;
	 int minDifference = INT_MAX;

	 while(currentBlock != NULL)
	 {
		 if(!currentBlock->free || currentBlock->size < size)
		 	 {
		 	    tail = currentBlock;

		 	    printf("Block checked, not free\n");
		 	 }
		 	 else
		 	 {
		 		 printf("Found free block of size %d\n", currentBlock->size);
		 		 sizeDifference = currentBlock->size - size;
		 		 if(sizeDifference < minDifference)
		 		 {
		 			 printf("New best block!- min diff: %d\n", sizeDifference);
		 			 minDifference = sizeDifference;
		 			 bestBlock = currentBlock;
		 		 }

		 	 }
	 	  currentBlock = currentBlock->next;
	 }

	  return bestBlock;
}

int main(){
	printf("Start: %d \n", sbrk(0));
	// Why Print random address???
	Block *newBlock = sbrk(1);
	Block *firstBlock = requestMemory(3000);
	Block *secondBlock = requestMemory(5000);
	Block *thirdBlock = requestMemory(1000);

	printf("Address 1: %d, Address 2: %d, Addr 3: %d", firstBlock, secondBlock, thirdBlock);

	return 0;
}
