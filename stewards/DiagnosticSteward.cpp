//
//  DiagnosticSteward.cpp
//  ortus
//
//  Created by andrew on 1/23/17.
//  Copyright © 2017 Andrew W.E. McDonald. All rights reserved.
//

#include "DiagnosticSteward.hpp"

DiagnosticSteward::DiagnosticSteward(DataSteward* dStewiep){
    this->dStewiep = dStewiep;
    vizzer = new DataVisualizer(this->dStewiep);
}

DiagnosticSteward::~DiagnosticSteward(){
    delete vizzer;
}

// standard information that we always want (either printed to console, or graphed)
void DiagnosticSteward::runStandardDiagnostics(){
    //vizzer.plotAll();
    vizzer->makePlots();
    //vizzer.plotSet(plotSet, plotSetSize);
    
}

// more complex stuff like analyzing xcorr, rate of change, etc.
void DiagnosticSteward::runAdvancedDiagnostics(){
    if (stepsSinceLastXCorr >= dStewiep->XCORR_COMPUTATIONS){
        std::unordered_map<int, ortus::vectrix> xcorrResults = statistician.computeXCorrBetweenVoltages(dStewiep);
    }
    // add the computation of xcorr every XCORR_COMPUTATIONS times to the map of fullXCorrResults... we get a window of 4 xcorr values each time we run it, for each element, and since the voltages get pushed out one by one, on each run, we only need to run the xcorr once ever 4 calls (for graphing purposes... the kernel must do it on every iteration for weight adjustmenet purposes).
    
    
}




