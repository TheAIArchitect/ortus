//
//  Signal.cpp
//  ortus
//
//  Created by andrew on 1/7/17.
//  Copyright © 2017 Andrew W.E. McDonald. All rights reserved.
//

#include "Signal.hpp"

float Signal::ZEROF = 0.f;

/* if initialize, make sure signal points to something. if not, don't. */
Signal::Signal(bool initialize){
    if (initialize){
        signal = &ZEROF;
        signalLength = 1;
    }
    callDeleteOnSignal = false;
}

Signal::~Signal(){
    if (callDeleteOnSignal){
        delete signal;
    }
}

/* generates a signal, inclusive: [startTime, endTime], of specified length */
void Signal::generateFullSignal(float startTime, float endTime, int length){
    // to make this easier for the individual signals,
    // we compute the increment size:
    // => e.g. if start is .05 and end is .75, and length is 12,
    // then increment would be .0636
    // so, we would start at .05, and finish at .75, with 12 values (so, this is inclusive)
    signalLength = length;
    int numDivisions = signalLength - 1;
    float domain = endTime - startTime;
    float increment = domain/((float)numDivisions);
    signal = new float[length];
    callDeleteOnSignal = true;
    generateFullSignalDerived(startTime, increment, length);
}

float Signal::getValueAtIndex(int index){
    if (index >= 0 && index < signalLength){
        return signal[index];
    }
    return 0.f;
}

void Signal::generateFullSignalDerived(float startTime, float increment, int length){
    return;
}

