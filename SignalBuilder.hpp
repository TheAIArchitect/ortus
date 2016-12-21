//
//  SignalBuilder.hpp
//  App_ExampleBrowser
//
//  Created by onyx on 3/3/16.
//
//

#pragma once

#include <stdio.h>
#include <vector>
#include <cmath>
#include "BodyParameterSet.hpp"

class SignalBuilder {
    
    
public:
    SignalBuilder();
    SignalBuilder(BodyParameterSet* paramSet);
    void init(BodyParameterSet* paramSet);
    void buildSignal();
    void initSignal();
    void wreckSignal();
    std::vector<float> buildSingleMuscleContraction();
    float getForceMultiplier(int section);
    BodyParameterSet* paramSet;
    std::vector<std::vector<std::vector<float>>> signal;
    int signal_index;
    std::vector<std::vector<float>>* getNextWindow();
    bool first;
    static int numWindows;

};