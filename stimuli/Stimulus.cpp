//
//  Stimulus.cpp
//  ortus
//
//  Created by andrew on 1/7/17.
//  Copyright © 2017 Andrew W.E. McDonald. All rights reserved.
//

#include "Stimulus.hpp"

Stimulus::Stimulus(){
    timeStep = 0;
    finished = false; // true when timeStep == duration, iff loop == false
    loop = false;
    duration = 0;
    iterationNumber = 0;
    delay = 0;
    signalp = NULL;
}

Stimulus::~Stimulus(){
    if (signalp != NULL){
        delete signalp;
    }
}

/* needs to be implemented by all derived classes
 *
 * dont want to make pure virtual, maybe for no good reason.
 * */
float Stimulus::getDerivedStimulus(int timeStep){
    return 0.f;
}

float Stimulus::getStimulus(){
    if (delay > 0){ // causes function to return zero until we've gone 'delay' timesteps.
        delay--; // maybe --delay...
        return 0.f;
    }
    float derivedStimulus = getDerivedStimulus(timeStep);
    stepTime();
    if (timeStep >= duration){
        if (loop){
            resetTime();
        }
        else {
            finish();
        }
    }
    return derivedStimulus;
}

void Stimulus::setSignal(Signal* signalp){
    this->signalp = signalp;
}


void Stimulus::stepTime(){
    timeStep++;
}

void Stimulus::resetTime(){
    timeStep = 0;
    iterationNumber++;
}

void Stimulus::finish(){
    finished = true;
    
}

void Stimulus::setDuration(int numSteps){
    duration = numSteps;
}
