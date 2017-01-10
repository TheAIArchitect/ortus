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
    //Blade(CLHelper* clhelper, cl_mem_flags flags, int currentRows, int currentCols, int maxRows, int maxCols);
    //~Blade();
    //T* getp(int row, int col);
    //T getv(int row, int col);
    //bool set(int row, int col, T value);
    //bool set(int col, T value); // only valid if the Blade is a vector
    //bool add(int col, T value); // only valid if the Blade is a vector
    //int addEntry();
    //void createCLBuffer();
    //bool copyData(Blade* otherBlade);
    //void setCLArgIndex(cl_uint argIndex, cl_kernel* kernel);
    //void pushCLBuffer();
    //bool readCLBuffer();
    //void clearCLBuffer();
    bool squareAndNotScalar;
    bool scalar;
    bool readOnly;
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
    
    /////////////// TODO: add two more constructors (scalar, vector) to make it easier to create non-matrix Blades
public:
    
    /* NOTE: row access starts from 0, but the number of rows must be at least 1 */
    
    // look into making the float* private, but allowing 'getter' access to the pointer -- see if that will prevent changes without using a setter
    /* Allows creation of a 1D array that allows accessing like a 2D array, and allows quick 'growth'.
     */
    Blade(CLHelper* clhelper, cl_mem_flags flags, int currentRows, int currentCols, int maxRows, int maxCols){
        squareAndNotScalar = false;
        scalar = false;
        if (currentRows == currentCols){ // it's square
            squareAndNotScalar = true;
            if ((maxRows == maxCols) && (maxRows == 1)){ // it's a scalar
                scalar = true;
                squareAndNotScalar = false;
            }
        }
        else if ((currentRows == maxRows) && (maxRows == 1)) { // it's a vector, not a 2D matrix
            squareAndNotScalar = false;
        }
        else { // this is not something we are equipped to handle.
            printf("ERROR! 'Blade' can only handle square matricies, 1D vectors, and scalars.\n");
            exit(5);
        }
        currentSize = currentRows*currentCols;
        maxSize = maxRows * maxCols;
        this->currentRows = currentRows;
        this->currentCols = currentCols;
        this->maxRows = maxRows;
        this->maxCols = maxCols;
        data = new T[maxSize](); // init zeroed with '()'
        zeros = new T[maxSize]();
        // OpenCL buffer creation
        this->clhelper = clhelper;
        memFlags = flags;
        printf("ro - %d... memFlags, scalar: %d, %d\n",CL_MEM_READ_ONLY, memFlags, scalar);
        if ((memFlags & CL_MEM_READ_ONLY) && scalar){ // then we don't need to create a buffer
            readOnly = true;
            printf("booyahkasha\n");
        } else {
            createCLBuffer();
        }
    }
    
    /* Simplified constructor for a vector (1D) */
    Blade(CLHelper* clhelper, cl_mem_flags flags, int currentCols, int maxCols) : Blade(clhelper, flags, 1, currentCols, 1, maxCols) {};
    
    /* Simplified constructor for a scalar */
    Blade(CLHelper* clhelper, cl_mem_flags flags) : Blade(clhelper, flags, 1, 1, 1, 1) {};
    
    ~Blade(){
        delete data;
        delete zeros;
    }
    
    /* returns a pointer to the data at row, col. nullptr if out of range. */
    T* getp(int row, int col){
        if (row < maxRows && col < maxCols){
            return &data[(currentCols*row)+col];
        }
        return nullptr;
    }
    
    /* only valid for vectors. returns a pointer to the data at col. nullptr if out of range */
    T* getp(int col){
        if (maxRows == 1 && col < currentCols){
            return &data[col];
        }
        return nullptr;
    }
    
    /* only valid for scalars. returns a pointer to the data*/
    T* getp(){
        if (scalar){
            return data;
        }
        return nullptr;
    }
    
    /* returns the data at row, col. NULL if out of range. */
    T getv(int row, int col){
        if (row < maxRows && col < maxCols){
            return data[(currentCols*row)+col];
        }
        return NULL;
    }
    
    /* only valid for vectors. returns the data at col. nullptr if out of range */
    T getv(int col){
        if (maxRows == 1 && col < currentCols){
            return data[col];
        }
        return nullptr;
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
    
    
    /* returns the new width if successful, -1 if not */
    int addEntry(){
        if (currentCols < maxCols){ // if not scalar, we always increase the cols, so this is enough of a check (scalar values won't pass this).
            if(squareAndNotScalar){ // increment cols and rows
                currentRows++;
                currentCols++;
                currentSize = currentRows * currentCols;
            }
            else { // increment cols
                currentCols++;
                currentSize = currentRows * currentCols;
            }
            return currentCols;
        }
        return -1;
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
    
};



#endif /* Blade_hpp */
