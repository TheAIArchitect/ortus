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
#include "WormWiringDatabaseManager.hpp"
#include "NumUtils.hpp"
#include "StrUtils.hpp"
#include "NeuronInfoModule.hpp"
#include "ElementInfoModule.hpp"
#include "MuscleInfoModule.hpp"
#include "WormwiringCSVToSQLite.hpp"
#include "MassPoint.hpp"
#include "FileShit.hpp"
#include "WormwiringCSVToSQLite.hpp"
#include "Geometrician.hpp"
#include "PolarityConfigurator.hpp"
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
    
    
    // Don't use the below constructor yet...it's for future library integration
    //DataSetup(float **matrix, size_t num_rows, size_t num_cols);
    
    
    // It's cool, they get inlined
    inline size_t get_1d_index(size_t arr_width, int row_num, int col_num);
    inline void set_1d_element(float *arr, size_t arr_width, int row_num, int col_num, float value);
    void transpose(float **matrix, float **matrix_t, size_t num_rows, size_t num_cols);
    void populate_cs_gj_matricies(float **cs_matrix, float **gj_matrix, unordered_map<string,int>& neuron_names_to_index_map, size_t num_rows, size_t num_cols);
    
    float **cs_matrix;
    float **gj_matrix;
    int conn_matricies_size;
    float maxGapWeight;
    float maxChemWeight;
    
    // NOTE: The below 3 will cause memory leaks if you don't delete the returned arrays
    float **transpose(float **matrix, size_t num_rows, size_t num_cols);
    float *convert_2d_to_1d(float **matrix, size_t num_rows, size_t num_cols);
    float **convert_1d_to_2d(float *matrix, size_t num_rows, size_t num_cols);
    float *get_init_vector(size_t num_rows);
    void clean_alloced_mem(); // Needs to be setup in the future.
    float *get_polarity_vector(size_t num_rows);
    bool read_data_csv_files();
    bool mapNeuronsToMuscles();
    void saveCurrentConnectome();
    //unordered_map<string, ElementType> typesMap;
    unordered_map<string, vector<MuscleWeight>> n2mGjMap;
    unordered_map<string, vector<MuscleWeight>> n2mCsMap;
    unordered_map<string, int> n2idxMap;
    unordered_map<int,string> idx2nMap;
    vector<string> muscles;
   
public:
    static double constexpr CLOCKS_PER_MS = CLOCKS_PER_SEC / 1000;
#warning -- NUM_NEURONS_CLOSEST_LARGER_MULTIPLE_OF_8 in Core.hpp uses NUM_ELEMS... NUM_ELEMS either must remain a multiple of 8, or we need to change how we set NUM_NEURONS_CLOSEST_LARGER_MULTIPLE_OF_8
    static unsigned int NUM_ELEMS;
    static unsigned int NUM_ROWS;
    static unsigned int LEN;
    
private:
    vector<vector<string>> query_wdm();
    unordered_map<string, int> map_name_to_id();
    
private:
    // For future use, don't touch or Jackie will come after you.
    float **_matrix;
    float **_matrix_t;
    
private: // MIGHT ONLY BE TEMPORARY
    bool push_neuron_data(vector<int> post_syn_ids, float fake_weight_from_section, int syn_type, int pre_id);
    unordered_map<string,int> neurons_not_pre_to_anything_with_temp_id;//temp id is negative
    vector<vector<int>> vec_of_post_syn_ids;
    vector<float> vec_of_fake_weights;
    vector<int> vec_of_syn_types;
    vector<int> vec_of_pre_ids;
    
/** NOTE: MERGED IN FROM "BioData" */
public:
    // NOTE: this taken from GraphicsGovernor
    Geometrician geometrician;
    MassPoint** bezPts;
    int numBezPts;
    //void makeGraph(std::vector<GapJunction> gaps, std::vector<ChemSynapse> chems, NeuronInfoModule* nim, MuscleInfoModule* mim, int nim_count, int mim_count);
    void makeModules();
    void makeMuscleModules();
    void addElementInfoToModules();
    void makeNeuronModules();
    void generateBodyMassPoints();
    void generateCenterCylinderMassPoints();
    
    float* neuron_colors;
    float* neuron_positions;
    
    MuscleInfoModule** mim;
    NeuronInfoModule** nim;
    MassPoint** bodyMassPoints;
    int num_body_mass_points;
    MassPoint** neuronMassPoints;
    int num_neuron_mass_points;
    int num_neurons_in_json;
    std::vector<ElementInfoModule*> elements;
    
    const int NUM_MUSCLES = 95;
    const int NUM_CC = 26;
    const float CENTER_CYLINDER_HEIGHT = .5; // meters, so, 50cm
    const float CENTER_CYLINDER_DIAM = .05; // meters, so, 5cm
    
    
    
    int num_cc_mass_points;
    MassPoint** centerCylinderMassPoints;
/* END BioData */
    
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
