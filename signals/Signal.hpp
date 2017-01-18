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
#include <utility>

class Signal {
    
public:
    Signal(bool initialize);
    Signal(const Signal& s);
    Signal(Signal&& s);
    Signal& operator=(Signal s);
    void swap(Signal& to, Signal& from);
    Signal& operator=(Signal&& s);
    ~Signal();

    
    /* makes computing a signal easier 
     * startTime/endTime can be something like 0/100,
     * or something like .4/1.5,
     * in either case, a signal with 'length' indices will be generated */
    virtual void generateFullSignal(float startTime, float endTime, int length);
    
    //virtual float* getSignal();
    
    virtual void invert();
    
    /* this is actual time -- e.g., if the signal is a sine wave, 'time' will be the 'x' value used to compute the value.
     * This isn't the right function if you want to get an index from a precomputed signal. In that case, call getValueAtIndex(...) */
    virtual float getValueAtTime(float t);
    
    virtual float getValueAtIndex(int index);
    
protected:
    int signalLength;
    int signalDelay;
    float* signal; // can be a single value, or not...
    virtual void generateFullSignalDerived(float startTime, float increment, int length);
private:
    bool callDeleteOnSignal;
    static float ZEROF;
};

#endif /* Signal_hpp */
