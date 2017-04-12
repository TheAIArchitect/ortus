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
    size_t globalSize = 64; ///512; // total work-items to execute (number of times to call kernel)
    size_t localSize = 8;///64;// 16 or 32? // number of work-items to group into a work-group
    // assuming global = 512 and local = 64:
    // the number of work-groups = global / local = 512/64  = 8 work-groups
 
    
    
    computeStewardp = new ComputeSteward(globalSize, localSize);
    computeStewardp->initializeOpenCL();
    
    
    dataStewardp = new DataSteward();
    computeStewardp->dStewiep = dataStewardp;
    dataStewardp->setComputeStewardPointers(computeStewardp);
    
    // we need to do some initial processing on the .ort file (e.g., set some metadata variables) in order to initialize the KernelArgs and Blades
    dataStewardp->initializeConnectome("ort/simple.ort");
    dataStewardp->initializeKernelArgsAndBlades(&computeStewardp->clHelper, &computeStewardp->kernel);
    // once initializeKernelArgsAndBlades returns, we can create the Elements and ElementRelations,
    // which will use the Blades to store (some) of their data.
    dataStewardp->createElementsAndElementRelations();
    
    architectp = new Architect();
    architectp->setDataSteward(dataStewardp);
    architectp->setConnectome(dataStewardp->connectomep);
    
    // this takes the elements and satisfies the constraints described.
    // rather, a better description, would be,
    // it satisfies any constraints not yet satisfied.
    architectp->designConnectome();
    
    
    //dataStewardp->init(computeStewardp->workGroupSize);
    //sensoryStimulationStewardp = new SensoryStimulationSteward(dataStewardp);
    //sensoryStimulationStewardp->setStimuli();
    diagnosticStewardp = new DiagnosticSteward(dataStewardp);
    
}

void Steward::run(){

    
    /** maybe some function to give all classes all pointers -- e.g:
     -> it sets all instances of dataStewardp necessary, etc.
     */
    
    for (int i = 0; i < Ort::NUM_ITERATIONS; ++i){
        printf("iteration: %d starting...\n", i);
        computeStewardp->executePreRunOperations();
        
        computeStewardp->run();
        
        
        computeStewardp->executePostRunOperations();
        
        // Stimulate the sensors 
        //sensoryStimulationStewardp->performSensoryStimulation();
        
        //diagnosticStewardp->runAdvancedDiagnostics();
        printf("iteration: %d complete...\n", i);
    }
    
    //diagnosticStewardp->plotXCorr();
    
    // temporary placement...
    //dataStewardp->writeConnectome("doesn't matter yet");

    // Just a basic runtime report and small sample of elements
    //stewie.printReport(DataSteward::numKernelLoops);
    
    // temporaray print statement(s)
    int numKVs = dataStewardp->fullActivationHistory.size();
    printf("size of 'kernel_voltages': %d\n", numKVs);
    for (int i = 0; i < numKVs; ++i){
        for (int j = 0; j < Ort::NUM_ELEMENTS; ++j){
            printf("%.2f, ",dataStewardp->fullActivationHistory[i][j]);
        }
        printf("\n");
    }
    //diagnosticStewardp->plotXCorr();
    ////diagnosticStewardp->runStandardDiagnostics();
    
    //initGraphics(&core);
    cleanUp();
    exit(89);   
    
}

void Steward::cleanUp(){
    // do in reverse order of allocation? might not matter.
    
    delete diagnosticStewardp;
    delete sensoryStimulationStewardp;
    delete computeStewardp;
    //delete dataStewardp;
}

