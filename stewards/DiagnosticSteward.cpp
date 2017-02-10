//
//  DiagnosticSteward.cpp
//  ortus
//
//  Created by andrew on 1/23/17.
//  Copyright © 2017 Andrew W.E. McDonald. All rights reserved.
//

#include "DiagnosticSteward.hpp"

ortus::vtkFullxcorr* DiagnosticSteward::morphToVTK(){
    int save = 1;
    int lagsSize = 4;
    int outerElementIndex = 0;
    ortus::vtkFullxcorr* theXCorr = new ortus::vtkFullxcorr();
    for (auto outerElement : fullXCorr){
        ortus::vtkTrivec* tempTri = new ortus::vtkTrivec();
        for (auto xcorrWindow : outerElement.second){
            int innerElementIndex = 0;
            // each element in innerElement is a vector of lags corresponding to the
            // xcorr computation between the outerElement starting at outerElement.first,
            // and innerElement starting at index outerElement.first,
            // and moving forward 3 windows (so, a total of 4)
            
            ortus::vtkVectrix* tempTrix = new ortus::vtkVectrix();
            for (auto innerElement: xcorrWindow){
                //printf("inner element size (expected 4): %d\n", innerElement.size());
                // not using array just to make code more clear/readable
                float* lags = new float[lagsSize];
                for (int i = 0; i < lagsSize; ++i){
                    lags[i] = innerElement[i]; // lag
                }
                ortus::vtkVector* tempVec = ortus::vtkVector::New();
                tempVec->SetArray(lags, lagsSize, save);
                tempTrix->push_back(tempVec);
                
                innerElementIndex++;
            }
            tempTri->push_back(tempTrix);
        }
        (*theXCorr)[outerElementIndex] = tempTri;
        outerElementIndex++;
    }
    return theXCorr;
}

vtkTable* getNewTable(int numWindows){
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
    theTable->SetReferenceCount(theTable->GetReferenceCount()+1);
    return theTable.GetPointer();
}

