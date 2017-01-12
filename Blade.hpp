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

template<class T>
class Blade {
   
public:
    bool scalar;
    bool vector;
    bool readOnly;
    bool deviceScratchPad;
    bool clMemAllocated;
    bool deleteDataBuffers;
    int currentRows;
    int maxRows;
    int currentCols;
    int maxCols;
    cl_mem_flags memFlags;
    size_t currentSize;
    size_t maxSize;
    T* data;
    T* zeros;
    cl_mem clData;
    cl_uint clArgIndex;
    CLHelper* clhelper;
    
public:
    
    /* NOTE: row access starts from 0, but the number of rows must be at least 1 */
    
    /* Allows creation of a 1D array that allows accessing like a 2D array, and allows quick 'growth'.
     */
    Blade(CLHelper* clhelper, cl_mem_flags flags, int currentRows, int currentCols, int maxRows, int maxCols){
        scalar = false;
        vector = false;
        deviceScratchPad = false;
        clMemAllocated = false;
        deleteDataBuffers = false;
        if ((maxRows == maxCols) && (maxRows == 1)){ // it's a scalar
            scalar = true;
        }
        else if ((currentRows == maxRows) && (maxRows == 1)) { // it's a vector, not a 2D matrix
            vector = true;
        }
        this->currentRows = currentRows;
        this->currentCols = currentCols;
        updateCurrentSize();
        maxSize = maxRows * maxCols;
        this->maxRows = maxRows;
        this->maxCols = maxCols;
        data = new T[maxSize](); // init zeroed with '()'
        zeros = new T[maxSize]();
        deleteDataBuffers = true;
        // OpenCL buffer creation
        this->clhelper = clhelper;
        memFlags = flags;
        if ((memFlags & CL_MEM_READ_ONLY) && scalar){ // then we don't need to create a buffer
            readOnly = true;
        } else {
            createCLBuffer();
            clMemAllocated = true;
        }
    }
    
    /* Simplified constructor for a vector (1D) */
    Blade(CLHelper* clhelper, cl_mem_flags flags, int currentCols, int maxCols) : Blade(clhelper, flags, 1, currentCols, 1, maxCols) {};
    
    /* Simplified constructor for a scalar */
    Blade(CLHelper* clhelper, cl_mem_flags flags) : Blade(clhelper, flags, 1, 1, 1, 1) {};
    
    /* Create a Blade this way if you only want to use it as a device scratch pad. No buffer gets created, nothing gets pushed. You just need to set the kernel arg.
     *
     * IMORTANT NOTE: The kernel parameter MUST be declared with the __local qualifier  (in the kernel definition) for this to work.  If it isn't, things might not work as you wish...
     */
    Blade(CLHelper* clhelper, int currentRows, int currentCols, int maxRows, int maxCols){
        this->clhelper = clhelper;
        scalar = false;
        vector = false;
        deviceScratchPad = true;// TRUE!!!
        clMemAllocated = false; // we don't create a buffer
        deleteDataBuffers = false; // nor do we new anything
        this->currentRows = currentRows;
        this->currentCols = currentCols;
        updateCurrentSize();
        maxSize = maxRows * maxCols;
        this->maxRows = maxRows;
        this->maxCols = maxCols;
    }
    
    ~Blade(){
        if (deleteDataBuffers){
            delete[] data;
            delete[] zeros;
            deleteDataBuffers = false;
        }
        if (clMemAllocated){
            clReleaseMemObject(clData);
            clMemAllocated = false;
        }
    }
    
    /* returns a pointer to the data at row, col. NULL if out of range. */
    T* getp(int row, int col){
        if (row < maxRows && col < maxCols){
            return &data[(currentCols*row)+col];
        }
        return NULL;
    }
    
    /* only valid for vectors. returns a pointer to the data at col. NULL if out of range */
    T* getp(int col){
        if (maxRows == 1 && col < currentCols){
            return &data[col];
        }
        return NULL;
    }
    
    /* only valid for scalars. returns a pointer to the data*/
    T* getp(){
        if (scalar){
            return data;
        }
        return NULL;
    }
    
    /* returns the data at row, col. NULL if out of range. */
    T getv(int row, int col){
        if (row < maxRows && col < maxCols){
            return data[(currentCols*row)+col];
        }
        return NULL;
    }
    
    /* only valid for vectors. returns the data at col. NULL if out of range */
    T getv(int col){
        if (maxRows == 1 && col < currentCols){
            return data[col];
        }
        return NULL;
    }
    
    /* only valid for scalars. returns the data*/
    T getv(){
        if (scalar){
            return *data;
        }
        return NULL;
    }
    
    
    
