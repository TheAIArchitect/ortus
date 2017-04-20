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
#include "StandardVTKHeader.hpp"
#include "OrtusXCorrVisualizer.hpp"


class DiagnosticSteward {
public:
    
    void tempVTKPlot();
    ortus::vtkFullxcorr* morphToVTK();
    
    DiagnosticSteward(DataSteward* dStewiep);
    ~DiagnosticSteward();
    
    void runStandardDiagnostics();
    void runAdvancedDiagnostics();
    
    void plotXCorr();
    ortus::partialxcorr computeXCorrBetweenVoltages(const std::string* computeList = {}, int numToCompute = -1);
    
    ortus::fullxcorr fullXCorr;
    int stepsSinceLastXCorr = 0;
    int nextXCorrWindowNum = 0;
    
    DataSteward* dStewiep;
    DataVisualizer* vizzer;
    Statistician statistician;
};

#endif /* DiagnosticSteward_hpp */
