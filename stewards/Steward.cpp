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
    
    dataStewardp = new DataSteward();
    computeStewardp = new ComputeSteward(globalSize, localSize);
    computeStewardp->dStewiep = dataStewardp;
    computeStewardp->initializeOpenCL();
    dataStewardp->init();
    stimulationStewardp = new StimulationSteward(dataStewardp);
    stimulationStewardp->setStimuli();
    
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
        computeStewardp->executePreRunOperations();
        computeStewardp->run();
        computeStewardp->executePostRunOperations();
        
        // temporary print statement
        for (int j = 0; j < dataStewardp->outputVoltageVector.size(); ++j){
            printf("%.2f, ",dataStewardp->outputVoltageVector[j]);
        }
        printf("\n");
        
        // Stimulate the sensors << THIS WILL BE A STEWARD >>
        stimulationStewardp->performSensoryStimulation();
        //dataStewardp->gym.stimulateSensors(*(dataStewardp->inputVoltages), dataStewardp->officialNameToIndexMap);
        
        // now we need to re-push that buffer
    
        
    
    }

    // Just a basic runtime report and small sample of elements
    //stewie.printReport(DataSteward::numKernelLoops);
    
    // temporaray print statement(s)
    int numKVs = dataStewardp->kernelVoltages.size();
    printf("size of 'kernel_voltages': %d\n", numKVs);
    for (int i = 0; i < numKVs; ++i){
        for (int j = 0; j < dataStewardp->kernelVoltages[i].size(); ++j){
            printf("%.2f, ",dataStewardp->kernelVoltages[i][j]);
        }
        printf("\n");
    }
    DataVisualizer vizzer = DataVisualizer(dataStewardp);
    const int plotSetSize = 6;
    std::string plotSet[plotSetSize] = {"SO2","SCO2","IPO2", "INO2", "MINHALE", "MEXHALE"};
    //vizzer.plotAll();
    vizzer.plotSet(plotSet, plotSetSize);
    //initGraphics(&core);
    cleanUp();
    exit(89);   
    
}

void Steward::cleanUp(){
    // do in reverse order of allocation? might not matter.
    delete computeStewardp;
    delete dataStewardp;
    
    
}

