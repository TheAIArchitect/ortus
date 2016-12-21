//
//  CLHelperClass.hpp
//  pelegans
//
//  Created by Andrew W.E. McDonald
//  Put into class structure by Sean Grimes on 11/21/15.
//  Copyright © 2015. All rights reserved.
//

#pragma once

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#include <iostream>
#include "Logger.hpp"

class CLHelperClass{
public:
    bool setup_opencl(void);
    void read_kernels_from_file(const char *file, char **programBuffer);
    void check_and_print_cl_err(int err);
    void check_and_print_cl_program_build_err(int err, cl_program *program);
    void print_selected_device_info(cl_device_id cdi, int device_num);
    std::string get_opencl_error(int err);
    void print_dferr(const char *opt);
    
public:
    int err;
    unsigned int correct;
    cl_device_id *device_ids = new cl_device_id[2];
    cl_device_id device_id;
    cl_context context;
    cl_command_queue commands;
    cl_uint num_devices;
    Logger log_;
    
};