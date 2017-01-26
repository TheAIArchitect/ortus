//
//  DiagnosticSteward.hpp
//  ortus
//
//  Created by andrew on 1/23/17.
//  Copyright © 2017 Andrew W.E. McDonald. All rights reserved.
//

#ifndef DiagnosticSteward_hpp
#define DiagnosticSteward_hpp

#include <stdio.h>
#include "DataSteward.hpp"
#include "DataVisualizer.hpp"
#include "Statistician.hpp"



class DiagnosticSteward {
public:
    DiagnosticSteward(DataSteward* dStewiep);
    ~DiagnosticSteward();
    
    void runStandardDiagnostics();
    void runAdvancedDiagnostics();
    
    
    
    DataSteward* dStewiep;
    DataVisualizer* vizzer;
    Statistician statistician;
};

#endif /* DiagnosticSteward_hpp */
