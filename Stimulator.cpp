//
//  ElementStimulator.cpp
//  ortus
//
//  Created by onyx on 12/20/16.
//  Copyright © 2016 Andrew W.E. McDonald. All rights reserved.
//

#include "Stimulator.hpp"

/*
Notes:


Okay, so, there’s going to be a Stimulator that takes input from either a PrimitiveStimulus, or a ComplexStimulus -- both subclasses of Stimulus. A PrimitiveStimulus will be an array of some length that holds values to add to a single sensory neuron (or, perhaps interneuron, when we want to make it ‘think’).  there will also be a time step variable, that gets incremented, until it gets to the end, at which point the object should mark itself for deletion (can an object delete itself?). A Complex stimulus will have an array of PrimitveStimuli, and upon being called will call the stimulate() function for each primitive. Once all primitives have been marked for deletion, it will mark itself for deletion. The Stimulator will hold all stimuli -- complex and primitive, and will, each time it’s called, run through its vector of stimuli, and tell them to do their thing -- BUT, it gets the data from them, in a map, and then these values run through a function in stimulator to ‘normalize’ them, before stimulator adds them to the voltageVector. This is so that there’s a logarithmic nature to the activations of sensory neurons (e.g., you can’t add 150 activation to a neuron... it’ll probably be a function of the neuron’s current activation). 
 
*/
