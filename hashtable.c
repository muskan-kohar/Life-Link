//Creating a hashtable to store database

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "hashtable.h"



//creating the array
Node *hash_table[TABLE_SIZE];

//function to create a node
Node* create(Donor *donor){
    Node* newnode = malloc(sizeof(Node));
    newnode->donor = *donor;
    newnode->next = NULL;

    return newnode;
}

/*
    Now we need a function to create the keys for hash table

    The keys will be created by hashing the input

*/

unsigned long hash_fun(char *key){

    //we use 5321 because it is standerd for djb2 hashing
    unsigned long hash = 5321;

    while(*key){
        hash = hash*33 + *key;
        key++;
    }

    return hash % TABLE_SIZE;
}

//intializing the hastables to NULL
void initialize_hash(){
    int i;
    for(i=0;i<TABLE_SIZE;++i){
        hash_table[i] = NULL;
    }
    return;
}

void insert_donor(Donor *donor){
    printf("Enter the name of donor:\n");
    scanf("%s", donor->name);
    printf("Enter the blood group of donor:\n");
    scanf("%s", donor->blood_group);
    printf("Enter the location of the person:\n");
    scanf("%s", donor->location);
    printf("Enter the distance of the person from the center:\n");
    scanf("%d", &donor->distance);
    printf("Enter the contact number of the donor:\n");
    scanf("%s", donor->contact);
    strcpy(donor->last_donation_date, "00-00-0000");


    char new[50];
    strcpy(new, donor->blood_group);
    strcat(new, "|");
    strcat(new, donor->location);
    //now generate hash based on blood group and location
    int key = hash_fun(new);

    if(hash_table[key] == NULL){
        //creating a newnode 
        Node* newnode = create(donor);
        hash_table[key] = newnode;
    }
    else{
        //creating a newnode 
        Node* newnode = create(donor);
        newnode->next = hash_table[key];
        hash_table[key] = newnode;
    }

}

//function to display the hash table
void display(){

    int i;
    for(i=0;i<TABLE_SIZE;i++){

        if(hash_table[i] != NULL){
            printf("%d : ", i+1);
        

            Node* temp = hash_table[i];

            while(temp != NULL){
                printf("%s | %s | %s | %s | %s | %d -> ",
                temp->donor.name,
                temp->donor.blood_group,
                temp->donor.location,
                temp->donor.last_donation_date,
                temp->donor.contact,
                temp->donor.distance);

                temp = temp->next;
            }   

            printf("\nNULL\n");

        }
    }

}

//search donor