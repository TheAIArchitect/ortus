//
//  Statistician.hpp
//  ortus
//
//  Created by andrew on 1/23/17.
//  Copyright © 2017 Andrew W.E. McDonald. All rights reserved.
//

#ifndef Statistician_hpp
#define Statistician_hpp

#include <stdio.h>
#include <vector>
#include <cmath>
#include "Blade.hpp"
#include "OrtusNamespace.hpp"
#include <unordered_map>
#include "DataSteward.hpp"

class Statistician {
public:
    Statistician();
    //~Statistician();
    
    
    float xcorrMultiply(ortus::vector A, ortus::vector B, int windowNum);
    int computeXCorrNumWindows(int len);
    float xcorrMultiplyLimited(ortus::vector A, int aOffset, ortus::vector B, int bOffset, int len);
    //std::unordered_map<int, ortus::vectrix> computeXCorrBetweenVoltages(ortus::vectrix voltageHistoryVector, int numXCorrComputations, const std::string* computeList = {});
    std::unordered_map<int, ortus::vectrix> computeXCorrBetweenVoltages(DataSteward* dStewiep, const std::string* computeList = {}, int numToCompute = -1);
    ortus::vector xcorrLimited(ortus::vector A, ortus::vector B, float normalizer, int start, int len, int end);
    ortus::vector xcorr(ortus::vector A, ortus::vector B, float normalizer);
    ortus::vector normalizedXCorr(ortus::vector A, ortus::vector B);
};

#endif /* Statistician_hpp */
