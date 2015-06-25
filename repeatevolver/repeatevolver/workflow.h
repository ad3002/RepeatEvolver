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


struct Node;
typedef struct Node {
    struct Node* next;
    int data;
} Node;


typedef struct Queue {
    Node* head;
    Node* tail;
    pthread_mutex_t queue_lock;
    pthread_cond_t condition;
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
    assert(queue->head);
    int data = queue->head->data;
    if (queue->head->next) {
        queue->head = queue->head->next;
    } else {
        queue->head = NULL;
    }
    return data;
}

void push_data(Queue* queue, int data) {
    assert(data);
    Node* new_node = malloc(sizeof *new_node);
    assert(new_node);
    new_node->data = 0;
    new_node->next = NULL;
    if (queue->head && queue->tail) {
        queue->tail->next = new_node;
        queue->tail = new_node;
    } else {
        queue->head = new_node;
        queue->tail = new_node;
    }
}

void* worker(void* args) {
    ThreadArgs* thread_args = (ThreadArgs* )args;
    Queue* queue = thread_args->queue;
    ThreadController* thread_controller = thread_args->thread_controller;
    // Locking the queue
    pthread_mutex_lock(&queue->queue_lock);
    while (!queue->head) {
        pthread_cond_wait(&queue->condition, &queue->queue_lock);
    }
    int data = pop_data(queue);
    if (data == thread_args->goal) {
        exit(0); // We're done
    }
    for (int i=0; i<2; i++) {
        push_data(queue, data+i+1);
    }
    pthread_cond_signal(&queue->condition);
    // Unlocking queue
    pthread_mutex_unlock(&queue->queue_lock);
    // Updating thread_controller counter
    pthread_mutex_lock(&thread_controller->controller_lock);
    thread_controller->threads_available++;
    pthread_cond_signal(&thread_controller->condition);
    pthread_mutex_unlock(&thread_controller->controller_lock);
    pthread_exit(0);
}


void* boss_thread(void* args) {
    ThreadArgs* thread_args = (ThreadArgs* )args;
    ThreadController* thread_controller = thread_args->thread_controller;
    pthread_t thread;
    pthread_attr_t attr;
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    int counter = 0;
    while (1) {
        pthread_mutex_lock(&thread_controller->controller_lock);
        while (!thread_controller->threads_available) {
            // If number of threads is maxed out
            pthread_cond_wait(&thread_controller->condition,
                              &thread_controller->controller_lock);
        }
        pthread_create(&thread, &attr, worker, args);
        thread_controller->threads_available--;
        counter++;
        printf("%d\n", counter);
        pthread_mutex_unlock(&thread_controller->controller_lock);
    }
    pthread_exit(NULL);
}

int main() {
    Queue queue;
    push_data(&queue, 0);
    pthread_mutex_init(&queue.queue_lock, NULL);
    pthread_cond_init(&queue.condition, NULL);
    
    ThreadController controller;
    controller.threads_available = 1;
    pthread_mutex_init(&controller.controller_lock, NULL);
    pthread_cond_init(&controller.condition, NULL);
    
    ThreadArgs args;
    args.goal = 100;
    args.queue = &queue;
    args.thread_controller = &controller;
    
    pthread_t thread;
    pthread_create(&thread, NULL, boss_thread, &args);
    pthread_join(thread, NULL);
    return 0;
}

///////////////////////////
//////// Functions ////////
///////////////////////////





#endif //REPEATEVOLVER_WORKFLOW_H
