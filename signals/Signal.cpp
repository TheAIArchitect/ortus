//
//  Signal.cpp
//  ortus
//
//  Created by andrew on 1/7/17.
//  Copyright © 2017 Andrew W.E. McDonald. All rights reserved.
//

#include "Signal.hpp"

float Signal::ZEROF = 0.f;

/* move semantics: http://blog.smartbear.com/c-plus-plus/c11-tutorial-introducing-the-move-constructor-and-the-move-assignment-operator/ 
 *
 * --> apparently, not necessary to 'reset' old object, 
 * other than ensuring the pointer is set to null so that the destructor isn't called,
 * and memory that hasn't been allocated gets free'd.
 */

/* if initialize, make sure signal points to something. if not, don't. */
Signal::Signal(bool initialize){
    if (initialize){
        signal = &ZEROF;
        signalLength = 1;
    }
    callDeleteOnSignal = false;
}

Signal::Signal(const Signal& s){
    // we don't want to worry whether or not the old one had data or anything...
    // just create a new float array. it's safer this way.
    signal = new float[s.signalLength];
    callDeleteOnSignal = true;
    for (int i = 0; i < s.signalLength; ++i){
        signal[i] = s.signal[i];
    }
    signalLength = s.signalLength;
}

/* copy assignment operator
 * copy-and-swap idiom: http://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
 */
Signal& Signal::operator=(Signal s){
    swap(*this, s);
    return *this;
}

/* swap -- see link in assignment operator function's description */
void Signal::swap(Signal& to, Signal& from){
    std::swap(to.signal, from.signal);
    std::swap(to.signalLength, from.signalLength);
    std::swap(to.callDeleteOnSignal, from.callDeleteOnSignal);
}

/* move constructor */
Signal::Signal(Signal&& s){
    // first take from 's'
    signal = s.signal;
    signalLength = s.signalLength;
    callDeleteOnSignal = s.callDeleteOnSignal;
    // then reset s
    s.signal = NULL;
    s.signalLength = 0;
    s.callDeleteOnSignal = false;
}

/* move assignment operator...
 * http://stackoverflow.com/questions/5481539/what-does-t-double-ampersand-mean-in-c11
 
 */
Signal& Signal::operator=(Signal&& s){
    if (this != &s){
        // first release anything current object ('this') has..
        if (callDeleteOnSignal){
            delete[] signal;
        }
        signalLength = 0;
        callDeleteOnSignal = false;
        // then take from 's'
        signal = s.signal;
        signalLength = s.signalLength;
        callDeleteOnSignal = s.callDeleteOnSignal;
        // reset 's'
        s.signal = NULL;
        s.signalLength = 0;
        s.callDeleteOnSignal = false;
    }
    return *this;
}

Signal::~Signal(){
    if (callDeleteOnSignal){
        delete[] signal;
        callDeleteOnSignal = false;
    }
}

/* generates a signal, inclusive of startTime, but exclusive of endTime: [startTime, endTime), of specified length */
void Signal::generateFullSignal(float startTime, float endTime, int length){
    // to make this easier for the individual signals,
    // we compute the increment size:
    // => e.g. if start is .05 and end is .75, and length is 12,
    // then increment would be .0636
    // so, we would start at .05, and finish at .75, with 12 values (so, this is inclusive)
    signalLength = length;
    int numDivisions = signalLength - 1;
    float domain = (endTime-1) - startTime; // endTime not inclusive
    float increment = domain/((float)numDivisions);
    signal = new float[length];
    callDeleteOnSignal = true;
    generateFullSignalDerived(startTime, increment, length);
}

float Signal::getValueAtIndex(int index){
    if (index >= 0 && index < signalLength){
        return signal[index];
    }
    return 0.f;
}


void Signal::generateFullSignalDerived(float startTime, float increment, int length){}


float Signal::getValueAtTime(float t){
    return 0.f;
}

void Signal::invert(){
    for (int i = 0; i < signalLength; ++i){
        signal[i] = -1*signal[i];
    }
}
