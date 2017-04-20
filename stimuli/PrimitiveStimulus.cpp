//
//  PrimitiveStimulus.cpp
//  ortus
//
//  Created by andrew on 1/7/17.
//  Copyright © 2017 Andrew W.E. McDonald. All rights reserved.
//

#include "PrimitiveStimulus.hpp"

PrimitiveStimulus::PrimitiveStimulus(std::string name) : Stimulus(){
    this->name = name;
}

float PrimitiveStimulus::getDerivedStimulus(int timeStep){
    return signalp->getValueAtIndex(timeStep);
}





