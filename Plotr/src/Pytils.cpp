//
//  Pytils.cpp
//  Plotr
//
//  Created by andrew on 1/13/17.
//  Copyright © 2017 Ecodren Research. All rights reserved.
//

#include "Pytils.hpp"


bool Pytils::call(PyObject* pop){
    if (PyObject_CallFunctionObjArgs(pop, NULL)){
        return true;
    }
    return false;
}

bool Pytils::call(PyObject* pop, PyObject* argsp){
    if (PyObject_CallObject(pop, argsp)){
        return true;
    }
    return false;
}

bool Pytils::call(PyObject* pop, PyObject* argsp, PyObject* kwargsp){
    if (PyObject_Call(pop, argsp, kwargsp)){
        return true;
    }
    return false;
}


/* creates a tuple from tupleToBe, and puts everything form items into that tuple, ready to be passed as an 'argsp' in one of the above 'call' functions.*/
void Pytils::packTuple(PyObject** tupleToBe, std::initializer_list<PyObject*> items){
    size_t tupleSize = items.size();
    int index = 0;
    *tupleToBe = PyTuple_New(tupleSize);
    for (PyObject* item : items){
        PyTuple_SetItem(*tupleToBe, index, item);
        index++;
    }
}

void Pytils::packDict(PyObject** dictToBe, std::unordered_map<std::string, std::string> map){
    long tempLong;
    bool tempBool;
    std::string tempString;
    std::string theKey;
    *dictToBe = PyDict_New();// might need to call Py_INCREF(...)
    for (auto& mapIter : map){
        PyObject* itemObjectp;
        theKey = mapIter.first;
        tempString = mapIter.second;
        printf("<%s, %s>\n",theKey.c_str(), tempString.c_str());
        if (tryLongConversion(tempString, tempLong)){
            printf("\rkey: %s -- the long is: %ld\n",theKey.c_str(), tempLong);
            itemObjectp = PyInt_FromLong(tempLong);
        }
        else if (tryBoolConversion(tempString, tempBool)){
            printf("\rkey: %s -- the bool is: %d\n", theKey.c_str(), tempBool);
            itemObjectp = tempBool ? Py_True : Py_False;
        }
        else {
            printf("\rkey: %s -- the string is: %s\n",theKey.c_str(), tempString.c_str());
            itemObjectp = PyString_FromString(tempString.c_str());
        }
        PyDict_SetItemString(*dictToBe, mapIter.first.c_str(), itemObjectp);
    }
}
           

/* calls Py_DECREF for all inputs.
 *
 * note: PyObject_, PyNumber_, PySequence_, and PyMapping_,
 * all call Py_INCREF(). So, once *finished* (not before that)
 * with a result from functions that start with those,
 * just call Pytils::decref(...) on the result(s).
 */
void Pytils::decref(std::initializer_list<PyObject*> pops){
    for (PyObject* pop : pops){
        decref(pop);
    }
}



void Pytils::decref(PyObject* pop){
    if (pop){
        Py_DECREF(pop);
    }
    else {
        PyErr_Print();
    }
}

/*
 * http://stackoverflow.com/questions/4654636/how-to-determine-if-a-string-is-a-number-with-c
 */
bool Pytils::tryLongConversion(std::string& s, long& theLong){
    char* p;
    long converted = strtol(s.c_str(), &p, 10);
    if (!*p){
        theLong = converted;
        return true;
    }
    theLong = 0;
    return false;
}

bool Pytils::tryBoolConversion(std::string &s, bool& theBool){
    if (s == "True" || s == "true" || s == "TRUE"){
        theBool = true;
        return true;
    }
    else if (s == "False" || s == "false" || s == "FALSE"){
        theBool = false;
        return true;
    }
    return false;
}
