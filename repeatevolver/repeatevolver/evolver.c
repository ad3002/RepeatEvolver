//
// Created by Ilia Korvigo on 25.06.15.
//


#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "evolver.h"
#include "queue.h"
#include "gsl/gsl_rng.h"


const char SUBST_MATRIX[4][4] = {
    /*
     Substitution matrix used to select replacement bases
     */
    "TGC", // substitutions for A
    "AGC", // substitutions for T
    "ATC", // substitutions for G
    "ATG"  // substitutions for C
};


///////////////////////////
//////// Functions ////////
///////////////////////////


extern unsigned int gsl_ran_bernoulli(const gsl_rng* r, double p);

extern unsigned int gsl_ran_poisson(const gsl_rng* r, double mu);


Individual* init_individual() {
    /*
     Initializes an empty individual
     */
    Individual* individual = malloc(sizeof *individual);
    assert(individual && "Failed to allocate memory for an individual");
    individual->replications = 0;
    individual->generation = 0;
    individual->sequence = NULL;
    return individual;
}


void dealloc_individual(Individual* individual) {
    /*
     Frees memory allocated by an individual including the undelrying sequence
     */
    assert(individual && "Got NULL instead of an individual pointer");
    /*free(individual->sequence);*/
    individual->sequence = NULL;
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


unsigned int get_n_of_replications(double expected) {
    /*
     Draws a discrete number from a Poisson distribution with specified
     average expectation - expected.
     */
    const gsl_rng_type* T = gsl_rng_taus2;
    static gsl_rng* r = NULL;
    if (!r) {
        r = gsl_rng_alloc(T);
        gsl_rng_set(r, 123);
        return gsl_ran_poisson(r, expected);
    }
    return gsl_ran_poisson(r, expected);
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
                    successor[i] = SUBST_MATRIX[keyA][arc4random_uniform(3)];
                    break;
                case 'T':
                    successor[i] = SUBST_MATRIX[keyT][arc4random_uniform(3)];
                    break;
                case 'G':
                    successor[i] = SUBST_MATRIX[keyG][arc4random_uniform(3)];
                    break;
                case 'C':
                    successor[i] = SUBST_MATRIX[keyC][arc4random_uniform(3)];
                    break;
                default:
                    assert(0 && "Non-DNA symbol passed");
            }
        } else {
            successor[i] = ancestor[i];
        }
    }
    return successor;
}


Individual**
reproduce_parent(Individual* parent, int seq_len, double mut_r, double exp_rep) {
    /*
     Allocates and writes an array of mutated descendants of a parent.
     seq_len specifies sequence length. Mutation rate is defined by mut_r.
     exp_rep specifies expected number of replication per individual.
     */
    assert(parent->sequence);
    Individual** children = malloc(parent->replications * sizeof *children);
    assert(children && "Failed to allocate pointers to children individuals");
    for (int i = 0; i < parent->replications; i++) {
        children[i] = init_individual();
        children[i]->sequence = evolve_str(parent->sequence, seq_len, mut_r);
        children[i]->replications = get_n_of_replications(exp_rep);
        //printf("Replications %d\n", children[i]->replications);
        children[i]->generation = parent->generation + 1;
    }
    return children;
}



