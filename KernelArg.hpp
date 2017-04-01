//
//  KernelArg.hpp
//  ortus
//
//  Created by andrew on 3/30/17.
//  Copyright © 2017 Andrew W.E. McDonald. All rights reserved.
//

#ifndef KernelArg_hpp
#define KernelArg_hpp

#include <stdio.h>
#include "Blade.hpp"
#include "CLBuffer.hpp"
#include <unordered_map>
#include <vector>
#include "OrtusStd.hpp"

template<class T, class U>
class KernelArg {
public:
    CLBuffer<T>* clBufferp;
    CLHelper* clHelper;
    cl_kernel* kernelp;
    std::vector<U> theKeys;
    std::vector<size_t> byteOffsets;
    std::unordered_map<U,Blade<T>*> bladeMap;
    cl_uint kernelArgIndex;
    size_t bufferSize;
    int keyCount;
    
    // should have a kernelarg pointer that it uses to set its kernel arg id and list of offsets
    // so, there will be one kernel arg for offsets.. or maybe a subclass?
public:
    
    /** Use this constructor for multiple blades using the same CLBuffer */
    KernelArg(CLHelper* clhelper, cl_kernel* kernelp, cl_uint kernelArgIndex, std::vector<U> keyVec){
        clHelper = clhelper;
        this->kernelp = kernelp;
        this->kernelArgIndex = kernelArgIndex;
        setKeys(keyVec);
    }
    
    /** Use this constructor for a single Blade using a single CLBuffer */
    KernelArg(CLHelper* clhelper, cl_kernel* kernelp, cl_uint kernelArgIndex){
        clHelper = clhelper;
        this->kernelp = kernelp;
        this->kernelArgIndex = kernelArgIndex;
    }
    
    ~KernelArg(){
        for (auto blade : bladeMap){
            delete blade.second;
        }
        delete clBufferp;
    }
    
    
    void setKey(U u){//, bool storeAsInt = true){
        int potentialKey = static_cast<int>(u);
        if (potentialKey < 0){
            printf("Error: can't set a key in KernelArg with int value less than 0. These keys are used to access vector indices. Enumerations are fine, as long as they are greater than 0.");
        }
        //if (!storeAsInt){
            // we want to store the actual key, not an int
            theKeys.push_back(u);
        //}
        //else{
        //    theKeys.push_back(potentialKey);
        //}
        keyCount = theKeys.size();
    }
    
    void setKeys(std::vector<U> uVec){
        for (auto u : uVec){
            setKey(u);
        }
    }
    
    /* multiple Blades */
    
    /** 2D matrix **/
    std::unordered_map<U,Blade<T>*>* addKernelArgWithBufferAndBlades(size_t initialRowsPerKey, size_t initialColsPerKey, size_t maxRowsPerKey, size_t maxColsPerKey, cl_mem_flags memFlags){
        return addKernelArgWithBufferAndBlades(initialRowsPerKey, initialColsPerKey, 1, maxRowsPerKey, maxColsPerKey, 1, memFlags, false);
    }
    
    /** vector */
    std::unordered_map<U,Blade<T>*>* addKernelArgWithBufferAndBlades(size_t initialColsPerKey, size_t maxColsPerKey, cl_mem_flags memFlags){
        return addKernelArgWithBufferAndBlades(1, initialColsPerKey, 1, 1, maxColsPerKey, 1, memFlags, false);
    }
    
    /** scalar */
    std::unordered_map<U,Blade<T>*>* addKernelArgWithBufferAndBlades(cl_mem_flags memFlags){
        return addKernelArgWithBufferAndBlades(1, 1, 1, 1, 1, 1, memFlags, false);
    }
    
    /** creates the opencl buffer, and the map of Blade* that access it. use the non--plural form for a single blade.
     * 
     * simplified versions of this can be called, as seen above. */
    std::unordered_map<U,Blade<T>*>* addKernelArgWithBufferAndBlades(size_t initialRowsPerKey, size_t initialColsPerKey, size_t initialPagesPerKey, size_t maxRowsPerKey, size_t maxColsPerKey, size_t maxPagesPerKey, cl_mem_flags memFlags, bool isDeviceScratchpad = false){
        // create the buffer
        size_t byteOffsetMultiplier = createBuffer(maxRowsPerKey, maxColsPerKey, maxPagesPerKey, memFlags);
        // create the blades, and add the offset and buffer info to each one
        bladeMap.reserve(keyCount);
        byteOffsets.reserve(keyCount);
        byteOffsets[0] = 0; // no offset for first one
        for (int i = 0; i < keyCount; ++i){
            bladeMap[theKeys[i]] = new Blade<T>(clHelper, initialRowsPerKey, initialColsPerKey, initialPagesPerKey, maxRowsPerKey, maxColsPerKey, maxPagesPerKey, memFlags);
            if (i > 0){ // only add offset after first
                byteOffsets[i] = byteOffsets[i-1] + (byteOffsetMultiplier * sizeof(T));
            }
            bladeMap[theKeys[i]]->setCLBufferAndOffset(clBufferp, byteOffsets[i]);
        }
        return &bladeMap;
    }
    
