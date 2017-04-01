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

#include <vector>
#include "CLBuffer.hpp"


template<class T>
class Blade {
    
public:
    bool readOnly;
    bool deviceScratchPad;
    bool clMemAllocated;
    bool deleteDataBuffers;
    bool dataPushed;
    bool dataRead;
    int currentRows;
    int currentCols;
    int currentPages;
    int maxRows;
    int maxCols;
    int maxPages;
    int pageSize; // currentRows * currentCols  -- this is multiplied by 'page' when accessing a 3D index in a Blade, to get the right offset for the page being requested.
    int dimensions;
    cl_mem_flags memFlags;
    size_t currentSize;
    size_t maxSize;
    T* data;
    T* zeros;
    cl_mem* clDatap;
    size_t clBufferOffset;
    cl_uint clReadOnlyScalarArgIndex;// ONLY USED IF NO BUFFER IS USED/CREATED!!! (so, if Blade is being used for a read-only scalar)
    CLHelper* clhelper;
    
public:
    
    /* NOTE: row access starts from 0, but the number of rows must be at least 1 */
    
    /* Allows creation of a 1D array that allows accessing like a 2D array, and allows quick 'growth'.
     */
    Blade(CLHelper* clhelper, int currentRows, int currentCols, int currentPages, int maxRows, int maxCols, int maxPages, cl_mem_flags flags, bool isDeviceScratchPad){
        if ((currentRows < 1 ) || (maxRows < 1 ) || (currentCols < 1 ) || (maxCols < 1 ) || (currentPages < 1 ) || (maxPages < 1 )){
            printf("Error: Cannot create a blade with less than 1 current or max parameter.\n");
            exit(24);
        }
        else if ((currentRows < maxRows) || (currentCols < maxCols) || (currentPages < maxPages)){
            printf("Error: Cannot create a blade with current rows, columns, or pages greater than their respective max parameters.\n");
            exit(25);
        }
        dataPushed = false;
        dataRead = false;
        deviceScratchPad = false;
        clMemAllocated = false;
        deleteDataBuffers = false;
        dimensions = -1; // if 
        if ((maxCols == maxRows) && (maxPages == maxRows) && (maxRows == 1)){ // it's a scalar
            dimensions = 0; // we'll say a scalar has 0 dimensions... like a point.
        }
        else if ((currentRows == maxRows) && (maxPages == maxRows) && (maxRows == 1)) { // it's a vector
            dimensions = 1;
        }
        else if ((currentPages == maxPages) && (maxPages == 1)){ // it's a 2D matrix
            dimensions = 2;
        }
        else { // it's a 3D matrix
            dimensions = 3;
        }
        this->currentRows = currentRows;
        this->currentCols = currentCols;
        this->currentPages = currentPages;
        this->maxRows = maxRows;
        this->maxCols = maxCols;
        this->maxPages = maxPages;
        pageSize = currentRows * currentCols;
        updateCurrentSize(); // also updates pageSize
        maxSize = maxRows * maxCols * maxPages;
        data = new T[maxSize](); // init zeroed with '()'
        zeros = new T[maxSize]();
        deleteDataBuffers = true;
        this->clhelper = clhelper;
        memFlags = flags;
        // OpenCL buffer creation
        if ((memFlags & CL_MEM_READ_ONLY) && (dimensions == 0)){ // then we don't need to create a buffer
            readOnly = true;
        }
        else if (memFlags == NULL){ // then it's a device scratch pad, and we don't create a buffer, nor do we push one.
            deviceScratchPad = true;
        }
    }
    
    /* 'normal' constructor  -- don't have to specify isDeviceScratchPad as false */
    Blade(CLHelper* clhelper, int currentRows, int currentCols, int currentPages, int maxRows, int maxCols, int maxPages, cl_mem_flags flags) : Blade(clhelper, currentRows, currentCols, 1, maxRows, maxCols, 1, flags, false) {};
    
    
    /* Simplified constructor for a 2D matrix */
    Blade(CLHelper* clhelper, int currentRows, int currentCols, int maxRows, int maxCols, cl_mem_flags flags) : Blade(clhelper, currentRows, currentCols, 1, maxRows, maxCols, 1, flags, false) {};
    
    /* Simplified constructor for a vector (1D) */
    Blade(CLHelper* clhelper, int currentCols, int maxCols, cl_mem_flags flags) : Blade(clhelper, 1, currentCols, 1, 1, maxCols, 1, flags, false) {};
    
    /* Simplified constructor for a scalar */
    Blade(CLHelper* clhelper, cl_mem_flags flags) : Blade(clhelper, 1, 1, 1, 1, flags, false) {};
    
