
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>

#include "graph.h"
#include "hashtable.h"
#include "heap.h"
#include "emergency.h"

static int next_request_id = 1;

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

// Read the blood group from one complete input line.
int read_blood_group(BloodGroup *group)
{
    char input[5];

    if (group == NULL)
        return 0;
    if (!read_text_line("Enter blood group (O-, O+, A-, A+, B-, B+, AB-, AB+): ",input, sizeof(input)))
    {
        printf("Invalid blood group!\n");
        return 0;
    }

    *group = convert_To_enum(input);

    if (*group == INVALID_BLOOD_GROUP)
    {
        printf("Invalid blood group!\n");
        return 0;
    }

    return 1;
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
        int character = getchar();
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
            if (heap_remove_donor(group, id) != 1)
            {
                fprintf(stderr, "Unable to remove donor from distance index.\n");
                return;
            }

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

// Find the nearest active donor among groups compatible with the patient.
static int find_nearest_compatible_donor(Graph *graph, BloodGroup patient_group,
                                         Donors *nearest_donor,
                                         BloodGroup *nearest_group)
{
    BloodGroup compatible[8];
    int count = 0;

    get_Compatible_Groups(graph, patient_group, compatible, &count);
    int found = heap_getmin_active(compatible, count, nearest_donor,
                                   nearest_group);
    if (found <= 0)
        return 0;

    return found;
}

// Find and display a compatible donor for a patient.
void list_compatible_donors(Graph *graph)
{
    BloodGroup patient_group;
    BloodGroup nearest_group;
    Donors nearest_donor;

    if (!read_blood_group(&patient_group))
        return;

    int result = find_nearest_compatible_donor(graph, patient_group,
                                               &nearest_donor, &nearest_group);
    if (result < 0)
    {
        fprintf(stderr, "Unable to build donor heap.\n");
        return;
    }
    if (result == 0)
    {
        printf("No compatible donors found.\n");
        return;
    }

    if (update_donation_date(nearest_donor.donor_id, &nearest_donor) != 1)
    {
        fprintf(stderr, "Unable to update the matched donor's donation date.\n");
        return;
    }

    printf("\nNearest active donor for %s; donor marked as donated on %s:\n",
           bloodGroupName(patient_group), nearest_donor.last_donation_date);
    printf("Compatible blood group: %s\n", bloodGroupName(nearest_group));
    print_donor(nearest_donor);
}

static void add_emergency_request(RequestQueue *queue)
{
    BloodGroup patient_group;

    if (!read_blood_group(&patient_group))
        return;
    if (next_request_id == INT_MAX)
    {
        fprintf(stderr, "Unable to generate another emergency request ID.\n");
        return;
    }
    if (emergency_enqueue(queue, next_request_id, patient_group) != 0)
    {
        fprintf(stderr, "Unable to add emergency request.\n");
        return;
    }

    printf("Emergency request added. Request ID: %d\n", next_request_id);
    next_request_id++;
}

static void process_next_emergency_request(Graph *graph, RequestQueue *queue,
                                           RequestHistory *history)
{
    const EmergencyRecord *request = emergency_front(queue);
    if (request == NULL)
    {
        printf("No pending emergency requests.\n");
        return;
    }

    Donors donor;
    BloodGroup donor_group;
    int result = find_nearest_compatible_donor(graph, request->patient_group,
                                               &donor, &donor_group);
    if (result < 0)
    {
        fprintf(stderr, "Unable to find a donor because heap allocation failed.\n");
        return;
    }
    if (result == 0)
    {
        /* Keep the FIFO head in place until an eligible donor can be matched. */
        printf("No eligible compatible donor found. Request %d remains pending.\n",
               request->request_id);
        return;
    }

    int request_id = request->request_id;
    BloodGroup patient_group = request->patient_group;
    if (update_donation_date(donor.donor_id, &donor) != 1)
    {
        fprintf(stderr, "Unable to update the matched donor's donation date.\n");
        return;
    }
    if (emergency_complete_next(queue, history, donor, donor_group) != 1)
    {
        fprintf(stderr, "Unable to record completed emergency request.\n");
        return;
    }

    printf("\nRequest %d matched; donor marked as donated on %s.\n",
           request_id, donor.last_donation_date);
    printf("Patient blood group: %s\n", bloodGroupName(patient_group));
    printf("Matched donor blood group: %s\n", bloodGroupName(donor_group));
    print_donor(donor);
}

static void display_emergency_history(const RequestHistory *history)
{
    const EmergencyRecord *record = emergency_history_top(history);
    if (record == NULL)
    {
        printf("No completed emergency requests.\n");
        return;
    }

    printf("\nCompleted matching requests (most recent first):\n");
    while (record != NULL)
    {
        printf("\nRequest ID: %d | Patient: %s | Donor group: %s\n",
               record->request_id, bloodGroupName(record->patient_group),
               bloodGroupName(record->donor_group));
        print_donor(record->matched_donor);
        record = record->next;
    }
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

    heap_reset_donor_index();
}

int main()
{
    Graph graph;
    RequestQueue request_queue;
    RequestHistory request_history;
    int choice;

    initialize_hash();
    initialise_Graph(&graph);
    emergency_init(&request_queue, &request_history);

    while (1)
    {
        printf("\n===== LIFE LINK =====\n");
        printf("1. Search donors by blood group\n");
        printf("2. Insert a new donor\n");
        printf("3. Remove a donor\n");
        printf("4. Display all donors\n");
        printf("5. Find compatible donors\n");
        printf("6. Check if a donor is registered\n");
        printf("7. Add emergency request\n");
        printf("8. Process next emergency request\n");
        printf("9. View pending emergency requests\n");
        printf("10. View completed request history\n");
        printf("0. Exit\n");

        printf("Enter your choice: ");
        char choice_input[64];
        if (fgets(choice_input, sizeof(choice_input), stdin) == NULL)
        {
            free_donors();
            emergency_free(&request_queue, &request_history);
            return 0;
        }

        size_t choice_length = strlen(choice_input);
        if (choice_length > 0 && choice_input[choice_length - 1] == '\n')
        {
            choice_input[--choice_length] = '\0';
            if (choice_length > 0 && choice_input[choice_length - 1] == '\r')
                choice_input[--choice_length] = '\0';
        }
        else
        {
            int character = getchar();
            if (character != '\n' && character != EOF)
            {
                while ((character = getchar()) != '\n' && character != EOF)
                {
                }
                printf("Invalid choice. Try again.\n");
                continue;
            }
        }

        char *end;
        errno = 0;
        long parsed_choice = strtol(choice_input, &end, 10);
        while (isspace((unsigned char)*end))
            end++;
        if (errno == ERANGE || end == choice_input || *end != '\0' ||
            parsed_choice < INT_MIN || parsed_choice > INT_MAX)
        {
            printf("Invalid choice. Try again.\n");
            continue;
        }
        choice = (int)parsed_choice;

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

            case 7:
                add_emergency_request(&request_queue);
                break;

            case 8:
                process_next_emergency_request(&graph, &request_queue,
                                               &request_history);
                break;

            case 9:
                emergency_display_pending(&request_queue);
                break;

            case 10:
                display_emergency_history(&request_history);
                break;

            case 0:
                free_donors();
                emergency_free(&request_queue, &request_history);
                printf("Exiting Life Link...\n");
                return 0;

            default:
                printf("Invalid choice. Try again.\n");
        }
    }

    return 0;
}
