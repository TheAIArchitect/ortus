//
//  Pyterpreter.cpp
//  Plotr
//
//  Created by andrew on 1/12/17.
//

#include "Pyterpreter.hpp"

int Pyterpreter::totalModuleCount = 0;

Pyterpreter::Pyterpreter(){
    char name[] = "Plotr"; // silence compiler warning about const strings
    Py_SetProgramName(name);  // optional but recommended
    Py_Initialize();
    
    PyObject* pyplotname = PyString_FromString("matplotlib.pyplot");
    PyObject* pylabname  = PyString_FromString("pylab");
    if(!pyplotname || !pylabname) { throw std::runtime_error("couldnt create string"); }
    
    PyObject* pymod = PyImport_Import(pyplotname);
    Py_DECREF(pyplotname);
    if(!pymod) { throw std::runtime_error("Error loading module matplotlib.pyplot!"); }
    
    PyObject* pylabmod = PyImport_Import(pylabname);
    Py_DECREF(pylabname);
    if(!pymod) { throw std::runtime_error("Error loading module pylab!"); }
    
    totalModuleCount = numPyPlotModules + numPyLabModules;
    pyObjectps = new PyObject*[totalModuleCount];
    int i = 0;
    for (i = 0; i < numPyPlotModules; ++i){
        printf("module name: %s\n",pyPlotModules[i].c_str());
        nameToIndex[pyPlotModules[i]] = i;
        pyObjectps[i] = PyObject_GetAttrString(pymod, pyPlotModules[i].c_str());
    }
    // start where we left off.
    for (int j = i; j < totalModuleCount; ++j){
        printf("module name: %s\n",pyLabModules[i-j].c_str());
        nameToIndex[pyLabModules[i-j]] = j;
        pyObjectps[j] =  PyObject_GetAttrString(pylabmod, pyLabModules[i-j].c_str());
    }
    
    for (i = 0; i < totalModuleCount; ++i){
        if (i < numPyPlotModules){
            if (!pyObjectps[i]){
                std::string s;
                s = "Couldn't open PyPlot function '"+pyPlotModules[i]+"'";
                throw std::runtime_error(s.c_str());
            }
            if (!PyFunction_Check(pyObjectps[i])){// is this check even necessary??
                std::string s;
                s = "Presumed PyLab function '"+pyLabModules[i]+"' not a PyFunction";
                throw std::runtime_error(s.c_str());
            }
        }
    }
    
    emptyPythonTuple = PyTuple_New(0);
    
}

Pyterpreter::~Pyterpreter(){
    Py_Finalize();
}

PyObject* Pyterpreter::getPyObject(std::string name){
    int index = nameToIndex[name];
    return pyObjectps[index];
    
}


