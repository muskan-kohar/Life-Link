#ifndef HASHTABLE_H
#define HASHTABLE_H

#include"donor.h"

#define TABLE_SIZE 100


/*
    we craete a array of tables so we can access by index
    like table[i]

    but one multiple donors can be at the same index so here we need linked list

*/

//creating the linked list structure

typedef struct Node{
    Donors donor;
    struct Node* next;
}Node;

extern Node *hash_table[TABLE_SIZE];

//declaring functions
Node* create(Donors* donor);
unsigned long hash_fun(char *key);
void initialize_hash();
void insert_donor(Donors *donor);
void display();
Node* search_donor(Node* hash_table, BloodGroup key);

#endif