//
//  Plot.cpp
//  Plotr
//
//  Created by andrew on 1/13/17.
//  Copyright © 2017 Ecodren Research. All rights reserved.
//

/***
 **
 ** http://stackoverflow.com/questions/32812509/how-to-make-c-code-into-a-library-for-xcode
 **
 ** ==> scroll down to the answer starting off, "This is a big question..."
 **
 **/


#include "Plot.hpp"


Plot::Plot(bool useOneXDataset){
    setUseOneXDataset(useOneXDataset);
    length = 0;
}

Plot::~Plot(){
    size_t numXValLists = xValuesListpVector.size();
    size_t numYValLists = yValuesListpVector.size();
    for (int i = 0; i < numXValLists; ++i){
        // NOTE: I don't know if I need to call decref on each list item...
        Pytils::decref(xValuesListpVector[i]);
    }
    for (int i = 0; i < numYValLists; ++i){
        // NOTE: I don't know if I need to call decref on each list item...
        Pytils::decref(yValuesListpVector[i]);
    }
}

/* updates the figure that's been altered, but not automatically rederawn
 * 
 * I haven't found a time i needed to use this yet...*/
bool Plot::draw(){
    return Pytils::call(py.getPyObject("draw"));
}

/* creates a figure (or calls up an existing one), and returns its 'figure number' */
bool Plot::figure(int figNum){
    PyObject* packedArgs;
    Pytils::packTuple(&packedArgs, {PyInt_FromLong(figNum)});
    return Pytils::call(py.getPyObject("figure"),packedArgs);
}

/* turns the grid on or off */
bool Plot::grid(bool gridOn){
    PyObject* gridOnPop = gridOn ? Py_True : Py_False;
    PyObject* packedArgs;
    Pytils::packTuple(&packedArgs, {gridOnPop});
    return Pytils::call(py.getPyObject("grid"));
}

/* turns hold on or off */
bool Plot::hold(bool holdOn){
    PyObject* holdOnPop = holdOn ? Py_True : Py_False;
    PyObject* packedArgs;
    Pytils::packTuple(&packedArgs, {holdOnPop});
    return Pytils::call(py.getPyObject("hold"));
}


/* turns interactive mode off */
bool Plot::ioff(){
    return Pytils::call(py.getPyObject("ioff"));
}

/* turns interactive mode on */
bool Plot::ion(){
    return Pytils::call(py.getPyObject("ion"));
}

/* as of now, just causes a legend to show up. labels must be declared in plot statement */
bool Plot::legend(){
    return Pytils::call(py.getPyObject("legend"));
}

bool Plot::pause(double duration){
    PyObject* packedArgs;
    Pytils::packTuple( &packedArgs, {PyFloat_FromDouble(duration)});
    return Pytils::call(py.getPyObject("pause"), packedArgs);
}


bool Plot::show(){
    return Pytils::call(py.getPyObject("show"));
}

/* for subplot specification with a 3 digit number, rows, cols, and subplot number must all be less than 10 */
bool Plot::subplot(std::string threeDigitNumber){
    PyObject* packedArgs;
    Pytils::packTuple( &packedArgs, {PyString_FromString(threeDigitNumber.c_str())});
    return Pytils::call(py.getPyObject("subplot"), packedArgs);
}

bool Plot::title(std::string title){
    PyObject* packedArgs;
    Pytils::packTuple( &packedArgs, {PyString_FromString(title.c_str())});
    return Pytils::call(py.getPyObject("title"), packedArgs);
}

/* NOTE: this will increment the counters for the X and Y datasets... figure out a better solution here
 *
 */
bool Plot::xcorr(std::unordered_map<std::string, std::string> args, int xIndex, int yIndex){
    // first deal with indices
    int gottenXIndex = -1;
    int gottenYIndex = -1;
    bool gotIndices = false;
    if (xIndex < 0 || yIndex < 0){
        getMostRecentlyAddedValuesIndices(gottenXIndex, gottenYIndex);
        gotIndices = true;
    }
    if (gotIndices && (gottenXIndex < 0 || gottenYIndex < 0)){ // then one of them is negative, and we can't plot anything.
        return false;
    }
    else if (gotIndices){
        xIndex = gottenXIndex;
        yIndex = gottenYIndex;
    }
    // then deal with the "optional" parameters -- assume neither exist, and adjust as necessary
    bool noArgs = true;
    // first the dictionary
    PyObject* kwargsp = NULL;
    if (!args.empty()){
        noArgs = false;
        Pytils::packDict(&kwargsp, args);
    }
    PyObject* plotArgsTuplep;
    Pytils::packTuple(&plotArgsTuplep, {xValuesListpVector[xIndex], yValuesListpVector[yIndex]});
    if (noArgs){
        return Pytils::call(py.getPyObject("xcorr"), plotArgsTuplep);
    }
    else {
        return Pytils::call(py.getPyObject("xcorr"), plotArgsTuplep, kwargsp);
    }
    
}

/* default plot settings will be used if no 'real' args are passed,
 * which is the same as matplotlib's default plot settings
 *
 * if either the X index or the Y (or both) are < 0, the default index-getting function,
 * getMostRecentlyAddedValuesIndices(...) will be called to get the indices of the datasets to plot.
 */
