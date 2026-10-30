#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include "hashtable.h"

Node *hash_table[TABLE_SIZE];

/* Allocate a list node and copy the donor record into it. */
Node *create(Donors *donor){
    if (donor == NULL)
        return NULL;

    Node *newnode = malloc(sizeof(*newnode));
    if (newnode == NULL)
        return NULL;

    newnode->donor = *donor;
    newnode->next = NULL;
    return newnode;
}

/* Free all donor chains and clear every hash-table bucket. */
void initialize_hash(void){
    for (int i = 0; i < TABLE_SIZE; i++) {
        Node *current = hash_table[i];
        while (current != NULL) {
            Node *next = current->next;
            free(current);
            current = next;
        }
        hash_table[i] = NULL;
    }
}

/* Read donor fields, validate them, then prepend the donor to its group list. */
void insert_donor(Donors *donor){
    if (donor == NULL){
        fprintf(stderr, "Cannot insert a null donor.\n");
        return;
    }
    printf("Enter the name of donor:\n");
    scanf("%49s", donor->name);
    printf("Enter the blood group of donor:\n");
    scanf("%4s", donor->blood_group);
    printf("Enter the location of the person:\n");
    scanf("%49s", donor->location);
    printf("Enter the distance of the person from the center:\n");
    scanf("%d", &donor->distance);
    printf("Enter the contact number of the donor:\n");
    scanf("%14s", donor->contact);
    printf("Enter the last donation date in 'DD-MM-YYYY' format:\n");
    scanf("%14s", donor->last_donation_date);

    //now generate hash based on blood group
    BloodGroup key = convert_To_enum(donor->blood_group);
    if((int)key == -1){
        printf("Invalid blood group\n");
        return;
    }

    printf("Enter the name of donor:\n");
    if (scanf("%49s", donor->name) != 1) {
        fprintf(stderr, "Unable to read donor name.\n");
        return;
    }
    printf("Enter the blood group of donor:\n");
    if (scanf("%4s", donor->blood_group) != 1) {
        fprintf(stderr, "Unable to read donor blood group.\n");
        return;
    }
    printf("Enter the location of the person:\n");
    if (scanf("%49s", donor->location) != 1) {
        fprintf(stderr, "Unable to read donor location.\n");
        return;
    }
    printf("Enter the distance of the person from the center:\n");
    char distance_input[32];
    if (scanf("%31s", distance_input) != 1) {
        fprintf(stderr, "Unable to read distance; it must be a non-negative integer.\n");
        return;
    }
    char *end;
    errno = 0;
    long distance = strtol(distance_input, &end, 10);
    if (errno == ERANGE || end == distance_input || *end != '\0' ||
        distance < 0 || distance > INT_MAX) {
        fprintf(stderr, "Distance must be a non-negative integer.\n");
        return;
    }
    donor->distance = (int)distance;
    printf("Enter the contact number of the donor:\n");
    if (scanf("%14s", donor->contact) != 1) {
        fprintf(stderr, "Unable to read donor contact number.\n");
        return;
    }

    BloodGroup key = convert_To_enum(donor->blood_group);
    if (key < O_NEG || key > AB_POS) {
        fprintf(stderr, "Invalid blood group.\n");
        return;
    }

    /* New donors have no donation date on record yet. */
    strcpy(donor->last_donation_date, "00-00-0000");
    Node *newnode = create(donor);
    if (newnode == NULL) {
        fprintf(stderr, "Unable to allocate memory for donor.\n");
        return;
    }

    newnode->next = hash_table[key];
    hash_table[key] = newnode;
}

/* Print the donor records in each non-empty blood-group bucket. */
void display(void){
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (hash_table[i] == NULL)
            continue;

        printf("%d : ", i + 1);
        Node *temp = hash_table[i];
        while (temp != NULL) {
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


//search donor by their blood group
Node* search_donor(Node** table, BloodGroup key){
    if(table == NULL || key < O_NEG || key > AB_POS){
        return NULL;
    }

    return table[key];
}
