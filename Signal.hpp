//
//  Signal.hpp
//  ortus
//
//  Created by andrew on 1/7/17.
//  Copyright © 2017 Andrew W.E. McDonald. All rights reserved.
//

#ifndef Signal_hpp
#define Signal_hpp

#include <stdio.h>

class Signal {
    
public:
    Signal();
    float* getFullSignal(float startTime, float endTime, int numValues);
    float* getFullInvertedSignal();
    float getValueAtTime(float t);
    
    
};

#endif /* Signal_hpp */
