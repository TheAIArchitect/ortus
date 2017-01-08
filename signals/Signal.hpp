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
    Signal(bool initialize);
    ~Signal();
    bool callDeleteOnSignal;
    static float ZEROF;
    
    float* signal; // can be a single value, or not...
    int signalLength;
    
    /* makes computing a signal easier 
     * startTime/endTime can be something like 0/100, 
     * or something like .4/1.5,
     * in either case, a signal with 'length' indices will be generated */
    virtual void generateFullSignal(float startTime, float endTime, int length);
    
    virtual float* getFullSignal() = 0;
    /* should just multiply signal by -1 */
    virtual float* getFullInvertedSignal() = 0;
    
    /* this is actual time -- e.g., if the signal is a sine wave, 'time' will be the 'x' value used to compute the value.
     * This isn't the right function if you want to get an index from a precomputed signal. In that case, call getValueAtIndex(...) */
    virtual float getValueAtTime(float t) = 0;
    
    virtual float getValueAtIndex(int index);
    
protected:
    
    virtual void generateFullSignalDerived(float startTime, float increment, int length);
};

#endif /* Signal_hpp */
