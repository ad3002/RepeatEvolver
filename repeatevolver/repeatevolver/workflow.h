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
#include "queue.h"
#include "evolver.h"


///////////////////////////
//////// Typedefs  ////////
///////////////////////////


typedef struct ThreadController {
    pthread_cond_t condition;
    pthread_mutex_t controller_lock;
    unsigned int threads_available;
} ThreadController;


typedef struct ThreadArgs {
    LinkedQueue *       queue;
    ThreadController*   thread_controller;
    int                 goal; // The generation to reach
    double              mutation_rate;
    double              expected_replications;
    int                 sequence_length;
    FILE*               output_file;
    pthread_mutex_t     file_lock;
} ThreadArgs;



///////////////////////////
//////// Functions ////////
///////////////////////////


ThreadArgs* init_args(LinkedQueue* queue,
                      ThreadController* thread_controller,
                      double expected_replications,
                      double mutation_rate,
                      char* output_file_path,
                      int sequence_len,
                      int goal);


void print_individual_to_file(Individual* indiv, FILE* file, pthread_mutex_t* file_lock);


void* worker_thread(void* args);


void* boss_thread(void* args);






#endif //REPEATEVOLVER_WORKFLOW_H
