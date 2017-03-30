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
 
    
    
    //dataStewardp = new DataSteward();
    computeStewardp = new ComputeSteward(globalSize, localSize);
    
    //computeStewardp->dStewiep = dataStewardp;
    computeStewardp->initializeOpenCL();
    
    // maybe DataSteward should hold the actual blades, and KB just accesses them? seems screwy...
    // maybe KernelBuddy should just be DataSteward... 
    kernelBuddyp = new KernelBuddy(static_cast<int>(Attribute::NUM_ATTRIBUTES), &computeStewardp->clHelper, &computeStewardp->kernel);
    
    //dataStewardp->init(computeStewardp->workGroupSize);
    //sensoryStimulationStewardp = new SensoryStimulationSteward(dataStewardp);
    //sensoryStimulationStewardp->setStimuli();
    //diagnosticStewardp = new DiagnosticSteward(dataStewardp);
    
}

/** TEMPORARY FUNCTION */
void fillBlade(bool _2D, int size, KernelBuddy& thing, Attribute attrib, float value){
    if (_2D){
        int i,j;
        for (i = 0; i < size; ++i){
            for (j = 0; j < size; ++j){
                thing.addThing(attrib, i, j, value);
            }
        }
    }
    else {
        int i;
        for (i = 0; i < size; ++i){
            thing.addThing(attrib, i, value);
        }
    }
}
/** END TEMPORARY FUNCTION */

void Steward::run(){
    
    
    fillBlade(false, ortus::NUM_ELEMENTS, *kernelBuddyp, Attribute::Type, 2.0);
    fillBlade(false, ortus::NUM_ELEMENTS, *kernelBuddyp, Attribute::Affect, 4.0);
    fillBlade(false, ortus::NUM_ELEMENTS, *kernelBuddyp, Attribute::Activation, 6.0);
    fillBlade(true, ortus::NUM_ELEMENTS, *kernelBuddyp, Attribute::Weight, 3.0);
    fillBlade(true, ortus::NUM_ELEMENTS, *kernelBuddyp, Attribute::Polarity, 5.0);
    fillBlade(true, ortus::NUM_ELEMENTS, *kernelBuddyp, Attribute::Thresh, 7.0);
    
    
    kernelBuddyp->pushToOpenCL();
    
    numIterations = 1;//300;// SHOULD BE ~600!!!
    for (int i = 0; i < numIterations; ++i){
        computeStewardp->executePreRunOperations();
        computeStewardp->run();
        computeStewardp->executePostRunOperations();
        
        // Stimulate the sensors 
        //sensoryStimulationStewardp->performSensoryStimulation();
        
        //diagnosticStewardp->runAdvancedDiagnostics();
    }
    //diagnosticStewardp->plotXCorr();
    
    // temporary placement...
    //dataStewardp->writeConnectome("doesn't matter yet");

    // Just a basic runtime report and small sample of elements
    //stewie.printReport(DataSteward::numKernelLoops);
    
    // temporaray print statement(s)
    /*
    int numKVs = dataStewardp->kernelVoltages.size();
    printf("size of 'kernel_voltages': %d\n", numKVs);
    for (int i = 0; i < numKVs; ++i){
        for (int j = 0; j < dataStewardp->kernelVoltages[i].size(); ++j){
            printf("%.2f, ",dataStewardp->kernelVoltages[i][j]);
        }
        printf("\n");
    }
     */
    //diagnosticStewardp->plotXCorr();
    //diagnosticStewardp->runStandardDiagnostics();
    
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

