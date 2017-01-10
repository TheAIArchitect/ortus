//
//  ComputeSteward.cpp
//  ortus
//
//  Created by andrew on 1/8/17.
//  Copyright © 2017 Andrew W.E. McDonald. All rights reserved.
//

#include "ComputeSteward.hpp"

ComputeSteward::ComputeSteward(size_t globalSize, size_t localSize){
    global = globalSize;
    local = localSize;
    currentIteration = 0;
}

void ComputeSteward::run(){
    enqueueKernel();
    currentIteration++;
}

void ComputeSteward::executePreRunOperations(){
    
    dStewiep->setOpenCLKernelArgs();
    printf("Set OpenCL Kernel Args\n");
    
    if (currentIteration == 0){
        // this only happens for all buffers prior to the first run
        dStewiep->pushOpenCLBuffers();
        printf("Pushed OpenCL Buffers\n");
    }
    else {// just push the ones we want to push on each iteration (so, the ones that we made changes to)
        
        // soon: gaps, chems
        dStewiep->inputVoltages->pushCLBuffer();
    }
}


void ComputeSteward::executePostRunOperations(){
    dStewiep->executePostRunMemoryTransfers();
}



/* initialize the actual OpenCL system -- must be done before creating any Blade objects, because we need a valid OpenCL context in order for the Blade to create its buffer, which it does upon initialization */
void ComputeSteward::initializeOpenCL(){
    clHelper.setup_opencl();
    clHelper.read_kernels_from_file("OrtusKernelOne.cl", &programBuffer);
    program = clCreateProgramWithSource(clHelper.context, 1, (const char**) &programBuffer, NULL, &clHelper.err);
    clHelper.check_and_print_cl_err(clHelper.err);
    clHelper.err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    clHelper.check_and_print_cl_program_build_err(clHelper.err, &program);
    kernel = clCreateKernel(program, "OrtusKernel", &clHelper.err);
    clHelper.check_and_print_cl_err(clHelper.err);
    
    dStewiep->setKernelp(&kernel);
    dStewiep->setCLHelperp(&clHelper);
}

void ComputeSteward::cleanUpOpenCL(){
    clReleaseProgram(program);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(clHelper.commands);
    clReleaseContext(clHelper.context);
}

void ComputeSteward::cleanUp(){
    cleanUpOpenCL();
}

void ComputeSteward::printReport(int num_runs){
    /*
    std::cout << "\n\n\n";
    total_run_time += opencl_run_time;
    //std::cout << "Number of iterations: " << _number_of_loops << ".\n";
    std::cout << "Total run time: " << total_run_time << " ms" << std::endl;
    std::cout << "Setup run time: " << setup_run_time << " ms" << std::endl;
    std::cout << "OpenCL processing time: " << opencl_run_time << " ms" << std::endl;
    std::cout << "OpenCL memory transer time: " << opencl_mem_time << " ms" << std::endl;
    std::cout << "Reset vector time: " << reset_time << " ms" << std::endl;
     */
    
    
}

// NOTE: store the global and local... stop passing.
void ComputeSteward::enqueueKernel(){
    cl_event timing_event;
    
    size_t g, l;
    g = global;
    l = local;
    clHelper.err = clEnqueueNDRangeKernel(clHelper.commands, kernel, 1, NULL, &g, &l, 0, NULL, &timing_event);
    clHelper.check_and_print_cl_err(clHelper.err);
    
    clFinish(clHelper.commands);
    cl_ulong starttime;
    cl_ulong endtime;
    clHelper.check_and_print_cl_err(clGetEventProfilingInfo(timing_event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &starttime, NULL));
    clHelper.check_and_print_cl_err(clGetEventProfilingInfo(timing_event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &endtime, NULL));
    opencl_run_time += ((endtime - starttime) / 1000000.0);
}
