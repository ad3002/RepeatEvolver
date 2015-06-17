#! /usr/bin/env python

"""
Basic nucleotide sequence objects. This module is to be rewritten in Cython.
"""


from __future__ import division, print_function
from collections import deque
import numpy as np
import random
import copy
import argparse
import os


BASES = map(ord, ["A", "T", "G", "C"])


class ReplicationError(BaseException):
    def __init__(self):
        BaseException.__init__(self, "Exceeded replication limit")


class Entity(object):
    """
    A basic abstraction of an individual in the simulation.
    """
    def __init__(self, sequence, poisson_lambda, evolver, generation=0):
        """
        :param sequence: a nucleotide sequence
        :param poisson_lambda: the lambda parameter of Poisson distribution used
        to pick the number of replications an individual can undergo before
        death
        :param evolver: a callable object that returns substitutions with
        corresponding positions in the original sequence
        :param generation: the number of replication past since the most recent
        common ancestor
        :return:
        """
        self._seq = bytearray(sequence)
        self._poisson_lambda = poisson_lambda
        self._replications_left = self.life_cycles()
        self._evolver = evolver
        self._generation = generation

    @property
    def sequence(self):
        return self._seq

    @property
    def generation(self):
        return self._generation

    def replicate(self):
        if not self._replications_left:
            raise ReplicationError
        self._replications_left -= 1
        return Entity(self._evolver(self._seq),
                      self._poisson_lambda,
                      self._evolver,
                      self._generation+1)

    def life_cycles(self):
        """
        Returns the number of times an individual can replicate before it dies.
        The number number is picked from Poisson distribution with given lambda
        :return:
        """
        return np.random.poisson(self._poisson_lambda)


def basic_evolver(seq, mutation_rate=0.05):
    """
    Picks probability of base substitution from binomial distribution with given
    mutation rate.
    :param seq: mutable sequence object
    :param mutation_rate: chance of independent substitution
    :rtype: bytearray
    """
    seq = copy.copy(seq)
    for i in xrange(len(seq)):
        if np.random.binomial(1, mutation_rate):
            seq[i] = random.choice([base for base in BASES if base != seq[i]])
    return seq


def simulate(starting_seq, generations, poisson_lambda, evolver):
    pool = deque()
    pool.append(Entity(starting_seq, poisson_lambda, evolver))
    dead = []
    while pool[0].generation <= generations:
        try:
            pool.append(pool[0].replicate())
        except ReplicationError:
            dead.append(pool.popleft())
    return dead


def generate_random_dna_string(length):
    return bytearray([random.choice(BASES) for _ in xrange(length)])


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-n', dest='ngen', type=int, help='n generations')
    parser.add_argument('-o', dest='out', type=str, help='an output file')
    args = parser.parse_args()
    starting_seq = generate_random_dna_string(200)
    generations = args.ngen
    output_file = os.path.abspath(args.out)
    poisson_lambda = 3
    seqs = simulate(starting_seq, generations, poisson_lambda, basic_evolver)
    with open(output_file, "w") as op:
        for seq in seqs:
            print(seq.sequence, seq.generation, sep="\t", file=op)


if __name__ == "__main__":
    main()
