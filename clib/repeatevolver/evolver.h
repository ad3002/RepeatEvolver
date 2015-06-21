//
//  llist.h
//  repeatevolver
//  Created by Ilya Korvigo on 20.06.15.
//

#ifndef evolver_h
#define evolver_h


#include "gsl/gsl_rng.h"


///////////////////////////
//////// Typedefs  ////////
///////////////////////////


typedef struct Individual {
    uint rep_left;  //replications left
    char *sequence; // dna
} Individual;


///////////////////////////
//////// Functions ////////
///////////////////////////


Individual* replicate_individual(Individual* individual) {
    /*
     Returns an individual that evolved from the given individual
     */
    
    return 0;
};


char* extract_sequence_from_individual(Individual* individual) {
    /*
     Extracts a nucleotide sequence from an individual
     */
    
    return 0;
};



#endif
