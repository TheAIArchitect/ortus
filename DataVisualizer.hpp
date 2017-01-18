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




class DataVisualizer {
    
public:
    
    DataVisualizer(DataSteward* stewie);
    void plotSet(std::string* names, int numNames);
    void makePlots();
    void plotAll();
    
    DataSteward* stewie;
};

#endif /* DataVisualizer_hpp */
