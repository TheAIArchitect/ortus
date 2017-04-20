//
//  OrtusVisualizer.hpp
//  VtkTool
//
//  Created by andrew on 2/13/17.
//  Copyright © 2017 andrew. All rights reserved.
//

#ifndef OrtusVisualizer_hpp
#define OrtusVisualizer_hpp

#include "StandardVTKHeader.hpp"

#include <unordered_map>
#include <string>
#include <stdio.h>

class OrtusVisualizer {
    
public:
    OrtusVisualizer();
    static const int numColors = 9;
    vtkColor4f colors[numColors];
    std::unordered_map<std::string,vtkColor4f> colormap;
};

#endif /* OrtusVisualizer_hpp */
