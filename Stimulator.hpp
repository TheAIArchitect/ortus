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
#include "Stimulus.hpp"
#include "PrimitiveStimulus.hpp"
#include "ComplexStimulus.hpp"


class Stimulator {
    
public:
    Stimulator();
    std::vector<Stimulus> stimuli;
    
};

#endif /* ElementStimulator_hpp */

