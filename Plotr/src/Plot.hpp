//
//  Plot.hpp
//  Plotr
//
//  Created by andrew on 1/13/17.
//  Copyright © 2017 Ecodren Research. All rights reserved.
//

#ifndef Plot_hpp
#define Plot_hpp

#include <stdio.h>
#include "Pyterpreter.hpp"
#include "Pytils.hpp"
#include <vector>
#include <string>
#include <type_traits>
#include <unordered_map>

class Plot {
    
public:
    Pyterpreter py;
    
    
public:
    typedef std::unordered_map<std::string, std::string> kwargsMap;
    
    Plot(bool useOneXDataset);
    ~Plot();
    
    template<typename N>
    Plot(std::vector<N> &xValues, std::vector<N> &yValues, bool useOneXDataset = true) : Plot(true) {
        addValues(xValues, yValues);
    }
    
    
    //bool annotate(); // implement
    //bool arrow(); //implement
    //bool axis(); // implement
    //bool axes(); // implement
    //bool colorbar(); // implement
    bool draw();
    //bool eventplot();// implement
    bool figure(int figNum);
    bool grid(bool gridOn);
    //bool hist();// implement
    //bool hist2d();//implement
    bool hold(bool holdOn);
    //bool imsave(); // implement
    //bool imshow(); // implement
    bool ioff();
    bool ion();
    bool legend();
    bool pause(double duration);
    //bool pie(); // implement
    bool plot(std::string formatString, int xIndex = -1, int yIndex = -1);
    bool plot(kwargsMap& args, int xIndex = -1, int yIndex = -1);
    bool plot(std::string& formatString, kwargsMap& args, int xIndex = -1, int yIndex = -1);
    //bool quiver(); // implement
    //bool scatter(); // implement
    bool show();
    //bool specgram(); // implement
    bool subplot(std::string threeDigitNumber);
    bool title(std::string title);
    // xcorr doesn't work for bool or numeric kwargs.. need to fix that.
    bool xcorr(kwargsMap args = kwargsMap(), int xIndex = -1, int yIndex = -1);
    bool xlabel(std::string xlabel); // implement
    bool xlim(double lower, double uppper); // implement
    bool xticks(); // implement
    bool ylabel(std::string ylabel); // implement
    bool ylim(double lower, double uppper); // implement
    bool yticks(); // implement
    bool saveFig(std::string fileName);// implement
    
    
    
    
    
    
    
    
    const static int NUM_COLORS = 7;
    const static int NUM_STYLES = 4;
    const static int NUM_MARKERS = 12;
    std::string COLORS[NUM_COLORS] = {"blue", "green", "red","cyan", "magenta", "yellow", "black"};
    std::string STYLES[NUM_STYLES] = {"solid", "dashed", "dashdot","dotted"};
    std::string MARKERS[NUM_MARKERS] = {"x", "+","*", "o", "s", "v",".", "^", "<", ">", "D","d"};
    std::string MARKERS_DESCRIPTIONS[NUM_MARKERS] = {"x","plus", "star","circle", "square", "triangle_down", "point", "triangle_up", "triangle_left", "triangle_right", "Diamond", "thin_diamond"};
    
    
   /*
    * below here is code that deals with adding data to Plot (other than the constructors)
    **/
    
    
    /* adds a vector of x values and y values, if the number of x and y vectors is equal*/
    template<typename N>
    void addValues(std::vector<N> &xValues, std::vector<N> &yValues){
        static_assert(std::is_arithmetic<N>::value, "Plot only accepts arithmetic types");
        assert(xValues.size() == yValues.size());
        if (length != 0){
            assert(xValues.size() == length);
        }
        else{
            length = xValues.size();
        }
        if (xValuesListpVector.size() != yValuesListpVector.size()){
            printf("Plot can only add vectors of 'x' and 'y' values if there are equal numbers of 'x' and 'y' values already added to Plot.\n");
            return;
        }
        std::vector<double> xValuesDoubles = static_cast<std::vector<double>>(xValues);
        std::vector<double> yValuesDoubles = static_cast<std::vector<double>>(yValues);
        _addValues(xValuesDoubles, yValuesDoubles);
    }
    
    /* adds a vector of y values, but iff there is one x vector (no more, and no less)
     *
     * The idea here, is that one can plot multiple datasets that have the same domain,
     * and to do so, must only add the domain once.
     */
    template<typename N>
    void addYValues(std::vector<double> &yValues){
        if (xValuesListpVector.size() != 1){
            printf("Plot can only add 'y' values if there is exactly one set of 'x' values\n.");
            return;
        }
        assert((length != 0) && (yValues.size() == length));
        std::vector<double> yValuesDoubles = static_cast<std::vector<double>>(yValues);
        _addYValues(yValuesDoubles);
    }
    
    
    void setPlotArgs(PyObject** plotArgsp, int xValuesListIndex, int yValuesListIndex);
    //void getNextValuesListIndex(int& xValuesIndex, int& yValuesIndex);
    void getMostRecentlyAddedValuesIndices(int& xValuesIndex, int& yValuesIndex);
    
    // prevents auto-incrementing the 'currentXValuesIndex' on calls to plot
    // this must be called in order to allow use of a single X dataset for multiple Y datasets
    void setUseOneXDataset(bool useOneXDataset);
    int getDataLength();
    
   
    
 private:
    // vectors of python lists of x and y values.
    // these must both be the same length,
    // or xValuesListpVector can have a size of 1,
    // in which case all yValuesListp's will use that when being plot.
    std::vector<PyObject*> xValuesListpVector;
    std::vector<PyObject*> yValuesListpVector;
    bool useOneXDataset = false; // set to true if 'setUseOneXDataset(true)' is called.
    int currentXValuesIndex = -1;
    int currentYValuesIndex = -1;
    
    
    void _addValues(std::vector<double> &xValues, std::vector<double> &yValues);
    void _addYValues(std::vector<double> &yValues);
    
    // length is the length of the signal/data -- it has nothing to do with the number of signals (that is, the size of [x/y]ValuesListpVector).
    int length;
    
};

#endif /* Plot_hpp */
