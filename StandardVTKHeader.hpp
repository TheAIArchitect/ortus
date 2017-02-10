//
//  StandardVTKHeader.h
//  VtkTool
//
//  Created by andrew on 2/2/17.
//  Copyright © 2017 andrew. All rights reserved.
//

#ifndef StandardVTKHeader_h
#define StandardVTKHeader_h

#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingContextOpenGL2);
VTK_MODULE_INIT(vtkRenderingOpenGL2)
VTK_MODULE_INIT(vtkInteractionStyle)
VTK_MODULE_INIT(vtkRenderingFreeType);


#include "vtkActor.h"
#include "vtkBoxWidget.h"
#include "vtkCamera.h"
#include "vtkChartXYZ.h"
#include "vtkCommand.h"
#include "vtkContextScene.h"
#include "vtkConeSource.h"
#include "vtkContextView.h"
#include "vtkCylinderSource.h"
#include "vtkFloatArray.h"
#include "vtkGenericDataArray.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkNew.h"
#include "vtkPen.h"
#include "vtkPlotLine3D.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkSmartPointer.h"
#include "vtkTable.h"
#include "vtkTransform.h"




#endif /* StandardVTKHeader_h */
