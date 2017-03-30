//
//  KernelBuddy.hpp
//  ortus
//
//  Created by andrew on 3/29/17.
//  Copyright © 2017 Andrew W.E. McDonald. All rights reserved.
//

#ifndef KernelBuddy_hpp
#define KernelBuddy_hpp

#include <stdio.h>
#include "CLHelper.hpp"
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#include <Blade.hpp>
#include "Attribute.hpp"
#include <unordered_map>
#include <string>

#include <unordered_map>
#include "OrtusNamespace.hpp"

#include "CLBuffer.hpp"
#include "KernelArg.hpp"

/* STUFF FROM DATASTEWARD */
/**
 // NOTE: the voltages Blade gets read from and written to by opencl!!!
    Blade<float>* voltages;
    // NOTE: the outputVotlageHistory Blade has one neuron's voltages per ROW (so, it's an n x 5 'matrix', if we store 5 previous values)
    //NOTE2: maybe we should make 2 of these -- one for reading only, one for writing only... maybe same for voltages, idk...
    //NOTE3: look into CL_MEM_USE_HOST_PTR and clEnqueueMapBuffer
    Blade<float>* outputVoltageHistory;
    Blade<float>* gaps;
    Blade<float>* chems;
    Blade<float>* learningRates;
    Blade<float>* chemContrib;
    Blade<float>* gapContrib;
 
    Blade<cl_float>* gapNormalizer; // we divide gap weights by this
    Blade<cl_float>* chemNormalizer; // we divide chem weights by this
    
    // (zero indexed): [rowCount, colCount, kernelIterationNum, voltageHistorySize ]
    Blade<cl_int>* metadata;
    
    Blade<cl_float>* deviceScratchPadXCorr; // memory is local on device, so size depends upon work-group size, XCorr comptuations
    Blade<cl_float>* deviceScratchPadVoltageROC; // memory is local on device, so size depends upon work-group size, voltage rate of change computations
    
 
 // these are all the kernel args we used to have
    voltages->setCLArgIndex(0, kernelp);
    outputVoltageHistory->setCLArgIndex(1, kernelp);
    gaps->setCLArgIndex(2, kernelp);
    chems->setCLArgIndex(3, kernelp);
    gapContrib->setCLArgIndex(4, kernelp);
    chemContrib->setCLArgIndex(5, kernelp);
    clHelperp->err = clSetKernelArg(*kernelp, 6, sizeof(cl_int), &Probe::shouldProbe);
    clHelperp->check_and_print_cl_err(clHelperp->err);
    gapNormalizer->setCLArgIndex(7, kernelp);
    chemNormalizer->setCLArgIndex(8, kernelp);
    metadata->setCLArgIndex(9, kernelp);
    deviceScratchPadXCorr->setCLArgIndex(10,kernelp);
    deviceScratchPadVoltageROC->setCLArgIndex(11,kernelp);
 
 */




class KernelBuddy {
    public:
        KernelBuddy(int numThings, CLHelper* clhelper, cl_kernel* kernelp);
    
        // should probably be in DataSteward
        std::unordered_map<Attribute, Blade<cl_float>*> attributeBladeMap;
        Blade<cl_float>* activationBlade;
    
    
    
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

#endif /* KernelBuddy_hpp */
