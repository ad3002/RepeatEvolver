//
//  workflow.h
//  repeatevolver
//
//  Created by Ilya Korvigo on 25.06.15.
//  Copyright (c) 2015 ARRIAM. All rights reserved.
//

#ifndef REPEATEVOLVER_WORKFLOW_H
#define REPEATEVOLVER_WORKFLOW_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

///////////////////////////
//////// Typedefs  ////////
///////////////////////////


#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


struct Node;
typedef struct Node {
    struct Node* next;
    int data;
} Node;


typedef struct Queue {
    Node* head;
    Node* tail;
    unsigned long size;
    pthread_mutex_t queue_lock;
    pthread_cond_t  condition;
} Queue;


typedef struct ThreadController {
    pthread_cond_t condition;
    pthread_mutex_t controller_lock;
    unsigned int threads_available;
} ThreadController;


typedef struct ThreadArgs {
    Queue* queue;
    ThreadController* thread_controller;
    int goal; // The number to reach
} ThreadArgs;


int pop_data(Queue* queue) {
    assert(queue->size);
    int data = queue->head->data;
    Node* current_head = queue->head;
    if (queue->head->next) {
        queue->head = queue->head->next;
    } else {
        queue->head = NULL;
        queue->tail = NULL;
    }
    --queue->size;
    free(current_head);
    return data;
}

void push_data(Queue* queue, int data) {
    Node* node = malloc(sizeof *node);
    assert(node);
    node->data = data;
    node->next = NULL;
    if (!queue->size) {
        queue->head = node;
        queue->tail = node;
    } else {
        queue->tail->next = node;
        queue->tail = node;
    }
    queue->size++;
}


void* worker_thread(void* args) {
    ThreadArgs* thread_args = args;
    ThreadController* thread_controller = thread_args->thread_controller;
    Queue* queue = thread_args->queue;
    
    pthread_mutex_lock(&queue->queue_lock);
    while (!queue->size) {
        pthread_cond_wait(&queue->condition, &queue->queue_lock);
    }
    
    int data = pop_data(queue);
    printf("Worker popped %d\n", data);
    
    if (data >= thread_args->goal) {
        exit(0);
    }
    pthread_mutex_unlock(&queue->queue_lock);
    
    // Processing data...
    
    pthread_mutex_lock(&queue->queue_lock);
    for (int i = 1; i < 3; i++) {
        push_data(queue, data+(10*i));
        printf("Worker pushed %d\n", data+(10*i));
    }
    pthread_cond_signal(&queue->condition);
    pthread_mutex_unlock(&queue->queue_lock);
    
    pthread_mutex_lock(&thread_controller->controller_lock);
    thread_controller->threads_available++;
    pthread_cond_signal(&thread_controller->condition);
    pthread_mutex_unlock(&thread_controller->controller_lock);
    pthread_exit(NULL);
}


void* boss_thread(void* args) {
    ThreadArgs* thread_args = args;
    ThreadController* thread_controller = thread_args->thread_controller;
    pthread_t thread;
    int counter = 0;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    while (1) {
        pthread_mutex_lock(&thread_controller->controller_lock);
        while (!thread_controller->threads_available) {
            pthread_cond_wait(&thread_controller->condition, &thread_controller->controller_lock);
        }
        if (!pthread_create(&thread, &attr, worker_thread, args)) {
            pthread_detach(thread);
            thread_controller->threads_available--;
            printf("Started thread # %d\n", ++counter);
        }
        pthread_mutex_unlock(&thread_controller->controller_lock);
    }
    pthread_exit(NULL);
}


int main() {
    Queue queue;
    queue.size = 0;
    queue.head = NULL;
    queue.tail = NULL;
    push_data(&queue, 1);
    pthread_mutex_init(&queue.queue_lock, NULL);
    pthread_cond_init(&queue.condition, NULL);
    
    ThreadController controller;
    controller.threads_available = 1;
    pthread_mutex_init(&controller.controller_lock, NULL);
    pthread_cond_init(&controller.condition, NULL);
    
    ThreadArgs args;
    args.goal = 1000;
    args.thread_controller = &controller;
    args.queue = &queue;
    
    pthread_t thread;
    pthread_create(&thread, NULL, boss_thread, &args);
    pthread_join(thread, NULL);
    return 0;
}
///////////////////////////
//////// Functions ////////
///////////////////////////





#endif //REPEATEVOLVER_WORKFLOW_H
