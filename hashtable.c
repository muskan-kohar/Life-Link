#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <time.h>
#include "hashtable.h"
#include "heap.h"

Node *hash_table[TABLE_SIZE];

/* Assigns a different ID to each donor registered during this run. */
static unsigned long long next_donor_id = 1001;

/* Read one complete line, including spaces, and reject overlong or empty input. */
static int read_line(const char *prompt, char *buffer, size_t size){
    if (buffer == NULL || size < 2 || size > INT_MAX)
        return 0;

    printf("%s", prompt);
    if (fgets(buffer, (int)size, stdin) == NULL)
        return 0;

    size_t length = strlen(buffer);
    if (length > 0 && buffer[length - 1] == '\n') {
        buffer[--length] = '\0';
    } else {
        int ch = getchar();
        if (ch != '\n' && ch != EOF) {
            while ((ch = getchar()) != '\n' && ch != EOF) {
            }
            return 0;
        }
    }

    if (length > 0 && buffer[length - 1] == '\r')
        buffer[--length] = '\0';

    return buffer[0] != '\0';
}

/* Accept exactly 10 decimal digits for an Indian mobile contact number. */
static int valid_contact(const char *contact){
    if (contact == NULL || strlen(contact) != 10)
        return 0;

    for (int i = 0; i < 10; i++) {
        if (contact[i] < '0' || contact[i] > '9')
            return 0;
    }

    return 1;
}

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
    heap_reset_donor_index();
}

/* Read donor fields, validate them, then prepend the donor to its group list. */
void insert_donor(Donors *donor){
    if (donor == NULL){
        fprintf(stderr, "Cannot insert a null donor.\n");
        return;
    }

    if (!read_line("Enter the name of donor: ", donor->name, sizeof(donor->name))) {
        fprintf(stderr, "Unable to read a valid donor name.\n");
        return;
    }
    if (!read_line("Enter the blood group of donor: ", donor->blood_group, sizeof(donor->blood_group))) {
        fprintf(stderr, "Unable to read donor blood group.\n");
        return;
    }
    if (!read_line("Enter the location of the person: ", donor->location, sizeof(donor->location))) {
        fprintf(stderr, "Unable to read a valid donor location.\n");
        return;
    }

    char distance_input[32];
    if (!read_line("Enter the distance of the person from the center: ", distance_input, sizeof(distance_input))) {
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

    if (!read_line("Enter the contact number of the donor: ", donor->contact, sizeof(donor->contact))) {
        fprintf(stderr, "Unable to read donor contact number.\n");
        return;
    }
    if (!valid_contact(donor->contact)) {
        fprintf(stderr, "Contact number must contain exactly 10 digits.\n");
        return;
    }

    char date_input[32];
    if (!read_line("Enter last donation date (DD-MM-YYYY, or 00-00-0000 if never donated): ", date_input, sizeof(date_input))) {
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

    if (next_donor_id > (unsigned long long)INT_MAX) {
        fprintf(stderr, "Unable to generate another donor ID.\n");
        return;
    }
    donor->donor_id = (int)next_donor_id;

    Node *newnode = create(donor);
    if (newnode == NULL) {
        fprintf(stderr, "Unable to allocate memory for donor.\n");
        return;
    }

    if (heap_add_donor(key, *donor) != 0) {
        free(newnode);
        fprintf(stderr, "Unable to index donor by distance.\n");
        return;
    }

    next_donor_id++;
    newnode->next = hash_table[key];
    hash_table[key] = newnode;
    printf("Donor registered successfully. Donor ID: %d\n", donor->donor_id);
}

/* Record a donation and keep the donor list and distance index synchronized. */
int update_donation_date(int donor_id, Donors *updated_donor){
    if (donor_id <= 0 || updated_donor == NULL)
        return -1;

    time_t now = time(NULL);
    struct tm *current = localtime(&now);
    if (current == NULL)
        return -1;

    Node *match = NULL;
    for (int i = 0; i < TABLE_SIZE && match == NULL; i++) {
        for (Node *node = hash_table[i]; node != NULL; node = node->next) {
            if (node->donor.donor_id == donor_id) {
                match = node;
                break;
            }
        }
    }
    if (match == NULL)
        return 0;

    Donors updated = match->donor;
    int date_length = snprintf(updated.last_donation_date,
                               sizeof(updated.last_donation_date),
                               "%02d-%02d-%04d", current->tm_mday,
                               current->tm_mon + 1, current->tm_year + 1900);
    if (date_length < 0 ||
        date_length >= (int)sizeof(updated.last_donation_date))
        return -1;

    /* Update the indexed copy before publishing the new record in the list. */
    if (heap_update_donor(updated) != 1)
        return -1;

    match->donor = updated;
    *updated_donor = updated;
    return 1;
}

/* Print the donor records in each non-empty blood-group bucket. */
void display(void){
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (hash_table[i] == NULL)
            continue;

        printf("%s : ", bloodGroupName((BloodGroup)i));
        Node *temp = hash_table[i];
        while (temp != NULL) {
            printf("ID: %d | %s | %s | %s | %s | %s | %d -> ",
                   temp->donor.donor_id,
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
