//
//  PolarityConfigurator.hpp
//  delegans
//
//  Created by onyx on 10/13/16.
//  Copyright © 2016 Andrew W.E. McDonald. All rights reserved.
//

#ifndef PolarityConfigurator_hpp
#define PolarityConfigurator_hpp

#include <stdio.h>
#include "Connection.hpp"
#include "ElementInfoModule.hpp"
#include <vector>
#include <unordered_map>

enum Polarity {EXCITATORY, INHIBITORY};

class PolarityConfigurator {
    
public:
    PolarityConfigurator();
    Polarity getPolarity(Connection* conn);
    float* makePolarityVector(std::vector<ElementInfoModule*>* bioElements, int num_rows);
    void initMap();
    bool setPolarityIfExists(std::string name,float** polVec, int index);
    
    std::unordered_map<std::string,int> polarityMap;
};

#endif /* PolarityConfigurator_hpp */
