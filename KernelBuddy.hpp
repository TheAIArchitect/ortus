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
#include <unordered_map>
#include <string>
#include <unordered_map>
#include "CLBuffer.hpp"
#include "KernelArg.hpp"


/** DEPRECATED */

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
            return kArg->createBufferAndBlades(numElements, maxElements, memFlags);
        }
    
        /**
         * This is the same as addKernelArgAndBlades(),
         * but creates a CLBuffer big enough for a single Blade,
         * and returns a pointer to that Blade
         * (rather than an unordered_map* of Blade*s)
         */
        template <class T>
        Blade<T>* addKernelArgAndBlade(cl_uint kernelArgNum, size_t numElements, size_t maxElements, cl_mem_flags memFlags){
            // we'll set 'int' as KernelArg's second template parameter,
            // just so that KernelArg can stay a template class...
            // in reality, the int does nothing (other than allow the code to compile/run).
            KernelArg<T,int>* kArg = new KernelArg<T,int>(clHelper, kernelp, kernelArgNum);
            // don't set any keys, because we only create a single Blade,
            // and assign it to a single CLBuffer
            return kArg->createBufferAndBlade(numElements, maxElements, memFlags);
        }
    
};

#endif /* KernelBuddy_hpp */
