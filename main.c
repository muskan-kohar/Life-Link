
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "graph.h"
#include "hashtable.h"

// Read the blood group from the user
int read_blood_group(BloodGroup *group)
{
    char input[5];

    printf("Enter blood group (O-, O+, A-, A+, B-, B+, AB-, AB+): ");
    scanf("%4s", input);

    *group = convert_To_enum(input);

    if (*group == -1)
    {
        printf("Invalid blood group!\n");
        return 0;
    }

    return 1;
}

// Print the details of a donor
void print_donor(Donors donor)
{
    printf("\nName: %s\n", donor.name);
    printf("Blood Group: %s\n", donor.blood_group);
    printf("Location: %s\n", donor.location);
    printf("Distance: %d\n", donor.distance);
    printf("Last Donation: %s\n", donor.last_donation_date);
    printf("Contact: %s\n", donor.contact);
}

// Search and display donors of a particular blood group
void list_donors(BloodGroup group)
{
    Node *temp = search_donor(hash_table, group);

    if (temp == NULL)
    {
        printf("No donors found.\n");
        return;
    }

    while (temp != NULL)
    {
        print_donor(temp->donor);
        temp = temp->next;
    }
}

// Remove a donor using their name and blood group
void remove_donor()
{
    BloodGroup group;
    char name[50];

    if (!read_blood_group(&group))
        return;

    printf("Enter donor name: ");
    scanf("%49s", name);

    Node **temp = &hash_table[group];

    while (*temp != NULL)
    {
        if (strcmp((*temp)->donor.name, name) == 0)
        {
            Node *del = *temp;
            *temp = (*temp)->next;
            free(del);

            printf("Donor removed successfully.\n");
            return;
        }

        temp = &(*temp)->next;
    }

    printf("Donor not found.\n");
}

// Find donors who can donate to the given patient
void list_compatible_donors(Graph *graph)
{
    BloodGroup patient_group;
    BloodGroup compatible[8];
    int count = 0;
    int found = 0;

    if (!read_blood_group(&patient_group))
        return;

    get_Compatible_Groups(graph, patient_group, compatible, &count);

    for (int i = 0; i < count; i++)
    {
        printf("\nCompatible blood group: %s\n",
               bloodGroupName(compatible[i]));

        Node *temp = search_donor(hash_table, compatible[i]);

        while (temp != NULL)
        {
            print_donor(temp->donor);
            temp = temp->next;
            found = 1;
        }
    }

    if (!found)
        printf("No compatible donors found.\n");
}

// Free the memory used by the donor list
void free_donors()
{
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        Node *temp = hash_table[i];

        while (temp != NULL)
        {
            Node *next = temp->next;
            free(temp);
            temp = next;
        }

        hash_table[i] = NULL;
    }
}

int main()
{
    Graph graph;
    int choice;

    initialize_hash();
    initialise_Graph(&graph);

    while (1)
    {
        printf("\n===== LIFE LINK =====\n");
        printf("1. Search donors by blood group\n");
        printf("2. Insert a new donor\n");
        printf("3. Remove a donor\n");
        printf("4. Display all donors\n");
        printf("5. Find compatible donors\n");
        printf("0. Exit\n");

        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
            case 1:
            {
                BloodGroup group;

                if (read_blood_group(&group))
                    list_donors(group);

                break;
            }

            case 2:
            {
                Donors donor;
                insert_donor(&donor);
                break;
            }

            case 3:
                remove_donor();
                break;

            case 4:
                display();
                break;

            case 5:
                list_compatible_donors(&graph);
                break;

            case 0:
                free_donors();
                printf("Exiting Life Link...\n");
                return 0;

            default:
                printf("Invalid choice. Try again.\n");
        }
    }

    return 0;
}
