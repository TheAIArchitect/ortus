//
//  ComputeSteward.hpp
//  ortus
//
//  Created by andrew on 1/8/17.
//  Copyright © 2017 Andrew W.E. McDonald. All rights reserved.
//

#ifndef ComputeSteward_hpp
#define ComputeSteward_hpp


#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#include <stdio.h>
//#include "Timer.hpp"

#include "CLHelper.hpp"
#include "DataSteward.hpp"

class ComputeSteward {
    
public:
    ComputeSteward(size_t globalSize, size_t localSize);
    void enqueueKernel();
    void setupOpenCL();
    void printReport(int num_runs);
    
    void initializeOpenCL();
    void run(int numIterations = 1);
    void cleanUpOpenCL();
    void cleanUp();
    
    DataSteward* dStewiep;
    
    int currentIteration;
    
    CLHelper clHelper;
    // OpenCL related variables
    size_t global; // global size
    size_t local; // local size
    cl_program program;
    cl_kernel kernel;
    char *programBuffer;
    
    // Timing variables
    clock_t start_time_total;
    clock_t stop_time_total;
    clock_t start_setup_time;
    clock_t stop_setup_time;
    clock_t start_opencl_time;
    clock_t stop_opencl_time;
    clock_t start_memtran_time;
    clock_t stop_memtran_time;
    clock_t start_reset_time;
    clock_t stop_reset_time;
    double total_run_time{0};
    double setup_run_time{0};
    double opencl_run_time{0};
    double opencl_mem_time{0};
    double reset_time{0};
    /*
    Timer total_timer_;
    Timer setup_timer_;
    */
    
};

#endif /* ComputeSteward_hpp */
