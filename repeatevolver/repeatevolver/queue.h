//
// Created by Ilia Korvigo on 25.06.15.
//

#ifndef REPEATEVOLVER_QUEUE_H
#define REPEATEVOLVER_QUEUE_H


///////////////////////////
//////// Typedefs  ////////
///////////////////////////


struct QueueNode;
typedef struct QueueNode {
    struct QueueNode* next; // pointer to the next node
    void*             data;
} QueueNode;


typedef struct LinkedQueue {
    QueueNode* head;   // pointer to the first Node
    QueueNode* tail;   // pointer to the last Node
} LinkedQueue;


///////////////////////////
//////// Functions ////////
///////////////////////////


LinkedQueue* init_queue();


QueueNode* init_node();


void dealloc_queue(LinkedQueue *queue);


void dealloc_node(QueueNode* node);


void update_head_node(LinkedQueue *queue);


void push_data(void* data, LinkedQueue* queue);


void* pop_data(LinkedQueue* queue);


void destroy_queue(LinkedQueue* queue);


LinkedQueue* enqueue_data_pointers(void** array_of_pointers, int n_pointers);


void merge_queues(LinkedQueue* head_queue, LinkedQueue* tail_queue);


void merge_queues_memfree(LinkedQueue* head_queue, LinkedQueue* tail_queue);


#endif //REPEATEVOLVER_QUEUE_H