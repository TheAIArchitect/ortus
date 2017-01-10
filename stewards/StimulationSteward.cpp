//
//  StimulationSteward.cpp
//  ortus
//
//  Created by andrew on 1/9/17.
//  Copyright © 2017 Andrew W.E. McDonald. All rights reserved.
//

#include "StimulationSteward.hpp"


StimulationSteward::StimulationSteward(DataSteward* dStewiep){
    this->dStewiep = dStewiep;
    stimulatorp = new Stimulator();
}

StimulationSteward::~StimulationSteward(){
    delete stimulatorp;
}

void StimulationSteward::setStimuli(){
   
    // note: this is a very rough, initial implementation of how this whole system will work...
    // NOTE: this also must be a pointer, not a reference...
    ConstantSignal* co2Signal = new ConstantSignal(2.5f, 0, 499, 500);
    // NOTE: this must be a pointer, not a reference... 
    PrimitiveStimulus* co2Generator = new PrimitiveStimulus("co2Generator");
    co2Generator->signal = co2Signal;
    int SCO2_index = dStewiep->officialNameToIndexMap["SCO2"];
    stimulatorp->stimuliMap[SCO2_index].push_back(co2Generator);
    
}

void StimulationSteward::addStimulus(std::string name, Stimulus stimulus){
    
}

void StimulationSteward::performSensoryStimulation(){
    for (auto& mapIter : stimulatorp->stimuliMap){
        for (auto& vecIter : mapIter.second){
            float poop = vecIter->getStimulus();
            printf("%s: %f\n",vecIter->name.c_str(),poop);
            //dStewiep->inputVoltages->add(mapIter.first, vecIter->getStimulus());
            dStewiep->inputVoltages->add(mapIter.first, poop);
        }
    }
    
}
