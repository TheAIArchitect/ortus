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

class Stimulus {
public:
    Stimulus();
    
    std::vector<float> signal;
    virtual float stimulate();
    void setDuration(int numSteps);
    int getTimeStep();
    void resetTime();
    bool loop;
    int delay; // delay signal start by this number of time steps
    
protected:
    virtual float getStimulus();
    int duration; // the number of steps the signal lasts
    
private:
    int iterationNumber;
    int timeStep;
    void stepTime();
    void finish();
    bool finished;
};

#endif /* Stimulus_hpp */
