//
//  StimulationSteward.cpp
//  ortus
//
//  Created by andrew on 1/9/17.
//  Copyright © 2017 Andrew W.E. McDonald. All rights reserved.
//

#include "SensoryStimulationSteward.hpp"

// these two values were determined by experiementation.
// you need to balance the rate of o2 consumption with co2 generation,
// and how much o2 is inhaled and co2 exhaled by lung...
// further, the kernel threshold must be taken into account -- nothing gets passed on from CSes
// if activation level is below threshold.
float SensoryStimulationSteward::o2Consumption = 2.5f;//2.f;
float SensoryStimulationSteward::co2Generation = 3.5f; // 3

SensoryStimulationSteward::SensoryStimulationSteward(DataSteward* dStewiep, Connectome* cp){
    this->dStewiep = dStewiep;
    this->cp = cp;
    stimuliStewardp = new StimuliSteward();
}

SensoryStimulationSteward::~SensoryStimulationSteward(){
    delete stimuliStewardp;
}

void SensoryStimulationSteward::setStimuli(){
    // these two are necessary for "life"
    createCO2Generator();
    createO2Consumer();
    //createO2DeprevationAndH2OGenerator();
    //createH2OStimulator();
    elementsToStimulate.push_back("sO2");
    elementsToStimulate.push_back("sCO2");
    //elementsToStimulate.push_back("sH2O");
}

void SensoryStimulationSteward::createCO2Generator(){
    // NOTE: this also must be a pointer, not a reference...
    ConstantSignal* co2Signal = new ConstantSignal(co2Generation, 0, 500, 500);
    // NOTE: this must be a pointer, not a reference...
    PrimitiveStimulus* co2Generator = new PrimitiveStimulus("co2Generator");
    co2Generator->setSignal(co2Signal);
    addStimulus("sCO2", co2Generator);
}

/** humans typically use less O2 than the amount of CO2 they expel.
 *
 * so, if we 'use' 2 oxygen, and 'generate' 2.5 CO2, that puts the
 * ratio of O2 used to CO2 produced at .8, which is reasonable.
 *
 * it's possible im thinking about this improperly...
 */
void SensoryStimulationSteward::createO2Consumer(){
    // NOTE: this also must be a pointer, not a reference...
    ConstantSignal* o2NegSignal = new ConstantSignal(-o2Consumption, 0, 500, 500);
    // NOTE: this must be a pointer, not a reference...
    PrimitiveStimulus* o2Consumer = new PrimitiveStimulus("o2Consumer");
    o2Consumer->setSignal(o2NegSignal);
    addStimulus("sO2", o2Consumer);
}


void SensoryStimulationSteward::createO2DeprevationAndH2OGenerator(){
    ConstantSignal* h2oSignal = new ConstantSignal(2.5f, 100, 150, 50);
    PrimitiveStimulus* h2oGenerator = new PrimitiveStimulus("h2oGenerator");
    h2oGenerator->setSignal(h2oSignal);
    addStimulus("SH2O", h2oGenerator);
    
    // this is a 'special' signal, and we're going to just z
    ConstantSignal* o2InhibitorySignal = new ConstantSignal(-1.f, 100, 150, 50);
    PrimitiveStimulus* o2Inhibitor = new PrimitiveStimulus("o2Inhibitor");
    o2Inhibitor->setSignal(o2InhibitorySignal);
    addStimulus("SO2", o2Inhibitor);
    
}

void SensoryStimulationSteward::createH2OStimulator(){
    ConstantSignal* h2oSignal = new ConstantSignal(2.f, 50, 350, 50);
    PrimitiveStimulus* h2oGenerator = new PrimitiveStimulus("h2oGenerator");
    h2oGenerator->setSignal(h2oSignal);
    addStimulus("sH2O", h2oGenerator);
}



void SensoryStimulationSteward::addStimulus(std::string elementName, Stimulus* stimulusp){
    int elementIndex = cp->nameMap[elementName];
    stimuliStewardp->addStimulus(elementIndex, stimulusp);
}

