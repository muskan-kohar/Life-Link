#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "heap.h"

// Initializing a heap

void heap_init(MinHeap *heap){
    heap->data = NULL;
    heap->size = 0;
    heap->capacity = 0;
}

// Free the heap

void heap_free(MinHeap *heap){
    free(heap->data);
    heap_init(heap);
}

/*
    Sift Up Operation
        If any child value is less than the parent value, this operation ensures that it is above the parent for all nodes such that min-heap property maintains throughout the heap

*/

MinHeap* siftup(MinHeap* heap){
    if(heap->size == 1)
        return heap;
    for(int i=1;i<heap->size;i++){
        while((heap->data[(i-1)/2]).distance > (heap->data[i].distance)){
            Donor temp = heap->data[(i-1)/2];
            heap->data[(i-1)/2] = heap->data[i];
            heap->data[i] = temp;
            i = (i-1)/2;
        }
    }
    return heap;
}

// Inserting a donor in the heap. Returns 0 on success, -1 if allocation fails.

int heap_insert(MinHeap *heap, Donor d){
    
    if(heap->capacity==0)
        heap->capacity = 1;
    
    if(heap->size == heap->capacity){
        heap->capacity*=2;
        
        Donor* temp = realloc(heap->data,heap->capacity * sizeof(Donor));

        if(temp == NULL)
            return -1;
    
        heap->data = temp;
    }
    
    heap->data[heap->size] = d;
    heap->size++;
    heap = siftup(heap);
    return 0;
}

/*
    Heapify
        When we remove a node from the heap, we place the last value(in array form) in the root place.
        Then we perform this operation(heapify) to ensure every node is in correct position i.e if parent is greater than its child it will be replaced by the smallest of its children.
        We can say this works exactly opposite of sift up operation.

*/

MinHeap* heapify(MinHeap* heap){
    for(int i=0;i<heap->size && (2*i+2 <= (heap->size)-1);i++){
        while((heap->data[2*i + 1]).distance < (heap->data[i].distance) || (heap->data[2*i + 2]).distance < (heap->data[i].distance) ){
            if((heap->data[2*i + 1]).distance < ((heap->data[2*i+2]).distance)){
                Donor temp = heap->data[2*i + 1];
                heap->data[2*i + 1] = heap->data[i];
                heap->data[i] = temp;
                i = 2*i + 1;
            }

            else{
                Donor temp = heap->data[2*i + 2];
                heap->data[2*i + 2]= heap->data[i];
                heap->data[i] = temp;
                i = 2*i + 2;
            }
        }
    }
    return heap;
}

// Here we extract the donor which is at the top of the heap and heapify the remaining heap. 

Donor heap_extract_min(MinHeap *heap){
    if(heap == NULL || heap->size == 0){
        Donor empty = {0};
        return empty;
    }

    Donor d = heap->data[0];
    heap->data[0] = heap->data[(heap->size)-1];
    heap->size--;
    heap = heapify(heap);
    return d;
}

// This function is used to find the no of days from last donation time of a donor.

int no_of_days(Donor d){
    int day = 0;
    int month =0;
    int year = 0;
    char date[11];
    strcpy(date,d.last_donation_date);
    day = (date[0]-'0')*10 + date[1]-'0';
    month = (date[3]-'0')*10 + date[4]-'0';
    year = (date[7]-'0')*1000 + (date[8]-'0')*100 + (date[9]-'0')*10 + date[10]-'0'; 

    struct tm donationDate = {0};
    donationDate.tm_year = year - 1900;   // tm_year is years since 1900
    donationDate.tm_mon  = month - 1;     // tm_mon is 0-11
    donationDate.tm_mday = day;

    time_t donationTime = mktime(&donationDate);
    time_t now = time(NULL);

    double seconds = difftime(now, donationTime);
    int days = (int)(seconds / (60 * 60 * 24));

    return days;
}

/*
    Here we find the minimum active donor by checking if the no of days since last donation date is atleast 90. 
    Returns  1 and copies that donor into *result if one is found,
            0 if the heap has no active donor,
           -1 if a temporary allocation fails.
   In every case the heap holds the same donors afterwards: everything popped during the search is reinserted.
*/

int heap_getmin_active(MinHeap *heap, Donor *result){
   if (heap == NULL || result == NULL || heap->size == 0)
        return 0;

    int count = 0;
    Donor *temp = malloc(heap->size * sizeof(Donor));

    if (temp == NULL)
        return -1;

    while (heap->size > 0) {
        Donor d = heap_extract_min(heap);
        temp[count++] = d;

        if (no_of_days(d) >= 90) {
            *result = d;

            for (int i = 0; i < count; i++) {
                heap_insert(heap, temp[i]);
            }

            free(temp);
            return 1;
        }
    }

    for (int i = 0; i < count; i++) {
        heap_insert(heap, temp[i]);
    }

    free(temp);
    return 0;
}