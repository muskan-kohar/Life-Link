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
    Donor donor;
    struct Node* next;
}Node;

extern Node *hash_table[TABLE_SIZE];

//declaring functions
Node* create(Donor* donor);
unsigned long hash_fun(char *key);
void initialize_hash();
void insert_donor(Donor *donor);
void display();
void search_donor();

#endif