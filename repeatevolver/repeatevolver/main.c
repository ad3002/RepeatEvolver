//
//  main.c
//  repeatevolver
//
//  Created by Ilya Korvigo on 25.06.15.
//  Copyright (c) 2015 ARRIAM. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "evolver.h"
#include "queue.h"
#include "workflow.h"
#include <assert.h>
#include <string.h>



int main(int argc, char* argv[]) {
    if (argc < 4) {
        printf("Pass all parameters, i.e. expected replication, mutation rate, output file, generations\n, threads available");
        exit(1);
    }
    double expected_replications = strtod(argv[1], NULL);
    double mutation_rate = strtod(argv[2], NULL);
    char* output_file_path = argv[3];
    long goal = strtol(argv[4], NULL, 10);
    long threads = strtol(argv[5], NULL, 10);
    int seq_len = 10;
    
    LinkedQueue* queue = init_queue();
    Individual* ancestor = init_individual();
    ancestor->generation = 0;
    ancestor->replications = 2;
    ancestor->sequence = malloc(11 * sizeof( char*));
    strcpy(ancestor->sequence, "AAAAAAAAAA\0");
    push_data(ancestor, queue);

    ThreadController controller;
    controller.threads_available = (int)threads;
    pthread_mutex_init(&controller.controller_lock, NULL);
    pthread_cond_init(&controller.condition, NULL);
    
    ThreadArgs* args = init_args(queue,
                                 &controller,
                                 expected_replications,
                                 mutation_rate,
                                 output_file_path,
                                 seq_len,
                                 (int)goal);

    pthread_t thread;
    pthread_create(&thread, NULL, boss_thread, args);
    pthread_join(thread, NULL);
    return 0;
}
