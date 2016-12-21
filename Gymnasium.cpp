//
//  Gymnasium.cpp
//  ortus
//
//  Created by onyx on 12/20/16.
//  Copyright © 2016 Andrew W.E. McDonald. All rights reserved.
//

#include "Gymnasium.hpp"

/* NOTES:
 *
 * In order to get ortus to learn that a stimulus is bad,
 * we must introduce it while causing something negative to happen.
 * 
 * need to have a 'good' and 'bad' part of the brain, which we kind of have (bad neurons)
 * but then when the 'good' or 'happy' part is stimulated,
 * that causes the weights of the things that caused it to happen to increase a little bit.
 * Conversely, if the 'bad' part is stimulated, synapses that tie that stimulus to the
 * bad state are triggered. 
 

 * for every new stimulus, perhaps we need to add a new neuron to the 'good' or 'bad' part, 
 * which will be responsible for creating 'meta' or 'different' connections to the synapses
 * that caused the 'good' or 'bad' feeling (that is, the thing that caused activation in the good or bad part)
 */