/** NOTE: this is a poorly implemented bit of functionality -- as it is now, we push back the element to stimulate, perhaps more than once if we don't check, and if we make ComplexStimuli, we have no way to differentitate a part of the signal that should stimulate one element from another. we should have a set of elements that are getting stimulated, and when we call that element, we get the total stimulation for that element... OR, we could run through each stimulus and add to each element's stimulation total, and then run through all the totals, and then stimulate each element that we have a non-zero total for. that might make the most sense
 */
// these are temporary variables...
int steps = 0;
bool inhibitO2 = false;
bool lungOff = true;
bool lungOn = false;
void SensoryStimulationSteward::performSensoryStimulation(){
    
    for (auto& vecIter : elementsToStimulate){
        int elementIndex = cp->nameMap[vecIter];
        float finalStimulus = stimuliStewardp->getStimuliForElement(elementIndex);
        // ... this call should *probably* really tell DataSteward to do it... we probably shouldn't do it ourselves....
        dStewiep->activationBlade->add(elementIndex, 0, finalStimulus);
        ////if (vecIter == "SO2" && finalStimulus == -1.0){
        ////    inhibitO2 = true;
        ////}
       
    }
    if (steps >= 50){
        // at 50 steps, then cycle 20 on, 30 off.
        if (inhibitO2 && fmod(steps, 50) == 20){
            inhibitO2 = false;
        }
        else if (!inhibitO2 && fmod(steps, 50) == 0){
            inhibitO2 = true;
        }
        
        if (inhibitO2){ // also using this as a "stimulate h20" trigger
            int h2oIndex = cp->nameMap["sH2O"];
            dStewiep->activationBlade->add(h2oIndex, 0, 5);
        }
    }
    steps++;
    
    /**NOTE:
     ****
     ** ... the below code should *>OBVIOUSLY<* be put into a function...
     ***/
    
    //  Note 2: this below chunk of code suggests an issue -- how do we store the weights we want from motors (or muscles, at some point) to sensors?
    //      -> one possible solution would be to store the weights in the connectome, but put them in a different area (not the gap/chem connectome Blades), so that they can be used/changed from this class (SensoryStimulationSteward), or similar.
    
    // since motor neurons can only *try* to get something to happen (e.g., MINHALE can activate and *try* to increase oxygen (O2)), here we will look at the activations of Motors, and stimulate the sensory neurons accordingly. This allows us to control whether or not inhaling actually increases oxygen or not -- something we couldn't do if it was 'wired' into the connectome.
    
    // inhale and o2
    int lungIndex;
    float lungActivation;
    int sensorIndex;
    float sensorV;
    float vFromMotor = 0.f;
    if (!inhibitO2){
        lungIndex = cp->nameMap["LUNG"];
        lungActivation = dStewiep->activationBlade->getv(lungIndex, 0);
        float prevLungAct = dStewiep->activationBlade->getv(lungIndex, 1);
        float lungActivationSlope = lungActivation - prevLungAct;
        if (lungOn){
            int co2Index = cp->nameMap["sCO2"];
            dStewiep->activationBlade->add(co2Index, 0, -5*co2Generation);// 3 //  multiplier by trial and error (need to balance threshold, co2 gen, o2 consumption)
            lungOn = false;
        }
        if (prevLungAct <= 1 && lungActivation > 1){
            int o2Index = cp->nameMap["sO2"];
            dStewiep->activationBlade->add(o2Index, 0, 6*o2Consumption); // multiplier by trial and error.
            lungOn = true;
        }
        
       
        printf("LUNG activation: %f\n", lungActivation);
        
        /*
        ////sensorIndex = dStewiep->officialNameToIndexMap["SO2"];
        sensorV = dStewiep->voltages->getv(sensorIndex);
        vFromMotor = chemicalSynapseSimulator(1, motorV, sensorV);
        dStewiep->voltages->add(sensorIndex, vFromMotor);
        // exhale and CO2
        ////motorIndex = dStewiep->officialNameToIndexMap["MEXHALE"];
        motorV = dStewiep->voltages->getv(motorIndex);
        ////sensorIndex = dStewiep->officialNameToIndexMap["SCO2"];
        sensorV = dStewiep->voltages->getv(sensorIndex);
        vFromMotor = chemicalSynapseSimulator(-1, motorV, sensorV);
        dStewiep->voltages->add(sensorIndex, vFromMotor);
         */
    }
 
    
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
