#ifndef HEAP_H
#define HEAP_H

#include "donor.h"

typedef struct {
    Donors *data;     
    int size;        
    int capacity; 
} MinHeap;

/*
  Sets the heap to empty (data = NULL, size = 0, capacity = 0).
  Allocates nothing, so it cannot fail.
*/

void heap_init(MinHeap *heap);

/*
  Frees the array (if any) and resets the heap to empty.
  Safe to call on a heap that never received a donor.
*/

void heap_free(MinHeap *heap);

/*
  Adds a donor. Allocates the array on the first insert and grows it when full.
  Returns 0 on success, -1 if allocation fails.
*/

int heap_insert(MinHeap *heap, Donors d);

/*
   Removes and returns the donor with the smallest distance.
   The caller must make sure the heap is not empty (heap->size > 0).
*/

Donors heap_extract_min(MinHeap *heap);

/*
   Finds the nearest ACTIVE donor.
   Returns  1 and copies that donor into *result if one is found,
            0 if the heap has no active donor,
           -1 if a temporary allocation fails.
   In every case the heap holds the same donors afterwards: everything popped during the search is reinserted.
*/

int heap_getmin_active(MinHeap *heap, Donors *result);

#endif