void DiagnosticSteward::tempVTKPlot(){
    // convert fullxcorr to vtkFullxcorr
    ortus::vtkFullxcorr* theXCorr = morphToVTK();
    ortus::vtkTrivec outerTrivec = *((*theXCorr)[2]); // H20
    
    
    int numElements = dStewiep->NUM_ELEMS;
    int numWindows = outerTrivec.size();
    int numXCorrs = dStewiep->XCORR_COMPUTATIONS;
    int numXCMinusOne = numXCorrs-1;
    int numWindowsMinusOne = numWindows-1;
    
    float alpha = 1.0;
    float colors[4][4] = {
        {1.0, 0.0, 0.0, alpha}, // red
        {0.0, 1.0, 0.0, alpha}, // green
        {0.0, 0.0, 1.0, alpha}, // blue
        {1.0, 0.0, 1.0, alpha}};// purple
    
    // set up 3d scene
    vtkNew<vtkContextView> view;
    view->GetRenderWindow()->SetSize(1000,1000);
    view->GetRenderer()->GetActiveCamera()->SetClippingRange(0.00000001, 1000.0);
    vtkNew<vtkChartXYZ> chart;
    chart->SetAroundX(false);
//    chart->SetGeometry(vtkRectf(numElements*numXCorrs, 1.2, 2000, 2000));
    chart->SetGeometry(vtkRectf(0,0, 1000, 1000));
    
    view->GetScene()->AddItem(chart.GetPointer());
    // add plots
    std::vector<std::vector<vtkTable*>> vecOfLags; // indices are elements
    for (int j = 0; j < numElements; ++j){
        std::vector<vtkTable*> lagTables;
        lagTables.push_back(getNewTable(numWindows)); // lag 0
        lagTables.push_back(getNewTable(numWindows));
        lagTables.push_back(getNewTable(numWindows));
        lagTables.push_back(getNewTable(numWindows)); // lag 4
        for (int i = 0; i < numWindows; ++i){
            //int j = 0; ////////// TEMP TEMP TEMP
            //for (int j = 0; j < numElements; ++j){
            //for (int k = 0; k < numXCMinusOne; ++k){
            // printf("%.2f, ",(*partialxcorr[i])[j]->GetValue(k));
            float y0 = (*outerTrivec[i])[j]->GetValue(0); // first line in group is always at 0 offset,
            float y1 = (*outerTrivec[i])[j]->GetValue(1); // then 1,
            float y2 = (*outerTrivec[i])[j]->GetValue(2); // then 2,
            float y3 = (*outerTrivec[i])[j]->GetValue(3); // then 3
            float ys[] = {y0, y1, y2, y3};
            //TEMP printf("(%d, %.2f, %d)\n", i, y, j);
            for (int w = 0; w < numXCorrs; ++w){
                lagTables[w]->SetValue(i, 0, i);// x value is the window number, which is the
                lagTables[w]->SetValue(i, 1, ys[w]); // y value is the xcorr value
                if (i == numWindowsMinusOne)
                    lagTables[w]->SetValue(i, 2, (j*numXCorrs)+w-.0001); // z value is the element number
                else
                    lagTables[w]->SetValue(i, 2, (j*numXCorrs)+w); // z value is the element number
            }
            //}
            //}
        }
        
        for (int w = 0; w < numXCorrs; ++w){
            vtkNew<vtkPlotLine3D> plot;
            plot->SetInputData(lagTables[w]);
            plot->GetPen()->SetColorF(colors[w][0], colors[w][1],colors[w][2],colors[w][3]);
            chart->AddPlot(plot.GetPointer());
        }
        vecOfLags.push_back(lagTables);
    }
    
    // render scene
    view->GetRenderWindow()->SetMultiSamples(0);
    view->GetInteractor()->Initialize();
    
    
    view->GetRenderWindow()->Render();
    vtkContextMouseEvent mouseEvent;
    mouseEvent.SetInteractor(view->GetInteractor());
    vtkVector2i pos;
    vtkVector2i lastPos;
    // rotate
    mouseEvent.SetButton(vtkContextMouseEvent::LEFT_BUTTON);
    lastPos.Set(0, 0);
    mouseEvent.SetLastScreenPos(lastPos);
    pos.Set(0, 0);
    mouseEvent.SetScreenPos(pos);
    vtkVector2d sP(pos.Cast<double>().GetData());
    vtkVector2d lSP(lastPos.Cast<double>().GetData());
    vtkVector2d screenPos(mouseEvent.GetScreenPos().Cast<double>().GetData());
    vtkVector2d lastScreenPos(mouseEvent.GetLastScreenPos().Cast<double>().GetData());
    chart->MouseMoveEvent(mouseEvent);
    // spin
    mouseEvent.SetButton(vtkContextMouseEvent::LEFT_BUTTON);
    mouseEvent.GetInteractor()->SetShiftKey(1);
    lastPos.Set(0, 0);
    mouseEvent.SetLastScreenPos(lastPos);
    pos.Set(0, 0);
    mouseEvent.SetScreenPos(pos);
    chart->MouseMoveEvent(mouseEvent);
    // zoom
    mouseEvent.SetButton(vtkContextMouseEvent::RIGHT_BUTTON);
    mouseEvent.GetInteractor()->SetShiftKey(0);
    lastPos.Set(0, 0);
    mouseEvent.SetLastScreenPos(lastPos);
    pos.Set(0, 10);
    mouseEvent.SetScreenPos(pos);
    chart->MouseMoveEvent(mouseEvent);
    // mouse wheel zoom
    chart->MouseWheelEvent(mouseEvent, -1);
    // pan
    mouseEvent.SetButton(vtkContextMouseEvent::RIGHT_BUTTON);
    mouseEvent.GetInteractor()->SetShiftKey(1);
    lastPos.Set(10, 10);
    mouseEvent.SetLastScreenPos(lastPos);
    pos.Set(0, 0);
    mouseEvent.SetScreenPos(pos);
    chart->MouseMoveEvent(mouseEvent);
    //// remove colors
    //plot->SetInputData(table.GetPointer(), "X Axis", "Sine", "Cosine");
    //view->GetRenderWindow()->Render();
    //// add them back in
    //plot->SetColors(arrColor.GetPointer());
    
    
    /*
      for (int w = 0; w < numXCorrs; ++w){
            vtkNew<vtkPlotLine3D> plot;
            plot->SetInputData(lagTables[w]);
            plot->GetPen()->SetColorF(colors[w][0], colors[w][1],colors[w][2],colors[w][3]);
            chart->AddPlot(plot.GetPointer());
        }
   */
    view->GetInteractor()->Start();
    
    
}


DiagnosticSteward::DiagnosticSteward(DataSteward* dStewiep){
    this->dStewiep = dStewiep;
    vizzer = new DataVisualizer(this->dStewiep);
}

DiagnosticSteward::~DiagnosticSteward(){
    delete vizzer;
}

// standard information that we always want (either printed to console, or graphed)
void DiagnosticSteward::runStandardDiagnostics(){
    //vizzer.plotAll();
    vizzer->makePlots();
    //vizzer.plotSet(plotSet, plotSetSize);
    
}

// more complex stuff like analyzing xcorr, rate of change, etc.
void DiagnosticSteward::runAdvancedDiagnostics(){
    ortus::partialxcorr xcorrResults = computeXCorrBetweenVoltages();
    for (auto mapIter : xcorrResults){
        fullXCorr[mapIter.first].push_back(mapIter.second);
    }
}


