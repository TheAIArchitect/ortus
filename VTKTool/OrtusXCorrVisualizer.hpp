//
//  OrtusXCorrVisualizer.hpp
//  VtkTool
//
//  Created by andrew on 2/13/17.
//  Copyright © 2017 andrew. All rights reserved.
//

#ifndef OrtusXCorrVisualizer_hpp
#define OrtusXCorrVisualizer_hpp

#include "StandardVTKHeader.hpp"
#include "OrtusVisualizer.hpp"
#include <vector>
#include "vtkStdString.h"

#include "OrtusStd.hpp"
#include <stdio.h>


class OrtusXCorrVisualizer : OrtusVisualizer {
    
    // TEMP
public:
    ortus::vtkFullxcorr* theXCorr;
    
    
public:
    OrtusXCorrVisualizer(int numWindows, int numElements, int numXCorrs);
    void addXCorrPlots(int staticElementId, int dynamicElementId, int plotGroupId);
    void plotXCorrForStaticElement(int staticElementId);
    void visualize();
    
public:
    vtkNew<Ortus3DChart> chart;
    vtkTable* getNewTable();
    std::vector<vtkTable*> allLagTables;
    
    int numWindows;
    int numElements;
    int numXCorrs;
    int numXCMinusOne;
    int numWindowsMinusOne;
    
    
    
};

#endif /* OrtusXCorrVisualizer_hpp */
