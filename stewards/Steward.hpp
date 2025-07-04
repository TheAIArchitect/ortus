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
#include "Architect.hpp"
#include "SensoryStimulationSteward.hpp"
#include "DiagnosticSteward.hpp"
#include "OrtusStd.hpp"

class Steward {
public:
    Steward();
    void initialize();
    void cleanUp();
    
    void run();
    
    
    Architect* architectp;
    DataSteward* dataStewardp;
    ComputeSteward* computeStewardp;
    SensoryStimulationSteward* sensoryStimulationStewardp;
    DiagnosticSteward* diagnosticStewardp;
    
    
};

#endif /* Steward_hpp */
