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
    //ConstantSignal co2Signal = ConstantSignal(2.5f, 0, 499, 500);
    
}

void StimulationSteward::addStimulus(std::string name, Stimulus stimulus){
    
}

void StimulationSteward::performSensoryStimulation(){
    
}
