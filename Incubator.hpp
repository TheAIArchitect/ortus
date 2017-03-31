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
    Incubator(Connectome* ctomep);
    ~Incubator();
    
    Connectome* ctomep;
};




#endif /* Incubator_hpp */
