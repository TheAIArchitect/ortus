//
//  Core.hpp
//  delegans
//
//  Created by Sean Grimes on 11/20/15.
//  Copyright © 2015 delegans group. All rights reserved.
//

#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <time.h>
#include <unordered_map>
//#include "DataSteward.hpp"
#include "CLHelperClass.hpp"
#include "Logger.hpp"
#include "NumUtils.hpp"
#include "StrUtils.hpp"
#include "ExeTimer.hpp"
#include "Probe.hpp";

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif
using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::unordered_map;

class Core{
public:
    void init(size_t number_of_loops);
    void run(size_t global, size_t local);
    void clean_up();
    void print_report(int num_runs);
    vector<vector<float>> get_voltage_vector();
    size_t get_number_of_loops();
    DataSteward stewie;
    Probe* probe;
    static int numMainLoops;
    static int numKernelLoops;
    float* zeros1D;

    
private:
    // Matrix functions
    void init_data_containers();
    void fill_data_containers();
    void transpose_and_fill_1d_representation();
    
    // OpenCL functions
    void setup_opencl(size_t global, size_t local, float * to_processor_voltages);
    void run_in_loop(size_t global, size_t local);
    void init_cl();
    void push_buffers();
    void pushVoltageBuffer(bool isFirst);
    void set_kernel_args(size_t local);
    void enqueue_kernel(size_t global, size_t local);
    void read_voltages_from_kernel();
    void clean_up_kernel();
    
    // General
    void clean_up_memory();
    void feedProbe();
    
    
private:
    // Helpers
    CLHelperClass clhc;
    //VectorProcessor vp;
    
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
    
    // Matrix variables
    //float *voltages;
    float *polarities;
    float **cs_2d_matrix;
    float **gj_2d_matrix;
    float **cs_2d_matrix_t;
    float **gj_2d_matrix_t;
    float *cs_matrix;
    float *gj_matrix;
    float *inputKernelActivations;
    float *returned_voltages;
    float *temp_voltages;
    volatile bool looping = false;
    vector<vector<float>> vector_of_returned_voltages;
    size_t _number_of_loops;
    volatile size_t _iteration_counter{0};
    unordered_map<string, int> neuron_names_to_index_map;
    unordered_map<string, vector<string>> motorneurons_to_muscles_map;
    
    // OpenCL Variables
    
    cl_program program;
    cl_kernel kernel;
    char *programBuffer;
    cl_mem neuron_voltages;
    cl_mem polarity_vec;
    cl_mem cs_mat;
    cl_mem gj_mat;
    cl_mem cl_returned_voltages;
    
    
    // for the probe
    float** cs_2d_contrib;
    float** gj_2d_contrib;
    // where the data goes when we transform it from 1d to 2d, but still transposed (so, each row is a postsynaptic neuron, and cols are presynaptic)
    float** cs_2d_contrib_t;
    float** gj_2d_contrib_t;
    // where we read data to from the cs_ and gj_ contrib buffers
    float* cs_1d_contrib;
    float* gj_1d_contrib;
    // the actual buffers
    cl_mem cs_contrib;
    cl_mem gj_contrib;
    
    
};
