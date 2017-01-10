//
//  PrimitiveStimulus.hpp
//  ortus
//
//  Created by andrew on 1/7/17.
//  Copyright © 2017 Andrew W.E. McDonald. All rights reserved.
//

#ifndef PrimitiveStimulus_hpp
#define PrimitiveStimulus_hpp

#include <stdio.h>
#include "Stimulus.hpp"
#include "Signal.hpp"

class PrimitiveStimulus : public Stimulus {
    
public:
    PrimitiveStimulus(std::string name);
    virtual float getDerivedStimulus(int timeStep);
    
};

#endif /* PrimitiveStimulus_hpp */