    /* updates the value of data at row, col. This function won't stop you from trying to access a negative index.
     * Note that row access starts from 0 -- the first row, is row 0 (same for columns).*/
    bool set(int row, int col, T value){
        if (row < currentRows && col < currentCols){
            data[(currentCols*row)+col] = value;
            return true;
        }
        return false;
    }
    
    
    /* only valid for a vector. will return false if index is greater than currentCols, or if not a vector */
    bool set(int col, T value){
        if (maxRows == 1 && col < currentCols){
            data[col] = value;
            return true;
        }
        return false;
    }
    
    /* only valid for a scalar */
    bool set(T value){
        if (scalar){
            data[0] = value;
            return true;
        }
        return false;
    }
    
    bool add(int col, T value){
        if (maxRows == 1 && col < currentCols){
            data[col] += value;
            return true;
        }
        return false;
    }
    
    /* adds a row, if currentRows < maxRows, and returns new row count (will be unchanged if check fails) */
    int addRow(){
        if(currentRows < maxRows){
            currentRows++;
        }
        updateCurrentSize();
        return currentRows;
    }
    
    /* adds a col, if currentCols < maxCols, and returns new col count (will be unchanged if check fails) */
    int addCol(){
        if(currentCols < maxCols){
            currentCols++;
        }
        updateCurrentSize();
        return currentCols;
    }
    
    /* adds a row and col if possible. if either fail, nothing changes. */
    void addRowAndCol(int& newRowCount, int& newColCount){
        int oldRowCount = currentRows;
        int oldColCount = currentCols;
        newRowCount = addRow();
        newColCount = addCol();
        if (oldRowCount == newRowCount){// if adding a row failed, reset cols (easier to just do it, than to check to see if we need to first)
            currentCols = oldColCount;
        }
        else if (oldColCount == newColCount){
            currentRows = oldRowCount; // same as above
        }
        updateCurrentSize();
    }
    

    
    /* copys the data array from otherBlade to this blade's data, overwriting the contents.
     *
     * NOTE: This doesn't do any error checking, other than total length. Make sure the two data arrays are of equal dimensions.
     */
    bool copyData(Blade* otherBlade){
        if ((*otherBlade).currentSize != currentSize)
            return false;
        for (int i = 0; i < currentSize; ++i){
            data[i] = (*otherBlade).data[i];
        }
        return true;
    }
    
    /* sets the openCL kernel argument index, to be used when calling clSetKernelArg */
    void setCLArgIndex(cl_uint argIndex, cl_kernel* kernel){
        clArgIndex = argIndex;
        if (scalar && readOnly){ // then we don't need a buffer, and just send the data over (which is already a pointer... don't send a reference to it.
            this->clhelper->err = clSetKernelArg(*kernel, clArgIndex, sizeof(T), data);
        }
        else if (deviceScratchPad){
            this->clhelper->err = clSetKernelArg(*kernel, clArgIndex, currentSize*sizeof(T), NULL);// note, we don't set an address for the arg, BUT, we tell the kernel how big the __local buffer needs to be!
        }
        else {
            this->clhelper->err = clSetKernelArg(*kernel, clArgIndex, sizeof(cl_mem), &clData);
        }
        this->clhelper->check_and_print_cl_err(this->clhelper->err);
    }
    
    /* create the buffer with the maxSize (when we push the buffer, we'll only use currentSize) */
    void createCLBuffer(){
        clData = clCreateBuffer(this->clhelper->context, memFlags, sizeof(T) * maxSize, NULL,&this->clhelper->err);
        this->clhelper->check_and_print_cl_err(this->clhelper->err);
    }
    
    /* moves data into clData, ready for use by the kernel */
    void pushCLBuffer(){
        this->clhelper->err = clEnqueueWriteBuffer(this->clhelper->commands, clData, CL_TRUE, 0, sizeof(T) * currentSize, data, 0, NULL, NULL);
        this->clhelper->check_and_print_cl_err(this->clhelper->err);
    }
    
    /* reads data from the OpenCL buffer into data. This will overwrite anything in data.
     * NOTE: if the cl_mem_flags variable set during buffer creation does not permit writing (for OpenCL), nothing will be read, and readCLBuffer will return false. */
    bool readCLBuffer(){
        if (!((memFlags & CL_MEM_READ_WRITE) || (memFlags & CL_MEM_WRITE_ONLY))){
            return false;
        }
        this->clhelper->err = clEnqueueReadBuffer(this->clhelper->commands, clData, CL_TRUE, 0, sizeof(T) * currentSize, data, 0, NULL, NULL);
        this->clhelper->check_and_print_cl_err(this->clhelper->err);
        return true;
    }
    
    /* writes zeros to the OpenCL buffer, to ensure it is empty */
    void clearCLBuffer(){
        this->clhelper->err = clEnqueueWriteBuffer(this->clhelper->commands, clData, CL_TRUE, 0, sizeof(T) * maxSize, zeros, 0, NULL, NULL);
        this->clhelper->check_and_print_cl_err(this->clhelper->err);
    }
    
private:
    
    void updateCurrentSize(){
        currentSize = currentRows * currentCols;
    }
    
};



#endif /* Blade_hpp */
