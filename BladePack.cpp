//
//  BladePack.cpp
//  ortus
//
//  Created by andrew on 3/27/17.
//  Copyright © 2017 Andrew W.E. McDonald. All rights reserved.
//

#include "BladePack.hpp"
#include <unordered_map>
#include "OrtusNamespace.hpp"

template <class T>
BladePack<T>::BladePack(){}

template <class T>
void BladePack<T>::addBlade(Blade<T>* blade){
    
}




class Thing {
public:
    ortus::attribute_blade_unordered_map mapOfThings;
    Thing(int numThings, CLHelper* clhelper, cl_kernel* kernelp, int NUM_ELEMENTS, int MAX_ELEMENTS);
    void addThing(Attribute attribute, int element_id, float value);
    void addThing(Attribute attribute, int pre_id, int post_id, float value);
    float* getThing(Attribute attribute, int element_id);
    float* getThing(Attribute attribute, int pre_id, int post_id);
    
    CLHelper* clHelper;
    cl_kernel* kernelp;
    
    void setKernelArgs();
    void pushToOpenCL();
    void pullFromOpenCL();
    
    std::vector<Attribute> elementThings;
    std::vector<size_t> elementThingsByteOffsets;
    CLBuffer<cl_float>* elementThingsBuffer;
    std::vector<Attribute> relationThings;
    std::vector<size_t> relationThingsByteOffsets;
    CLBuffer<cl_float>* relationThingsBuffer;
    
};

Thing::Thing(int numThings, CLHelper* clhelper, cl_kernel* kernelp, int NUM_ELEMENTS, int MAX_ELEMENTS){
    clHelper = clhelper;
    int MAX_ELEMENTS_SQ = MAX_ELEMENTS * MAX_ELEMENTS;
    mapOfThings.reserve(numThings);
    elementThings = { Attribute::Type, Attribute::Affect, Attribute::Activation };
    int numElementThings = elementThings.size();
    elementThingsByteOffsets.reserve(numElementThings);
    // this assumes all Blades in the proceeding loop have the same size
    size_t bufferSize = numElementThings * MAX_ELEMENTS; // 1D, not bytes -- CLBuffer computes that
    // CLBuffer must be the same type as the Blades' type, same for cl_mem_flags
    elementThingsBuffer = new CLBuffer<cl_float>(clhelper, bufferSize, CL_MEM_READ_WRITE);
    elementThingsBuffer->createBuffer();
    elementThingsBuffer->setCLArgIndex(0, kernelp);
    for (int i = 0; i < numElementThings; ++i){
        mapOfThings[elementThings[i]] = new Blade<cl_float>(clHelper, CL_MEM_READ_WRITE, NUM_ELEMENTS, MAX_ELEMENTS);
        if (i > 0){
            // NOTE: the type we get the 'sizeof' must be the same as the Blade's type.
            elementThingsByteOffsets[i] = elementThingsByteOffsets[i-1] + MAX_ELEMENTS*sizeof(cl_float);
        }
        else {
            elementThingsByteOffsets[i] = 0; // first one doesn't need an offset.
        }
        mapOfThings[elementThings[i]]->setCLBufferAndOffset(elementThingsBuffer, elementThingsByteOffsets[i]);
    }
    //
    
    
    relationThings = { Attribute::Weight, Attribute::Polarity, Attribute::Thresh };
    int numRelationThings = relationThings.size();
    relationThingsByteOffsets.reserve(numRelationThings);
    // this assumes all Blades in the proceeding loop have the same size
    bufferSize = numRelationThings * MAX_ELEMENTS_SQ; // 2D, not bytes -- CLBuffer computes that
    // CLBuffer must be the same type as the Blades' type, same for cl_mem_flags
    relationThingsBuffer = new CLBuffer<cl_float>(clhelper, bufferSize, CL_MEM_READ_WRITE);
    relationThingsBuffer->createBuffer();
    relationThingsBuffer->setCLArgIndex(1, kernelp);
    for (int i = 0; i < numRelationThings; ++i){
        mapOfThings[relationThings[i]] = new Blade<cl_float>(clHelper,CL_MEM_READ_WRITE, NUM_ELEMENTS, NUM_ELEMENTS, MAX_ELEMENTS, MAX_ELEMENTS);
        if (i > 0){
            // NOTE: the type we get the 'sizeof' must be the same as the Blade's type.
            relationThingsByteOffsets[i] = relationThingsByteOffsets[i-1] + MAX_ELEMENTS_SQ*sizeof(cl_float);
        }
        else {
            relationThingsByteOffsets[i] = 0; // first one doesn't need an offset.
        }
        mapOfThings[relationThings[i]]->setCLBufferAndOffset(relationThingsBuffer,relationThingsByteOffsets[i]);
    }
    
};

