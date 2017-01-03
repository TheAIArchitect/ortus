//
//  DataVisualizer.cpp
//  delegans
//
//  Created by onyx on 11/4/16.
//  Copyright © 2016 Andrew W.E. McDonald. All rights reserved.
//

#include "DataVisualizer.hpp"

DataVisualizer::DataVisualizer(DataSteward* stewie){
    this->stewie = stewie;
}


void DataVisualizer::plotAll(){

    int numWindows = stewie->kernelVoltages.size();
    std::vector<double> xVals;// windows
    std::vector<double> yVals;// neuron voltages
    for ( int i = 0; i < numWindows; ++i){
        xVals.push_back(i);//window number
    }
    
    Plotr plt;
    std::map<std::string, std::string> kwArgs;
    for (int i = 0; i < DataSteward::NUM_ELEMS; ++i){
        kwArgs["label"] = stewie->bioElements[i]->name;
        yVals.clear();
        for (int j = 0; j < numWindows; ++j){
            yVals.push_back(stewie->kernelVoltages[j][i]);
        }
        plt.plot(xVals, yVals, kwArgs);
        
    }
    plt.legend();
    plt.show();
    
    
}

void DataVisualizer::plotSet(std::string* names, int numNames){

    
    int max = stewie->kernelVoltages.size();
    std::vector<double> xVals;// windows
    std::vector<double> yVals;// neuron voltages
    for ( int i = 0; i < max; ++i){
        xVals.push_back(i);//window number
        
    }
    
    Plotr plt;
    std::map<std::string, std::string> kwArgs;
    for (int i = 0; i < numNames; ++i){
        int idx = stewie->n2idxMap[names[i]];
        kwArgs["label"] = names[i]; 
        yVals.clear();
        for (int j = 0; j < max; ++j){
            yVals.push_back(stewie->kernelVoltages[j][idx]);
        }
        plt.plot(xVals, yVals, kwArgs);
        
    }
    plt.legend();
    plt.show();
    
    
}
