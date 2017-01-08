//
//  ConstantSignal.hpp
//  ortus
//
//  Created by andrew on 1/8/17.
//  Copyright © 2017 Andrew W.E. McDonald. All rights reserved.
//

#ifndef ConstantSignal_hpp
#define ConstantSignal_hpp

#include <stdio.h>
#include "Signal.hpp"

class ConstantSignal : public Signal {
public:
    ConstantSignal(int magnitude, int startTime, int endTime, int length);
    int magnitude;
    ~ConstantSignal();
    void generateFullSignalDerived(float startTime, float endTime, int length);
};

#endif /* ConstantSignal_hpp */
