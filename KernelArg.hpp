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
    }
    
    void setKeys(std::vector<U> uVec){
        for (auto u : uVec){
            setKey(u);
        }
        keyCount = theKeys.size();
    }
    
    /**
     * dimensions:
     *  # 0 - scalar
     *  # 1 - vector
     *  # 2 - "2D" array
     */
    std::unordered_map<U,Blade<T>*>* createBufferAndBlades(int dimensions, size_t currentEntriesPerKey, size_t maxEntriesPerKey, cl_mem_flags memFlags){
        if (dimensions < 0 || dimensions > 2){
            printf("Error: Blade dimensions may only be 0, 1, or 2.\n");
            exit(39);
        }
        // assume it's a scalar or vector (current and max entries per key will be 1 if scalar, and if they aren't, it's at least a vector, and the current and max entries passed to the function will always go to columns)
        int currentRows, currentCols, maxRows, maxCols;
        currentRows = 1;
        currentCols = currentEntriesPerKey;
        maxRows = 1;
        maxCols = maxEntriesPerKey;
        size_t byteOffsetMultiplier = 1; // 1 if scalar, maxEntriesPerKey if vector, and maxEntriesPerKey^2 if 2d array
        // we don't need to check dimensions for Blade creation, because we can make all types of Blades using the most 'verbose' Blade constructor
        if (dimensions == 1){ // vector
            byteOffsetMultiplier = maxEntriesPerKey;
        }
        else if (dimensions == 2){ // 2d array
            byteOffsetMultiplier = maxEntriesPerKey * maxEntriesPerKey;
            currentRows = currentEntriesPerKey;
            maxRows = maxEntriesPerKey;
        }
        // create the buffer
        clBufferp = new CLBuffer<T>(clHelper, byteOffsetMultiplier*theKeys.size(), memFlags);
        clBufferp->createBuffer();
        clBufferp->setCLArgIndex(kernelArgIndex, kernelp);
        // create the blades, and add the offset and buffer info to each one
        bladeMap.reserve(keyCount);
        byteOffsets.reserve(keyCount);
        byteOffsets[0] = 0; // no offset for first one
        for (int i = 0; i < keyCount; ++i){
            bladeMap[theKeys[i]] = new Blade<T>(clHelper, memFlags, currentRows, currentCols, maxRows, maxCols);
            if (i > 0){ // only add offset after first
                byteOffsets[i] = byteOffsets[i-1] + (byteOffsetMultiplier * sizeof(T));
            }
            bladeMap[theKeys[i]]->setCLBufferAndOffset(clBufferp, byteOffsets[i]);
        }
        return &bladeMap;
    }
    

        
};


#endif /* KernelArg_hpp */
