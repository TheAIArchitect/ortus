//
//  Blade.cpp
//  ortus
//
//  Created by Andrew McDonald on 1/4/17.
//  Copyright © 2017 Andrew W.E. McDonald. All rights reserved.
//

#include "Blade.hpp"



// look into making the float* private, but allowing 'getter' access to the pointer -- see if that will prevent changes without using a setter
/* Allows creation of a 1D array that allows accessing like a 2D array, and allows quick 'growth'.
 */
Blade::Blade(int currentRows, int currentCols, int maxRows, int maxCols){
    square = false;
    if (currentRows == currentCols){ // it's square
        square = true;
    }
    else if ((currentRows == maxRows) && (maxRows == 1)) { // it's a vector, not a 2D matrix 
        square = false; 
    }
    else { // this is not something we are equipped to handle.
        printf("ERROR! 'Blade' can only handle square matricies and 1D vectors.\n");
        exit(5);
    }
    currentSize = currentRows*currentCols;
    maxSize = maxRows * maxCols;
    this->currentRows = currentRows;
    this->currentCols = currentCols;
    this->maxRows = maxRows;
    this->maxCols = maxCols;
    data = new float[maxSize](); // init zeroed with '()'
    zeros = new float[maxSize]();
}

Blade::~Blade(){
    delete data;
    delete zeros;
}

/* returns the location of data at row, col */
float* Blade::getp(int row, int col){
    return &data[(currentCols*row)+col];
}

/* returns the value of data at row, col */
float Blade::getv(int row, int col){
    return data[(currentCols*row)+col];
}

/* updates the value of data at row, col */
bool Blade::set(int row, int col, float value){
    if (row < currentRows && col < currentCols){
        data[(currentCols*row)+col] = value;
        return true;
    }
    return false;
}


/* returns the new width if successful, -1 if not */
int Blade::addEntry(){
    if (currentCols < maxCols){ // we always increase the cols, so this is enough of a check.
        if(square){ // increment cols and rows
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
bool Blade::copyData(Blade& otherBlade){
    if (otherBlade.currentSize != currentSize)
        return false;
    for (int i = 0; i < currentSize; ++i){
        data[i] = otherBlade.data[i];
    }
    return true;
}

/* sets the openCL kernel argument index, to be used when calling clSetKernelArg */
void Blade::setCLArgIndex(int argIndex){
    clArgIndex = argIndex;
}

/* create the buffer with the maxSize (when we push the buffer, we'll only use currentSize) */
void Blade::createCLBuffer(CLHelperClass& clhelper, cl_mem_flags flags){
    this->clhelper = &clhelper;
    clData = clCreateBuffer(this->clhelper->context, flags, sizeof(float) * maxSize, NULL,&this->clhelper->err);
    this->clhelper->check_and_print_cl_err(this->clhelper->err);
}

/* moves data into clData, ready for use by the kernel */
void Blade::pushCLBuffer(){
    this->clhelper->err = clEnqueueWriteBuffer(this->clhelper->commands, clData, CL_TRUE, 0, sizeof(float) * currentSize, data, 0, NULL, NULL);
    this->clhelper->check_and_print_cl_err(this->clhelper->err);
}

void Blade::clearCLBuffer(){
    this->clhelper->err = clEnqueueWriteBuffer(this->clhelper->commands, clData, CL_TRUE, 0, sizeof(float) * maxSize, zeros, 0, NULL, NULL);
    this->clhelper->check_and_print_cl_err(this->clhelper->err);
}