void Thing::addThing(Attribute attribute, int element_id, float value){
    mapOfThings[attribute]->set(element_id, value);
}

void Thing::addThing(Attribute attribute, int pre_id, int post_id, float value){
    mapOfThings[attribute]->set(pre_id, post_id, value);
}

/** element_id would be automatically input by the ElementInfoModule */
float* Thing::getThing(Attribute attribute, int element_id){
    return mapOfThings[attribute]->getp(element_id);
}

/** element_id would be automatically input by the ElementRelation */
float* Thing::getThing(Attribute attribute, int pre_id, int post_id){
    return mapOfThings[attribute]->getp(pre_id,post_id);
}

/**
 * will have to set the right Blade for each attribute upon Element/ElementRelation creation. */

void Thing::setKernelArgs(){
    
}

void Thing::pushToOpenCL(){
    for (auto blade : mapOfThings){
        blade.second->pushDataToDevice();
    }
}

void Thing::pullFromOpenCL(){
    
}

void fillBlade(bool _2D, int size, Thing& thing, Attribute attrib, float value){
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


int main (int argc, char** argv){
    size_t globalSize = 512; // total work-items to execute (number of times to call kernel)
    size_t localSize = 64;// 16 or 32? // number of work-items to group into a work-group
    // the number of work-groups = global / local = 512/64  = 8 work-groups
    CLBuddy clBuddy(globalSize, localSize);
    clBuddy.initializeOpenCL();
    CLHelper* clHelper = &clBuddy.clHelper;
    int NUM_ELEMENTS = 10;
    int NUM_ELEMENTS_SQ = NUM_ELEMENTS * NUM_ELEMENTS; // size of 1D square matrix
    int MAX_ELEMENTS = 100;
    int MAX_ELEMENTS_SQ = MAX_ELEMENTS * MAX_ELEMENTS; // max size of 1D square matrix
    
    
    //BladePack<cl_float> bpack;
    //Blade<cl_float>* b = new Blade<cl_float>(clHelper,CL_MEM_READ_WRITE, NUM_ELEMENTS, NUM_ELEMENTS, MAX_ELEMENTS, MAX_ELEMENTS);
    
    
    //bpack.addBlade(b);
    
    Thing theThing(static_cast<int>(Attribute::NUM_ATTRIBUTES), clHelper, &clBuddy.kernel, NUM_ELEMENTS, MAX_ELEMENTS);
    
    
    fillBlade(false, NUM_ELEMENTS, theThing, Attribute::Type, 2.0);
    fillBlade(false, NUM_ELEMENTS, theThing, Attribute::Affect, 4.0);
    fillBlade(false, NUM_ELEMENTS, theThing, Attribute::Activation, 6.0);
    fillBlade(true, NUM_ELEMENTS, theThing, Attribute::Weight, 3.0);
    fillBlade(true, NUM_ELEMENTS, theThing, Attribute::Polarity, 5.0);
    fillBlade(true, NUM_ELEMENTS, theThing, Attribute::Thresh, 7.0);
    
    
    theThing.pushToOpenCL();
    clBuddy.enqueueKernel();
    
    printf("I work.\n");
    return 0;
}
