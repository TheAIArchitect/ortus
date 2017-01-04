//
//  Blade.hpp
//  ortus
//
//  Created by Andrew McDonald on 1/4/17.
//  Copyright © 2017 Andrew W.E. McDonald. All rights reserved.
//

#ifndef Blade_hpp
#define Blade_hpp

#include <stdio.h>
#include "CLHelperClass.hpp"
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif


class Blade {
   
public:
    Blade(int currentRows, int currentCols, int maxRows, int maxCols);
    ~Blade();
    float* getp(int row, int col);
    float getv(int row, int col);
    bool set(int row, int col, float value);
    int addEntry();
    void createCLBuffer(CLHelperClass& clhelper, cl_mem_flags flags);
    bool copyData(Blade& otherBlade);
    void setCLArgIndex(int argIndex);
    void pushCLBuffer();
    void clearCLBuffer();
    bool square;
    int currentRows;
    int maxRows;
    int currentCols;
    int maxCols;
    size_t currentSize;
    size_t maxSize;
    float* data;
    float* zeros;
    cl_mem clData;
    cl_uint clArgIndex;
    CLHelperClass* clhelper;
    
};

#endif /* Blade_hpp */
