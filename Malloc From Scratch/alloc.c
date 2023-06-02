/**
 * Malloc
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct _metadata_t
{
  unsigned int size;
  // 0 if free, 1 if in use
  int isUsed;
  struct _metadata_t *next;
  struct _metadata_t *prev;
} metadata_t;
// initialize pointers that will signify the start and end of heap

metadata_t *head = NULL;

void print()
{
  metadata_t *temp = head;
  while (temp != NULL)
  {
    printf("%d\n", temp->size);
    temp = (void *)temp + temp->size + sizeof(metadata_t);
  }
}
/**
 * Allocate space for array in memory
 *
 * Allocates a block of memory for an array of num elements, each of them size
 * bytes long, and initializes all its bits to zero. The effective result is
 * the allocation of an zero-initialized memory block of (num * size) bytes.
 *
 * @param num
 *    Number of elements to be allocated.
 * @param size
 *    Size of elements.
 *
 * @return
 *    A pointer to the memory block allocated by the function.
 *
 *    The type of this pointer is always void*, which can be cast to the
 *    desired type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory, a
 *    NULL pointer is returned.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/calloc/
 */
void *calloc(size_t num, size_t size)
{
  // implement calloc:
  void *ptr = malloc(num * size);
  // if ptr isnt null set every bit that is malloced to 0
  if (ptr != NULL)
    memset(ptr, 0, num * size);
  return ptr;
}

/**
 * Allocate memory block
 *
 * Allocates a block of size bytes of memory, returning a pointer to the
 * beginning of the block.  The content of the newly allocated block of
 * memory is not initialized, remaining with indeterminate values.
 *
 * @param size
 *    Size of the memory block, in bytes.
 *
 * @return
 *    On success, a pointer to the memory block allocated by the function.
 *
 *    The type of this pointer is always void*, which can be cast to the
 *    desired type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory,
 *    a null pointer is returned.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/malloc/
 */

void *malloc(size_t size)
{
  // implement malloc:
  if (size == 0)
    return NULL;

  if (head == NULL)
  {
    head = sbrk(sizeof(metadata_t));
    head->size = 0;
    head->isUsed = 1;
    head->next = NULL;
    head->prev = NULL;
  }

  metadata_t *curr = head;

  while (curr != NULL)
  {
    if (curr->isUsed == 0)
    {
      // all blocks are properly coalesced so either it will fit in the block or it needs to be added to the end
      if (curr->size >= size + sizeof(metadata_t))
      {
        metadata_t *block = (metadata_t *)((void *)curr + size + sizeof(metadata_t));
        block->size = curr->size - size - sizeof(metadata_t);
        block->isUsed = 0;
        block->next = curr->next;
        block->prev = curr;

        if (curr->next != NULL)
          curr->next->prev = block;

        curr->next = block;
        curr->size = size;
      }

      curr->isUsed = 1;

      return (void *)curr + sizeof(metadata_t);
    }

    curr = curr->next;
  }
  // if loop terminates then there was no space for the malloc and the block
  //  must be added to the end
  metadata_t *block = sbrk(sizeof(metadata_t) + size);
  block->size = size;
  block->isUsed = 1;
  block->next = NULL;
  block->prev = curr;
  // skips metadata and returns a pointer to the actual block
  return (void *)block + sizeof(metadata_t);
}

/**
 * Deallocate space in memory
 *
 * A block of memory previously allocated using a call to malloc(),
 * calloc() or realloc() is deallocated, making it available again for
 * further allocations.
 *
 * Notice that this function leaves the value of ptr unchanged, hence
 * it still points to the same (now invalid) location, and not to the
 * null pointer.
 *
 * @param ptr
 *    Pointer to a memory block previously allocated with malloc(),
 *    calloc() or realloc() to be deallocated.  If a null pointer is
 *    passed as argument, no action occurs.
 */
void free(void *ptr)
{
  // implement free
  // create a new block that points to the meta data of the block we want to free
  // assign pointers properly
  if (ptr != NULL)
  {
    metadata_t *to_free = (void *)ptr - sizeof(metadata_t);
    to_free->isUsed = 0;
    to_free->next = head;
    to_free->prev = head->prev;
    head = to_free;
  }
}

/**
 * Reallocate memory block
 *
 * The size of the memory block pointed to by the ptr parameter is changed
 * to the size bytes, expanding or reducing the amount of memory available
 * in the block.
 *
 * The function may move the memory block to a new location, in which case
 * the new location is returned. The content of the memory block is preserved
 * up to the lesser of the new and old sizes, even if the block is moved. If
 * the new size is larger, the value of the newly allocated portion is
 * indeterminate.
 *
 * In case that ptr is NULL, the function behaves exactly as malloc, assigning
 * a new block of size bytes and returning a pointer to the beginning of it.
 *
 * In case that the size is 0, the memory previously allocated in ptr is
 * deallocated as if a call to free was made, and a NULL pointer is returned.
 *
 * @param ptr
 *    Pointer to a memory block previously allocated with malloc(), calloc()
 *    or realloc() to be reallocated.
 *
 *    If this is NULL, a new block is allocated and a pointer to it is
 *    returned by the function.
 *
 * @param size
 *    New size for the memory block, in bytes.
 *
 *    If it is 0 and ptr points to an existing block of memory, the memory
 *    block pointed by ptr is deallocated and a NULL pointer is returned.
 *
 * @return
 *    A pointer to the reallocated memory block, which may be either the
 *    same as the ptr argument or a new location.
 *
 *    The type of this pointer is void*, which can be cast to the desired
 *    type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory,
 *    a NULL pointer is returned, and the memory block pointed to by
 *    argument ptr is left unchanged.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/realloc/
 */
void *realloc(void *ptr, size_t size)
{
  // implement realloc:
  if (ptr == NULL)
    return malloc(size);
  // checking to make sure that we are reallocing to size that is not 0
  if (size > 0)
  {
    // make a ptr that points to the end of the current block
    // make a new block that points to the same place as the the ptr
    void *realloc_ptr = ptr - sizeof(metadata_t);
    metadata_t *block = (metadata_t *)realloc_ptr;

    if (block->size >= size)
      return ptr;

    void *to_realloc = malloc(size);

    if (to_realloc != NULL)
    {
      // if new ptr isnt null the we need to copy in the bits of ptr into new_ptr

      memcpy(to_realloc, ptr, block->size);
      free(ptr);
    }
    return to_realloc;
  }
  else
  {
    // if we are reallocing to 0 then it is just treated as free
    free(ptr);
    return NULL;
  }
}