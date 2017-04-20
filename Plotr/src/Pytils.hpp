//
//  Pytils.hpp
//  Plotr
//
//  Created by andrew on 1/13/17.
//  Copyright © 2017 Ecodren Research. All rights reserved.
//

#ifndef Pytils_hpp
#define Pytils_hpp

#include <stdio.h>
#include <initializer_list>
#include <string>
#include <unordered_map>
#include <python2.7/Python.h>

class Pytils {
    
public:
    
    
    static bool call(PyObject*);
    /* NOTE: argsp must be a tuple */
    static bool call(PyObject* pop, PyObject* argsp);
    /* NOTE: argsp must be a tuple */
    static bool call(PyObject* pop, PyObject* argsp, PyObject* kwargsp);
    
    static void packTuple(PyObject** tupleToBe, std::initializer_list<PyObject*> items);
    static void packDict(PyObject** dictToBe, std::unordered_map<std::string, std::string> map);
    
    static void decref(std::initializer_list<PyObject*> pops);
    static void decref(PyObject* pop);
    
    static bool tryLongConversion(std::string& s, long& theLong);
    static bool tryBoolConversion(std::string& s, bool& theBool);
    
    
};

#endif /* Pytils_hpp */
