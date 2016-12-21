//
//  Probe.cpp
//  delegans
//
//  Created by onyx on 10/21/16.
//  Copyright © 2016 Andrew W.E. McDonald. All rights reserved.
//

#include "Probe.hpp"


 // this is set by the person inputting the element, and then later (on next loop through graphics), is checked.
// the reason for this is simply poor planning... hopefully will be fixed soon...
std::string Probe::newElementToProbe = "";
bool Probe::newElementToProbeRequested = false;
bool Probe::probeEnabled = true;
bool Probe::update = false;
cl_uint Probe::shouldProbe = 1;
bool Probe::probeAll = true;


// c'tor assumes square matrix.
Probe::Probe(DataSteward* ds){
    stewie = ds;
    float** firstWindow = new float*[DataSteward::NUM_ELEMS]; // numelements is supplied by DataSteward::NUM_ELEMS
    for (int i = 0; i < DataSteward::NUM_ELEMS; ++i){
        firstWindow[i] = new float[DataSteward::NUM_ELEMS];
        for (int j = 0; j < DataSteward::NUM_ELEMS; ++j){
            firstWindow[i][j] = 0.f;
        }
    }
    // both windows will initially be zero, so they can point to the same place.
    // setting the first window to zero allows us to use the same index for these 'windows'
    // as the voltage windows.
    csContribVec.push_back(firstWindow);
    gjContribVec.push_back(firstWindow);
    
    // set default element to probe
    elementToProbe = "SO2";
    numElemsMinusOne = DataSteward::NUM_ELEMS  - 1;
}

void Probe::printAll(int window){
    std::string thisWindow = "(" + std::to_string(window) + ") [";
    for (int i = 0; i < numElemsMinusOne; ++i){
        thisWindow += stewie->bioElements[i]->name + ":" + std::to_string(stewie->kernelVoltages[window][i]) + ", ";
    }
    thisWindow += stewie->bioElements[numElemsMinusOne]->name + ":" + std::to_string(stewie->kernelVoltages[window][numElemsMinusOne]) + "]";
    printf("%s\n",thisWindow.c_str());
}

void Probe::printCurrentProbe(int window){
    int row = stewie->n2idxMap[elementToProbe];
    float voltage = stewie->kernelVoltages[window][row];
    printf("==============================================================================================\n");
    printf(" ``````````` PROBE: %s, index %d, window %d ```````````\n",elementToProbe.c_str(), row, window);
    printf(" >>>>>>>>>>>>>>>>>>>>>>>>> receiving the following (voltage is %.4f) <<<<<<<<<<<<<<<<<<<<<<<\n",voltage);
    printf("CS: ");
    for (int i = 0; i < DataSteward::NUM_ELEMS; ++i){
        if (csContribVec[window][row][i] != 0.f){
            printf("[%s <idx: %d>: %.4f] ",stewie->bioElements[i]->name.c_str(), i, csContribVec[window][row][i]);
        }
    }
    printf("\n---------------------------------------------------------------------------------------------\nGJ: ");
    for (int i = 0; i < DataSteward::NUM_ELEMS; ++i){
        if (gjContribVec[window][row][i] != 0.f){
            printf("[%s <idx: %d>: %.4f] ",stewie->bioElements[i]->name.c_str(), i, gjContribVec[window][row][i]);
        }
    }
    printf("\n==============================================================================================\n");
    
}

bool Probe::toggleProbe(){
    probeEnabled = !probeEnabled;
    if (probeEnabled){
        shouldProbe = 1;
        return true;
    }
    else{
        shouldProbe = 0;
        return false;
    }
}



std::string Probe::setNewElementToProbe(){
    if (newElementToProbe == ""){
        return "";
    }
    std::string temps;
    for (int i = 0; i < stewie->NUM_ELEMS; ++i){
        temps = stewie->bioElements[i]->name;
        StrUtils::toUpper(temps);
        if (temps == newElementToProbe){
            elementToProbe = stewie->bioElements[i]->name; // don't use the 'newElementToProbe' because as of this writing, it's all caps..
            newElementToProbe = "";
            newElementToProbeRequested = false;
            return "Now probing '"+elementToProbe+"'.";
        }
    }
    std::string response = "Error: Cannot find '"+newElementToProbe+"'. Will continue probing '"+elementToProbe+"'.";
    newElementToProbe = "";
    newElementToProbeRequested = false;
    return response;
}
