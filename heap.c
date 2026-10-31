#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include <time.h>
#include "heap.h"

/* Blood-group enum values are also the indexes into this donor index. */
static MinHeap donor_heaps[8];

static int heap_is_valid(const MinHeap *heap){
    return heap != NULL && heap->size >= 0 && heap->capacity >= 0 && heap->size <= heap->capacity && ((heap->capacity == 0 && heap->data == NULL) || (heap->capacity > 0 && heap->data != NULL));
}

/* Reset the heap without allocating memory. */
void heap_init(MinHeap *heap){
    if (heap == NULL)
        return;

    heap->data = NULL;
    heap->size = 0;
    heap->capacity = 0;
}

/* Release the backing array and leave the heap in its initialized state. */
void heap_free(MinHeap *heap){
    if (heap == NULL)
        return;

    free(heap->data);
    heap_init(heap);
}

/* Restore heap order after inserting a value at the end of the array. */
static void sift_up(MinHeap *heap, int index){
    while (index > 0) {
        int parent = (index - 1) / 2;
        if (heap->data[parent].distance <= heap->data[index].distance)
            break;

        Donors temp = heap->data[parent];
        heap->data[parent] = heap->data[index];
        heap->data[index] = temp;
        index = parent;
    }
}

/* Append a donor, growing storage if needed, then restore min-heap order. */
int heap_insert(MinHeap *heap, Donors d){
    if (!heap_is_valid(heap))
        return -1;

    if (heap->size == heap->capacity) {
        if (heap->capacity > INT_MAX / 2)
            return -1;

        int new_capacity = heap->capacity == 0 ? 1 : heap->capacity * 2;
        if ((size_t)new_capacity > SIZE_MAX / sizeof(*heap->data))
            return -1;
        Donors *temp = realloc(heap->data, (size_t)new_capacity * sizeof(*temp));
        if (temp == NULL)
            return -1;

        heap->data = temp;
        heap->capacity = new_capacity;
    }

    heap->data[heap->size] = d;
    heap->size++;
    sift_up(heap, heap->size - 1);
    return 0;
}

/* Restore min-heap order after the root has been replaced by the last item. */
static void sift_down(MinHeap *heap, int index){
    while(1){
        if (index >= heap->size / 2)
            break;

        int left = 2 * index + 1;
        int right = left + 1;
        int smallest = index;

        if (left < heap->size && heap->data[left].distance < heap->data[smallest].distance)
            smallest = left;
        if (right < heap->size && heap->data[right].distance < heap->data[smallest].distance)
            smallest = right;
        if (smallest == index)
            break;

        Donors temp = heap->data[index];
        heap->data[index] = heap->data[smallest];
        heap->data[smallest] = temp;
        index = smallest;
    }
}

/* Remove and return the donor with the smallest distance. */
Donors heap_extract_min(MinHeap *heap){
    Donors empty = {0};
    if (!heap_is_valid(heap) || heap->size == 0)
        return empty;

    Donors minimum = heap->data[0];
    heap->size--;
    if (heap->size > 0) {
        heap->data[0] = heap->data[heap->size];
        sift_down(heap, 0);
    }
    return minimum;
}

/* Convert a Gregorian calendar date to a day number for date-only differences. */
static int days_from_civil(int year, unsigned month, unsigned day){
    year -= month <= 2;
    int era = (year >= 0 ? year : year - 399) / 400;
    unsigned year_of_era = (unsigned)(year - era * 400);
    unsigned adjusted_month = month > 2 ? month - 3 : month + 9;
    unsigned day_of_year = (153 * adjusted_month + 2) / 5 + day - 1;
    unsigned day_of_era = year_of_era * 365 + year_of_era / 4 - year_of_era / 100 + day_of_year;
    return era * 146097 + (int)day_of_era;
}

