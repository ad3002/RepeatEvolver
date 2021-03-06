//
// Created by Ilia Korvigo on 25.06.15.
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include "queue.h"


LinkedQueue* init_queue() {
    /*
     Initializes an empty queue
     */
    LinkedQueue* queue = malloc(sizeof *queue);
    assert(queue && "Failed to allocate memory for a queue");
    queue->head = NULL;
    queue->tail = NULL;
    queue->size = 0;
    pthread_mutex_init(&queue->queue_lock, NULL);
    pthread_cond_init(&queue->queue_cond, NULL);
    return queue;
}


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


void dealloc_queue(LinkedQueue *queue) {
    /*
     Frees memory allocated by a queue
     */
    assert(queue && "Got NULL instead of queue pointer");
    free(queue);
    queue = NULL;
}


void dealloc_node(QueueNode* node) {
    /*
     Frees memory allocated by a node.
     */
    assert(node && "Got NULL instead of node pointer");
    free(node);
    node = NULL;
}


void cut_head_node(LinkedQueue *queue) {
    /*
     Moves head position to the next node and frees the memory
     allocated by the old head node.
     */
    assert(queue->head->next && "No next node to replace head");
    QueueNode* old_head_pointer = queue->head;
    queue->head = queue->head->next;           // replacing head with a new one
    dealloc_node(old_head_pointer);
    queue->size--;
}


void push_data(void* data, LinkedQueue* queue) {
    /*
     Initializes a node with a void pointer to data and pushes it
     to the tail of a queue
     */
    
    QueueNode* node = init_node();
    node->data = data;
    if (!queue->size) { // If queue is empty
        queue->head = node;
        queue->tail = node;
    } else {
        queue->tail->next = node; // linking new node to the current tail
        queue->tail = node;       // changing tail to the new node
    }
    queue->size++;
}


void* pop_data(LinkedQueue* queue) {
    /*
     Pops the head node from a queue, extracts data out of it and
     frees the memory allocated by that node. This works as long
     as data is dynamically alocated.
     */
    assert(queue->head && "Can't pop data from an empty queue");
    
    void* data = queue->head->data;            // extract data from head
    assert(data && "Popped an empty node");
    
    if (queue->head->next) {                   // destroying the old head
        cut_head_node(queue);
    } else {
        dealloc_node(queue->head);
        queue->head = NULL;
        queue->tail = NULL;
    }
    queue->size--;
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
            cut_head_node(queue);
        }
        dealloc_node(queue->head);
        dealloc_queue(queue);
    }
}


LinkedQueue* enqueue_data_pointers(void** array_of_pointers, int n_pointers) {
    /*
     Wraps an array of pointers to data into a queue
     */
    LinkedQueue* queue = init_queue();
    for (int i = 0; i < n_pointers; i++) {
        push_data(array_of_pointers[i], queue);
    }
    return queue;
}


void merge_queues(LinkedQueue* head_queue, LinkedQueue* tail_queue) {
    /*
     Merges 2 LinkedQueues. Links tail-node of the head_queue to the head-node
     of the tail_queue. Replaces tail of the head_queue with that of the
     tail_queue
     */
    assert(head_queue && tail_queue && "Passed a NULL queue pointer");
    assert(tail_queue->head && tail_queue->tail && "Passed empty tail_queue");
    if (head_queue->head && head_queue->tail) {
        head_queue->tail->next = tail_queue->head;
        head_queue->tail = tail_queue->tail;
    } else {
        head_queue->head = tail_queue->head;
        head_queue->tail = tail_queue->tail;
    }
    head_queue->size += tail_queue->size;
}


void merge_queues_memfree(LinkedQueue* head_queue, LinkedQueue* tail_queue) {
    /*
     A wrapper-function of the merge_queues function. Merges 2 LinkedQueues.
     Links tail-node of the head_queue to the head-node of the tail_queue.
     Replaces tail of the head_queue with that of the tail_queue. Frees the space
     allocated by the tail_queue.
     */
    merge_queues(head_queue, tail_queue);
    dealloc_queue(tail_queue);
}
