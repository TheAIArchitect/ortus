//
//  Steward.cpp
//  ortus
//
//  Created by andrew on 1/8/17.
//  Copyright © 2017 Andrew W.E. McDonald. All rights reserved.
//

#include "Steward.hpp"


Steward::Steward(){
    
}

void Steward::initialize(){
    size_t globalSize = 512;
    size_t localSize = 64;
    
    dataSteward = new DataSteward();
    computeSteward = new ComputeSteward(globalSize, localSize);
    computeSteward->dStewiep = dataSteward;
    computeSteward->initializeOpenCL();
    dataSteward->init();
    computeSteward->setupOpenCL();
}

/****************** NOTES:
 *
 * 1) A lot of the stuff with poitner acces to dataSteward should probably go directly into datastward.
 * 2) What is the deal with that Timer error? it's odd.
 * 3) All three of these classes need to be cleaned up before introducing the stimuli.
 ********************************/

void Steward::run(){
    
    numIterations = 1000;
    for (int i = 0; i < numIterations; ++i){
        computeSteward->run();
        
        
        dataSteward->readOpenCLBuffers();
        dataSteward->updateOutputVoltageVector();// this is a copy of the data.
        for (int j = 0; j < dataSteward->outputVoltageVector.size(); ++j){
            printf("%.2f, ",dataSteward->outputVoltageVector[j]);
        }
        printf("\n");
        
        // copy the ouput data (which has been updated, see below) into the input data
        dataSteward->inputVoltages->copyData(dataSteward->outputVoltages);
        // then, stimulate the sensors << THIS WILL BE A STEWARD >>
        dataSteward->gym.stimulateSensors(*(dataSteward->inputVoltages), dataSteward->officialNameToIndexMap);
        // now we need to re-push that buffer
        dataSteward->inputVoltages->pushCLBuffer();
        dataSteward->setOpenCLKernelArgs();
    
        
    
        kernel_voltages.push_back(dataSteward->getOutputVoltageVector());
    }
    ///// NOTE: TODO: re-do cleanup and printReport code
    //stewie.cleanUp();

    // Just a basic runtime report and small sample of elements
    //stewie.printReport(DataSteward::numKernelLoops);
        
    int numKVs = kernel_voltages.size();
    printf("size of 'kernel_voltages': %d\n", numKVs);
    for (int i = 0; i < numKVs; ++i){
        for (int j = 0; j < kernel_voltages[i].size(); ++j){
            printf("%.2f, ",kernel_voltages[i][j]);
        }
        printf("\n");
    }
    //printf("info: %d, %d\n", aselIdx, core.stewie.bioElements[aselIdx]->element_id);
    dataSteward->kernelVoltages = kernel_voltages;
    DataVisualizer vizzer = DataVisualizer(dataSteward);
    // NOTE: must create set
    const int plotSetSize = 6;
    std::string plotSet[plotSetSize] = {"SO2","SCO2","IPO2", "INO2", "MINHALE", "MEXHALE"};
    //vizzer.plotAll();
    vizzer.plotSet(plotSet, plotSetSize);
    //initGraphics(&core);
    exit(89);   
    
}


