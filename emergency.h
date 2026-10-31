#ifndef EMERGENCY_H
#define EMERGENCY_H

#include "donor.h"
#include "graph.h"

/* A pending request or completed match; next links queues/history respectively. */
typedef struct EmergencyRecord {
    int request_id;
    BloodGroup patient_group;
    Donors matched_donor;
    BloodGroup donor_group;
    struct EmergencyRecord *next;
} EmergencyRecord;

/* Requests are appended at rear and processed from front (FIFO). */
typedef struct {
    EmergencyRecord *front;
    EmergencyRecord *rear;
} RequestQueue;

/* Completed matches are pushed and displayed from top (LIFO). */
typedef struct {
    EmergencyRecord *top;
} RequestHistory;

void emergency_init(RequestQueue *queue, RequestHistory *history);
/* Returns 0 on success and -1 for invalid input or allocation failure. */
int emergency_enqueue(RequestQueue *queue, int request_id, BloodGroup patient_group);
const EmergencyRecord *emergency_front(const RequestQueue *queue);
/* Returns 1 when moved to history, 0 when empty, or -1 for invalid arguments. */
int emergency_complete_next(RequestQueue *queue, RequestHistory *history,
                            Donors donor, BloodGroup donor_group);
void emergency_display_pending(const RequestQueue *queue);
const EmergencyRecord *emergency_history_top(const RequestHistory *history);
void emergency_free(RequestQueue *queue, RequestHistory *history);

#endif
