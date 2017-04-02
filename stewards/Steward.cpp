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
    
    dataStewardp->initializeKernelArgsAndBlades(&computeStewardp->clHelper, &computeStewardp->kernel, computeStewardp->workGroupSize);
    
    printf("cool\n");
    // once initializeKernelArgsAndBlades returns, we can create the connectome, which will use the Blades
    dataStewardp->loadConnectome("ort/simple.ort");
    
    exit(3);
    
    //dataStewardp->init(computeStewardp->workGroupSize);
    //sensoryStimulationStewardp = new SensoryStimulationSteward(dataStewardp);
    //sensoryStimulationStewardp->setStimuli();
    //diagnosticStewardp = new DiagnosticSteward(dataStewardp);
    
}

/*
void fillBlade(DataSteward* dsp, bool _2D, int size, Attribute attrib, float value){
    if (_2D){
        int i,j;
        for (i = 0; i < size; ++i){
            for (j = 0; j < size; ++j){
                dsp->attributeBladeMap[attrib]->set(i,j, value);
            }
        }
    }
    else {
        int i;
        for (i = 0; i < size; ++i){
            dsp->attributeBladeMap[attrib]->set(i, value);
        }
    }
}
*/
 
// NOTE: these two are just for reference purposes... not going to stay here...
/** element_id would be automatically input by the ElementInfoModule 
float* getThing(DataSteward* dsp, Attribute attribute, int element_id){
    return dsp->attributeBladeMap[attribute]->getp(element_id);
}
 */

/** element_id would be automatically input by the ElementRelation 
float* getThing(DataSteward* dsp, Attribute attribute, int pre_id, int post_id){
    return dsp->attributeBladeMap[attribute]->getp(pre_id,post_id);
}
*/
 
// END reference functions

void Steward::run(){
    
    /** NEXT:
     * trace execution such that it's clear where to start making sure that the data from the .ort file is coming in and ready to be used, and where we can create KernelBuddy. KernelBuddy must be assigned to the Elements and ElementRelations prior to reading the data into the elements.
     
     * the above means that the further above list of attributes must be implemented before assigning KernelBuddy to the Elements/Relations, and assigning data.
     
     * the kernel must be redone, but only once there is enough data being passed to it to be able to test things.
     */
    
    
    /*
        fillBlade(dataStewardp, false, ortus::NUM_ELEMENTS, Attribute::EType, 2.0);
        fillBlade(dataStewardp, false, ortus::NUM_ELEMENTS, Attribute::Affect, 4.0);
        fillBlade(dataStewardp, false, ortus::NUM_ELEMENTS, Attribute::Activation, 6.0);
        fillBlade(dataStewardp, true, ortus::NUM_ELEMENTS, Attribute::Weight, 3.0);
        fillBlade(dataStewardp, true, ortus::NUM_ELEMENTS, Attribute::Polarity, 5.0);
        fillBlade(dataStewardp, true, ortus::NUM_ELEMENTS, Attribute::RThresh, 7.0);
     */
    
    //kernelBuddyp->pushToOpenCL();
    
    /** maybe some function to give all classes all pointers -- e.g:
     -> it sets all instances of dataStewardp necessary, etc.
     */
    
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

