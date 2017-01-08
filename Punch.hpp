//
//  Punch.hpp
//  ortus
//
//  Created by andrew on 1/8/17.
//  Copyright © 2017 Andrew W.E. McDonald. All rights reserved.
//

#ifndef Punch_hpp
#define Punch_hpp

#include <stdio.h>
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif


template <class T>
class Punch {
    
};

template <>
class Punch <cl_uint>{
    
};

#endif /* Punch_hpp */
