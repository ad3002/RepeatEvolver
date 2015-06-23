#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "gsl/gsl_rng.h"

#include "queue.h"
#include "evolver.h"
#include "workflow.h"


#define MAX_FILE_NAME_LEN 100


pthread_mutex_t queue_access_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t file_write_mutex = PTHREAD_MUTEX_INITIALIZER;



int main(int argc, char* argv[]) {
    // Sequence
    char* seq = malloc(101 * sizeof *seq);
    seq = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
    
    
    // Getting parameters
    if (argc < 5) {
        printf("This is supposed to be a help message, but help is for the weak\n");
        exit(0);
    }
    double mutation_rate = strtod(argv[1], NULL);
    double expected_replicates = strtod(argv[2], NULL);
    int num_of_generations = (int)strtol(argv[3], NULL, 10);
    char output_path[MAX_FILE_NAME_LEN];
    realpath(argv[4], output_path);
    
    printf("Starting process with the following parameters:\n");
    printf("mutation rate %f\n", mutation_rate);
    printf("expected repl %f\n", expected_replicates);
    printf("num of generations %d\n", num_of_generations);
    printf("op_file %s\n", output_path);
    
    // Creating a LinkedQueue and the first node
    LinkedQueue* main_queue = init_queue();
    QueueNode* ancestor_node = init_node();
    ancestor_node->data = seq;
    push_data(ancestor_node, main_queue);
    
    // Creating an output file and the original ThreadArgs template
    FILE* output_file = fopen(output_path, NULL);
    ThreadArgs* thread_args = init_thread_args();
    thread_args->file_access_mutex_ptr = &file_write_mutex;
    thread_args->queue_access_mutex_ptr = &queue_access_mutex;
    thread_args->output_file = output_file;
    thread_args->estimated_replications = expected_replicates;
    thread_args->generation_limit = num_of_generations;
    thread_args->mutation_rate = mutation_rate;
    thread_args->unloader_thread = basic_unloader;
    thread_args->queue = main_queue;
    thread_args->individual = NULL;
    thread_args->sequence_len = 100;
    
    // Kicking off the first supplier
    pthread_t* first_supplier;
    pthread_create(first_supplier, NULL, start_supplier_thread, thread_args);
    pthread_join(*first_supplier, NULL);
    
    // Waiting for the process to finish or to be killed
    printf("Input something to kill the process if you don't want to wait any longer\n");
    getchar();
    return(0);
}