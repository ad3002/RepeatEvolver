//
//  llist.h
//  repeatevolver
//  Created by Ilya Korvigo on 20.06.15.
//


#ifndef queue_h
#define queue_h


#include <stdlib.h>
#include <assert.h>


///////////////////////////
//////// Typedefs  ////////
///////////////////////////


struct QueueNode;
typedef struct QueueNode {
    struct QueueNode* next; // pointer to the next node
    void* data;
} QueueNode;


typedef struct LinkedQueue {
    QueueNode* head;   // pointer to the first Node
    QueueNode* tail;   // pointer to the last Node
    long long k;       // the number of nodes in the queue
} LinkedQueue;


///////////////////////////
//////// Functions ////////
///////////////////////////


LinkedQueue* init_queue();


QueueNode* init_node() {
    /*
     Initilizes an empty node.
     */
    QueueNode* node = malloc(sizeof *node);
    assert(node && "Failed to allocate memory for a node");
    node->data = NULL;
    node->next = NULL;
    return node;
}


static inline void dealloc_queue(LinkedQueue *queue) {
    /*
     Frees memory allocated by a queue
     */
    assert(queue && "Got NULL instead of queue pointer");
    free(queue);
    queue = NULL;
}


static inline void dealloc_node(QueueNode* node) {
    /*
     Frees memory allocated by a node.
     */
    assert(node && "Got NULL instead of node pointer");
    free(node);
    node = NULL;
}


static inline void update_head_node(LinkedQueue *queue) {
    /*
     Moves head position to the next node and frees the memory
     allocated by the old head node.
     */
    assert(queue->head->next && "No next node to replace head");
    QueueNode* old_head_pointer = queue->head;
    queue->head = queue->head->next;           // replacing head with a new one
    dealloc_node(old_head_pointer);
}


void push_data(void* data, LinkedQueue* queue) {
    /*
     Initializes a node with a void pointer to data and pushes it
     to the tail of a queue
     */
    
    QueueNode* node = init_node();
    node->data = data;
    if (!queue->head && !queue->tail) { // If queue is empty
        queue->head = node;
        queue->tail = node;
        queue->k++;
    } else {
        queue->tail->next = node; // linking new node to the current tail
        queue->tail = node;       // changing tail to the new node
        queue->k++;
    }
}


void* pop_data(LinkedQueue* queue) {
    /*
     Pops the head node from a queue, extracts data out of it and
     frees the memory allocated by that node. This works as long
     as data is dynamically alocated.
     */
    assert(queue->head && "Can't pop data from an empty queue");
    
    void* data = queue->head->data;            // extract data from head
    
    if (queue->head->next) {                   // destroying the old head
        update_head_node(queue);
    } else {
        dealloc_node(queue->head);
    }
    
    return data;
}


void destroy_queue(LinkedQueue* queue) {
    /*
     Frees memory allocated by a queue and all its nodes
     */
    if (!queue->head) { // If there are no nodes on the queue
        dealloc_queue(queue);
        
    } else if (!queue->head->next) {
        dealloc_node(queue->head);
        dealloc_queue(queue);
        
    } else {
        while (queue->head->next) {
            update_head_node(queue);
        }
        dealloc_node(queue->head);
        dealloc_queue(queue);
    }
}


#endif