//
//  DataSetup.hpp
//  delegans
//
//  Created by Sean Grimes on 11/10/15.
//  Copyright © 2015 delegans group. All rights reserved.
//

#pragma once

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <sstream>
#include <unordered_map>
#include <fstream>
#include <time.h>
#include "NumUtils.hpp"
#include "StrUtils.hpp"
#include "NeuronInfoModule.hpp"
#include "ElementInfoModule.hpp"
#include "MuscleInfoModule.hpp"
#include "MassPoint.hpp"
#include "FileAssistant.hpp"
#include "Geometrician.hpp"
#include "AblationStation.hpp"
#include "Blade.hpp"
#include "Probe.hpp"
#include "ExeTimer.hpp"
#include "Gymnasium.hpp"

using std::vector;
using std::string;
using std::set;
using std::endl;
using std::cout;
using std::unordered_map;
using std::stringstream;
using std::getline;
using std::ofstream;

struct MuscleWeight {
    string name;
    float weight;
};

class DataSteward{
public:
    
    DataSteward();
    void init();
   
    static vector<unordered_map<string,float>> muscleActivationsGJ;
    static vector<unordered_map<string,float>> muscleActivationsCS;
    
    /* NOTE NOTE NOTE: core should probably hold these.. not DataSteward... */
    vector<vector<float>> kernelVoltages;
    
    
    
    
    
    float maxGapWeight;
    float maxChemWeight;
    

    void saveCurrentConnectome();
  
public:
    static double constexpr CLOCKS_PER_MS = CLOCKS_PER_SEC / 1000;
#warning -- NUM_NEURONS_CLOSEST_LARGER_MULTIPLE_OF_8 in Core.hpp uses NUM_ELEMS... NUM_ELEMS either must remain a multiple of 8, or we need to change how we set NUM_NEURONS_CLOSEST_LARGER_MULTIPLE_OF_8
    static unsigned int NUM_ELEMS;
    static unsigned int NUM_ROWS;
    static unsigned int LEN;
    
   
    MuscleInfoModule** mim;
    NeuronInfoModule** nim;
    std::vector<ElementInfoModule*> elements;
    
   
    //////////////////// NEW ///////////////////
public:
    const static int CSV_OFFSET = 2;// first two rows and columns in both csv files are column/row information
    static int CSV_ROWS;
    static int CSV_COLS;
    static int CONNECTOME_ROWS;
    static int CONNECTOME_COLS;
    
    const static int MAX_ELEMENTS = 200;
    static unsigned int NUM_NEURONS_CLOSEST_LARGER_MULTIPLE_OF_8; 
    void createElements();
    std::vector<std::vector<std::string>> csvDat;
    AblationStation ablator;
    void createConnections();
    std::unordered_map<std::string,int> officialNameToIndexMap;
    std::unordered_map<int,std::string> officialIndexToNameMap;
    std::vector<std::string*> officialNamepVector; // change name to master
    Blade* inputVoltages;
    Blade* outputVoltages;
    Blade* gaps;
    Blade* chems;
    Blade* chemContrib;
    Blade* gapContrib;
    void initializeBlades();
    void fillInputVoltageBlade();
    void setupOpenCL(size_t global, size_t local);
    void initializeOpenCL();
    void pushOpenCLBuffers();
    void readOpenCLBuffers();
    void setOpenCLKernelArgs();
    void initializeData();
    cl_program program;
    cl_kernel kernel;
    char *programBuffer;
    CLHelperClass clhelper;
    cl_uint rowCount;
    cl_uint colCount;
    cl_float gapNormalizer; // we divide gap weights by this
    cl_float chemNormalizer; // we divide chem weights by this
    Probe* probe;
    void enqueueKernel(size_t global, size_t local);
    void printReport(int num_runs);
    void cleanUpOpenCL();
    void cleanUp();
    void feedProbe();
    void run(size_t global, size_t local, int numIterations = 1);
    int currentIteration;
    void updateOutputVoltageVector();
    std::vector<float> outputVoltageVector;
    std::vector<float> getOutputVoltageVector();
    static int numMainLoops;
    static int numKernelLoops;
    Gymnasium gym;
    
    
    // Timing variables
    clock_t start_time_total;
    clock_t stop_time_total;
    clock_t start_setup_time;
    clock_t stop_setup_time;
    clock_t start_opencl_time;
    clock_t stop_opencl_time;
    clock_t start_memtran_time;
    clock_t stop_memtran_time;
    clock_t start_reset_time;
    clock_t stop_reset_time;
    double total_run_time{0};
    double setup_run_time{0};
    double opencl_run_time{0};
    double opencl_mem_time{0};
    double reset_time{0};
    
    ExeTimer total_timer_;
    ExeTimer setup_timer_;
};
