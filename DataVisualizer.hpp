//
//  DataVisualizer.hpp
//  delegans
//
//  Created by onyx on 11/4/16.
//  Copyright © 2016 Sean Grimes. All rights reserved.
//

#ifndef DataVisualizer_hpp
#define DataVisualizer_hpp

#include <stdio.h>
#include "DataSteward.hpp"
#include "Plotr.hpp"
#include <vector>
#include <string>




class DataVisualizer {
    
public:
    
    DataVisualizer(DataSteward* stewie);
    void plotSet(std::string* names, int numNames);
    void plotAll();
    
    DataSteward* stewie;
};

#endif /* DataVisualizer_hpp */
