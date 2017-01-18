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
    
    
    // this function (as is noted in the source) is essentially a copy of the chemical synapse implementation in the kernel. if the kernel changes, this should too.
    float chemicalSynapseSimulator(float cs_weight, float their_v_curr, float my_v_current);
    
public:// the stimuli creators
    void createCO2Generator();
    void createO2DeprevationAndH2OGenerator();
};

#endif /* SensoryStimulationSteward_hpp */
