#ifndef HASHTABLE_H
#define HASHTABLE_H

#define TABLE_SIZE 100


// structure for the details of a donor
typedef struct donors{
    char name[50];
    char bloodgrp[5];
    char location[50];
    int distance;
    char last_donation[11];
    char contact[15];
}donors;

/*
    we craete a array of tables so we can access by index
    like table[i]

    but one multiple donors can be at the same index so here we need linked list

*/

//creating the linked list structure

typedef struct Node{
    donors donor;
    struct Node* next;
}Node;

extern Node *hash_table[TABLE_SIZE];

//declaring functions
Node* create(donors* donor);
unsigned long hash_fun(char *key);
void initialize_hash();
void insert_donor(donors *donor);
void display();
void search_donor();

#endif


