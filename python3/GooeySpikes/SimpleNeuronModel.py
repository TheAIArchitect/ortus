#!/usr/bin/python

from __future__ import division

import sys
 
import random

import math


class SimpleNeuron:

    # variables specified out here are class variables, and are *shared* between instances
    revPotExcite = -75
    revPotInhibit = 0
    
    def __init__(self, neuronType):
        # variables specified in here, are instance variables and are not shared between instances
        self.activationHistory = []
        self.newInputs = 0
        #self.d = 2
        # regular spiking (RS): d = 8, c = -65
        self.a = 0.02
        self.b = 0.2
        self.c = -65
        self.d = 8
        if (neuronType.__eq__("sensory")): #Intrinsically Bursting (IB)
            self.c = -55
            self.d = 4
        #elif (neuronType.__eq__("inter")): #regular spiking for now
            

        self.baselineActivation = self.c
        self.activation = self.baselineActivation
        self.decay = self.b*self.activation
        self.numUpdates = 0

    ''' NOTE: this resets:
        - the activation history
        - the number of new inputs 
        - all parameters
        - baseline activaiton
        - activation
        - decay
        - numUpdates
    '''
    def clearData(self):
        self.activationHistory = []
        self.newInputs = 0
        self.numUpates = 0

    ''' updates parameters, and then calls update() '''
    def updateParameters(self, a, b, c, d):
        self.a = a
        self.b = b
        self.c = c
        self.d = d
        self.baselineActivation = self.c
        self.activation = self.baselineActivation
        self.decay = self.b*self.activation


    def getActivation(self):
        return self.activation

    def fired(self):
        # would be better to use slope computation to determine this... right?
        return (self.activation >= 30)

    def addInput(self, inputAct):
        self.newInputs = self.newInputs + inputAct


    def shitupdate(self):
        actDiff = 0
        decayDiff = 0
        if (self.activation >= 30):
            self.activation = self.baselineActivation
            self.decay = self.decay + self.d
        else:
            # then compute the updated activation, based upon current activation, decay, and total of new inputs
            actDiff = 0.04*math.pow(self.activation,2) + 5*self.activation + 140 - self.decay + self.newInputs
            decayDiff = self.a*(self.b*self.activation - self.decay)
        self.activation = self.activation + actDiff
        self.decay = self.decay + decayDiff
        # update our activation history
        self.activationHistory.append(self.activation)
        self.newInputs = 0;

    def update(self):
        if (self.activation >= 30):
            self.activation = self.baselineActivation
            self.decay = self.decay + self.d
        # compute the updated activation, based upon current activation, decay, and total of new inputs
        actDiff = 0.04*math.pow(self.activation,2) + 5*self.activation + 140 - self.decay + self.newInputs
        decayDiff = self.a*(self.b*self.activation - self.decay)
        self.activation = self.activation + actDiff
        #if (self.activation > 30):
        #    self.activation = 30
        self.decay = self.decay + decayDiff
        
        # update our activation history
        self.activationHistory.append(self.activation)
        self.numUpdates += 1
        #if (len(self.activationHistory) % 1000 == 0):
        #    print(str(len(self.activationHistory))+", "+str(self.numUpdates))
        self.newInputs = 0;

        #merge the bolztmann deal from HH into above to allow membrane potential to impact... influence of individual neurons. maybe

