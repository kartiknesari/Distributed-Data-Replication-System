#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include "../headers/queue.h"
#include "../headers/myudp.h"

// Initialize a job queue
void initializeQueue(JobQueue *queue)
{
    queue->front = NULL;
    queue->rear = NULL;
}

// Enqueue a job
void enqueue(JobQueue *queue, struct msg_packet mymsg)
{
    JobNode *newNode = (JobNode *)malloc(sizeof(JobNode));
    if (newNode == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for job node\n");
        exit(EXIT_FAILURE);
    }

    newNode->msg.cmd = mymsg.cmd;
    newNode->msg.seq = mymsg.seq;
    memcpy(newNode->msg.data, mymsg.data, sizeof(mymsg.data));

    if (queue->rear == NULL)
    {
        // If the queue is empty, set both front and rear to the new node
        queue->front = newNode;
        queue->rear = newNode;
    }
    else
    {
        // Otherwise, add the new node to the rear of the queue
        queue->rear->next = newNode;
        queue->rear = newNode;
    }
}

// Dequeue a job
struct msg_packet *dequeue(JobQueue *queue)
{
    if (queue->front == NULL)
    {
        fprintf(stderr, "Job queue is empty\n");
        exit(EXIT_FAILURE);
    }

    JobNode *temp = queue->front;
    queue->front = queue->front->next;

    // If the front becomes NULL, set rear to NULL as well (empty queue)
    if (queue->front == NULL)
    {
        queue->rear = NULL;
    }
    return &(temp->msg);
}

// Print the jobs in the queue
void printQueue(JobQueue *queue)
{
    printf("Job Queue:\n");
    JobNode *current = queue->front;
    while (current != NULL)
    {
        printf("cmd:%d, seq:%d\n", ntohs(current->msg.cmd), ntohs(current->msg.seq));
        current = current->next;
    }
}