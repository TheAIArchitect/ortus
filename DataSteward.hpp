//
//  DataSetup.hpp
// ortus
//
// Andrew W.E. McDonald
// Sean Grimes
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
#include "FileAssistant.hpp"
#include "AblationStation.hpp"
#include "Blade.hpp"
#include "Probe.hpp"
#include "Timer.hpp"
#include "Gymnasium.hpp"
#include "CLHelper.hpp"


class DataSteward{
    
public: // super important variables
    std::unordered_map<std::string,int> officialNameToIndexMap;
    std::unordered_map<int,std::string> officialIndexToNameMap;
    std::vector<std::string*> officialNamepVector; // change name to master
    Blade<float>* inputVoltages;
    Blade<float>* outputVoltages;
    Blade<float>* gaps;
    Blade<float>* chems;
    Blade<float>* chemContrib;
    Blade<float>* gapContrib;
    Blade<cl_uint>* rowCount;
    Blade<cl_uint>* colCount;
    Blade<cl_float>* gapNormalizer; // we divide gap weights by this
    Blade<cl_float>* chemNormalizer; // we divide chem weights by this
    
public:
    
    DataSteward();
    void init();
    void createElements();
    void createConnections();
    void initializeData();
    bool writeConnectome(std::string csv_name);
    
    
    void setupOpenCL(size_t global, size_t local);
    void initializeOpenCL();
    void initializeBlades();
    void fillInputVoltageBlade();
    void pushOpenCLBuffers();
    void readOpenCLBuffers();
    void setOpenCLKernelArgs();
    void enqueueKernel(size_t global, size_t local);
    void printReport(int num_runs);
    void cleanUpOpenCL();
    void cleanUp();
    void feedProbe();
    void run(size_t global, size_t local, int numIterations = 1);
    void updateOutputVoltageVector();
    std::vector<float> getOutputVoltageVector();
    
    
public:
    std::vector<float> outputVoltageVector;
    std::vector<ElementInfoModule*> elements;
    std::vector<std::vector<std::string>> csvDat;
    float maxGapWeight;
    float maxChemWeight;
    Probe* probe;
    AblationStation ablator;
    Gymnasium gym;
    int currentIteration;
    std::vector<std::vector<float>> kernelVoltages; // might need to move
    //MuscleInfoModule** mim;
    //NeuronInfoModule** nim;
    
    
    // OpenCL related variables
    CLHelper clhelper;
    cl_program program;
    cl_kernel kernel;
    char *programBuffer;
    
    
    
    // static and/or constant vars
    static double constexpr CLOCKS_PER_MS = CLOCKS_PER_SEC / 1000;
#warning -- NUM_NEURONS_CLOSEST_LARGER_MULTIPLE_OF_8 in Core.hpp uses NUM_ELEMS... NUM_ELEMS either must remain a multiple of 8, or we need to change how we set NUM_NEURONS_CLOSEST_LARGER_MULTIPLE_OF_8
    static unsigned int NUM_ELEMS;
    static unsigned int NUM_ROWS;
    const static int CSV_OFFSET = 2;// first two rows and columns in both csv files are column/row information
    static int CSV_ROWS;
    static int CSV_COLS;
    static int CONNECTOME_ROWS;
    static int CONNECTOME_COLS;
    const static int MAX_ELEMENTS = 200;
    static unsigned int NUM_NEURONS_CLOSEST_LARGER_MULTIPLE_OF_8;
    
    static int numMainLoops;
    static int numKernelLoops;
    
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
    Timer total_timer_;
    Timer setup_timer_;
    
    
    
    
};
