//
//  DataVisualizer.hpp
//  delegans
//
//  Created by onyx on 11/4/16.
//  Copyright © 2016 Andrew W.E. McDonald. All rights reserved.
//

#ifndef DataVisualizer_hpp
#define DataVisualizer_hpp

#include <stdio.h>
#include "DataSteward.hpp"
#include "Plot.hpp"
#include <vector>
#include <string>
#include "OrtusStd.hpp"




class DataVisualizer {
    
public:
    
    DataVisualizer(DataSteward* stewie);
    void plotSet(std::string* names, int numNames);
    void makePlots();
    void makeConferencePlots();
    void plotXCorr(std::vector<double>& dataX, std::vector<double>& dataY0, std::vector<double>& dataY1, std::vector<double>& dataY2, std::vector<double>& dataY3);
    void plotAll();
    
    DataSteward* stewie;
};

#endif /* DataVisualizer_hpp */
