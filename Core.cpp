//
//  Core.cpp
//  delegans
//
//  Created by Sean Grimes on 11/20/15.
//  Copyright © 2015 delegans group. All rights reserved.
//

#include "Core.hpp"

int Core::numMainLoops = 1000;
int Core::numKernelLoops = 1;
unsigned int Core::NUM_NEURONS_CLOSEST_LARGER_MULTIPLE_OF_8 = 0;

void Core::init(size_t number_of_loops){
    total_timer_.start_timer();
    _iteration_counter = 0;
    if(number_of_loops < 1)
        throw std::logic_error("You need to call the kernel at least once");
    else if(number_of_loops > 100000)
        throw std::runtime_error("Currently we're limited to 100,000 loops for processing reasons");
    else
        _number_of_loops = number_of_loops;
    
    
    // Use the DataSteward class to load and setup neuron data
    stewie.init();
    // this can *only* be set after DataSteward is initialized, because NUM_ELEMS isn't set until then.
    int modEight = DataSteward::NUM_ELEMS % 8;
    NUM_NEURONS_CLOSEST_LARGER_MULTIPLE_OF_8 = DataSteward::NUM_ELEMS + (8 - modEight);
    //stewie.makeModules(); // NOTE NOTE NOTE: this was for the worm, though part of it was a repeat of what was going on in the csv reader (creating NIMs and MIMs), due to a sloppy code combination. the function that makes muscle modules that is called by this could be useful, though.
    zeros1D = new float[DataSteward::NUM_ELEMS*DataSteward::NUM_ELEMS];
    for (int i = 0; i < DataSteward::NUM_ELEMS; ++i){
        for (int j = 0; j < DataSteward::NUM_ELEMS; ++j){
            zeros1D[i*DataSteward::NUM_ELEMS+j] = 0.f;
        }
    }
    probe = new Probe(&stewie);
    // connectome error check
    printf("Running connectome error check...\n");
    int numElems = stewie.bioElements.size();
    for (int i = 0; i < numElems; i++){
        int numOuts = stewie.bioElements[i]->out_conns.size();
        printf("Neuron %s => ",stewie.bioElements[i]->name.c_str());
        for (int j = 0; j < numOuts; j++){
            Connection* tConn = &stewie.bioElements[i]->out_conns[j];
            printf("%s, ",tConn->toString().c_str());
            // NOTE NOTE NOTE: INSERT CHECK for connectome -- random line or something
        }
        printf("\n");
    }
    printf("Connectome ok\n");
    
    
    printf("core ok\n");
    init_data_containers();
    fill_data_containers();
    transpose_and_fill_1d_representation();
    // CHECK for matrix connectome
    for (int i = 0; i < DataSteward::NUM_ELEMS; i++){
        printf("[");
        for (int j = 0; j < DataSteward::NUM_ELEMS; j++){
            printf("\t");
            printf("%.2f",cs_2d_matrix[i][j]);
        }
        printf("]\n");
    }
    for (int i = 0; i < numElems; i++){
        int numOuts = stewie.bioElements[i]->out_conns.size();
        printf("Neuron %s => ",stewie.bioElements[i]->name.c_str());
        for (int j = 0; j < numOuts; j++){
            Connection* tConn = &stewie.bioElements[i]->out_conns[j];
            if (tConn->conntype == GAP){
                if (tConn->weight != gj_2d_matrix[tConn->pre->element_id][tConn->post->element_id]){
                    printf("Error found in gap junction 2D matrix. Remedy situation in order to proceed.\n");
                    exit(94);
                }
            }
            else{
                if (tConn->weight != cs_2d_matrix[tConn->pre->element_id][tConn->post->element_id]){
                    printf("Error found in chemical synapse 2D matrix. Remedy situation in order to proceed.\n");
                    exit(94);
                }
            }
        }
    }
    // END CHECK
}

void Core::run(size_t global, size_t local){
    for(int i = 0; i < _number_of_loops; ++i){
        // Do different things for the first iteration vs the rest
        fprintf(stderr, "ITERATION #%d\n",_iteration_counter);
        if(_iteration_counter == 0){
            looping = true;
            // Sets up the buffers on the GPU and runs 1st iteration of kernel
            setup_opencl(global, local, nullptr);
        }
        else{
            run_in_loop(global, local);
        }
        ++_iteration_counter;
    }
    fprintf(stderr,"LEAVING...\n");
    looping = false;
#warning read_voltages_from_kernel doesn't persist over multiple external loops
    read_voltages_from_kernel();
   
}

