//
//  KernelBuddy.cpp
//  ortus
//
//  Created by andrew on 3/29/17.
//  Copyright © 2017 Andrew W.E. McDonald. All rights reserved.
//

#include "KernelBuddy.hpp"

/** DEPRECATED */

KernelBuddy::KernelBuddy(CLHelper* clhelper, cl_kernel* kernelp){
    clHelper = clhelper;
    this->kernelp = kernelp;
}

