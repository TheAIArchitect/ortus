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


class Statistician {
public:
    Statistician();
    //~Statistician();
    
    
    float xcorrMultiply(ortus::vector A, ortus::vector B, int windowNum);
    int computeXCorrNumWindows(int len);
    float xcorrMultiplyLimited(ortus::vector A, int aOffset, ortus::vector B, int bOffset, int len);
    ortus::vector xcorrLimited(ortus::vector A, ortus::vector B, int start, int len);
    ortus::vector xcorr(ortus::vector A, ortus::vector B, float normalizer);
    ortus::vector normalizedXCorr(ortus::vector A, ortus::vector B);
};

#endif /* Statistician_hpp */
