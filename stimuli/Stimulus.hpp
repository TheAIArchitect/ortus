//
//  Stimulus.hpp
//  ortus
//
//  Created by andrew on 1/7/17.
//  Copyright © 2017 Andrew W.E. McDonald. All rights reserved.
//

#ifndef Stimulus_hpp
#define Stimulus_hpp

#include <stdio.h>
#include <vector>
#include <string>
#include "Signal.hpp"

class Stimulus {
public:
    Stimulus();
    ~Stimulus();
    std::string name;
    virtual float getStimulus();
    void setDuration(int numSteps);
    int getTimeStep();
    void resetTime();
    void setSignal(Signal* signalp);
    bool loop;
    int delay; // delay signal start by this number of time steps
    
protected:
    virtual float getDerivedStimulus(int timeStep);
    int duration; // the number of steps the signal lasts
    Signal* signalp;
    
private:
    int iterationNumber;
    int timeStep;
    void stepTime();
    void finish();
    bool finished;
};

#endif /* Stimulus_hpp */