vector<vector<float>> Core::get_voltage_vector(){
    return vector_of_returned_voltages;
}

void Core::clean_up(){
    clean_up_kernel();
    clean_up_memory();
    total_timer_.stop_timer();
    total_run_time = total_timer_.get_exe_time_in_ms();
}

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE ////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/* Matrix Functions */
void Core::init_data_containers(){
    setup_timer_.start_timer();
    
    cs_2d_matrix = new float*[DataSteward::NUM_ELEMS];
    gj_2d_matrix = new float*[DataSteward::NUM_ELEMS];
    cs_2d_matrix_t = new float*[DataSteward::NUM_ELEMS];
    gj_2d_matrix_t = new float*[DataSteward::NUM_ELEMS];
    cs_2d_contrib = new float*[DataSteward::NUM_ELEMS];
    gj_2d_contrib = new float*[DataSteward::NUM_ELEMS];
    cs_2d_contrib_t = new float*[DataSteward::NUM_ELEMS];
    gj_2d_contrib_t = new float*[DataSteward::NUM_ELEMS];
    for(int i = 0; i < DataSteward::NUM_ROWS; ++i){
        // DataSteward::NUM_ELEMS for rows and cols b/c this needs to be a square matrix
        cs_2d_matrix[i] = new float[DataSteward::NUM_ELEMS]();
        gj_2d_matrix[i] = new float[DataSteward::NUM_ELEMS]();
        cs_2d_matrix_t[i] = new float[DataSteward::NUM_ELEMS]();
        gj_2d_matrix_t[i] = new float[DataSteward::NUM_ELEMS]();
        // for the probe
        cs_2d_contrib[i] = new float[DataSteward::NUM_ELEMS]();;
        gj_2d_contrib[i] = new float[DataSteward::NUM_ELEMS]();;
        cs_2d_contrib_t[i] = new float[DataSteward::NUM_ELEMS]();;
        gj_2d_contrib_t[i] = new float[DataSteward::NUM_ELEMS]();;
    }
    
    // This is the 'matrix' that the kernel will play with
    // At the beginning the first 'row' is populated with the initial voltages
    // Each 'row' will be populated with new voltages for each iteration
    inputKernelActivations = new float[DataSteward::NUM_ELEMS * _number_of_loops]();
    
    // Spent hours trying to get the same buffer to read and write from kernel
    // Couldn't get consistent values, so we're going to use two...
#warning TODO: get this working on a single buffer
    returned_voltages = new float[DataSteward::NUM_ELEMS * _number_of_loops]();
    cs_1d_contrib = new float[DataSteward::NUM_ELEMS * DataSteward::NUM_ELEMS];
    gj_1d_contrib = new float[DataSteward::NUM_ELEMS * DataSteward::NUM_ELEMS];
}

void Core::fill_data_containers(){
    temp_voltages = stewie.get_init_vector(DataSteward::NUM_ELEMS);
    // Put the voltages in the first row of the fake matrix
    for(int i = 0; i < DataSteward::NUM_ELEMS; ++i){
        inputKernelActivations[i] = temp_voltages[i];
    }
    polarities = stewie.get_polarity_vector(DataSteward::NUM_ROWS);
    
    // Fill the matricies with data from our neuron database
    stewie.populate_cs_gj_matricies(cs_2d_matrix, gj_2d_matrix, neuron_names_to_index_map, DataSteward::NUM_ELEMS, DataSteward::NUM_ELEMS);
    //stewie.mapNeuronsToMuscles();
}

void Core::transpose_and_fill_1d_representation(){
    stewie.transpose(cs_2d_matrix, cs_2d_matrix_t, DataSteward::NUM_ELEMS, DataSteward::NUM_ELEMS);
    stewie.transpose(gj_2d_matrix, gj_2d_matrix_t, DataSteward::NUM_ELEMS, DataSteward::NUM_ELEMS);
    
    // Finally get the 1D representation of the CS and GJ matricies
    cs_matrix = stewie.convert_2d_to_1d(cs_2d_matrix_t, DataSteward::NUM_ELEMS, DataSteward::NUM_ELEMS);
    gj_matrix = stewie.convert_2d_to_1d(gj_2d_matrix_t, DataSteward::NUM_ELEMS, DataSteward::NUM_ELEMS);
    
    setup_timer_.stop_timer();
    setup_run_time = setup_timer_.get_exe_time_in_ms();
}


