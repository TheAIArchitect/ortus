//
//  Interpreter.hpp
//  Plotr
//
//  Created by onyx on 10/26/16.
//  Copyright © 2016 Ecodren Research. All rights reserved.
//

#ifndef Interpreter_hpp
#define Interpreter_hpp

#include <iostream>
#include <python2.7/Python.h>


class Interpreter {
    
public:
        PyObject *s_python_function_show;
        PyObject *s_python_function_save;
        PyObject *s_python_function_figure;
        PyObject *s_python_function_plot;
        PyObject *s_python_function_pie;
        PyObject *s_python_function_subplot;
        PyObject *s_python_function_legend;
        PyObject *s_python_function_xlim;
        PyObject *s_python_function_ylim;
        PyObject *s_python_function_title;
        PyObject *s_python_function_axis;
        PyObject *s_python_function_xlabel;
        PyObject *s_python_function_ylabel;
        PyObject *s_python_function_grid;
        PyObject *s_python_empty_tuple;
        
        /* For now, _interpreter is implemented as a singleton since its currently not possible to have
         multiple independent embedded python interpreters without patching the python source code
         or starting a separate process for each.
         
         http://bytes.com/topic/python/answers/793370-multiple-independent-python-interpreters-c-c-program
         */
        
        static Interpreter& get() {
            static Interpreter ctx;
            return ctx;
        }
        
private:
    Interpreter() {
        char name[] = "plotting"; // silence compiler warning about const strings
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
        
        s_python_function_show = PyObject_GetAttrString(pymod, "show");
        s_python_function_figure = PyObject_GetAttrString(pymod, "figure");
        s_python_function_plot = PyObject_GetAttrString(pymod, "plot");
        s_python_function_pie = PyObject_GetAttrString(pymod, "pie");
        s_python_function_subplot = PyObject_GetAttrString(pymod, "subplot");
        s_python_function_legend = PyObject_GetAttrString(pymod, "legend");
        s_python_function_ylim = PyObject_GetAttrString(pymod, "ylim");
        s_python_function_title = PyObject_GetAttrString(pymod, "title");
        s_python_function_axis = PyObject_GetAttrString(pymod, "axis");
        s_python_function_xlabel = PyObject_GetAttrString(pymod, "xlabel");
        s_python_function_ylabel = PyObject_GetAttrString(pymod, "ylabel");
        s_python_function_grid = PyObject_GetAttrString(pymod, "grid");
        s_python_function_xlim = PyObject_GetAttrString(pymod, "xlim");
        s_python_function_save = PyObject_GetAttrString(pylabmod, "savefig");
        
        if(        !s_python_function_show
           || !s_python_function_figure
           || !s_python_function_plot
           || !s_python_function_pie
           || !s_python_function_subplot
           || !s_python_function_legend
           || !s_python_function_ylim
           || !s_python_function_title
           || !s_python_function_axis
           || !s_python_function_xlabel
           || !s_python_function_ylabel
           || !s_python_function_grid
           || !s_python_function_xlim
           || !s_python_function_save
           )
        { throw std::runtime_error("Couldn't find required function!"); }
        
        if(    !PyFunction_Check(s_python_function_show)
           || !PyFunction_Check(s_python_function_figure)
           || !PyFunction_Check(s_python_function_plot)
           || !PyFunction_Check(s_python_function_pie)
           || !PyFunction_Check(s_python_function_subplot)
           || !PyFunction_Check(s_python_function_legend)
           || !PyFunction_Check(s_python_function_ylim)
           || !PyFunction_Check(s_python_function_title)
           || !PyFunction_Check(s_python_function_axis)
           || !PyFunction_Check(s_python_function_xlabel)
           || !PyFunction_Check(s_python_function_ylabel)
           || !PyFunction_Check(s_python_function_grid)
           || !PyFunction_Check(s_python_function_xlim)
           || !PyFunction_Check(s_python_function_save)
           )
        { throw std::runtime_error("Python object is unexpectedly not a PyFunction."); }
        
        s_python_empty_tuple = PyTuple_New(0);
    }
    
    ~Interpreter() {
        Py_Finalize();
    }
};


#endif /* Interpreter_hpp */
