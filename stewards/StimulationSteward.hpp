//
//  StimulationSteward.hpp
//  ortus
//
//  Created by andrew on 1/9/17.
//  Copyright © 2017 Andrew W.E. McDonald. All rights reserved.
//

#ifndef StimulationSteward_hpp
#define StimulationSteward_hpp

#include <stdio.h>
#include "DataSteward.hpp"
#include "Stimulator.hpp"
#include "ConstantSignal.hpp"

class StimulationSteward {
public:
    StimulationSteward(DataSteward* dStewiep);
    ~StimulationSteward();
    void performSensoryStimulation();
    void setStimuli();
    void addStimulus(std::string name, Stimulus stimulus);
    
    DataSteward* dStewiep;
    Stimulator* stimulatorp;
};

#endif /* StimulationSteward_hpp */
