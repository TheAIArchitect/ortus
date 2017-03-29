//
//  BladePack.hpp
//  ortus
//
//  Created by andrew on 3/27/17.
//  Copyright © 2017 Andrew W.E. McDonald. All rights reserved.
//

#ifndef BladePack_hpp
#define BladePack_hpp

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



#include "CLBuffer.hpp"
/** TEMP */
#include "CLBuddy.hpp"


template <class T>
class BladePack {
    
public:
    using blade_map = std::unordered_map< Attribute, Blade<T>*, AttributeHash>;
    
public:
    BladePack();
    
    void addBlade(Blade<T>* blade);
    
    //template <typename T>
    //blade_map bladeMap;
    
    blade_map bladeMap;
    
    

};


#endif /* BladePack_hpp */