/* Return elapsed calendar days, INT_MAX for an unset date, or -1 if invalid. */
static int donation_days_ago(const Donors *donor){
    const char *date = donor->last_donation_date;
    /* Treat the registration sentinel for "never donated" as always eligible. */
    if (date[0] == '0' && date[1] == '0' && date[2] == '-' && date[3] == '0' && date[4] == '0' && date[5] == '-' && date[6] == '0' && date[7] == '0' && date[8] == '0' && date[9] == '0' && date[10] == '\0')
        return INT_MAX;

    if (date[2] != '-' || date[5] != '-' || date[10] != '\0')
        return -1;

    for (int i = 0; i < 10; i++) {
        if (i != 2 && i != 5 && (date[i] < '0' || date[i] > '9'))
            return -1;
    }

    int day = (date[0] - '0') * 10 + date[1] - '0';
    int month = (date[3] - '0') * 10 + date[4] - '0';
    int year = (date[6] - '0') * 1000 + (date[7] - '0') * 100 + (date[8] - '0') * 10 + date[9] - '0';
    if (year < 1 || month < 1 || month > 12)
        return -1;

    static const int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int month_length = days_in_month[month - 1];
    if (month == 2 && (year % 400 == 0 || (year % 4 == 0 && year % 100 != 0)))
        month_length++;
    if (day < 1 || day > month_length)
        return -1;

    time_t now = time(NULL);
    struct tm *current = localtime(&now);
    if (current == NULL)
        return -1;

    int today = days_from_civil(current->tm_year + 1900,(unsigned)current->tm_mon + 1,(unsigned)current->tm_mday);
    return today - days_from_civil(year, (unsigned)month, (unsigned)day);
}

/* Release and reset every persistent blood-group heap. */
void heap_reset_donor_index(void){
    for (int i = 0; i < 8; i++)
        heap_free(&donor_heaps[i]);
}

int heap_add_donor(BloodGroup group, Donors donor){
    if (group < O_NEG || group > AB_POS)
        return -1;

    return heap_insert(&donor_heaps[group], donor);
}

int heap_remove_donor(BloodGroup group, int donor_id){
    if (group < O_NEG || group > AB_POS || donor_id <= 0)
        return -1;

    MinHeap *heap = &donor_heaps[group];
    if (!heap_is_valid(heap))
        return -1;

    int index = 0;
    while (index < heap->size && heap->data[index].donor_id != donor_id)
        index++;
    if (index == heap->size)
        return 0;

    heap->size--;
    if (index < heap->size) {
        /* Restore heap order in only the direction the replacement violates. */
        heap->data[index] = heap->data[heap->size];
        if (index > 0 &&
            heap->data[index].distance <
                heap->data[(index - 1) / 2].distance)
            sift_up(heap, index);
        else
            sift_down(heap, index);
    }
    return 1;
}

int heap_update_donor(Donors donor){
    BloodGroup group = convert_To_enum(donor.blood_group);
    if (group == INVALID_BLOOD_GROUP)
        return -1;

    MinHeap *heap = &donor_heaps[group];
    if (!heap_is_valid(heap))
        return -1;

    for (int i = 0; i < heap->size; i++) {
        if (heap->data[i].donor_id == donor.donor_id) {
            heap->data[i] = donor;
            /* A donation-date update leaves distance unchanged, but reheapify safely. */
            if (i > 0 &&
                heap->data[i].distance < heap->data[(i - 1) / 2].distance)
                sift_up(heap, i);
            else
                sift_down(heap, i);
            return 1;
        }
    }
    return 0;
}

/* Use heap distance bounds to prune branches that cannot beat the current match. */
static void find_nearest_active(const MinHeap *heap, int index, int *best_index){
    if (index >= heap->size)
        return;
    if (*best_index >= 0 &&
        heap->data[index].distance >= heap->data[*best_index].distance)
        return;

    if (donation_days_ago(&heap->data[index]) >= 90) {
        *best_index = index;
        return;
    }

    /* Heap children cannot be closer than this ineligible donor. */
    find_nearest_active(heap, 2 * index + 1, best_index);
    find_nearest_active(heap, 2 * index + 2, best_index);
}

/* Search persistent blood-group heaps for the nearest medically eligible donor. */
int heap_getmin_active(const BloodGroup compatible[], int count,
                       Donors *result, BloodGroup *donor_group){
    if (compatible == NULL || count < 0 || result == NULL || donor_group == NULL)
        return 0;

    int found = 0;
    for (int i = 0; i < count; i++) {
        BloodGroup group = compatible[i];
        if (group < O_NEG || group > AB_POS)
            continue;

        MinHeap *heap = &donor_heaps[group];
        if (!heap_is_valid(heap))
            return -1;

        int best_index = -1;
        find_nearest_active(heap, 0, &best_index);
        if (best_index >= 0 &&
            (!found || heap->data[best_index].distance < result->distance)) {
            *result = heap->data[best_index];
            *donor_group = group;
            found = 1;
        }
    }
    return found;
}
