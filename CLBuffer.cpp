//
//  CLBuffer.cpp
//  ortus
//
//  Created by andrew on 3/28/17.
//  Copyright © 2017 Andrew W.E. McDonald. All rights reserved.
//

#include "CLBuffer.hpp"


template <class T>
CLBuffer<T>::CLBuffer(CLHelper* clhelper, size_t bufferSize, cl_mem_flags flags, bool isDeviceScratchPad){
    clHelper = clhelper;
    clBufferSize = bufferSize;
    clMemFlags = flags;
    deviceScratchPad = isDeviceScratchPad;
    clMemAllocated = false;
    if(!deviceScratchPad){
        createBuffer();
        zeros = new T[clBufferSize]();// init zeroed with "()"
    }
}

template <class T>
CLBuffer<T>::CLBuffer(CLHelper* clhelper, size_t bufferSize, cl_mem_flags flags) : CLBuffer(clhelper, bufferSize, flags, false){};

template <class T>
CLBuffer<T>::~CLBuffer(){
    if (clMemAllocated){
        clReleaseMemObject(clData);
        clMemAllocated = false;
    }
}

template <class T>
bool CLBuffer<T>::createBuffer(){
    clData = clCreateBuffer(clHelper->context, clMemFlags, sizeof(T) * clBufferSize, NULL,&clHelper->err);
    clHelper->check_and_print_cl_err(clHelper->err);
    clMemAllocated = true;
    return true;
}


/* sets the openCL kernel argument index, to be used when calling clSetKernelArg */
template <class T>
void CLBuffer<T>::setCLArgIndex(cl_uint argIndex, cl_kernel* kernelp){
    clArgIndex = argIndex;
    if (deviceScratchPad){
        clHelper->err = clSetKernelArg(*kernelp, clArgIndex, clBufferSize*sizeof(T), NULL);// note, we don't set an address for the arg, BUT, we tell the kernel how big the __local buffer needs to be!
    }
    else {
        this->clHelper->err = clSetKernelArg(*kernelp, clArgIndex, sizeof(cl_mem), &clData);
    }
    this->clHelper->check_and_print_cl_err(this->clHelper->err);
}

/* writes zeros to the OpenCL buffer, to ensure it is empty */
template <class T>
void CLBuffer<T>::clearBuffer(){
    clHelper->err = clEnqueueWriteBuffer(clHelper->commands, clData, CL_TRUE, 0, sizeof(T) * clBufferSize, zeros, 0, NULL, NULL);
    clHelper->check_and_print_cl_err(clHelper->err);
 }

template class CLBuffer<cl_float>;
