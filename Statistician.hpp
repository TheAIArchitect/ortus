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
#include "OrtusStd.hpp"
#include <unordered_map>
#include <algorithm>




class Statistician {
public:
    Statistician();
    //~Statistician();
    
    static std::vector<std::vector<int>> getIndicesToLink(int numIndices, int resolution);
    
    float xcorrMultiply(ortus::vector A, ortus::vector B, int windowNum);
    int computeXCorrNumWindows(int len);
    float xcorrMultiplyLimited(ortus::vector A, int aOffset, ortus::vector B, int bOffset, int len);
    ortus::vector xcorrLimited(ortus::vector A, ortus::vector B, int start, int len);
    ortus::vector xcorr(ortus::vector A, ortus::vector B, float normalizer);
    ortus::vector normalizedXCorr(ortus::vector A, ortus::vector B);
};

#endif /* Statistician_hpp */