    /* Create a Blade this way if you only want to use it as a device scratch pad. No buffer gets created, nothing gets pushed. You just need to set the kernel arg.
     * just as with the CLBuffer, when creating a device scratch pad, it is much easier to simply set the cl_mem_flag arg,
     * than to eliminate that need. It will be ignored by this class.
     *
     * this function basically just makes it easier to create a device scratch pad, by lessening the confusion regarding the cl_mem_flags argument,
     * and minimizing the confusion (e.g., can't accidentally create a device scratch pad). Perhaps in the future a more streamlined approach will be taken.
     *
     * IMORTANT NOTE: The kernel parameter MUST be declared with the __local qualifier  (in the kernel definition) for this to work.  If it isn't, things might not work as you wish...
     */
    Blade(CLHelper* clhelper, int currentRows, int currentCols, int currentPages, int maxRows, int maxCols, int maxPages, bool isDeviceScratchpad) : Blade(clhelper, currentRows, currentCols, currentPages, maxRows, maxCols, maxPages, CL_MEM_READ_WRITE, isDeviceScratchpad) {
           if (!isDeviceScratchpad){
            printf("(Blade) Error: You may only create a Blade without specifying the 'cl_mem_flags memFlags' argument, if you are creating the Blade to be used as a device scratchpad.\n");
            exit(25);
        }
    }
    
    
    ~Blade(){
        if (deleteDataBuffers){
            delete[] data;
            delete[] zeros;
            deleteDataBuffers = false;
        }
        /*if (clMemAllocated){
            clReleaseMemObject(clData);
            clMemAllocated = false;
        }*/
    }
    
    
    /* only valid for 3D matricies. returns a pointer to the data at row, col. NULL if out of range. */
    T* getp(int row, int col, int page){
        if (row < maxRows && col < maxCols && page < maxPages){
            return &data[(page*(pageSize))+(currentCols*row)+col];
        }
        return NULL;
    }
    
    /* only valid for 2D matricies. returns a pointer to the data at row, col. NULL if out of range. */
    T* getp(int row, int col){
        if (row < maxRows && col < maxCols && dimensions == 2){
            return &data[(currentCols*row)+col];
        }
        return NULL;
    }
    
    /* only valid for vectors. returns a pointer to the data at col. NULL if out of range */
    T* getp(int col){
        if (dimensions == 1 && col < currentCols){
            return &data[col];
        }
        return NULL;
    }
    
    /* only valid for scalars. returns a pointer to the data*/
    T* getp(){
        if (dimensions == 0){
            return data;
        }
        return NULL;
    }
    
    /* only valid for 3D matricies. returns the data at row, col. NULL if out of range. */
    T getv(int row, int col, int page){
        if (row < maxRows && col < maxCols && page < maxPages){
            return data[(page*(pageSize))+(currentCols*row)+col];
        }
        return NULL;
    }
    
