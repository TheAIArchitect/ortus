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
#include "Gymnasium.hpp"
#include "CLHelper.hpp"


class DataSteward{
    
public: // super important variables
    std::unordered_map<std::string,int> officialNameToIndexMap;
    std::unordered_map<int,std::string> officialIndexToNameMap;
    std::vector<std::string*> officialNamepVector; // change name to master
    // NOTE: the voltages Blade gets read from and written to by opencl!!!
    Blade<float>* voltages;
    // NOTE: the outputVotlageHistory Blade has one neuron's voltages per ROW (so, it's an n x 5 'matrix', if we store 5 previous values)
    //NOTE2: maybe we should make 2 of these -- one for reading only, one for writing only... maybe same for voltages, idk...
    //NOTE3: look into CL_MEM_USE_HOST_PTR and clEnqueueMapBuffer
    Blade<float>* outputVoltageHistory;
    Blade<float>* gaps;
    Blade<float>* chems;
    Blade<float>* chemContrib;
    Blade<float>* gapContrib;
    
    //Blade<cl_uint>* rowCount;
    //Blade<cl_uint>* colCount;
    Blade<cl_float>* gapNormalizer; // we divide gap weights by this
    Blade<cl_float>* chemNormalizer; // we divide chem weights by this
    
    // (zero indexed): [rowCount, colCount, kernelIterationNum, voltageHistorySize ]
    Blade<cl_int>* metadata;
    
    Blade<cl_float>* deviceScratchPad; // memory is local on device, so size depends upon work-group size
    
protected: // private?
    cl_kernel* kernelp;
    CLHelper* clHelperp;
    
public:
    
    
    DataSteward();
    ~DataSteward();
    void init(size_t openCLWorkGroupSize);
    void createElements();
    void createConnections();
    void initializeData();
    void executePostRunMemoryTransfers();
    
    void growConnectome();
    bool writeConnectome(std::string csv_name);
    
    void setKernelp(cl_kernel* kp);
    void setCLHelperp(CLHelper* clhp);
    
    
    void initializeBlades();
    void updateMetadataBlade(int knernelIterationNum);
    void fillInputVoltageBlade();
    void pushOpenCLBuffers();
    void readOpenCLBuffers();
    void setOpenCLKernelArgs();

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
    size_t openCLWorkGroupSize;
    Probe* probe;
    AblationStation ablator;
    Gymnasium gym;
    std::vector<std::vector<float>> kernelVoltages; // might need to move
    //MuscleInfoModule** mim;
    //NeuronInfoModule** nim;
    
    
  
    
    
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
    const static int VOLTAGE_HISTORY_SIZE = 6; // 5 usable, and the 6th is the 'staging' area -- filled by the current one (but can't be read from because there's no [good] way to ensure other threads have updated theirs)
    const static int METADATA_COUNT = 5; // see 'metadata' definition for metadata metadata. haha.
    const static int XCORR_COMPUTATIONS = 3; // see notes/correlationNotes.txt
    static unsigned int NUM_NEURONS_CLOSEST_LARGER_MULTIPLE_OF_8;
    
    

    
    
    
    
};