    /** device scratch pad -- multiple (map of Blade* vs Blade*) */
    std::unordered_map<U,Blade<T>*>* addKernelArgWithBufferAndBlades(size_t initialRowsPerKey, size_t initialColsPerKey, size_t initialPagesPerKey, size_t maxRowsPerKey, size_t maxColsPerKey, size_t maxPagesPerKey, bool isDeviceScratchpad){
        // just as with the CLBuffer, when creating a device scratch pad, it is much easier to simply set the cl_mem_flag arg,
        // than to eliminate that need. It will be ignored by this class.
        if (!isDeviceScratchpad){
            printf("(KernelArg) Error: You may only create a KernelArg without specifying the 'cl_mem_flags memFlags' argument, if you are creating the KernelArg to be used as a device scratchpad.\n");
            exit(25);
        }
        return addKernelArgWithBufferAndBlades(initialRowsPerKey, initialColsPerKey, initialPagesPerKey, maxRowsPerKey, maxColsPerKey, maxPagesPerKey, CL_MEM_READ_WRITE, isDeviceScratchpad);
    }
    
    /* single Blades */
    
    
    Blade<T>* addKernelArgWithBufferAndBlade(size_t initialRows, size_t initialCols, size_t maxRows, size_t maxCols, cl_mem_flags memFlags){
        return  addKernelArgWithBufferAndBlade(initialRows, initialCols, 1, maxRows, maxCols, 1, memFlags, false);
    }
    
    Blade<T>* addKernelArgWithBufferAndBlade(size_t initialCols, size_t maxCols, cl_mem_flags memFlags){
        return  addKernelArgWithBufferAndBlade(1, initialCols, 1, 1, maxCols, 1, memFlags, false);
    }
    
    Blade<T>* addKernelArgWithBufferAndBlade(cl_mem_flags memFlags){
        return  addKernelArgWithBufferAndBlade(1, 1, 1, 1, 1, 1, memFlags, false);
    }
    
    
    /**
     * Does the same thing as 'addKernelArgWithBufferAndBlades', but only creates one buffer --
     * that means keys aren't necessary, and instead of returning 
     * an unordered_map*, it returns a Blade*.
     */
    Blade<T>* addKernelArgWithBufferAndBlade(size_t initialRows, size_t initialCols, size_t initialPages, size_t maxRows, size_t maxCols, size_t maxPages, cl_mem_flags memFlags, bool isDeviceScratchpad){
        size_t byteOffsetMultiplier = createBuffer(maxRows, maxCols, maxPages, memFlags);
        // create the blades, and add the offset and buffer info to each one
        Blade<T>* theBlade = new Blade<T>(clHelper, initialRows, initialCols, initialPages, maxRows, maxCols, maxPages, memFlags);
        int offsetSize = 0;
        theBlade->setCLBufferAndOffset(clBufferp, offsetSize);
        return theBlade;
    }
    
    /** device scratch pad -- single */
    Blade<T>* addKernelArgWithBufferAndBlade(size_t initialRows, size_t initialCols, size_t initialPages, size_t maxRows, size_t maxCols, size_t maxPages, bool isDeviceScratchpad){
        // just as with the CLBuffer, when creating a device scratch pad, it is much easier to simply set the cl_mem_flag arg,
        // than to eliminate that need. It will be ignored by this class.
        if (!isDeviceScratchpad){
            printf("(KernelArg) Error: You may only create a KernelArg without specifying the 'cl_mem_flags memFlags' argument, if you are creating the KernelArg to be used as a device scratchpad.\n");
            exit(25);
        }
        return addKernelArgWithBufferAndBlade(initialRows, initialCols, initialPages, maxRows, maxCols, maxPages, CL_MEM_READ_WRITE, isDeviceScratchpad);
    }
    
private:
    /** creates the CLBuffer, size = maxRows * maxCols * maxPages  -- ensures size > 0,
     * returns buffer size */
    size_t createBuffer(size_t maxRows, size_t maxCols, size_t maxPages, cl_mem_flags memFlags){
        size_t byteOffsetMultiplier = maxRows * maxCols * maxPages;
        if (byteOffsetMultiplier  < 1){
            printf("(KernelArg) Error: CLBuffer and Blade must both be at least 1 element in size.\n");
            exit(39);
        }
        // create the buffer
        clBufferp = new CLBuffer<T>(clHelper, byteOffsetMultiplier*theKeys.size(), memFlags);
        clBufferp->createBuffer();
        clBufferp->setCLArgIndex(kernelArgIndex, kernelp);
        return byteOffsetMultiplier;
    }
    

        
};


#endif /* KernelArg_hpp */
