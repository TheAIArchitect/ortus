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
    createCO2Generator();
    createO2DeprevationAndH2OGenerator();
}

void SensoryStimulationSteward::createCO2Generator(){
    // NOTE: this also must be a pointer, not a reference...
    ConstantSignal* co2Signal = new ConstantSignal(2.5f, 0, 500, 500);
    // NOTE: this must be a pointer, not a reference...
    PrimitiveStimulus* co2Generator = new PrimitiveStimulus("co2Generator");
    co2Generator->setSignal(co2Signal);
    addStimulus("SCO2", co2Generator);
    elementsToStimulate.push_back("SCO2");
}

void SensoryStimulationSteward::createO2DeprevationAndH2OGenerator(){
    ConstantSignal* h2oSignal = new ConstantSignal(2.5f, 100, 400, 300);
    PrimitiveStimulus* h2oGenerator = new PrimitiveStimulus("h2oGenerator");
    h2oGenerator->setSignal(h2oSignal);
    addStimulus("SH2O", h2oGenerator);
    elementsToStimulate.push_back("SH2O");
    
    // this is a 'special' signal, and we're going to just z
    ConstantSignal* o2InhibitorySignal = new ConstantSignal(-1.f, 100, 400, 300);
    PrimitiveStimulus* o2Inhibitor = new PrimitiveStimulus("o2Inhibitor");
    o2Inhibitor->setSignal(o2InhibitorySignal);
    addStimulus("SO2", o2Inhibitor);
    elementsToStimulate.push_back("SO2");
    
}

void SensoryStimulationSteward::addStimulus(std::string elementName, Stimulus* stimulusp){
    int elementIndex = dStewiep->officialNameToIndexMap[elementName];
    stimuliStewardp->addStimulus(elementIndex, stimulusp);
}

/** NOTE: this is a poorly implemented bit of functionality -- as it is now, we push back the element to stimulate, perhaps more than once if we don't check, and if we make ComplexStimuli, we have no way to differentitate a part of the signal that should stimulate one element from another. we should have a set of elements that are getting stimulated, and when we call that element, we get the total stimulation for that element... OR, we could run through each stimulus and add to each element's stimulation total, and then run through all the totals, and then stimulate each element that we have a non-zero total for. that might make the most sense
 */
void SensoryStimulationSteward::performSensoryStimulation(){
    bool inhibitO2 = false;
    
    for (auto& vecIter : elementsToStimulate){
        int elementIndex = dStewiep->officialNameToIndexMap[vecIter];
        float finalStimulus = stimuliStewardp->getStimuliForElement(elementIndex);
        // ... this call should *probably* really tell DataSteward to do it... we probably shouldn't do it ourselves....
        dStewiep->voltages->add(elementIndex, finalStimulus);
        if (vecIter == "SO2" && finalStimulus == -1.0){
            inhibitO2 = true;
        }
    }
    
    /**NOTE:
     ****
     ** ... the below code should *>OBVIOUSLY<* be put into a function...
     ***/
    
    //  Note 2: this below chunk of code suggests an issue -- how do we store the weights we want from motors (or muscles, at some point) to sensors?
    //      -> one possible solution would be to store the weights in the connectome, but put them in a different area (not the gap/chem connectome Blades), so that they can be used/changed from this class (SensoryStimulationSteward), or similar.
    
    // since motor neurons can only *try* to get something to happen (e.g., MINHALE can activate and *try* to increase oxygen (O2)), here we will look at the activations of Motors, and stimulate the sensory neurons accordingly. This allows us to control whether or not inhaling actually increases oxygen or not -- something we couldn't do if it was 'wired' into the connectome.
    
    // inhale and o2
    int motorIndex;
    float motorV;
    int sensorIndex;
    float sensorV;
    float vFromMotor = 0.f;
    if (!inhibitO2){
        motorIndex = dStewiep->officialNameToIndexMap["MINHALE"];
        motorV = dStewiep->voltages->getv(motorIndex);
        sensorIndex = dStewiep->officialNameToIndexMap["SO2"];
        sensorV = dStewiep->voltages->getv(sensorIndex);
        vFromMotor = chemicalSynapseSimulator(1, motorV, sensorV);
        dStewiep->voltages->add(sensorIndex, vFromMotor);
    }
    // exhale and CO2
    motorIndex = dStewiep->officialNameToIndexMap["MEXHALE"];
    motorV = dStewiep->voltages->getv(motorIndex);
    sensorIndex = dStewiep->officialNameToIndexMap["SCO2"];
    sensorV = dStewiep->voltages->getv(sensorIndex);
    vFromMotor = chemicalSynapseSimulator(-1, motorV, sensorV);
    dStewiep->voltages->add(sensorIndex, vFromMotor);
 
    
}


/********************************************************************
 ******* NOTE: THIS WAS COPIED FROM THE KERNEL!!! *******************
 ********************************************************************
 *** THAT MEANS THAT IF THE KERNEL CHANGES, THIS MUST CHANGE TOO! ***/
float SensoryStimulationSteward::chemicalSynapseSimulator(float cs_weight, float their_v_curr, float my_v_current){
    // perhaps the revPots should be kernel args??
    float thresh = 5.f;
    if (their_v_curr < thresh){
        return 0.f;
    }
    float inhibitRevPot = -10.f;
    float exciteRevPot = 30.f;
    float vRange = exciteRevPot - inhibitRevPot;
    float conductance = 0.f;
    float added_v = 0.f;
    // below two vars more for notes than anything else.
    //float preVCurr = their_v_curr;
    //float postVCurr = my_v_current;
    if(cs_weight < 0){ // if inhibit
        cs_weight *= -1; // make positive, for below computation
        float preToEq = fabs(their_v_curr - inhibitRevPot);
        float vDiff = inhibitRevPot - my_v_current;
        conductance = (2.0f - (2.0f/(1.0f + exp(-5.0f*(preToEq/vRange))))) - .0134;
        added_v = cs_weight * conductance * vDiff;
    }
    else { // excite
        float preToEq = fabs(their_v_curr - exciteRevPot);
        float vDiff = exciteRevPot - my_v_current;
        conductance = (2.0f - (2.0f/(1.0f + exp(-5.0f*(preToEq/vRange))))) - .0134;
        added_v = cs_weight * conductance * vDiff;
    }
    return added_v;
}
