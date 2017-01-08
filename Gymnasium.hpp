//
//  Gymnasium.hpp
//  ortus
//
//  Created by onyx on 12/20/16.
//  Copyright © 2016 Andrew W.E. McDonald. All rights reserved.
//

#ifndef Gymnasium_hpp
#define Gymnasium_hpp

#include <stdio.h>
#include "Blade.hpp"
#include <unordered_map>
#include <string>

class Gymnasium {
public:
    Gymnasium();
    void stimulateSensors(Blade<float>& voltageBlade, std::unordered_map<std::string,int>& nameToIndexMap);
};



#endif /* Gymnasium_hpp */
