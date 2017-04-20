//
//  Ortus3DChart.hpp
//  VtkTool
//
//  Created by andrew on 2/13/17.
//  Copyright © 2017 andrew. All rights reserved.
//

#ifndef Ortus3DChart_hpp
#define Ortus3DChart_hpp

#include "StandardVTKHeader.hpp"
#include <stdio.h>

#include "vtkObjectFactory.h"
#include "vtkContextMouseEvent.h"


class Ortus3DChart : public vtkChartXYZ {
    
public:
    Ortus3DChart();
   static Ortus3DChart * New();
    bool MouseWheelEvent(const vtkContextMouseEvent &mouse, int delta);
    //bool MouseButtonPressEvent(const vtkContextMouseEvent &mouse);
    void DrawGrid(vtkContext3D *context);
    bool Paint(vtkContext2D *painter);
    vtkNew<vtkPen> gridPen;
};


#endif /* Ortus3DChart_hpp */
