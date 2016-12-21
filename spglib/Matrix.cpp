//
//  Matrix.cpp
//
//  Created by Sean Grimes on 11/10/15.
//  Copyright © 2015 Sean Grimes. All rights reserved.
//

#include "include/Matrix.hpp"
#include "include/ExeTimer.hpp"

#define ROUND_UP(x, s) (((x)+((s)-1)) & -(s))

/*
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif
*/
Matrix::Matrix(){
    fprintf(stderr, "This file was mistakenly migrated from the dev branch, it is far from complete\n");
    fprintf(stderr, "and should not be used for any reason until this message is removed...to make this\n");
    fprintf(stderr, "point more clear I will now be throwing an exception if a cmdlind option has not been defined\n");

#ifndef USE_DEV_CLASSES
    throw std::logic_error("Matrix class is not usable at the moment, see error printout");
#endif
}

Matrix::Matrix(float **matrix, size_t num_rows, size_t num_cols){
    fprintf(stderr, "I told you not to use this constructor!!!\n");
    _matrix = new float*[num_rows];
    for(int i = 0; i < num_rows; ++i){
        _matrix[i] = new float[num_cols];
    }
}

size_t Matrix::get_1d_index(size_t arr_width, int row_num, int col_num){
    return arr_width * row_num + col_num;
}

void Matrix::set_1d_element(float *arr, size_t arr_width, int row_num, int col_num, float value){
    arr[get_1d_index(arr_width, row_num, col_num)] = value;
}

float *Matrix::get_init_vector(size_t num_rows){
    float *vec = new float[num_rows]();
    std::fill_n(vec, num_rows, -0.065f);
    return vec;
}

float *Matrix::convert_2d_to_1d(float **matrix, size_t num_rows, size_t num_cols){
    size_t len = num_rows * num_cols;
    float *flat_arr = new float[len](); // Note the "()", init to zeroed.
    for(int i = 0; i < num_rows; ++i){
        for(int j = 0; j < num_cols; ++j){
            set_1d_element(flat_arr, num_cols, i, j, matrix[i][j]);
        }
    }
    return flat_arr;
}

// NOTE: At the moment this is just returning semi-randomly generated polarities
float *Matrix::get_polarity_vector(size_t num_rows){
    float *pol_vec = new float[num_rows]();
    srand((int)6107101036);
    for(int i = 0; i < num_rows; ++i){
        int rand_1_or_0 = rand() % 2;
        pol_vec[i] = rand_1_or_0;
    }
    return pol_vec;
}

float **Matrix::convert_1d_to_2d(float *matrix, size_t num_rows, size_t num_cols){
    float **matrix_re = new float*[num_rows];
    for(int i = 0; i < num_rows; ++i){
        matrix_re[i] = new float[num_cols]();
    }
    
    for(int i = 0; i < num_rows; ++i){
        for(int j = 0; j < num_cols; ++j){
            matrix_re[i][j] = matrix[get_1d_index(num_cols, i, j)];
        }
    }
    
    return matrix_re;
}

void Matrix::transpose(float **matrix, float **matrix_t, size_t num_rows, size_t num_cols){
    size_t i, j;
    for(i = 0; i < num_rows; ++i){
        for(j = 0; j < num_cols; ++j){
            matrix_t[i][j] = matrix[j][i];
        }
    }
}

/*
void Matrix::transpose_SIMD(float **matrix, size_t num_rows, size_t num_cols){
    for(auto i = 0; i < num_rows; ++i){
        for(auto j = 0; j < num_cols; ++j){
            std::swap(matrix[i][j], matrix[j][i]);
        }
    }
    
    
}
 */

void Matrix::transpose_SIMD(float **matrix, size_t num_rows, size_t num_cols){
    
    int block_size = 16;
    
    int lda = ROUND_UP((int)num_rows, block_size);
    int ldb = ROUND_UP((int)num_cols, block_size);
    
    auto mat_1d_tmp = convert_2d_to_1d(matrix, num_rows, num_cols);
    float* mat_t = (float*)_mm_malloc(sizeof(float)*lda*ldb, 64);
    float* mat_1d = (float*)_mm_malloc(sizeof(float)*lda*ldb, 64);
    int cnt;
    for(cnt = 0; cnt < num_rows*num_cols; ++cnt){
        mat_1d[cnt] = mat_1d_tmp[cnt];
    }
    for(auto i = cnt; i < lda*ldb; ++i){
        mat_1d[i] = 0;
    }
    
    size_t m = num_rows;
    size_t n = num_cols;
    
    
    ExeTimer simd;
    simd.start_timer();
    for(int i=0; i<n; i+=block_size) {
        for(int j=0; j<m; j+=block_size) {
            auto max_i2 = i+block_size < n ? i + block_size : n;
            auto max_j2 = j+block_size < m ? j + block_size : m;
            for(auto i2=i; i2<max_i2; i2+=4) {
                for(auto j2=j; j2<max_j2; j2+=4) {
                    doSSE(&mat_1d[i2*lda +j2], &mat_t[j2*ldb + i2], lda, ldb);
                }
            }
        }
    }
    simd.stop_timer();
    cout << "simd timer: " << simd.get_exe_time_in_ms() << "ms\n";

}

void Matrix::doSSE(float *A, float *B, const int lda, const int ldb) {
    __m128 row1 = _mm_load_ps(&A[0*lda]);
    __m128 row2 = _mm_load_ps(&A[1*lda]);
    __m128 row3 = _mm_load_ps(&A[2*lda]);
    __m128 row4 = _mm_load_ps(&A[3*lda]);
    _MM_TRANSPOSE4_PS(row1, row2, row3, row4);
    _mm_store_ps(&B[0*ldb], row1);
    _mm_store_ps(&B[1*ldb], row2);
    _mm_store_ps(&B[2*ldb], row3);
    _mm_store_ps(&B[3*ldb], row4);
}


