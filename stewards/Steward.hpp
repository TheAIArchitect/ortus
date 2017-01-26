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
#include "SensoryStimulationSteward.hpp"
#include "DiagnosticSteward.hpp"

class Steward {
public:
    Steward();
    void initialize();
    void cleanUp();
    
    void run();
    
    DataSteward* dataStewardp;
    ComputeSteward* computeStewardp;
    SensoryStimulationSteward* sensoryStimulationStewardp;
    DiagnosticSteward* diagnosticStewardp;
    
    int numIterations;
    
};

#endif /* Steward_hpp */
