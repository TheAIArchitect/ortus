//
//  Incubator.cpp
//  ortus
//
//  Created by andrew on 3/20/17.
//  Copyright © 2017 Andrew W.E. McDonald. All rights reserved.
//

#include "Incubator.hpp"

Incubator::Incubator(std::string ortFileName){
    
   odrVec = ortUtil.getOdr(ortFileName);
}

void Incubator::parseOdr(){
    
    for (auto line : odrVec){
        printf("%s\n",line.c_str());
    }
   
    
    
}
