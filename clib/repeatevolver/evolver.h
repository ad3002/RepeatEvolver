//
//  llist.h
//  repeatevolver
//  Created by Ilya Korvigo on 20.06.15.
//


#ifndef evolver_h
#define evolver_h


#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "gsl/gsl_rng.h"


const char substitution_matrix[4][4] = {
    "TGC",
    "AGC",
    "ATC",
    "ATG"
};


///////////////////////////
//////// Typedefs  ////////
///////////////////////////


typedef struct Individual {
    short rep_left;  //replications left
    char *sequence; // dna
} Individual;


enum substitution_keys {
    /*
     This enum contains "keys" to the substitution_matrix char array
     */
    A = 0,
    T = 1,
    G = 2,
    C = 3
};


///////////////////////////
//////// Functions ////////
///////////////////////////


extern unsigned int gsl_ran_bernoulli(const gsl_rng* r, double p);

extern unsigned int gsl_ran_poisson(const gsl_rng* r, double mu);


static inline void dealloc_individual(Individual* individual) {
    /*
     Frees memory allocated by an individual
     */
    assert(individual && "Got NULL instead of an individual pointer");
    free(individual);
    individual = NULL;
}


unsigned int substitute(double mutation_rate) {
    /* 
     Draws a boolean number from a binomial distribution with chance of
     success specified by the mutation_rate argument. 
     */
    const gsl_rng_type* T = gsl_rng_taus2;
    static gsl_rng* r = NULL;
    if (!r) {
        r = gsl_rng_alloc(T);
        gsl_rng_set(r, 123);
        return gsl_ran_bernoulli(r, mutation_rate);
    }
    return gsl_ran_bernoulli(r, mutation_rate);
}


unsigned int get_n_of_replicates(double lambda) {
    /*
     Draws a discrete number from a Poisson distribution with specified 
     average expectation - lambda.
     */
    const gsl_rng_type* T = gsl_rng_taus2;
    static gsl_rng* r = NULL;
    if (!r) {
        r = gsl_rng_alloc(T);
        gsl_rng_set(r, 123);
        return gsl_ran_poisson(r, lambda);
    }
    return gsl_ran_poisson(r, lambda);
}


static inline char* individuals_sequence(Individual* individual) {
    /*
     Extracts a nucleotide sequence from an individual
     */
    return individual->sequence;
}


static inline short individuals_rep_left(Individual* individual) {
    /*
     Extracts the number of replications an individual can undergo
     */
    return individual->rep_left;
}


char* str_alloc(int str_length) {
    /*
     Dynamically allocates a char array of size str_length + 1 and puts
     the string terminator in the very end.
     */
    char* str = malloc((str_length + 1) * sizeof *str);
    assert(str && "Failed to allocate string");
    str[str_length] = '\0';
    return str;
}


char* evolve_str(char* ancestor, int ancestor_length, double mutation_rate) {
    char* successor = str_alloc(ancestor_length);
    for (int i = 0; i < ancestor_length; i++) {
        if (substitute(mutation_rate)) {
            switch (ancestor[i]) {
                case 'A':
                    successor[i] = substitution_matrix[A][arc4random_uniform(3)];
                    break;
                case 'T':
                    successor[i] = substitution_matrix[T][arc4random_uniform(3)];
                    break;
                case 'G':
                    successor[i] = substitution_matrix[G][arc4random_uniform(3)];
                    break;
                case 'C':
                    successor[i] = substitution_matrix[C][arc4random_uniform(3)];
                    break;
                default:
                    break;
            }
        } else {
            successor[i] = ancestor[i];
        }
    }
    return successor;
}


char** reproduce_ancestor(char* seq, int seq_len, short n_child, double mut_r) {
    /*
     Allocates and writes an array of mutated copies of provided sequence (seq).
     The number of copies is defined by the n_children parameter; seq_len
     specifies sequence length. Mutation rate is defined by mut_r
     */
    char** children = malloc(n_child * sizeof *children);
    assert(children && "Failed to allocate pointers to children sequences");
    for (int i = 0; i < n_child; i++) {
        children[i] = evolve_str(seq, seq_len, mut_r);
    }
    return children;
}




void unload_sequence(char* sequence, int sequence_length);


LinkedQueue* process_individual(Individual* individual, int seq_len, double mut_r) {
    /*
     Returns a queue of descendant; calls unload_sequence to store the
     origninal sequence of passed individual. Frees memory allocated by the 
     passed individual.
     */
    char* seq = individuals_sequence(individual);
    short n_children = individuals_rep_left(individual);
    char** children = reproduce_ancestor(seq, seq_len, n_children, mut_r);
    LinkedQueue* child_queue = enqueue_pointers((void** )children, n_children);
    return child_queue;
}


#endif
