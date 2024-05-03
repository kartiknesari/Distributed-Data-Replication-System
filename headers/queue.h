#ifndef QUEUE
#define QUEUE

#include "myudp.h"

// Define a structure for a job node
typedef struct JobNode
{
    struct msg_packet msg;
    struct JobNode *next;
} JobNode;

// Define a structure for the job queue
typedef struct JobQueue
{
    JobNode *front;
    JobNode *rear;
} JobQueue;

struct msg_packet *dequeue(JobQueue *queue);
void enqueue(JobQueue *queue, struct msg_packet mymsg);
void initializeQueue(JobQueue *queue);
void printQueue(JobQueue *queue);

#endif