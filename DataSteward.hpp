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
    
    enum ElementType {SENSORY, MOTOR, INTER, SENSORYMOTOR, WFK};
    
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
    std::vector<ElementInfoModule*> bioElements;
    
    const int NUM_MUSCLES = 95;
    const int NUM_CC = 26;
    const float CENTER_CYLINDER_HEIGHT = .5; // meters, so, 50cm
    const float CENTER_CYLINDER_DIAM = .05; // meters, so, 5cm
    
    
    int num_cc_mass_points;
    MassPoint** centerCylinderMassPoints;
/* END BioData */
    
};
