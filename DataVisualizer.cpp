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

void DataVisualizer::plotXCorr(std::vector<double>& dataX, std::vector<double>& dataY0, std::vector<double>& dataY1, std::vector<double>& dataY2, std::vector<double>& dataY3){
    
    Plot p(true);
   
    p.addXValues<double>(dataX);
    std::unordered_map<std::string, std::string> kwArgs;
    p.addYValues<double>(dataY0);
    kwArgs["label"] = "lag "+std::to_string(0);
    p.plot(kwArgs);
    p.addYValues<double>(dataY1);
    kwArgs["label"] = "lag "+std::to_string(1);
    p.plot(kwArgs);
    p.addYValues<double>(dataY2);
    kwArgs["label"] = "lag "+std::to_string(2);
    p.plot(kwArgs);
     p.addYValues<double>(dataY3);
    kwArgs["label"] = "lag "+std::to_string(3);
    p.plot(kwArgs);
    p.title("Holding SH20 at 0, with IFEAR sliding -- xcorr size of 4");
    p.grid(true);
    p.legend();
    p.show();
    
}

void DataVisualizer::makePlots(){
    const int plotOneSize = 6;
    std::string plotOneSet[plotOneSize] = {"SO2","SCO2","IPO2", "INO2", "MINHALE", "MEXHALE"};
    const int plotTwoSize = 5;
    std::string plotTwoSet[plotTwoSize] = {"SO2","SCO2","INO2", "IFEAR", "SH2O"};
    
    int max = stewie->kernelVoltages.size();
    Plot plot(true);
    
    std::vector<double> xVals;// windows
    std::vector<double> yVals;// neuron voltages
    for ( int i = 0; i < max; ++i){
        xVals.push_back(i);//window number
    }
    plot.addXValues<double>(xVals);
    plot.figure();
    
    plot.subplot("211");
    //Plot::kwargsMap kwArgs;
    std::unordered_map<std::string, std::string> kwArgs;
    for (int i = 0; i < plotOneSize; ++i){
        int idx = stewie->officialNameToIndexMap[plotOneSet[i]];
        kwArgs["label"] = plotOneSet[i];
        yVals.clear();
        for (int j = 0; j < max; ++j){
            yVals.push_back(stewie->kernelVoltages[j][idx]);
        }
        plot.addYValues<double>(yVals);
        plot.plot(kwArgs);
    }
    plot.grid(true);
    plot.legend();
    plot.subplot("212");
    kwArgs.clear();
    for (int i = 0; i < plotTwoSize; ++i){
        int idx = stewie->officialNameToIndexMap[plotTwoSet[i]];
        kwArgs["label"] = plotTwoSet[i];
        yVals.clear();
        for (int j = 0; j < max; ++j){
            yVals.push_back(stewie->kernelVoltages[j][idx]);
        }
        plot.addYValues<double>(yVals);
        plot.plot(kwArgs);
    }
    plot.grid(true);
    plot.legend();
    
    plot.show();
 
    
}

/*
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
        kwArgs["label"] = stewie->elements[i]->name();
        yVals.clear();
        for (int j = 0; j < numWindows; ++j){
            yVals.push_back(stewie->kernelVoltages[j][i]);
        }
        plt.plot(xVals, yVals, kwArgs);
        
    }
    plt.legend();
    plt.show();
    
    
}
*/

void DataVisualizer::plotSet(std::string* names, int numNames){

    
    int max = stewie->kernelVoltages.size();
    std::vector<double> xVals;// windows
    std::vector<double> yVals;// neuron voltages
    for ( int i = 0; i < max; ++i){
        xVals.push_back(i);//window number
        
    }
    
    Plot plotOne(true);
    //Plot::kwargsMap kwArgs;
    std::unordered_map<std::string, std::string> kwArgs;
    for (int i = 0; i < numNames; ++i){
        int idx = stewie->officialNameToIndexMap[names[i]];
        kwArgs["label"] = names[i]; 
        yVals.clear();
        for (int j = 0; j < max; ++j){
            yVals.push_back(stewie->kernelVoltages[j][idx]);
        }
        plotOne.addValues<double>(xVals, yVals);
        plotOne.plot(kwArgs);
    }
    plotOne.grid(true);
    plotOne.legend();
    plotOne.show();
    
    
}
