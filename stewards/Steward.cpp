//
//  Steward.cpp
//  ortus
//
//  Created by andrew on 1/8/17.
//  Copyright © 2017 Andrew W.E. McDonald. All rights reserved.
//

#include "Steward.hpp"

/*
 
 -> DevelopmentSteward
    -> this will add neurons, and adjusts weights and connections
        => so, it must have direct access to the Blade objects.
 -> ThinkingSteward
    -> would generate thoughts, by stimulating interneurons
 -> ActionSteward
    -> take outputs and graph, or activate a physics simulation, maybe pass data back to StimulationSteward from the physics simulation
    -> obviously would need access to the Blades
*/


Steward::Steward(){
    
}

void Steward::initialize(){
    size_t globalSize = 512; // total work-items to execute (number of times to call kernel)
    size_t localSize = 64;// 16 or 32? // number of work-items to group into a work-group
    // assuming global = 512 and local = 64:
    // the number of work-groups = global / local = 512/64  = 8 work-groups
    
    dataStewardp = new DataSteward();
    computeStewardp = new ComputeSteward(globalSize, localSize);
    computeStewardp->dStewiep = dataStewardp;
    computeStewardp->initializeOpenCL();
    dataStewardp->init(computeStewardp->workGroupSize);
    sensoryStimulationStewardp = new SensoryStimulationSteward(dataStewardp);
    sensoryStimulationStewardp->setStimuli();
    
}

void Steward::run(){
    
    numIterations = 1000;
    for (int i = 0; i < numIterations; ++i){
        computeStewardp->executePreRunOperations();
        computeStewardp->run();
        computeStewardp->executePostRunOperations();
        
        // Stimulate the sensors 
        sensoryStimulationStewardp->performSensoryStimulation();
        
    }
    // temporary placement...
    dataStewardp->writeConnectome("doesn't matter yet");

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
    delete sensoryStimulationStewardp;
    
    
}

