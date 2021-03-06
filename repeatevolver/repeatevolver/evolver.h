//
// Created by Ilia Korvigo on 25.06.15.
//

#ifndef REPEATEVOLVER_EVOLVER_H
#define REPEATEVOLVER_EVOLVER_H


///////////////////////////
//////// Typedefs  ////////
///////////////////////////


typedef struct Individual {
    unsigned int replications;  //replications left
    char*        sequence;      // dna
    unsigned int generation;
} Individual;


enum substitution_keys {
    /*
     This enum contains "keys" to the substitution_matrix char array
     */
    keyA = 0,
    keyT = 1,
    keyG = 2,
    keyC = 3
};


///////////////////////////
//////// Functions ////////
///////////////////////////


int randint(int l, int u);


Individual* init_individual();


void dealloc_individual(Individual* individual);


unsigned int substitute(double mutation_rate);


unsigned int get_n_of_replications(double expected);


char* str_alloc(int str_length);


char* evolve_str(char* ancestor, int ancestor_length, double mutation_rate);


Individual**
reproduce_parent(Individual* parent, int seq_len, double mut_r, double exp_rep);


#endif //REPEATEVOLVER_EVOLVER_H

