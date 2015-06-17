#! /usr/bin/env python

"""
Basic nucleotide sequence objects. This module is to be rewritten in Cython.
"""


from __future__ import division, print_function
import numpy as np


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
        :param evolver: a callable object that takes a sequence and returns a
        new sequence
        :param generation: the number of replication past since the most recent
        common ancestor
        :return:
        """
        self._seq = bytearray(sequence)
        self._poisson_lambda = poisson_lambda
        self._replications_left = self.life_cycles(self._poisson_lambda)
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

    @staticmethod
    def life_cycles(poisson_lambda):
        """
        Returns the number of times an individual can replicate before it dies.
        The number number is picked from Poisson distribution with given lambda
        :return:
        """
        return np.random.poisson(poisson_lambda)
