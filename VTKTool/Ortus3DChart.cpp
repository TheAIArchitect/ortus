//
//  Ortus3DChart.cpp
//  VtkTool
//
//  Created by andrew on 2/13/17.
//  Copyright © 2017 andrew. All rights reserved.
//

#include "Ortus3DChart.hpp"


vtkStandardNewMacro(Ortus3DChart)

Ortus3DChart::Ortus3DChart() : vtkChartXYZ() {
    this->Pen->SetWidth(1);  // on Mac OS X Sierra, can't draw lines thicker than 1.
    gridPen->SetColor(0,0,0, 30);
    gridPen->SetWidth(1.0);
}

bool Ortus3DChart::MouseWheelEvent(const vtkContextMouseEvent &mouse, int delta){
    ZoomAxes(delta);
    return true;
}

/*
bool Ortus3DChart::MouseButtonPressEvent(const vtkContextMouseEvent &mouse){
    bool res = vtkChartXYZ::MouseButtonPressEvent(mouse);
    
    vtkVector2f v;
    v = mouse.GetScenePos();
    printf("scene pos: %.2f, %.2f\n",v[0], v[1]);
    v = mouse.GetPos();
    printf("normal pos: %.2f, %.2f\n",v[0], v[1]);
    int plotNum = 0;
 
 
    for (auto plot : this->Plots){
            if (plot->Hit(mouse)){
                printf("HIT on plot number %d\n", plotNum);
            }
            else{
                printf(":( no hit on plot number %d\n", plotNum);
            }
        plotNum++;
    }
 
    return res;
}
*/


bool Ortus3DChart::Paint(vtkContext2D *painter){
    
    vtkChartXYZ::Paint(painter);
    
    // Get the 3D context.
    vtkContext3D *context = painter->GetContext3D();
    
    DrawGrid(context);
    return true;
}

void Ortus3DChart::DrawGrid(vtkContext3D *context)
{
    context->PushMatrix();
    context->AppendTransform(this->Box.GetPointer());
    
    context->ApplyPen(gridPen.GetPointer());
    
    float min = 0.0;
    float max = 1.1;
    float div = 0.1;
    // draw grid along X axis
    for (float i = min; i <= max; i += div){
        float currentY = i;
        for (float j = min; j <= max; j += div){
            float currentZ = j;
            context->DrawLine(vtkVector3f(0, currentY, currentZ), vtkVector3f(1, currentY, currentZ));
        }
    }
    
    // draw grid along Y axis
    for (float i = min; i <= max; i += div){
        float currentX = i;
        for (float j = min; j <= max; j += div){
            float currentZ = j;
            context->DrawLine(vtkVector3f(currentX, 0, currentZ), vtkVector3f(currentX, 1, currentZ));
        }
    }
   
    // draw grid along Z axis
    for (float i = min; i <= max; i += div){
        float currentX = i;
        for (float j = min; j <= max; j += div){
            float currentY = j;
            context->DrawLine(vtkVector3f(currentX, currentY, 0), vtkVector3f(currentX, currentY, 1));
        }
    }
    
}