    /* only valid for 2D matricies. returns the data at row, col. NULL if out of range. */
    T getv(int row, int col){
        if (row < maxRows && col < maxCols && dimensions == 2){
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
        if (dimensions == 0){
            return *data;
        }
        return NULL;
    }
    
    
    /* only valid for 2D matricies. updates the value of data at row, col. This function won't stop you from trying to access a negative index.
     * Note that row access starts from 0 -- the first row, is row 0 (same for columns).*/
    bool set(int row, int col, int page, T value){
        if (row < currentRows && col < currentCols && page < currentPages){
            data[(page*(pageSize))+(currentCols*row)+col] = value;
            return true;
        }
        return false;
    }
    
    
    /* only valid for 2D matricies. updates the value of data at row, col. This function won't stop you from trying to access a negative index.
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
        if (dimensions == 0){
            data[0] = value;
            return true;
        }
        return false;
    }
    
    /* only valid for a vector. adds 'value' to the data contained at location 'col'. will return false if not a 1D vector */
    bool add(int col, T value){
        if (dimensions == 1 && col < currentCols){
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
    
    /* adds a page, if currentPages < maxPages, and returns new page count (will be unchanged if check fails) */
    int addPage(){
        if(currentPages < maxPages){
            currentPages++;
        }
        updateCurrentSize();
        return currentPages;
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
    
    
    /** check to see if this is necessary! (see: CLBuffer, KernelArg) */
    /* sets the openCL kernel argument index, to be used when calling clSetKernelArg
     * NOTE: NOT FOR THE 'normal'  USE CASE WHERE WE NEED TO PUSH DATA -- IN THAT CASE, USE 'CLBuffer' to push the buffer*/
    void setCLReadOnlyScalarArgIndex(cl_uint readOnlyScalarArgIndex, cl_kernel* kernel){
        clReadOnlyScalarArgIndex = readOnlyScalarArgIndex;
        if ((dimensions == 0) && readOnly){ // then we don't need a buffer, and just send the data over (which is already a pointer... don't send a reference to it.
            this->clHelper->err = clSetKernelArg(*kernel, clReadOnlyScalarArgIndex, sizeof(T), data);
        }
        else{
            printf("Error: Attempting to set kernel argument from Blade, however Blade is not a read-only scalar. This action *must* be completed using the 'CLBuffer'.\n");
            exit(43);
        }
        /* NOTE: BELOW CASE (the general use case) *MUST* BE TAKEN CARE OF IN 'CLBuffer'!!!
         * this->clHelper->err = clSetKernelArg(*kernel, clArgIndex, sizeof(cl_mem), &clData);
        else if (deviceScratchPad){
            this->clHelper->err = clSetKernelArg(*kernel, clArgIndex, currentSize*sizeof(T), NULL);// note, we don't set an address for the arg, BUT, we tell the kernel how big the __local buffer needs to be!
        }
        else {
        
        */
        this->clHelper->check_and_print_cl_err(this->clHelper->err);
    }
    
    /*
     * NOTE: we need to use the buffer wrapper so we can use one buffer for multiple blades if we like*/
    void setCLBufferAndOffset(CLBuffer<T>* clBuffer, size_t bufferOffset){
        clDatap = &clBuffer->clData;
        clBufferOffset = bufferOffset;
    }
    
    /* moves data into the buffer pointed to by 'clDatap', ready for use by the kernel starting at 'clBufferOffset' bytes (set when buffer is set with 'setCLBuffer()') */
    void pushDataToDevice(){
        this->clhelper->err = clEnqueueWriteBuffer(this->clhelper->commands, *clDatap, CL_TRUE, clBufferOffset, sizeof(T) * currentSize, data, 0, NULL, NULL);
        this->clhelper->check_and_print_cl_err(this->clhelper->err);
        dataPushed = true;
        dataRead = false; // we know current data is invalid
    }
    
    /* reads data from the OpenCL buffer into data, starting at 'bufferOffset' bytes (set when buffer is set with 'setCLBuffer()'). This will overwrite anything in data.
     * NOTE: if the cl_mem_flags variable set during buffer creation does not permit writing (for OpenCL), nothing will be read, and readCLBuffer will return false. */
    bool readDataFromDevice(){
        if (!((memFlags & CL_MEM_READ_WRITE) || (memFlags & CL_MEM_WRITE_ONLY))){
            return false;
        }
        this->clhelper->err = clEnqueueReadBuffer(this->clhelper->commands, *clDatap, CL_TRUE, clBufferOffset, sizeof(T) * currentSize, data, 0, NULL, NULL);
        this->clhelper->check_and_print_cl_err(this->clhelper->err);
        dataPushed = false;
        dataRead = true;
        return true;
    }
    
    /* writes zeros to the OpenCL buffer, to ensure it is empty
     * NOTE: if this is a 'shared buffer', it only clears this Blade's part (so, it clears from 'bufferOfffset' to 'maxSize' */
    void clearCLBuffer(){
        this->clhelper->err = clEnqueueWriteBuffer(this->clhelper->commands, *clDatap, CL_TRUE, clBufferOffset, sizeof(T) * maxSize, zeros, 0, NULL, NULL);
        this->clhelper->check_and_print_cl_err(this->clhelper->err);
    }
    
    
    /* FIXME FOR 3D */
    std::vector<std::vector<T>> convertDataTo2DVector(){
        if (dimensions == 0){
            printf("Blade Warning: Placing scalar into vector of vector. Are you sure you want to do this?\n");
        }
        else if (dimensions == 1){
            printf("Blade Warning: Placing vector into vector of vector. Are you sure you want to do this?\n");
        }
        else if (dimensions > 2){
            printf("Blade Warning: Placing 3D matrix into vector of vector. Are you sure you want to do this?\n");
        }
        std::vector<std::vector<T>> output;
        for (int i = 0; i < currentRows; ++i){
            std::vector<T> tempRow;
            for (int j = 0; j < currentCols; ++j){
                tempRow.push_back(getv(i, j));
            }
            output.push_back(tempRow);
        }
        return output;
    }
    
    /* FIXME FOR 3D */
    std::vector<T> convertDataToVector(){
        if (dimensions == 0){
            printf("Blade Warning: Placing scalar into vector. Are you sure you want to do this?\n");
        }
        if (dimensions == 2){
            printf("Blade Error: Attempting to place 2D Data into vector.\n");
        }
        std::vector<T> output;
        for (int i = 0; i < currentCols; ++i){
            output.push_back(getv(i));
        }
        return output;
    }
    
    
    
private:
    
    void updateCurrentSize(){
        currentSize = currentRows * currentCols * currentPages;
        pageSize = currentRows * currentCols;
    }
    
};



#endif /* Blade_hpp */
