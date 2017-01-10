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
    ConstantSignal(float magnitude, int startTime, int endTime, int length);
    ConstantSignal(const ConstantSignal& cs);
    ConstantSignal(ConstantSignal&& cs);
    ConstantSignal& operator=(ConstantSignal cs);
    void swap(ConstantSignal& to, ConstantSignal& from);
    ConstantSignal& operator=(ConstantSignal&& cs);
    float magnitude;
    ~ConstantSignal();
protected:
    virtual void generateFullSignalDerived(float startTime, float increment, int length);
};

#endif /* ConstantSignal_hpp */