bool Plot::plot(std::string& formatString, std::unordered_map<std::string, std::string>& args, int xIndex, int yIndex){
    // first deal with indices
    int gottenXIndex = -1;
    int gottenYIndex = -1;
    bool gotIndices = false;
    if (xIndex < 0 || yIndex < 0){
        getMostRecentlyAddedValuesIndices(gottenXIndex, gottenYIndex);
        gotIndices = true;
    }
    if (gotIndices && (gottenXIndex < 0 || gottenYIndex < 0)){ // then one of them is negative, and we can't plot anything.
        return false;
    }
    else if (gotIndices){
        xIndex = gottenXIndex;
        yIndex = gottenYIndex;
    }
    // then deal with the "optional" parameters -- assume neither exist, and adjust as necessary
    bool noArgs = true;
    bool noFormatString = true;
    // first the dictionary
    PyObject* kwargsp = NULL;
    if (!args.empty()){
        noArgs = false;
        Pytils::packDict(&kwargsp, args);
    }
    // then the format string
    PyObject* plotArgsTuplep;
    if (formatString != ""){
        noFormatString = false;
        Pytils::packTuple(&plotArgsTuplep, {xValuesListpVector[xIndex], yValuesListpVector[yIndex], PyString_FromString(formatString.c_str())});
    }
    else {
        Pytils::packTuple(&plotArgsTuplep, {xValuesListpVector[xIndex], yValuesListpVector[yIndex]});
    }
    if (noArgs){
        return Pytils::call(py.getPyObject("plot"), plotArgsTuplep);
    }
    else {
        return Pytils::call(py.getPyObject("plot"), plotArgsTuplep, kwargsp);
    }
}

bool Plot::plot(std::string formatString, int xIndex, int yIndex){
    std::unordered_map<std::string, std::string> dummy;
    return plot(formatString, dummy, xIndex, yIndex);
}

bool Plot::plot(std::unordered_map<std::string, std::string>& args, int xIndex, int yIndex){
    std::string s = "";
    return plot(s,args, xIndex, yIndex);
}


/* allows plotting the most recently added dataset by adding data, and then calling plot (or another plotting function) */
void Plot::getMostRecentlyAddedValuesIndices(int& xValuesIndex, int& yValuesIndex){
    xValuesIndex = currentXValuesIndex;
    yValuesIndex = currentYValuesIndex;
}



int Plot::getDataLength(){
    return length;
}

void Plot::setUseOneXDataset(bool useOneXDataset){
    this->useOneXDataset = useOneXDataset;
}

void Plot::_addValues(std::vector<double> &xValues, std::vector<double> &yValues){
    PyObject* tempX = PyList_New(xValues.size());
    PyObject* tempY = PyList_New(yValues.size());
    for (size_t i = 0; i < length; ++i){
        PyList_SetItem(tempX, i, PyFloat_FromDouble(xValues[i]));
        PyList_SetItem(tempY, i, PyFloat_FromDouble(yValues[i]));
    }
    xValuesListpVector.push_back(tempX);
    currentXValuesIndex++;
    yValuesListpVector.push_back(tempY);
    currentYValuesIndex++;
}

void Plot::_addYValues(std::vector<double> &yValues){
    PyObject* tempY = PyList_New(yValues.size());
    for (size_t i = 0; i < length; ++i){
        PyList_SetItem(tempY, i, PyFloat_FromDouble(yValues[i]));
    }
    yValuesListpVector.push_back(tempY);
    currentYValuesIndex++;
}


/*
void Plot::getNextValuesListIndex(int& xValuesIndex, int& yValuesIndex){
    size_t xValuesListSize = xValuesListpVector.size();
    size_t yValuesListSize = yValuesListpVector.size();
    bool plotError = false;
    std::string errMsg = "";
    // error checks 
    // make sure there is some data in both lists
    if ((xValuesListSize > 0) && (yValuesListSize > 0)){
        // if we are using one X dataset, make sure we don't have more than one X dataset
        if (useOneXDataset && xValuesListSize != 1){ // then we can only have 1 x
            errMsg = "Plot Error: attempting to use only one X dataset, but found more than one X datasets have been added.";
            plotError = true;
        }
        // if we aren't using one X dataset, make sure we have equal numbers of X and Y datasets
        else if (!useOneXDataset && xValuesListSize != yValuesListSize){
            errMsg = "Plot Error: X and Y dataset lists are of unequal length, and setUseOneXDataset(true) has not been called.";
            plotError = true;
        }
        // make sure y index is in range
        else if (currentYValuesIndex >= yValuesListSize){
            errMsg = "Plot Error: Y index out of range.";
            plotError = true;
        }
        // make sure x index is in range
        else if (currentXValuesIndex >= xValuesListSize){
            errMsg = "Plot Error: X index out of range.";
            plotError = true;
        }
        // make sure x index is 0 if we are only using one X dataset
        else if (useOneXDataset && currentXValuesIndex != 0){
            errMsg = "Plot Error: attempting to use one X dataset, but X index not equal to 0.";
            plotError = true;
        }
    }
    else {
        errMsg = "Plot Error: xValuesListSize (size: "+std::to_string(xValuesListSize)+"), yValuesListSize (size: "+std::to_string(yValuesListSize)+") must both be greater than 0.";
        plotError = true;
    }
    // end error checks 
    if (plotError){
        printf("%s\n",errMsg.c_str());
        xValuesIndex = -1;
        yValuesIndex = -1;
    }
    else {
        xValuesIndex = currentXValuesIndex;
        yValuesIndex = currentYValuesIndex;
        if (!useOneXDataset){
                currentXValuesIndex++;
        }
        currentYValuesIndex++;
    }
}
*/
