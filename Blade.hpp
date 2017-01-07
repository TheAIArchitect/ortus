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
#include "CLHelper.hpp"
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif


class Blade {
   
public:
    Blade(CLHelper* clhelper, cl_mem_flags flags, int currentRows, int currentCols, int maxRows, int maxCols);
    ~Blade();
    float* getp(int row, int col);
    float getv(int row, int col);
    bool set(int row, int col, float value);
    bool set(int col, float value); // only valid if the Blade is a vector
    bool add(int col, float value); // only valid if the Blade is a vector
    int addEntry();
    void createCLBuffer();
    bool copyData(Blade* otherBlade);
    void setCLArgIndex(cl_uint argIndex, cl_kernel* kernel);
    void pushCLBuffer();
    bool readCLBuffer();
    void clearCLBuffer();
    bool square;
    int currentRows;
    int maxRows;
    int currentCols;
    int maxCols;
    cl_mem_flags memFlags;
    size_t currentSize;
    size_t maxSize;
    float* data;
    float* zeros;
    cl_mem clData;
    cl_uint clArgIndex;
    CLHelper* clhelper;
    
};

#endif /* Blade_hpp */
