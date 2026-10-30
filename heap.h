#ifndef HEAP_H
#define HEAP_H

#include "donor.h"

/* Dynamic array representing a min-heap ordered by donor distance. */
typedef struct {
    Donors *data;     
    int size;        
    int capacity; 
} MinHeap;

/* Set the heap to an empty, unallocated state. */
void heap_init(MinHeap *heap);

/* Free the backing array and reset the heap. */
void heap_free(MinHeap *heap);

/* Insert a donor; return 0 on success or -1 on invalid state/allocation failure. */
int heap_insert(MinHeap *heap, Donors d);

/* Extract the nearest donor, or return a zero-initialized donor if empty. */
Donors heap_extract_min(MinHeap *heap);

/* Find the nearest eligible donor; return 1 if found, otherwise 0. */
int heap_getmin_active(MinHeap *heap, Donors *result);

#endif