/* OpenCL Functions */
void Core::setup_opencl(size_t global, size_t local, float *to_processor_voltages){
    // Setup the kernel using Andrew's CLHelper code
    init_cl();
    fprintf(stderr,"init_cl() passed\n");   
    
    // Push the required buffers to the GPU
    push_buffers();
    fprintf(stderr, "push_buffers() passed\n");

    // Self explanatory
    set_kernel_args(local);
    fprintf(stderr, "set_kernel_args() passed\n");

    // Get the kernel running
    enqueue_kernel(global, local);
    fprintf(stderr, "enqueue_kernel() passed\n");
}

void Core::run_in_loop(size_t global, size_t local){
    pushVoltageBuffer(false);
    set_kernel_args(local);
    enqueue_kernel(global, local);
}

void Core::init_cl(){
    
    clhc.setup_opencl();
    
    //clhc.read_kernels_from_file("mainKernel.cl", &programBuffer);
    clhc.read_kernels_from_file("OrtusKernelOne.cl", &programBuffer);

    program = clCreateProgramWithSource(clhc.context, 1, (const char**) &programBuffer, NULL, &clhc.err);
    clhc.check_and_print_cl_err(clhc.err);

    clhc.err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    clhc.check_and_print_cl_program_build_err(clhc.err, &program);

    //kernel = clCreateKernel(program, "mainKernel", &clhc.err);
    kernel = clCreateKernel(program, "OrtusKernel", &clhc.err);
    clhc.check_and_print_cl_err(clhc.err);
}

void Core::pushVoltageBuffer(bool isFirst){
    if (isFirst){
        clhc.err = clEnqueueWriteBuffer(clhc.commands, neuron_voltages, CL_TRUE, 0, sizeof(float) * DataSteward::NUM_ELEMS * _number_of_loops, inputKernelActivations, 0, NULL, NULL);
    }
    else {
        fprintf(stderr, "pushing voltage buffer.\n");
        if (_iteration_counter < numMainLoops/2){
            int sco2Idx = stewie.n2idxMap["SCO2"];
            int minhaleIdx = stewie.n2idxMap["MINHALE"];
            int so2Idx = stewie.n2idxMap["SO2"];
            float CO2_addition = 2.5f;
            returned_voltages[sco2Idx] += CO2_addition;
            //if (returned_voltages[minhaleIdx] > 1.0){
            //    returned_voltages[so2Idx] += CO2_addition/2.0;
            //}
            printf("Added CO2: %.4f\n",returned_voltages[sco2Idx]);
        }
        clhc.err = clEnqueueWriteBuffer(clhc.commands, neuron_voltages, CL_TRUE, 0, sizeof(float) * DataSteward::NUM_ELEMS * _number_of_loops, returned_voltages, 0, NULL, NULL);
    }
    clhc.check_and_print_cl_err(clhc.err);
}

/**
 LATER TODO: for learning, we push weight buffer...
 */

