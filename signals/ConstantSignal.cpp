//
//  ConstantSignal.cpp
//  ortus
//
//  Created by andrew on 1/8/17.
//  Copyright © 2017 Andrew W.E. McDonald. All rights reserved.
//

#include "ConstantSignal.hpp"

/* regarding using base class' constructors and assignment operators:
 * http://stackoverflow.com/questions/1226634/how-to-use-base-classs-constructors-and-assignment-operator-in-c
 *
 * I assumed that one could do the same thing with move constructors and operators...
 *
 * look into 'using' keyword: http://en.cppreference.com/w/cpp/language/using_declaration
 */

ConstantSignal::ConstantSignal(float magnitude, int startTime, int endTime, int length) : Signal(false){
    this->magnitude = magnitude;
    generateFullSignal(startTime, endTime, length);
}

/* copy constructor */
ConstantSignal::ConstantSignal(const ConstantSignal& cs) : Signal(cs) {
    magnitude = cs.magnitude;
}

/* copy assignment */
ConstantSignal& ConstantSignal::operator=(ConstantSignal cs){
    Signal::operator=(cs); // base copy assignment
    // then do derived classes stuff
    swap(*this, cs);
    return *this;
}

void ConstantSignal::swap(ConstantSignal& to, ConstantSignal& from){
    std::swap(to.magnitude, from.magnitude);
}

/* move constructor */
ConstantSignal::ConstantSignal(ConstantSignal&& cs) : Signal(cs){
    // after calling base class' move constructor:
    // take from 'cs'
    magnitude = cs.magnitude;
    // then reset 'cs'
    cs.magnitude = 0.f;
}

/* move assignment operator */
ConstantSignal& ConstantSignal::operator=(ConstantSignal&& cs){
    // first call base class' move operator:
    Signal::operator=(cs);
    // then, do the same operations, but for derived stuff
    if (this != &cs){
        // first clear 'this'
        magnitude = 0.f;
        // then take from 'cs'
        magnitude = cs.magnitude;
        // and finally reset 'cs'
        cs.magnitude = 0.f;
    }
    return *this;
}


ConstantSignal::~ConstantSignal(){
    // parent deletes signal
}

/* very simple (and unncessary) example of how to use this class */
void ConstantSignal::generateFullSignalDerived(float startTime, float increment, int length){
    for (int i = 0; i < length; i += increment){
        signal[i] = magnitude;
    }
}
