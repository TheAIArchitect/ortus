//
//  CLBuffer.hpp
//  ortus
//
//  Created by andrew on 3/28/17.
//  Copyright © 2017 Andrew W.E. McDonald. All rights reserved.
//

#ifndef CLBuffer_hpp
#define CLBuffer_hpp

#include <stdio.h>
#include "CLHelper.hpp"
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

template <class T>
class CLBuffer {
public:
    CLBuffer(CLHelper* clhelper, size_t bufferSize, cl_mem_flags flags, bool isDeviceScratchPad);
    CLBuffer(CLHelper* clhelper, size_t bufferSize, bool isDeviceScratchpad);
    CLBuffer(CLHelper* clhelper, size_t bufferSize, cl_mem_flags flags);
    ~CLBuffer();
    CLHelper* clHelper;
    size_t clBufferSize;
    bool clMemAllocated;
    
    
    bool createBuffer();
    void clearBuffer();
    void setCLArgIndex(cl_uint argIndex, cl_kernel* kernel);
    
    cl_mem clData;
    cl_mem_flags clMemFlags;
    cl_uint clArgIndex;
    
    T* zeros;
    // in this case, no buffer is created, but a kernel argument is set.
    bool deviceScratchPad;
    
};



#endif /* CLBuffer_hpp */
