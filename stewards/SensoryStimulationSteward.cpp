//
//  StimulationSteward.cpp
//  ortus
//
//  Created by andrew on 1/9/17.
//  Copyright © 2017 Andrew W.E. McDonald. All rights reserved.
//

#include "SensoryStimulationSteward.hpp"


SensoryStimulationSteward::SensoryStimulationSteward(DataSteward* dStewiep){
    this->dStewiep = dStewiep;
    stimuliStewardp = new StimuliSteward();
}

SensoryStimulationSteward::~SensoryStimulationSteward(){
    delete stimuliStewardp;
}

void SensoryStimulationSteward::setStimuli(){
    Stimulus* co2Generator = createCO2Generator();
    addStimulus("SCO2",co2Generator);
    elementsToStimulate.push_back("SCO2");
}

Stimulus* SensoryStimulationSteward::createCO2Generator(){
    // NOTE: this also must be a pointer, not a reference...
    ConstantSignal* co2Signal = new ConstantSignal(2.5f, 0, 500, 500);
    // NOTE: this must be a pointer, not a reference...
    PrimitiveStimulus* co2Generator = new PrimitiveStimulus("co2Generator");
    co2Generator->setSignal(co2Signal);
    return co2Generator;
}

void SensoryStimulationSteward::addStimulus(std::string elementName, Stimulus* stimulusp){
    int elementIndex = dStewiep->officialNameToIndexMap[elementName];
    stimuliStewardp->addStimulus(elementIndex, stimulusp);
}

void SensoryStimulationSteward::performSensoryStimulation(){
    for (auto& vecIter : elementsToStimulate){
        int elementIndex = dStewiep->officialNameToIndexMap[vecIter];
        float finalStimulus = stimuliStewardp->getStimuliForElement(elementIndex);
        dStewiep->inputVoltages->add(elementIndex, finalStimulus);
    }
}
