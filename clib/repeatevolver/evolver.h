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


///////////////////////////
//////// Typedefs  ////////
///////////////////////////


typedef struct Individual {
    short rep_left;  //replications left
    char *sequence; // dna
} Individual;


///////////////////////////
//////// Functions ////////
///////////////////////////


extern unsigned int gsl_ran_bernoulli(const gsl_rng* r, double p);

extern unsigned int gsl_ran_poisson(const gsl_rng* r, double mu);


unsigned int substitute(double mutation_rate) {
    /* 
     Draws a boolean number from a binomial distribution with chance of
     success specified by the mutation_rate argument. 
     */
    const gsl_rng_type* T = gsl_rng_taus2;
    static gsl_rng* r = NULL;
    if (r) {
        return gsl_ran_bernoulli(r, mutation_rate);
    }
    r = gsl_rng_alloc(T);
    gsl_rng_set(r, 123);
    return gsl_ran_bernoulli(r, mutation_rate);
}


unsigned int get_n_of_replicates(double lambda) {
    /*
     Draws a discrete number from a Poisson distribution.
     */
    const gsl_rng_type* T = gsl_rng_taus2;
    static gsl_rng* r = NULL;
    if (r) {
        return gsl_ran_poisson(r, lambda);
    }
    r = gsl_rng_alloc(T);
    gsl_rng_set(r, 123);
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
    char* str = malloc((str_length + 1) * sizeof *str);
    assert(str && "Failed to allocate string");
    str[str_length] = '\0';
    return str;
}


char** reproduce_ancestor(char* sequence, int seq_len, short n_children) {
    char** children = malloc(n_children * sizeof *children);
    assert(children && "Failed to allocate pointers to children sequences");
    for (int i = 0; i < n_children; i++) {
        children[i] = str_alloc(seq_len);
        strcpy(children[i], sequence);
    }
    return children;
}


Individual* replicate_individual(Individual* individual) {
    /*
     Returns an individual that evolved from the given individual
     */
    char* sequence = individuals_sequence(individual);
    short n_children = individuals_rep_left(individual);
    
    return 0;
}


#endif
