//
//  Probe.hpp
//  delegans
//
//  Created by onyx on 10/21/16.
//  Copyright © 2016 Andrew W.E. McDonald. All rights reserved.
//

#ifndef Probe_hpp
#define Probe_hpp


#include "CLHelper.hpp"
#include <stdio.h>
#include <vector>
//#include "DataSteward.hpp"
#include "StrUtils.hpp"
#include <string>



class Probe {
private:
    std::string elementToProbe;
    
public:
    Probe();
    //Probe(DataSteward& ds);
    //DataSteward* stewie;
    static bool toggleProbe();
    static cl_int shouldProbe;
    static bool probeAll;
    static bool probeEnabled;
    static bool update;
    static std::string newElementToProbe;
    static bool newElementToProbeRequested;
    int numElemsMinusOne;
    
    
    
    std::string setNewElementToProbe();
    void printCurrentProbe(int window);
    void printAll(int window);
    std::vector<float**> csContribVec;
    std::vector<float**> gjContribVec;
    
    
    
};

#endif /* Probe_hpp */
