//
//  Steward.hpp
//  ortus
//
//  Created by andrew on 1/8/17.
//  Copyright © 2017 Andrew W.E. McDonald. All rights reserved.
//

#ifndef Steward_hpp
#define Steward_hpp

#include <stdio.h>
#include "DataSteward.hpp"
#include "ComputeSteward.hpp"
#include "DataVisualizer.hpp"
#include "StimulationSteward.hpp"

class Steward {
public:
    Steward();
    void initialize();
    void cleanUp();
    
    void run();
    
    DataSteward* dataStewardp;
    ComputeSteward* computeStewardp;
    // maybe call it a sensoryStimulationSteward, and use another instance for the 'thinking' one that stimulates interneurons? (might be better to have a separate class though... yeah, i think so)
    StimulationSteward* stimulationStewardp;
    
    int numIterations;
    
};

#endif /* Steward_hpp */
