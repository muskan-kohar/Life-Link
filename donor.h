#ifndef DONOR_H
#define DONOR_H

typedef struct {
    char name[50];
    char blood_group[5];
    char location[50];
    int distance;
    char last_donation_date[11];   // "DD-MM-YYYY" + null terminator = 11 chars
    char contact[15];
} Donor;

#endif