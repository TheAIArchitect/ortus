//
//  Steward.hpp
//  ortus
//
//  Created by andrew on 1/8/17.
//  Copyright © 2017 Andrew W.E. McDonald. All rights reserved.
//

#ifndef Steward_hpp
#define Steward_hpp

#include <stdio.h>
#include "DataSteward.hpp"
#include "ComputeSteward.hpp"
#include "DataVisualizer.hpp"

class Steward {
public:
    Steward();
    void initialize();
    
    void run();
    
    DataSteward* dataSteward;
    ComputeSteward* computeSteward;
    
    
    int numIterations;
    std::vector<std::vector<float>> kernel_voltages;
    
};

#endif /* Steward_hpp */
