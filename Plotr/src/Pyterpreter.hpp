//
//  Pyterpreter.hpp
//  Plotr
//
//  Created by andrew on 1/12/17.
//

#ifndef Pyterpreter_hpp
#define Pyterpreter_hpp

#include <stdio.h>
#include <string>
#include <unordered_map>
#include <python2.7/Python.h>


class Pyterpreter {
public:
    Pyterpreter();
    ~Pyterpreter();
    PyObject* getPyObject(std::string name);
    
    const static int numPyPlotModules = 32;
    const static int numPyLabModules = 1;
    static int totalModuleCount;
    std::string pyPlotModules[numPyPlotModules] = {"annotate","arrow","axis","colorbar","draw", "eventplot", "figure", "grid","hist", "hist2d", "hold","imsave", "imshow","ioff", "ion","legend","pause","pie","plot", "quiver","scatter","show","specgram","subplot", "title","xcorr","xlabel", "xlim","xticks","ylabel", "ylim","yticks"};
    std::unordered_map<std::string,int> nameToIndex;
    
    std::string pyLabModules[numPyLabModules] = {"savefig"};
    
    PyObject** pyObjectps;
    
    PyObject* emptyPythonTuple;
    
};

#endif /* Pyterpreter_hpp */
