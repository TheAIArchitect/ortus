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

/**
 * This constructor creates a CLBuffer that will be used as a device scratch pad.
 *
 * That means, the kernel arg must be given the 'local' specifier in the .cl file,
 * and, no actual buffer is created (so, clData is null).
 *
 * Further, while flags (cl_mem_flags) is set to CL_MEM_READ_WRITE,
 * it is irrelevant, because it is ignored.
 */
template <class T>
CLBuffer<T>::CLBuffer(CLHelper* clhelper, size_t bufferSize, bool isDeviceScratchpad) : CLBuffer(clhelper, bufferSize, CL_MEM_READ_WRITE, isDeviceScratchpad) {
    if (!isDeviceScratchpad){
        printf("(CLBuffer) Error: You may only create a CLBuffer without specifying the 'cl_mem_flags memFlags' argument, if you are creating the CLBuffer to be used as a device scratchpad.\n");
        exit(25);
    }
}

template <class T>
CLBuffer<T>::CLBuffer(CLHelper* clhelper, size_t bufferSize, cl_mem_flags flags) : CLBuffer(clhelper, bufferSize, flags, false) {}
    


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
        clHelper->err = clSetKernelArg(*kernelp, clArgIndex, sizeof(cl_mem), &clData);
    }
    clHelper->check_and_print_cl_err(this->clHelper->err);
}

/* writes zeros to the OpenCL buffer, to ensure it is empty */
template <class T>
void CLBuffer<T>::clearBuffer(){
    if (!deviceScratchPad){
        clHelper->err = clEnqueueWriteBuffer(clHelper->commands, clData, CL_TRUE, 0, sizeof(T) * clBufferSize, zeros, 0, NULL, NULL);
        clHelper->check_and_print_cl_err(clHelper->err);
    }
 }

template class CLBuffer<cl_float>;
/** add cl_int??? */
