//
//  ComplexStimulus.hpp
//  ortus
//
//  Created by andrew on 1/7/17.
//  Copyright © 2017 Andrew W.E. McDonald. All rights reserved.
//

#ifndef ComplexStimulus_hpp
#define ComplexStimulus_hpp

#include <stdio.h>
#include "Stimulus.hpp"

class ComplexStimulus : public Stimulus {

public:
    ComplexStimulus();
    virtual float getDerivedStimulus(int timeStep);
    void addStimulus(Stimulus* stimulusp);
    std::vector<Stimulus*> stimuli;
};

#endif /* ComplexStimulus_hpp */
