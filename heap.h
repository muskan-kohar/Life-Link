#ifndef HEAP_H
#define HEAP_H

#include "donor.h"
#include "graph.h"

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

/* Keep one distance heap per blood group synchronized with donor records. */
void heap_reset_donor_index(void);
/* Add returns 0 on success or -1 on invalid input/allocation failure. */
int heap_add_donor(BloodGroup group, Donors donor);
/* Remove/update return 1 on success, 0 if not found, or -1 on failure. */
int heap_remove_donor(BloodGroup group, int donor_id);
int heap_update_donor(Donors donor);

/* Find the nearest eligible donor across compatible groups without rebuilding. */
/* Returns 1 when found, 0 when none are eligible, or -1 for an invalid index. */
int heap_getmin_active(const BloodGroup compatible[], int count,
                       Donors *result, BloodGroup *donor_group);

#endif
