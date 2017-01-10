//
//  StimulationSteward.hpp
//  ortus
//
//  Created by andrew on 1/9/17.
//  Copyright © 2017 Andrew W.E. McDonald. All rights reserved.
//

#ifndef SensoryStimulationSteward_hpp
#define SensoryStimulationSteward_hpp

#include <stdio.h>
#include "DataSteward.hpp"
#include "StimuliSteward.hpp"
#include "ConstantSignal.hpp"
#include "Signal.hpp"

class SensoryStimulationSteward {
public:
    SensoryStimulationSteward(DataSteward* dStewiep);
    ~SensoryStimulationSteward();
    void performSensoryStimulation();
    void setStimuli();
    void addStimulus(std::string name, Stimulus* stimulusp);
    
    std::vector<std::string> elementsToStimulate;
    
    DataSteward* dStewiep;
    StimuliSteward* stimuliStewardp;
    
public:// the stimuli creators
    Stimulus* createCO2Generator();
};

#endif /* SensoryStimulationSteward_hpp */
