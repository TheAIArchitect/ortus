//
//  OrtusVisualizer.cpp
//  VtkTool
//
//  Created by andrew on 2/13/17.
//  Copyright © 2017 andrew. All rights reserved.
//

#include "OrtusVisualizer.hpp"

OrtusVisualizer::OrtusVisualizer(){
    
    float alpha = 1.0;
    
    
    colors[0] = vtkColor4f(1.0, 0.0, 0.0, alpha); // red
    colors[1] = vtkColor4f(0.0, 1.0, 0.0, alpha); // green
    colors[2] = vtkColor4f(0.0, 0.0, 1.0, alpha); // blue
    colors[3] = vtkColor4f(1.0, 0.0, 1.0, alpha); // pink
    colors[4] = vtkColor4f(1.0, 1.0, 0.0, alpha); // yellow
    colors[5] = vtkColor4f(1.0, 0.5, 0.0, alpha); // orange
    colors[6] = vtkColor4f(0.0, 1.0, 1.0, alpha); // lblue (light blue)
    colors[7] = vtkColor4f(0.4, 0.0, 0.8, alpha); // purple
    colors[8] = vtkColor4f(0.0, 0.0, 0.0, alpha); // black
    
    
    std::string colorNames[numColors] = {"red", "green", "blue", "pink", "yellow", "orange", "lblue", "purple", "black"};
    
    for (int i = 0; i < numColors; ++i){
        colormap[colorNames[i]] = colors[i];
    }
    
}
