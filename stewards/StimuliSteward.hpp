//
//  StimuliSteward.hpp
//  ortus
//
//  Created by onyx on 12/20/16.
//  Copyright © 2016 Andrew W.E. McDonald. All rights reserved.
//

#ifndef StimuliSteward_hpp
#define StimuliSteward_hpp

#include <stdio.h>
#include <vector>
#include "Blade.hpp"
#include "Stimulus.hpp"
#include "PrimitiveStimulus.hpp"
#include "ComplexStimulus.hpp"
#include <unordered_map>


class StimuliSteward {
    
public:
    StimuliSteward();
    ~StimuliSteward();
    void addStimulus(int elementIndex, Stimulus* stimulusp);
    float getStimuliForElement(int elementIndex);
    float normalizeStimuli(float aggregateStimuli);
    
    /* maps element ids to a vector of stimuli */
    std::unordered_map<int,std::vector<Stimulus*>> stimuliMap;
    
};

#endif /* StimuliSteward_hpp */

