#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <time.h>
#include "hashtable.h"

Node *hash_table[TABLE_SIZE];

static int valid_donation_date(const char *date){
    if (date == NULL)
        return 0;
    if (strcmp(date, "00-00-0000") == 0)
        return 1;
    if (strlen(date) != 10 || date[2] != '-' || date[5] != '-')
        return 0;

    for (int i = 0; i < 10; i++) {
        if (i != 2 && i != 5 && (date[i] < '0' || date[i] > '9'))
            return 0;
    }

    int day = (date[0] - '0') * 10 + date[1] - '0';
    int month = (date[3] - '0') * 10 + date[4] - '0';
    int year = (date[6] - '0') * 1000 + (date[7] - '0') * 100 +
               (date[8] - '0') * 10 + date[9] - '0';
    if (year < 1 || month < 1 || month > 12)
        return 0;

    static const int days_in_month[] = {31, 28, 31, 30, 31, 30,
                                         31, 31, 30, 31, 30, 31};
    int month_length = days_in_month[month - 1];
    if (month == 2 && (year % 400 == 0 || (year % 4 == 0 && year % 100 != 0)))
        month_length++;
    if (day < 1 || day > month_length)
        return 0;

    time_t now = time(NULL);
    struct tm *current = localtime(&now);
    if (current == NULL)
        return 0;

    int current_year = current->tm_year + 1900;
    int current_month = current->tm_mon + 1;
    return year < current_year ||
           (year == current_year && month < current_month) ||
           (year == current_year && month == current_month &&
            day <= current->tm_mday);
}

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
    printf("Enter last donation date (DD-MM-YYYY, or 00-00-0000 if never donated):\n");
    char date_input[32];
    if (scanf("%31s", date_input) != 1) {
        fprintf(stderr, "Unable to read last donation date.\n");
        return;
    }
    if (!valid_donation_date(date_input)) {
        fprintf(stderr, "Enter a valid donation date that is not in the future.\n");
        return;
    }

    BloodGroup key = convert_To_enum(donor->blood_group);
    if (key == INVALID_BLOOD_GROUP) {
        fprintf(stderr, "Invalid blood group.\n");
        return;
    }

    strcpy(donor->last_donation_date, date_input);
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

        printf("%s : ", bloodGroupName((BloodGroup)i));
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
