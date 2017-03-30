//
//  KernelBuddy.hpp
//  ortus
//
//  Created by andrew on 3/29/17.
//  Copyright © 2017 Andrew W.E. McDonald. All rights reserved.
//

#ifndef KernelBuddy_hpp
#define KernelBuddy_hpp

#include <stdio.h>
#include "CLHelper.hpp"
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#include <Blade.hpp>
#include "Attribute.hpp"
#include <unordered_map>
#include <string>
#include <unordered_map>
#include "OrtusNamespace.hpp"
#include "CLBuffer.hpp"
#include "KernelArg.hpp"

class KernelBuddy {
    public:
        KernelBuddy(CLHelper* clhelper, cl_kernel* kernelp);
    
        CLHelper* clHelper;
        cl_kernel* kernelp;
    

        /** This creates a 'new' KernelArg object,
         * creates the OpenCL buffer associated with it,
         * and returns a mapping between the input keyVec's elements (keys),
         * and the Blades that 'wrap' the individual portions of the CLBuffer.
         *
         * NOTE: we lose the reference to the KernelArg, which could perhaps be prevented by storing a reference in each Blade, and a flag to delete the KernelArg in the first blade, which gets triggered once it's been deleted from the bladeMap, or something..
         *
         * OR, just ignore that, and let the OS clean it up becuase we need these KernelArgs around until the program ends anyway.
         */
        template <class T, class U>
        std::unordered_map< U, Blade<T>* >* addKernelArgAndBlades(cl_uint kernelArgNum, std::vector<U>& keyVec, int dimensions, size_t numElements, size_t maxElements, cl_mem_flags memFlags){
            KernelArg<T, U>* kArg = new KernelArg<T, U>(clHelper, kernelp, kernelArgNum);
            kArg->setKeys(keyVec);
            return kArg->createBufferAndBlades(dimensions, numElements, maxElements, memFlags);
        }
};

#endif /* KernelBuddy_hpp */
