#ifndef DONOR_H
#define DONOR_H

/* Donor record shared by the heap and blood-group hash table. */
typedef struct {
    char name[50];
    char blood_group[5];
    char location[50];
    int distance;
    char last_donation_date[11];   // "DD-MM-YYYY" or "00-00-0000", plus terminator
    char contact[15];
} Donors;

#endif