float **Matrix::transpose(float **matrix, size_t num_rows, size_t num_cols){
    float **trans;
    trans = new float*[num_rows];
    for(int i = 0; i < num_rows; ++i){
        trans[i] = new float[num_cols]();
    }
    
    transpose(matrix, trans, num_rows, num_cols);
    return trans;
}

void Matrix::populate_cs_gj_matricies(float **cs_matrix, float **gj_matrix, size_t num_rows, size_t num_cols){
    // Cause people don't always do things the right way...
    for(int i = 0; i < num_rows; ++i){ // Move down the columns
        for(int j = 0; j < num_cols; ++j){ // Move down the rows
            cs_matrix[i][j] = 0.0f;
            gj_matrix[i][j] = 0.0f;
        }
    }
    
    // Make the Database Call
    vector<vector<string>> data = query_wdm();
 
    // Get a <map> that allows one to look up an id for a neuron based on name for easy array indexing
    unordered_map<string, int> name_to_id = map_name_to_id(data);
   
    // For each row from the db, get the info that's needed together and start putting the matrix together
    size_t data_size = data.size();
    for(int i = 0; i < data_size; ++i){
        // Some of these vars can be optimized out, but to see how it works leave them for now
        string pre_name = data[i][1];
        int syn_type = NumUtils::strToInt(data[i][7]);
        // Note: if this throws an exception the world is ending or I'm a dumb fucker
        int pre_id = -1;
        try{
            pre_id = name_to_id.at(pre_name);
        }
        catch(std::out_of_range exp){
            fprintf(stderr, "MatrixSetip::populate_matrix --> The reckoning has begun\n");
            exit(-666);
        }
        
        // Determine how many post_syns there are
        int num_post_syns = NumUtils::strToInt(data[i][4]);
        float fake_weight_from_sections = NumUtils::strToDouble(data[i][6]);
        vector<string> post_syn_names;
        vector<int> post_syn_ids;
        // If just 1 post_syn, still use the vector so below code doesn't have to be case specific
        if(num_post_syns == 1)
            post_syn_names.push_back(data[i][3]);
        else
            post_syn_names = StrUtils::parseOnCharDelim(data[i][3], ',');
        // Convert the post syn name vector to a post syn ID vector using the name to id mapping
        for(int j = 0; j < num_post_syns; ++j){
            int post_syn_id = -1;
            
            /*
             * The try/catch is here again because there are some bad names still in the DB.
             * They exist in the post_list, and they aren't all caught in the above try and catch because
             *  some of the 'bad names' aren't pre to anything, they only exist as post to something 'good'
             * Therefore, when we run across a bad name, set the post_syn_id for that connection to -1,
             *  and in the next loop (that sets up the actual matrix) when the post_syn_id is -1
             *  we'll ignore the connection for now and leave the weight in the matrix as 0 so it
             *  will get ignored in the OpenCL code
             */
            try{
                post_syn_id = name_to_id.at(post_syn_names[j]);
            }
            catch(std::out_of_range exp){
                post_syn_id = -1;
            }
            post_syn_ids.push_back(post_syn_id);
        }
        
        // Finally, add the elements to the matrix using the IDs and the fake weight
        for(int j = 0; j < num_post_syns; ++j){
            int post_id = post_syn_ids[j];
            // Check to make sure we aren't dealing with one of the 'bad names' still in the DB
            // If we are, pretend the connection doesn't exist
            float weight = fake_weight_from_sections;
            if(post_id == -1)
                weight = 0.0f;
            
            // Match the row id (i.e. "pre") with the column id(s) (i.e. "post(s)")
            // This is a chemical synapse
            if(syn_type == 1){
                cs_matrix[pre_id][post_id] = weight;
            }
            // This is not a chemical synapse
            else{
                gj_matrix[pre_id][post_id] = weight;
            }
            
        }
    }
}

/*
0|id|integer|0||1
1|name|text|0||0
2|description|text|0||0
3|post_list|text|0||0
4|num_post_synapses|integer|0||0
5|num_synapses|integer|0||0
6|num_sections|integer|0||0
7|synapse_type|integer|0||0  --> CS == 1, other == GJ
8|synapse_id|integer|0||0
9|add_date|text|0||0
*/


// Private
vector<vector<string>> Matrix::query_wdm(){
    //WormWiringDatabaseManager wdm;
    vector<vector<string>> wdm_res;
    //wdm_res = wdm.select_records("STILL_DOES_NOT_FUCKING_MATTER");
    //return wdm_res;
    return wdm_res;
}

unordered_map<string, int> Matrix::map_name_to_id(vector<vector<string> > &wdm_response){
    unordered_map<string, int> name_to_id_map;

    std::cout << "\n\n\n";
    
    int unique_id_counter = 0;
    // Run through the items and put together a name --> id mapping
    size_t runto = wdm_response.size();
    for(int i = 0; i < runto; ++i){
        // Grab the name from the DB return
        string name = wdm_response[i][1];
        unordered_map<string, int>::iterator it;
     
        // Check to see if the name is already in the map
        it = name_to_id_map.find(name);
        if(it == name_to_id_map.end()){
            // The name does not exist in the map yet, so add it and an id
            name_to_id_map[name] = unique_id_counter;
            unique_id_counter++;
        }
        else{
            // Was going to do something here....maybe I will, leave it for now.
            continue;
        }
    }
    return name_to_id_map;
}