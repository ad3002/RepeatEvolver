//
//  workflow.c
//  repeatevolver
//
//  Created by Ilya Korvigo on 25.06.15.
//  Copyright (c) 2015 ARRIAM. All rights reserved.
//

#include "workflow.h"
#include "queue.h"
#include "evolver.h"


ThreadArgs* init_args(LinkedQueue* queue,
                      ThreadController* thread_controller,
                      double expected_replications,
                      double mutation_rate,
                      char* output_file_path,
                      int sequence_len,
                      int goal)
{
    ThreadArgs* args = malloc(sizeof *args);
    assert(args && "Failed to allocate args");
    args->queue = queue;
    args->thread_controller = thread_controller;
    args->expected_replications = expected_replications;
    args->goal = goal;
    args->mutation_rate = mutation_rate;
    args->output_file = fopen(output_file_path, "w");
    pthread_mutex_init(&args->file_lock, NULL);
    args->sequence_length = sequence_len;
    return args;
}


void print_individual_to_file(Individual* indiv, FILE* file, pthread_mutex_t* file_lock) {
    pthread_mutex_lock(file_lock);
    fprintf(file, "%s\t%d\n", indiv->sequence, indiv->generation);
    pthread_mutex_unlock(file_lock);
}


void* worker_thread(void* args) {
    ThreadArgs* thread_args = args;
    LinkedQueue* queue = thread_args->queue;
    int current_generation = 0;
    while (current_generation < thread_args->goal) {
        pthread_mutex_lock(&queue->queue_lock);
        while (!queue->size) {
            pthread_cond_wait(&queue->queue_cond, &queue->queue_lock);
        }
        
        Individual* parent = (Individual* )pop_data(queue);
        pthread_mutex_unlock(&queue->queue_lock);
        
        // Processing data...
        
        current_generation = parent->generation;
        if (current_generation >= thread_args->goal) {
            exit(0);
        }
        Individual** descendants = reproduce_parent(parent,
                                                    thread_args->sequence_length,
                                                    thread_args->mutation_rate,
                                                    thread_args->expected_replications);
        LinkedQueue*
        descendants_queue = enqueue_data_pointers((void** )descendants,
                                                  parent->replications);
        print_individual_to_file(parent,
                                 thread_args->output_file,
                                 &thread_args->file_lock);
        dealloc_individual(parent);
        free(descendants);
        
        if (descendants_queue->size) {   // If parent had children
            pthread_mutex_lock(&queue->queue_lock);
            
            merge_queues_memfree(queue, descendants_queue);
            pthread_cond_signal(&queue->queue_cond);
            
            pthread_mutex_unlock(&queue->queue_lock);
        }
    }
    pthread_exit(NULL);
}


void* boss_thread(void* args) {
    ThreadArgs* thread_args = (ThreadArgs* )args;
    ThreadController* thread_controller = thread_args->thread_controller;
    pthread_t threads[thread_controller->threads_available];
    for (int i = 0; i < thread_controller->threads_available; i++) {
        pthread_create(&threads[i], NULL, worker_thread, args);
    }
    for (int i = 0; i < thread_controller->threads_available; i++) {
        pthread_join(threads[i], NULL);
    }
    pthread_exit(NULL);
}
