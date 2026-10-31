
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>

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

void print_donor(Donors donor);

// Read a complete line of text, including spaces.
static int read_text_line(const char *prompt, char *buffer, size_t size)
{
    printf("%s", prompt);
    if (fgets(buffer, (int)size, stdin) == NULL)
        return 0;

    size_t length = strlen(buffer);
    if (length > 0 && buffer[length - 1] == '\n')
    {
        buffer[--length] = '\0';
    }
    else
    {
        int character = getchar();
        if (character != '\n' && character != EOF)
        {
            while ((character = getchar()) != '\n' && character != EOF)
            {
            }
            return 0;
        }
    }

    if (length > 0 && buffer[length - 1] == '\r')
        buffer[--length] = '\0';

    return buffer[0] != '\0';
}

// Compare names without treating uppercase and lowercase as different.
static int names_match(const char *first, const char *second)
{
    while (*first != '\0' && *second != '\0')
    {
        if (tolower((unsigned char)*first) != tolower((unsigned char)*second))
            return 0;
        first++;
        second++;
    }

    return *first == '\0' && *second == '\0';
}

// Check whether a donor is registered using name, blood group and contact.
void check_donor()
{
    char name[50];
    char contact[15];
    BloodGroup group;

    if (!read_text_line("Enter donor name: ", name, sizeof(name)))
    {
        printf("Invalid name.\n");
        return;
    }

    if (!read_blood_group(&group))
        return;

    // Clear the newline left by read_blood_group() before reading the contact.
    int character;
    while ((character = getchar()) != '\n' && character != EOF)
    {
    }

    if (!read_text_line("Enter donor contact number: ", contact, sizeof(contact)))
    {
        printf("Invalid contact number.\n");
        return;
    }

    Node *temp = search_donor(hash_table, group);
    while (temp != NULL)
    {
        if (names_match(temp->donor.name, name) &&
            strcmp(temp->donor.contact, contact) == 0)
        {
            printf("\nDonor found in the system!\n");
            print_donor(temp->donor);
            return;
        }
        temp = temp->next;
    }

    printf("\nNo matching donor found in the system.\n");
}

// Print the details of a donor
void print_donor(Donors donor)
{
    printf("\nDonor ID: %d\n", donor.donor_id);
    printf("Name: %s\n", donor.name);
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

// Remove a donor using their unique ID and blood group.
void remove_donor()
{
    BloodGroup group;
    int id;

    if (!read_blood_group(&group))
        return;

    // Clear the newline left by read_blood_group() before reading the ID line.
    int character;
    while ((character = getchar()) != '\n' && character != EOF)
    {
    }

    char id_input[32];
    printf("Enter donor ID: ");
    if (fgets(id_input, sizeof(id_input), stdin) == NULL)
    {
        printf("Unable to read donor ID.\n");
        return;
    }

    size_t length = strlen(id_input);
    if (length > 0 && id_input[length - 1] == '\n')
    {
        id_input[--length] = '\0';
    }
    else
    {
        character = getchar();
        if (character != '\n' && character != EOF)
        {
            while ((character = getchar()) != '\n' && character != EOF)
            {
            }
            printf("Donor ID is too long.\n");
            return;
        }
    }

    char *end;
    errno = 0;
    long parsed_id = strtol(id_input, &end, 10);
    if (errno == ERANGE || end == id_input || *end != '\0' ||
        parsed_id <= 0 || parsed_id > INT_MAX)
    {
        printf("Enter a valid positive donor ID.\n");
        return;
    }
    id = (int)parsed_id;

    Node **temp = &hash_table[group];

    // Traverse the selected blood-group list and match the unique donor ID.
    while (*temp != NULL)
    {
        if ((*temp)->donor.donor_id == id)
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

    printf("\nNearest active donor for %s:\n",bloodGroupName(patient_group));
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
        printf("6. Check if a donor is registered\n");
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

        // Remove the newline left by scanf before any function uses fgets.
        int character;
        while ((character = getchar()) != '\n' && character != EOF)
        {
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

            case 6:
                check_donor();
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
