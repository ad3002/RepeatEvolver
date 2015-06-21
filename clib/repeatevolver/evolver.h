//
//  llist.h
//  repeatevolver
//  Created by Ilya Korvigo on 20.06.15.
//

#ifndef evolver_h
#define evolver_h


#include <string.h>
#include "gsl/gsl_rng.h"


///////////////////////////
//////// Typedefs  ////////
///////////////////////////


typedef struct Individual {
    ushort rep_left;  //replications left
    char *sequence; // dna
} Individual;


///////////////////////////
//////// Functions ////////
///////////////////////////


ushort substitute(float mutation_rate) {
    /* 
     Draws boolean numbers from a binomial distribution with chance of
     success specified by the mutation_rate argument. 
     */
    return 0;
}


inline char* individuals_sequence(Individual* individual) {
    /*
     Extracts a nucleotide sequence from an individual
     */
    return individual->sequence;
}


inline ushort individuals_rep_left(Individual* individual) {
    /*
     Extracts the number of replications an individual can undergo
     */
    return individual->rep_left;
}


char* str_alloc(str_length) {
    char* str = malloc((str_length + 1) * sizeof *str);
    assert(str && "Failed to allocate string");
    str[str_length] = '\0';
    return str;
}


char** reproduce_ancestor(char* sequence, int seq_len, ushort n_children) {
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
    ushort n_children = individuals_rep_left(individual);
    
    return 0;
}
                     




#endif
