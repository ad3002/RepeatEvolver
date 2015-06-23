//
//  unload.h
//  repeatevolver
//  Created by Ilia Korvigo on 23.06.15.
//
//
//  The workflow decribes all general steps that follow supplier initialisation:
//
//  1. Supplier pops data-nodes from a queue and passes it to a processor. It
//     will wait for the processor to finish its job.
//  2. The processor makes its magic and adds new data-nodes to the queue
//  3. The processor starts new suppliers. The number of suppliers is equal to
//     the number of new data nodes created by the processor.
//  4. The processor exits, hence the original supplier continues its work. It
//     calls an unloader to output the data into a file.
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include "evolver.h"
#include "queue.h"


#ifndef workflow_h
#define workflow_h


///////////////////////////
//////// Typedefs  ////////
///////////////////////////



struct ThreadArgs;
typedef struct ThreadArgs {
    LinkedQueue* queue;
    FILE* output_file;
    pthread_mutex_t* queue_access_mutex_ptr;
    pthread_mutex_t* file_access_mutex_ptr;
    double mutation_rate;
    double estimated_replications;
    int sequence_len;
    int generation_limit;
    void* (*unloader_thread)(void* );
    Individual* individual;
} ThreadArgs;


///////////////////////////
//////// Functions ////////
///////////////////////////


pthread_t* init_thread() {
    pthread_t* thread = malloc(sizeof *thread);
    assert(thread && "Failed to allocate thread");
    return thread;
}


ThreadArgs* init_thread_args() {
    ThreadArgs* thread_args = malloc(sizeof *thread_args);
    assert(thread_args && "Failed to allocate memory for a ThreadArgs instance");
    return thread_args;
}


ThreadArgs* init_indiv_args(ThreadArgs* template_args, Individual* individual) {
    ThreadArgs* thread_args = init_thread_args();
    memcpy(thread_args, template_args, sizeof *thread_args);
    assert(individual->sequence);
    thread_args->individual = individual;
    return thread_args;
}


void dealloc_individual_args(ThreadArgs* thread_args) {
    assert(thread_args && "NULL passed instead of ThreadArgs*");
    dealloc_individual(thread_args->individual);
    free(thread_args);
}


void* basic_unloader(void* thread_args) {
    printf("Unloader initated\n");
    ThreadArgs* args = (ThreadArgs* )thread_args;
    Individual* individual = args->individual;
    FILE* output_file = args->output_file;
    
    // Locking file access
    pthread_mutex_lock(args->file_access_mutex_ptr);
    
    fprintf(output_file, "%s\t%d\n", individual->sequence, individual->generation);
    
    pthread_mutex_unlock(args->file_access_mutex_ptr);
    
    //Lock released. Deallocating arguments struct
    dealloc_individual_args(args);
    
    pthread_exit(NULL);
}


void* start_processor_thread(void* thread_args) {
    ThreadArgs* args = (ThreadArgs* )thread_args;
    assert(args->individual->sequence);
    Individual** children = reproduce_parent(args->individual,
                                             args->sequence_len,
                                             args->mutation_rate,
                                             args->estimated_replications);
    LinkedQueue* children_queue = enqueue_data_pointers((void** )children,
                                                        args->individual->replications);
    // Locking queue access to push data
    pthread_mutex_lock(args->queue_access_mutex_ptr);
    
    merge_queues(args->queue, children_queue);
    
    pthread_mutex_unlock(args->queue_access_mutex_ptr);
    
    //Queue unlocked. Starting new suppliers
    for (int i = 0; i < args->individual->replications; i++) {
        pthread_t supplier;
        pthread_create(&supplier, NULL, start_processor_thread, thread_args);
    }
    
    pthread_exit(NULL);
}


void* start_supplier_thread(void* thread_args) {
    ThreadArgs* args = (ThreadArgs* )thread_args;
    LinkedQueue* queue = args->queue;
    
    // Locking queue access during processor initialization
    pthread_mutex_lock(args->queue_access_mutex_ptr);
    
    // exiting application if enough generations were simulated
    Individual* individual = pop_data(queue);
    assert(individual->sequence);
    
    if (individual->generation >= args->generation_limit) {
        printf("Successfully passed %d generations\n", args->generation_limit);
        exit(0);
    }
    
    pthread_mutex_unlock(args->queue_access_mutex_ptr);
    
    // Queue unlocked. Now packing data and starting an individual_processor
    ThreadArgs* indiv_thread_args = init_indiv_args(args, individual);
    pthread_t processor;
    pthread_create(&processor, NULL, start_processor_thread, indiv_thread_args);
    
    //Waiting for processor to finish
    pthread_join(processor, NULL);
    
    // Calling unloader. Unloader is supposed to deallocate individual and its
    // underlying data as well as the thread_args passed to it, since unloader
    // is the final consumer of this object.
    pthread_t unloader;
    pthread_create(&unloader, NULL, args->unloader_thread, indiv_thread_args);
    
    pthread_exit(NULL);
}



#endif