// change to push_all_buffers
void Core::push_buffers(){
    ExeTimer et;
    et.start_timer();
    
    // Buffer that holds the initial voltages, and then gets the updated voltages each iteration
    neuron_voltages = clCreateBuffer(clhc.context, CL_MEM_READ_WRITE, sizeof(float) * DataSteward::NUM_ELEMS * _number_of_loops, NULL, &clhc.err);
    clhc.check_and_print_cl_err(clhc.err);
    pushVoltageBuffer(true);// only called with 'true' on *first* run. false after that.
    
    // The initial polarities
    polarity_vec = clCreateBuffer(clhc.context, CL_MEM_READ_ONLY, sizeof(float) * DataSteward::NUM_ROWS, NULL, &clhc.err);
    clhc.check_and_print_cl_err(clhc.err);
    clhc.err = clEnqueueWriteBuffer(clhc.commands, polarity_vec, CL_TRUE, 0, sizeof(float) * DataSteward::NUM_ROWS, polarities, 0, NULL, NULL);
    clhc.check_and_print_cl_err(clhc.err);
    
    // The chemical synapse connection array
    cs_mat = clCreateBuffer(clhc.context, CL_MEM_READ_ONLY, sizeof(float) * DataSteward::LEN, NULL, &clhc.err);
    clhc.check_and_print_cl_err(clhc.err);
    clhc.err = clEnqueueWriteBuffer(clhc.commands, cs_mat, CL_TRUE, 0, sizeof(float) * DataSteward::LEN, cs_matrix, 0, NULL, NULL);
    clhc.check_and_print_cl_err(clhc.err);
   
    // The gap junction connection array
    gj_mat = clCreateBuffer(clhc.context, CL_MEM_READ_ONLY, sizeof(float) * DataSteward::LEN, NULL, &clhc.err);
    clhc.check_and_print_cl_err(clhc.err);
    clhc.err = clEnqueueWriteBuffer(clhc.commands, gj_mat, CL_TRUE, 0, sizeof(float) * DataSteward::LEN, gj_matrix, 0, NULL, NULL);
    clhc.check_and_print_cl_err(clhc.err);
    
    // Used to copy the data back from the GPU, had issues using single buffer
#warning TODO: get this working on a single buffer
    cl_returned_voltages = clCreateBuffer(clhc.context, CL_MEM_READ_WRITE, (sizeof(float) * DataSteward::NUM_ELEMS * _number_of_loops), NULL, &clhc.err);
    clhc.check_and_print_cl_err(clhc.err);
    
    // create the two buffers for the probe (cs and gj)
    // chem contrib buffer
    cs_contrib = clCreateBuffer(clhc.context, CL_MEM_READ_WRITE, (sizeof(float) * DataSteward::NUM_ELEMS * DataSteward::NUM_ELEMS), NULL, &clhc.err);
    clhc.check_and_print_cl_err(clhc.err);
    // clear the cs_contrib buffer
    clhc.err = clEnqueueWriteBuffer(clhc.commands, cs_contrib, CL_TRUE, 0, sizeof(float) * DataSteward::NUM_ELEMS * DataSteward::NUM_ELEMS, zeros1D, 0, NULL, NULL);
    clhc.check_and_print_cl_err(clhc.err);
    // gap junction contrib buffer
    gj_contrib = clCreateBuffer(clhc.context, CL_MEM_READ_WRITE, (sizeof(float) * DataSteward::NUM_ELEMS * DataSteward::NUM_ELEMS), NULL, &clhc.err);
    clhc.check_and_print_cl_err(clhc.err);
    // do the same with the gj_contrib buffer
    clhc.err = clEnqueueWriteBuffer(clhc.commands, gj_contrib, CL_TRUE, 0, sizeof(float) * DataSteward::NUM_ELEMS * DataSteward::NUM_ELEMS, zeros1D, 0, NULL, NULL);
    clhc.check_and_print_cl_err(clhc.err);
    
    
    et.stop_timer();
    opencl_mem_time += et.get_exe_time_in_ms();
}

