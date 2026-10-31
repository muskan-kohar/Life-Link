#include <stdio.h>
#include <stdlib.h>

#include "emergency.h"

void emergency_init(RequestQueue *queue, RequestHistory *history)
{
    if (queue != NULL) {
        queue->front = NULL;
        queue->rear = NULL;
    }
    if (history != NULL)
        history->top = NULL;
}

int emergency_enqueue(RequestQueue *queue, int request_id, BloodGroup patient_group)
{
    if (queue == NULL || request_id <= 0 ||
        patient_group < O_NEG || patient_group > AB_POS)
        return -1;

    EmergencyRecord *request = calloc(1, sizeof(*request));
    if (request == NULL)
        return -1;

    request->request_id = request_id;
    request->patient_group = patient_group;
    /* Preserve FIFO order by appending every new request at the rear. */
    if (queue->rear == NULL)
        queue->front = request;
    else
        queue->rear->next = request;
    queue->rear = request;
    return 0;
}

const EmergencyRecord *emergency_front(const RequestQueue *queue)
{
    return queue == NULL ? NULL : queue->front;
}

int emergency_complete_next(RequestQueue *queue, RequestHistory *history,
                            Donors donor, BloodGroup donor_group)
{
    if (queue == NULL || history == NULL)
        return -1;
    if (queue->front == NULL)
        return 0;

    /* Reuse the dequeued node so moving it to history cannot fail allocation. */
    EmergencyRecord *completed = queue->front;
    queue->front = completed->next;
    if (queue->front == NULL)
        queue->rear = NULL;

    completed->matched_donor = donor;
    completed->donor_group = donor_group;
    /* Push onto the history stack so the latest match is shown first. */
    completed->next = history->top;
    history->top = completed;
    return 1;
}

void emergency_display_pending(const RequestQueue *queue)
{
    if (queue == NULL || queue->front == NULL) {
        printf("No pending emergency requests.\n");
        return;
    }

    printf("\nPending emergency requests (first in, first out):\n");
    for (const EmergencyRecord *request = queue->front;
         request != NULL; request = request->next) {
        printf("Request ID: %d | Patient blood group: %s\n",
               request->request_id, bloodGroupName(request->patient_group));
    }
}

const EmergencyRecord *emergency_history_top(const RequestHistory *history)
{
    return history == NULL ? NULL : history->top;
}

void emergency_free(RequestQueue *queue, RequestHistory *history)
{
    if (queue != NULL) {
        EmergencyRecord *request = queue->front;
        while (request != NULL) {
            EmergencyRecord *next = request->next;
            free(request);
            request = next;
        }
        queue->front = NULL;
        queue->rear = NULL;
    }

    if (history != NULL) {
        EmergencyRecord *record = history->top;
        while (record != NULL) {
            EmergencyRecord *next = record->next;
            free(record);
            record = next;
        }
        history->top = NULL;
    }
}