void DiagnosticSteward::plotXCorr(){
    tempVTKPlot();
    return;
    std::vector<double> tempX, tempY0, tempY1, tempY2, tempY3;
    int innerElementIndex = 0;
    int xcorrCount = 0;
    int windowNum = 0;
    // 3D Vector (trivec), holding all xcorr comptuations where outerElement is static
    // (relative to the sliding window for the other elements at each xcorr window)
    // *so, size per entry should be the number of kernel iterations*
    // This doesn't mean that outerElement doesn't move, but for every incremented index,
    // we computed the xcorr between the signal starting at that index,
    // and every other element starting from that index, and sliding forward 4 windows
    // (as of the time of this writing)
    for (auto outerElement : fullXCorr){
        //printf("outer element size (expected 300): %d\n", outerElement.second.size());
        // 2D Vector (vectrix) of XCorr computations for outerElement at xcorrWindow0..n
        for (auto xcorrWindow : outerElement.second){
            //printf("xcorr Window size (expected 10): %d\n", xcorrWindow.size());
            innerElementIndex = 0;
            // each element in innerElement is a vector of lags corresponding to the
            // xcorr computation between the outerElement starting at outerElement.first,
            // and innerElement starting at index outerElement.first,
            // and moving forward 3 windows (so, a total of 4)
            for (auto innerElement: xcorrWindow){
                //printf("inner element size (expected 4): %d\n", innerElement.size());
                // not using array just to make code more clear/readable
                float lag0 = (double)innerElement[0];
                float lag1 = (double)innerElement[1];
                float lag2 = (double)innerElement[2];
                float lag3 = (double)innerElement[3];
                if (outerElement.first == 2 && innerElementIndex == 7){
                    printf("2, 7: %.2f, %.2f, %.2f, %.2f\n",lag0, lag1, lag2, lag3);
                    tempX.push_back((double)xcorrCount);
                    tempY0.push_back(lag0);
                    tempY1.push_back(lag1);
                    tempY2.push_back(lag2);
                    tempY3.push_back(lag3);
                    xcorrCount++;
                }
                innerElementIndex++;
            }
        }
    }
    vizzer->plotXCorr(tempX, tempY0, tempY1, tempY2, tempY3);
    
    printf("total xcorr count: %d\n", xcorrCount);
    exit(0);
}

/* The names of elements in computeList are the elements used as the "A" elements in the xcorr computations.
 
 * If computeList is empty, all elements are used.
 */
std::unordered_map<int, ortus::vectrix> DiagnosticSteward::computeXCorrBetweenVoltages(const std::string* computeList, int numToCompute){
    
    // this allows us to have the 'key' in the map be the same as the index the actual element has, so we can access what amounts to a sparse 3D matrix as if it were just that.
    ortus::vector indicesToUse(dStewiep->NUM_ELEMS);
    int numIndices = 0;
    if (numToCompute < 0){ // then we do all
        // doing this just so that if numToCompute is not empty, we can use the same code
        for (int i = 0; i < dStewiep->NUM_ELEMS; ++i){
            indicesToUse[i] = i;
        }
        numIndices = dStewiep->NUM_ELEMS;
    }
    else {
        for (int i = 0; i < numToCompute; ++i){
            indicesToUse[i] = dStewiep->officialNameToIndexMap[computeList[i]];
        }
        numIndices = numToCompute;
    }
    // the key, is the "A" element -- the static one
    // second index, is the "B" element -- this one, gets the window sliding across it
    // third index is xcorr computation 0 to dStewiep->numXCorrComputations-1
    // NOTE: totalXCorr[x][x][x] gives the autocorrelation for element X, which should be 1.0 (due to normalization)
    ortus::partialxcorr totalPartialXCorr(numIndices);
    ortus::vectrix voltageHistoryVector = dStewiep->outputVoltageHistory->convertDataTo2DVector();
    size_t numElements = voltageHistoryVector.size();
    /*
    printf("VOLTAGE OUTPUT WHAT WHAT:\n");
    for (int i = 0; i < numElements; ++i){
        for (int j = 0; j < voltageHistoryVector[i].size(); ++j){
            printf("%.2f, ",voltageHistoryVector[i][j]);
        }
        printf("\n");
    }
    printf("END THE VOLTAGE HISTORY VECTOR PRINTOUT\n");
    */
    int startIndex = 0; // we want to start at the beginning of the voltage history
    
    // endIndex is the startIndex, plus the length that we use for a single xcorr computation (but minus one because it's an index), plus the (XCORR_COMPUTATIONS -1) (because for each computation, we shift our window for 'B' over by 1, but not the first, because that has the starting offset)
    // It's 2*(XCORR_COMPUTATIONS-1) because XCORRR_COMPUTATIONS == the length that we use for a single xcorr computation.
    int endIndex = startIndex + 2*(dStewiep->XCORR_COMPUTATIONS - 1);
    // This is going to be slow, and it would probably be best to either rad the resutls back from blade, or thread this computation...
    for (int i = 0; i < numIndices; ++i){// go through each element,
        ortus::vectrix tempXCorrRes(numElements);
        for (int j = 0; j < numElements; ++j){ // and compute the xcorr between it, and everything else (including itself)
            
            tempXCorrRes[j] = statistician.xcorrLimited(voltageHistoryVector[indicesToUse[i]], voltageHistoryVector[j], startIndex, dStewiep->XCORR_COMPUTATIONS, endIndex);
        }
        totalPartialXCorr[indicesToUse[i]] = tempXCorrRes;
    }
    return totalPartialXCorr;
}
