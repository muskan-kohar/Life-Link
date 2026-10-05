//Creating a hashtable to store database

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "graph.h"
#include "hashtable.h"



//creating the array
Node *hash_table[TABLE_SIZE];

//function to create a node
Node* create(Donors *donor){
    Node* newnode = malloc(sizeof(Node));
    newnode->donor = *donor;
    newnode->next = NULL;

    return newnode;
}

//intializing the hastables to NULL
void initialize_hash(){
    int i;
    for(i=0;i<TABLE_SIZE;++i){
        hash_table[i] = NULL;
    }
    return;
}

void insert_donor(Donors *donor){
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

    //now generate hash based on blood group
    BloodGroup key = convert_To_enum(donor->blood_group);
    if(key == -1){
        printf("Invalid blood group\n");
        return;
    }

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


//search donor by their blood group
Node* search_donor(Node* hash_table, BloodGroup key){
    //initially making it a 0 sized 
    int length = 0;
    Node* the_bloodgroup = malloc(length*sizeof(Node));

    //we now go to the index key and take all the dnors one by one and fill in the array 
    //untill we reaches null

    Node* curr = hash_table[key];
    int i = 0;

    while(curr != NULL){
        
        //so we have one more donor so we resize the array by using realloc
        Node* the_bloodgroup = realloc(the_bloodgroup, (length + 1)*sizeof(Node));
        the_bloodgroup[i] = curr;
        i++;
        curr = curr->next;
    }

    return the_bloodgroup;

}