//
//  ComplexStimulus.cpp
//  ortus
//
//  Created by andrew on 1/7/17.
//  Copyright © 2017 Andrew W.E. McDonald. All rights reserved.
//

#include "ComplexStimulus.hpp"

/* upon its stimulate function being called, it should call the primitves, and add those to get its own value, which it returns.*/
ComplexStimulus::ComplexStimulus(){
    
}

void ComplexStimulus::addStimulus(Stimulus* stimulusp){
    stimuli.push_back(stimulusp);
}

float ComplexStimulus::getDerivedStimulus(int timeStep){
    return 0.f;
    
}
