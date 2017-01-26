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
    ortus::vectrix voltageHistoryVector = dStewiep->outputVoltageHistory->convertDataTo2DVector();
    ortus::vectrix xcorrResults = statistician.computeXCorrBetweenVoltages(voltageHistoryVector);
    
    
}




