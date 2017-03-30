//
//  KernelBuddy.cpp
//  ortus
//
//  Created by andrew on 3/29/17.
//  Copyright © 2017 Andrew W.E. McDonald. All rights reserved.
//

#include "KernelBuddy.hpp"





KernelBuddy::KernelBuddy(int numThings, CLHelper* clhelper, cl_kernel* kernelp){
    clHelper = clhelper;
    this->kernelp = kernelp;
    int NUM_ELEMENTS = ortus::NUM_ELEMENTS;
    int MAX_ELEMENTS = ortus::MAX_ELEMENTS;
    int MAX_ELEMENTS_SQ = ortus::MAX_ELEMENTS * ortus::MAX_ELEMENTS;
    //mapOfThings.reserve(numThings);
    
    
    /* these are the kernel args we want */
    /* NOTE: 'static' prefix suggests set items should rarely change,
     * while 'dynamic' suggests items in the set may change value every iteration */
    // staticElement = {Attribute::ElementType, Attribute::Affect};//, Attribute::EThresh}
    // dynamicElement = {Attribute::Activation}; // this one needs to have the history 
    // staticRelation = {Attribute::RType, Attribute::Polarity, Attribute::Direction, Attribute::RThresh};
    // dynamicRelation = {Attribute::a
    /*
     In addition to the above, we need:
        # metadata (see metadata from old DataSteward)
        # scalars -- separate from metadata scalars
        # offset info
        # scratchpads
     */
    // way to approach offset info:
        // 1) each kernel arg *must* only have data that is seperable by the same offset (so, either scalar, 1D, or 2D, but not a combination)
        // 2) there will be 1 '2d' kernel arg formatted like so (kai <=> kernel arg index):
            // [kai][0] => number of 'keys', [kai][1] => offset size between keys, 
            // it will be up to the implementor to ensure that the proper order of attributes/etc. are being accessed.
    
    /** NEXT: 
     * trace execution such that it's clear where to start making sure that the data from the .ort file is coming in and ready to be used, and where we can create KernelBuddy. KernelBuddy must be assigned to the Elements and ElementRelations prior to reading the data into the elements. 
     
     * the above means that the further above list of attributes must be implemented before assigning KernelBuddy to the Elements/Relations, and assigning data.
     
     * the kernel must be redone, but only once there is enough data being passed to it to be able to test things.
     */
    
    
    elementThings = { Attribute::Type, Attribute::Affect, Attribute::Activation };
    
    KernelArg<cl_float, Attribute>* kArg0 = new KernelArg<cl_float, Attribute>(clHelper, kernelp, 0);
    kArg0->setKeys(elementThings);
    std::unordered_map<Attribute, Blade<cl_float>*>* thingMapA = kArg0->createBufferAndBlades(1, NUM_ELEMENTS, MAX_ELEMENTS,  CL_MEM_READ_WRITE);
    for (auto thing : *thingMapA){
        attributeBladeMap[thing.first] = thing.second;
    }
    
    
    
    relationThings = { Attribute::Weight, Attribute::Polarity, Attribute::Thresh };
    
    KernelArg<cl_float, Attribute>* kArg1 = new KernelArg<cl_float, Attribute>(clHelper, kernelp, 1);
    kArg1->setKeys(relationThings);
    std::unordered_map<Attribute, Blade<cl_float>*>* thingMapB = kArg1->createBufferAndBlades(2, NUM_ELEMENTS, MAX_ELEMENTS, CL_MEM_READ_WRITE);
    for (auto thing : *thingMapB){
        attributeBladeMap[thing.first] = thing.second;
    }
   
    
};


void KernelBuddy::addThing(Attribute attribute, int element_id, float value){
    attributeBladeMap[attribute]->set(element_id, value);
}

void KernelBuddy::addThing(Attribute attribute, int pre_id, int post_id, float value){
    attributeBladeMap[attribute]->set(pre_id, post_id, value);
}

/** element_id would be automatically input by the ElementInfoModule */
float* KernelBuddy::getThing(Attribute attribute, int element_id){
    return attributeBladeMap[attribute]->getp(element_id);
}

/** element_id would be automatically input by the ElementRelation */
float* KernelBuddy::getThing(Attribute attribute, int pre_id, int post_id){
    return attributeBladeMap[attribute]->getp(pre_id,post_id);
}

/**
 * will have to set the right Blade for each attribute upon Element/ElementRelation creation. */

void KernelBuddy::setKernelArgs(){
    
}

void KernelBuddy::pushToOpenCL(){
    for (auto blade : attributeBladeMap){
        blade.second->pushDataToDevice();
    }
}

void KernelBuddy::pullFromOpenCL(){
    for (auto blade : attributeBladeMap){
        blade.second->readDataFromDevice();
    }
    
}



//// create new blade for this (all scalars, or one that rarely changes, and one that changes all the time, just like the other blades), and access it via enum or something, e.g., static cast to get blade index -- set as vector
/* this must be called for each kernel iteration...
void DataSteward::updateMetadataBlade(int kernelIterationNum){
    metadata->set(0, CONNECTOME_ROWS);
    metadata->set(1, CONNECTOME_COLS);
    metadata->set(2, kernelIterationNum);
    metadata->set(3, VOLTAGE_HISTORY_SIZE);
    metadata->set(4, XCORR_COMPUTATIONS);
}
*/



// the probe.
/*
void DataSteward::feedProbe(){
     // cs contrib
     clhc.err = clEnqueueReadBuffer(clhc.commands, cs_contrib, CL_TRUE, 0, (sizeof(float) * DataSteward::NUM_ELEMS * DataSteward::NUM_ELEMS), cs_1d_contrib, 0, NULL, NULL);
     clhc.check_and_print_cl_err(clhc.err);
     // gj contrib
     clhc.err = clEnqueueReadBuffer(clhc.commands, gj_contrib, CL_TRUE, 0, (sizeof(float) * DataSteward::NUM_ELEMS * DataSteward::NUM_ELEMS), gj_1d_contrib, 0, NULL, NULL);
     clhc.check_and_print_cl_err(clhc.err);
     
     
     cs_2d_contrib_t = stewie.convert_1d_to_2d(cs_1d_contrib, DataSteward::NUM_ELEMS, DataSteward::NUM_ELEMS);
     gj_2d_contrib_t = stewie.convert_1d_to_2d(gj_1d_contrib, DataSteward::NUM_ELEMS, DataSteward::NUM_ELEMS);
     probe->csContribVec.push_back(cs_2d_contrib_t);
     probe->gjContribVec.push_back(gj_2d_contrib_t);
     
     // clear the cs_contrib buffer
     clhc.err = clEnqueueWriteBuffer(clhc.commands, cs_contrib, CL_TRUE, 0, sizeof(float) * DataSteward::NUM_ELEMS * DataSteward::NUM_ELEMS, zeros1D, 0, NULL, NULL);
     clhc.check_and_print_cl_err(clhc.err);
     // do the same with the gj_contrib buffer
     clhc.err = clEnqueueWriteBuffer(clhc.commands, cs_contrib, CL_TRUE, 0, sizeof(float) * DataSteward::NUM_ELEMS * DataSteward::NUM_ELEMS, zeros1D, 0, NULL, NULL);
     clhc.check_and_print_cl_err(clhc.err);
     
}
*/
