
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "graph.h"
#include "hashtable.h"
#include "heap.h"

// Read the blood group from the user
int read_blood_group(BloodGroup *group)
{
    char input[5];

    printf("Enter blood group (O-, O+, A-, A+, B-, B+, AB-, AB+): ");
    if (scanf("%4s", input) != 1)
        return 0;

    *group = convert_To_enum(input);

    if (*group == INVALID_BLOOD_GROUP)
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
    if (scanf("%49s", name) != 1)
        return;

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
    MinHeap heaps[8];
    Donors nearest_donor = {0};
    BloodGroup nearest_group = INVALID_BLOOD_GROUP;
    int found = 0;

    if (!read_blood_group(&patient_group))
        return;

    get_Compatible_Groups(graph, patient_group, compatible, &count);

    for (int i = 0; i < 8; i++)
        heap_init(&heaps[i]);

    for (int i = 0; i < count; i++)
    {
        Node *temp = search_donor(hash_table, compatible[i]);

        while (temp != NULL)
        {
            if (heap_insert(&heaps[compatible[i]], temp->donor) != 0)
            {
                fprintf(stderr, "Unable to build donor heaps.\n");
                for (int j = 0; j < 8; j++)
                    heap_free(&heaps[j]);
                return;
            }
            temp = temp->next;
        }
    }

    for (int i = 0; i < count; i++)
    {
        Donors candidate;
        if (heap_getmin_active(&heaps[compatible[i]], &candidate) &&
            (!found || candidate.distance < nearest_donor.distance))
        {
            nearest_donor = candidate;
            nearest_group = compatible[i];
            found = 1;
        }
    }

    for (int i = 0; i < 8; i++)
        heap_free(&heaps[i]);

    if (!found)
    {
        printf("No compatible donors found.\n");
        return;
    }

    printf("\nNearest active donor for %s:\n",
           bloodGroupName(patient_group));
    printf("Compatible blood group: %s\n", bloodGroupName(nearest_group));
    print_donor(nearest_donor);
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
        if (scanf("%d", &choice) != 1)
        {
            int character;
            while ((character = getchar()) != '\n' && character != EOF)
            {
            }
            if (character == EOF)
            {
                free_donors();
                return 0;
            }
            printf("Invalid choice. Try again.\n");
            continue;
        }

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
