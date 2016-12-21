//
//  Matrix.hpp
//  pelegans
//
//  Created by Sean Grimes on 11/10/15.
//  Copyright © 2015 Sean Grimes. All rights reserved.
//

#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>
#include <fstream>
#include <time.h>
#include "NumUtils.hpp"
#include "StrUtils.hpp"
#include <mmintrin.h>
#include <xmmintrin.h>
using std::vector;
using std::string;
using std::endl;
using std::cout;
using std::unordered_map;
using std::stringstream;
using std::getline;
using std::ofstream;


class Matrix{
public:
    Matrix();
    // Don't use the below constructor yet...it's for future library integration
    Matrix(float **matrix, size_t num_rows, size_t num_cols);
    
    // It's cool, they get inlined
    inline size_t get_1d_index(size_t arr_width, int row_num, int col_num);
    inline void set_1d_element(float *arr, size_t arr_width, int row_num, int col_num, float value);
    void transpose(float **matrix, float **matrix_t, size_t num_rows, size_t num_cols);
    void populate_cs_gj_matricies(float **cs_matrix, float **gj_matrix, size_t num_rows, size_t num_cols);
    
    void transpose_SIMD(float **matrix, size_t num_rows, size_t num_cols);
    void transpose_SIMD(float **matrix, float **matrix_t, size_t num_rows, size_t num_cols);
    inline void doSSE(float *A, float *B, const int lda, const int ldb);
    
    
    // NOTE: The below 3 will cause memory leaks if you don't delete the returned arrays
    float **transpose(float **matrix, size_t num_rows, size_t num_cols);
    float *convert_2d_to_1d(float **matrix, size_t num_rows, size_t num_cols);
    float **convert_1d_to_2d(float *matrix, size_t num_rows, size_t num_cols);
    float *get_init_vector(size_t num_rows);
    void clean_alloced_mem(); // Needs to be setup in the future.
    float *get_polarity_vector(size_t num_rows);
    
private:
    vector<vector<string>> query_wdm();
    unordered_map<string, int> map_name_to_id(vector<vector<string>> &wdm_response);
    
private:
    // For future use, don't touch or Jackie will come after you.
    float **_matrix;
    float **_matrix_t;
    
};