//
//  Incubator.hpp
//  ortus
//
//  Created by andrew on 3/20/17.
//  Copyright © 2017 Andrew W.E. McDonald. All rights reserved.
//

#ifndef Incubator_hpp
#define Incubator_hpp

#include <stdio.h>
#include <string>
#include <vector>
#include "Blade.hpp"
#include "Connectome.hpp"


class Incubator {
public:
    Incubator();
    ~Incubator();
    
    void conceive(std::string ortFileName);
    
    Connectome* ctome;
    bool ctomeNewed = false;
};




#endif /* Incubator_hpp */
