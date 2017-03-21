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

/**
 * 
 * 1) S == sense, E == emotion, M == motor
 * 2) for each sense S:
 *      create 2 interneurons, one positive (P<sense>), and one negative (N<sense>)
 *      if S is desirable (good):
 *          S excites P<sense> and inhibits N<sense>
 *      if S is undesirable (bad):
 *          S inhibits P<sense> and excites N<sense>
 */
