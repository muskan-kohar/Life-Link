#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "donor.h"
#include "graph.h"

#define TABLE_SIZE 100

/* Each bucket contains a linked list of donor records. */
typedef struct Node{
    Donors donor;
    struct Node* next;
}Node;

extern Node *hash_table[TABLE_SIZE];

/* Create a heap-allocated list node containing a copy of donor. */
Node *create(Donors *donor);
/* Free all bucket lists and initialize the table to empty. */
void initialize_hash(void);
/* Read, validate, and add one donor to its blood-group bucket. */
void insert_donor(Donors *donor);
/* Display donors stored in the table. */
void display(void);
/* Return the first donor in a blood-group list, or NULL for an invalid group. */
Node* search_donor(Node** table, BloodGroup key);

#endif