void Core::set_kernel_args(size_t local){
    
    cl_uint local_rows, local_cols;
    local_rows = local_cols = DataSteward::NUM_ELEMS;
    cl_uint local_iterations = 0;//_iteration_counter;
    
    clhc.err = 0;

    // This is the 'fake matrix' that has initial values and will store the updated
    // values on the kernel so it can operate without buffer transfers each iteration
    clhc.err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &neuron_voltages);
    //fprintf(stderr, "arg: %d, err code: %d\n", 0, clhc.err); 

    // The vector of polarities used in the matrix multiplication
    clhc.err = clSetKernelArg(kernel, 1, sizeof(cl_mem), &polarity_vec);
    //fprintf(stderr, "arg: %d, err code: %d\n", 1, clhc.err); 

    // The chemical synapse connection array
    clhc.err = clSetKernelArg(kernel, 2, sizeof(cl_mem), &cs_mat);
    //fprintf(stderr, "arg: %d, err code: %d\n", 2, clhc.err); 
    
    // The gap junction connection array
    clhc.err = clSetKernelArg(kernel, 3, sizeof(cl_mem), &gj_mat);
    //fprintf(stderr, "arg: %d, err code: %d\n", 3, clhc.err); 
    
    // The array to read back the values after the specified number of iterations
    // Hoping to optimize this away at some point
    clhc.err = clSetKernelArg(kernel, 4, sizeof(cl_mem), &cl_returned_voltages);
    //fprintf(stderr, "arg: %d, err code: %d\n", 4, clhc.err); 
    
    // The number of rows and columns to represent the 'fake matrix'
    clhc.err = clSetKernelArg(kernel, 5, sizeof(cl_uint), &local_rows);
    //fprintf(stderr, "arg: %d, err code: %d\n", 5, clhc.err); 
    
    clhc.err = clSetKernelArg(kernel, 6, sizeof(cl_uint), &local_cols);
    //fprintf(stderr, "arg: %d, err code: %d\n", 6, clhc.err); 
    
    // The current iteration we're on, so the kernel knows which row to access
    clhc.err = clSetKernelArg(kernel, 7, sizeof(cl_uint), &local_iterations);
    //fprintf(stderr, "arg: %d, err code: %d\n", 7, clhc.err);
    
    // the variable that tells the kernel whether or not we're probing
    clhc.err = clSetKernelArg(kernel, 8, sizeof(cl_uint), &Probe::shouldProbe);
    
    // the cs and gj probes
    clhc.err = clSetKernelArg(kernel, 9, sizeof(cl_mem), &cs_contrib);
    clhc.err = clSetKernelArg(kernel, 10, sizeof(cl_mem), &gj_contrib);
    
    if(clhc.err != CL_SUCCESS){
        throw std::runtime_error("Error: Failed to set kernel arguments! " + std::to_string(clhc.err));
    }   
    
    clhc.err = clGetKernelWorkGroupInfo(kernel, clhc.device_id, CL_KERNEL_WORK_GROUP_SIZE, sizeof(local), &local, NULL);
    clhc.check_and_print_cl_err(clhc.err);
}
void Core::enqueue_kernel(size_t global, size_t local){
    cl_event timing_event;
    
    size_t g, l;
    g = global;
    l = local;
    clhc.err = clEnqueueNDRangeKernel(clhc.commands, kernel, 1, NULL, &g, &l, 0, NULL, &timing_event);
    clhc.check_and_print_cl_err(clhc.err);
    
    clFinish(clhc.commands);
    cl_ulong starttime;
    cl_ulong endtime;
    clhc.check_and_print_cl_err(clGetEventProfilingInfo(timing_event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &starttime, NULL));
    clhc.check_and_print_cl_err(clGetEventProfilingInfo(timing_event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &endtime, NULL));
    opencl_run_time += ((endtime - starttime) / 1000000.0);
}

void Core::read_voltages_from_kernel(){
    ExeTimer et;
    et.start_timer();
    
    // Read the voltages back from the kernel to the secondary array
    clhc.err = clEnqueueReadBuffer(clhc.commands, cl_returned_voltages, CL_TRUE, 0, (sizeof(float) * DataSteward::NUM_ELEMS * _number_of_loops), returned_voltages, 0, NULL, NULL);

    clhc.check_and_print_cl_err(clhc.err);
    
    // now, if we're probing, read the cs and gj contribs, **AND THEN ZERO THEM OUT**
    if (Probe::probeEnabled){ // shouldProbe is a cl_uint... for general purpose stuff, use probeEnabled, which is a bool.
        feedProbe();
    }

    et.stop_timer();
    opencl_mem_time += et.get_exe_time_in_ms();
    
    float **matrix_representation = stewie.convert_1d_to_2d(returned_voltages, _number_of_loops, DataSteward::NUM_ELEMS);
    vector<vector<float>> all_voltages(_number_of_loops);
    for(int i = 0; i < _number_of_loops; ++i){
        vector<float> single_iteration(DataSteward::NUM_ELEMS);
        for(int j = 0; j < DataSteward::NUM_ELEMS; ++j){
            //printf("rep[%d][%d]: %f\n", i, j, matrix_representation[i][j]);
            single_iteration[j] = matrix_representation[i][j];
        }
        all_voltages[i] = single_iteration;
    }
    
    vector_of_returned_voltages = all_voltages;
    
    // Cleanup
    for(int i = 0; i < _number_of_loops; ++i){
        delete[] matrix_representation[i];
    }
    
    delete[] matrix_representation;
    matrix_representation = nullptr;
    
}

