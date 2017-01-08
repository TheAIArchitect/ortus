//
//  ConstantSignal.cpp
//  ortus
//
//  Created by andrew on 1/8/17.
//  Copyright © 2017 Andrew W.E. McDonald. All rights reserved.
//

#include "ConstantSignal.hpp"


ConstantSignal::ConstantSignal(int magnitude, int startTime, int endTime, int length) : Signal(false){
    this->magnitude = magnitude;
    generateFullSignal(startTime, endTime, length);
}

ConstantSignal::~ConstantSignal(){
    delete signal;
}

/* very simple (and unncessary) example of how to use this class */
void ConstantSignal::generateFullSignalDerived(float startTime, float endTime, int length){
    for (int i = 0; i < length; ++i){
        signal[i] = magnitude;
    }
}
