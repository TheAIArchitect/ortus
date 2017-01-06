//
//  ElementStimulator.hpp
//  ortus
//
//  Created by onyx on 12/20/16.
//  Copyright © 2016 Andrew W.E. McDonald. All rights reserved.
//

#ifndef ElementStimulator_hpp
#define ElementStimulator_hpp

#include <stdio.h>
#include <vector>
#include "Blade.hpp"

class Stimulator {
    
public:
    Stimulator();
    std::vector<float> signal;
    int timeStep;
    void stimulate(Blade* activationVector); // what's the best way to do this??
};

#endif /* ElementStimulator_hpp */