void Core::feedProbe(){
    // cs contrib
    clhc.err = clEnqueueReadBuffer(clhc.commands, cs_contrib, CL_TRUE, 0, (sizeof(float) * DataSteward::NUM_ELEMS * DataSteward::NUM_ELEMS), cs_1d_contrib, 0, NULL, NULL);
    clhc.check_and_print_cl_err(clhc.err);
    // gj contrib
    clhc.err = clEnqueueReadBuffer(clhc.commands, gj_contrib, CL_TRUE, 0, (sizeof(float) * DataSteward::NUM_ELEMS * DataSteward::NUM_ELEMS), gj_1d_contrib, 0, NULL, NULL);
    clhc.check_and_print_cl_err(clhc.err);
    
    
    cs_2d_contrib_t = stewie.convert_1d_to_2d(cs_1d_contrib, DataSteward::NUM_ELEMS, DataSteward::NUM_ELEMS);
    gj_2d_contrib_t = stewie.convert_1d_to_2d(gj_1d_contrib, DataSteward::NUM_ELEMS, DataSteward::NUM_ELEMS);
    probe->csContribVec.push_back(cs_2d_contrib_t);
    probe->gjContribVec.push_back(gj_2d_contrib_t);
    
    // clear the cs_contrib buffer
    clhc.err = clEnqueueWriteBuffer(clhc.commands, cs_contrib, CL_TRUE, 0, sizeof(float) * DataSteward::NUM_ELEMS * DataSteward::NUM_ELEMS, zeros1D, 0, NULL, NULL);
    clhc.check_and_print_cl_err(clhc.err);
    // do the same with the gj_contrib buffer
    clhc.err = clEnqueueWriteBuffer(clhc.commands, cs_contrib, CL_TRUE, 0, sizeof(float) * DataSteward::NUM_ELEMS * DataSteward::NUM_ELEMS, zeros1D, 0, NULL, NULL);
    clhc.check_and_print_cl_err(clhc.err);
    
}


void Core::clean_up_kernel(){
    clReleaseProgram(program);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(clhc.commands);
    clReleaseContext(clhc.context);
}

void Core::clean_up_memory(){
    delete[] temp_voltages;
    delete[] cs_matrix;
    delete[] gj_matrix;
    delete[] inputKernelActivations;
    delete[] returned_voltages;
    delete[] polarities;
    for(int i = 0; i < DataSteward::NUM_ELEMS; ++i){
        delete[] cs_2d_matrix[i];
        delete[] gj_2d_matrix[i];
        delete[] cs_2d_matrix_t[i];
        delete[] gj_2d_matrix_t[i];
    }
    delete[] cs_2d_matrix;
    delete[] gj_2d_matrix;
    delete[] cs_2d_matrix_t;
    delete[] gj_2d_matrix_t;
}

void Core::print_report(int num_runs){
    cout << "\n\n\n";
    total_run_time += opencl_run_time;
    cout << "Number of iterations: " << _number_of_loops << ".\n";
    cout << "Total run time: " << total_run_time << " ms" << endl;
    cout << "Setup run time: " << setup_run_time << " ms" << endl;
    cout << "OpenCL processing time: " << opencl_run_time << " ms" << endl;
    cout << "OpenCL memory transer time: " << opencl_mem_time << " ms" << endl;
    cout << "Reset vector time: " << reset_time << " ms" << endl;
    
    auto col_num = DataSteward::NUM_ELEMS - 1;
    auto row_num = ((_number_of_loops > 1) ? (_number_of_loops / 2) : _number_of_loops - 1);
    cout << "\nVoltages sample: \n";
    float sample_1, sample_2;
    sample_1 = vector_of_returned_voltages[0][col_num];
    sample_2 = vector_of_returned_voltages[row_num][col_num];
    cout << "   voltages[0][" << col_num << "]: " << sample_1 << endl;
    cout << "   voltages[" << row_num << "][" << col_num << "]: " << sample_2 << endl;
}

size_t Core::get_number_of_loops(){
    return _number_of_loops;
}
