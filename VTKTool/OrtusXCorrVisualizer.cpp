//
//  OrtusXCorrVisualizer.cpp
//  VtkTool
//
//  Created by andrew on 2/13/17.
//  Copyright © 2017 andrew. All rights reserved.
//

#include "OrtusXCorrVisualizer.hpp"

OrtusXCorrVisualizer::OrtusXCorrVisualizer(int numWindows, int numElements, int numXCorrs){
    this->numWindows = numWindows;
    this->numElements = numElements;
    this->numXCorrs = numXCorrs;
    numXCMinusOne = numXCorrs-1;
    numWindowsMinusOne = numWindows-1;
    
}



void OrtusXCorrVisualizer::visualize(){
    
    // basic pipeline setup
    vtkNew<vtkRenderWindow> rendwin;
    rendwin->SetMultiSamples(0);
    rendwin->SetSize(1000,1000);
    vtkNew<vtkRenderWindowInteractor> rendi;
    
    
    rendi->SetRenderWindow(rendwin.GetPointer());
    vtkNew<vtkRenderer> rend;
    rend->SetBackground(0.95, 0.95, 0.95);
    rendwin->AddRenderer(rend.GetPointer());
    rend->ResetCamera();
    
    vtkNew<vtkContextScene> chartScene;
    vtkNew<vtkContextInteractorStyle> contextInteractorStyle;
    contextInteractorStyle->SetScene(chartScene.GetPointer());
    rendi->SetInteractorStyle(contextInteractorStyle.GetPointer());
    vtkNew<vtkContextActor> chartActor;
    
    chartScene->AddItem(chart.GetPointer());
    chartActor->SetScene(chartScene.GetPointer());
    rend->AddActor(chartActor.GetPointer());
    chartScene->SetRenderer(rend.GetPointer());
    
   
    //vtkNew<vtkContextView> view;
    //view->GetInteractor()->GetInteractorStyle()->Get
    //view->GetRenderWindow()->SetSize(500,500);
    
    // set up the chart itself 
    chart->SetGeometry(vtkRectf(0.0f, 0.0f, 1000, 1000));
    // this adds the plots to the chart
    plotXCorrForStaticElement(2);
    
    //view->GetScene()->AddItem(chart.GetPointer());
    
    // render scene
    //view->GetRenderWindow()->SetMultiSamples(0);
    //view->GetInteractor()->Initialize();
    //view->GetInteractor()->Start();
    rendi->Initialize();
    rendi->Start();
    
    
}

/*
void OrtusXCorrVisualizer::visualize(){
   
    vtkNew<vtkContextView> view;
    view->GetRenderWindow()->SetSize(500,500);
    chart->SetGeometry(vtkRectf(100.f, 2.f, 400, 400));
    
    // this adds the plots to the chart
    plotXCorrForStaticElement(0);
    
    view->GetScene()->AddItem(chart.GetPointer());
    
    // render scene
    view->GetRenderWindow()->SetMultiSamples(0);
    view->GetInteractor()->Initialize();
    view->GetInteractor()->Start();
}
*/


void OrtusXCorrVisualizer::plotXCorrForStaticElement(int staticElementId){
    // add plots
    for (int i = 0; i < numElements; ++i){
        addXCorrPlots(staticElementId, i, i);
    }
}


/* lag 0 refers to the two lined up in time.
 * lag 1 refers to the dynamic element moved forward by 1
 * lag 2, same as 1, but forward by 2
 * and so on.
 *
 * this function adds numXCorrs plots to the chart,
 * and those are placed at Z-index:
 * '(plotGroupId * numXCorrs + lagNum)',
 
 * The 'normal' way to set plotGroupId,
 * if you were plotting a set of dynamicElementId variables,
 * (via muiltiple calls to addXCorrPlots), for example,
 * {0, 1, 2, 3, 4}, for staticElementId = 2,
 *
 * then to have the group of XCorr plots for dynamicElementId '0' first,
 * followed by the group of XCorr plots for dynamicElementI '1', and so on,
 * simply set 'plotGroupId' to the dynamicElementId on each call to
 * addXCorrPlots.
 */
void OrtusXCorrVisualizer::addXCorrPlots(int staticElementId, int dynamicElementId, int plotGroupId){
    std::vector<vtkTable*> lagTables(numXCorrs);
    //std::vector<float> xcorrValues(numXCorrs);
    float xcorrValue = 0;
    for (int lagNum = 0; lagNum < numXCorrs; ++lagNum){
        lagTables[lagNum] = getNewTable();
    }
    // this is just for readability; could condense it.
    ortus::vtkTrivec partialxcorr = *((*theXCorr)[staticElementId]);
    for (int i = 0; i < numElements; ++i){
        for (int j = 0; j < numWindows; ++j){
            for (int lagNum = 0; lagNum < numXCorrs; ++lagNum){
                xcorrValue = (*partialxcorr[j])[dynamicElementId]->GetValue(lagNum);
                lagTables[lagNum]->SetValue(j, 0, j); // x value is window number
                lagTables[lagNum]->SetValue(j, 1, xcorrValue); // y value is the xcorr value
                // this check is because of (what I believe to be) a
                // but in vtk. The plot won't display the line unless
                // the Z axis isn't constant... so, for the last data point,
                // we move it *very* slightly.
                if (j == numWindowsMinusOne){
                    lagTables[lagNum]->SetValue(j, 2, (plotGroupId * numXCorrs) + lagNum - 0.0001);
                }
                else {
                    lagTables[lagNum]->SetValue(j, 2, (plotGroupId * numXCorrs) + lagNum);
                }
                
            }
        }
        
    }
    for (int lagNum = 0; lagNum < numXCorrs; ++lagNum){
        vtkNew<vtkPlotLine3D> plot;
        plot->SetInputData(lagTables[lagNum]);
        // this next line might not be necessary, but I figured it would be good to keep a record of the lags for now.
        allLagTables.push_back(lagTables[lagNum]);
        plot->GetPen()->SetColorF(colors[lagNum][0],colors[lagNum][1],colors[lagNum][2],colors[lagNum][3]);
        chart->AddPlot(plot.GetPointer());
    }
}





vtkTable* OrtusXCorrVisualizer::getNewTable(){
    vtkNew<vtkTable> theTable;
    vtkNew<vtkFloatArray> tableX;
    vtkNew<vtkFloatArray> tableY;
    vtkNew<vtkFloatArray> tableZ;
    tableX->SetName("X");
    tableY->SetName("Y");
    tableZ->SetName("Z");
    theTable->AddColumn(tableX.GetPointer());
    theTable->AddColumn(tableY.GetPointer());
    theTable->AddColumn(tableZ.GetPointer());
    theTable->SetNumberOfRows(numWindows);
    // need to increment reference count because when the funciton returns, it subtracts one.
    theTable->SetReferenceCount(theTable->GetReferenceCount()+1);
    return theTable.GetPointer();
}
