/*
 * @author Sean Grimes, spg63@cs.drexel.edu
 */

float get_v_leak(float v_curr, float v_leak_percent, float v_init){
    float v_leak;
    if(v_curr > v_init)
        v_leak = v_leak_percent * v_curr;
    else
        v_leak = 0;
    
    // Since Math.abs doesn't work here, do it old school
    if(v_leak < 0)
        v_leak = v_leak * -1;
    
    return v_leak;
}

int get_index(int row_num, int col_num, int arr_width){
    return arr_width * row_num + col_num;
}

/// NOTE: Where the hell is the polarity data?!?!
__kernel void mainKernel(__global float *fake_voltages_matrix,
                         __global float *polarity_vec,
                         __global float *cs_matrix,
                         __global float *gj_matrix,
                         __global float *fake_matrix_for_return_voltages,
                         const unsigned int num_rows_in_matrix,
                         const unsigned int num_cols_in_matrix,
                         int iteration_number_for_row_access){
    
    // iteration_number_for_row_access:
    //  Tells us what iteration we're on, 0 indexed.
    // gid:
    //  Determine the row of the CS / GJ matrix we're working on
    // loop_iter:
    //  The column numbers of the CS / GJ matrix that we get v_current values from
    //  on row = gid. So any given v_current comes from:
    //      idx = get_index(iteration_number_for_row_access, loop_iter, num_cols_in_matrix)
    //      float v_current = fake_voltages_matrix[idx]
    // Any given connection weight comes from:
    //  idx = get_index(gid, loop_iter, num_cols_in_matrix)
    //  {cs, gj}_matrix[idx]
    
    
    // What row are we playing on
    int gid = get_global_id(0);
    
    // NOTE: Apparently you can't declare and initialize in 1 statement...huh.
    // Well, regardless, these should be shared amoung the work-groups
    __local float memb_resist;
    //memb_resist = 5621463123.21;
    memb_resist = 5621463;
    __local float gap_junction_conductance;
    gap_junction_conductance = 0.000000005;
    __local float chemical_synapse_conductance;
    chemical_synapse_conductance = 0.000000006;
    __local float v_leak_percent;
    v_leak_percent = 0.25;
    __local float v_init;
    v_init = -0.065;
    __local float v_min;
    v_min = -.120;// -120 mV is the lowest a neuron can go.
    
    // These should be kept in private address space as they are unique to each 'neuron'
    float reversal_potential = -0.065;
    float external_current_in = 0.0f;
    float gj_incoming = 0.0f;
    float cs_incoming = 0.0f;
    float total_incoming_current = 0.0f;
    float total_incoming_voltage = 0.0f;
    
    //if(gid == 1)printf("Running from mainKernel.cl\n");
    /*
     * Since out array of neurons is a shitty number, we're probably not going to have
     * a proper multiple for work-group size...
     *  We could pad the array, but then we're doing calculations on bullshit
     *  Therefore, best option seems to be to just skip the work when we're out
     *  of range and let the thread(s) get back to something that is in range (if still available)
     */
    
    if(gid <= (num_rows_in_matrix - 1)){
        size_t my_v_curr_idx = get_index(iteration_number_for_row_access, gid, num_cols_in_matrix);
        float my_v_current = fake_voltages_matrix[my_v_curr_idx];
        float my_polarity = polarity_vec[gid];
        float v_leak = get_v_leak(my_v_current, v_leak_percent, v_init);
        size_t voltage_idx;
        size_t weight_idx;
        voltage_idx = 0;
        weight_idx = 0;
        
        // We are going to run across the full row of the cs/gj matrix and play with the v_curr value
        int loop_iter;
        for(loop_iter = 0; loop_iter < num_cols_in_matrix; ++loop_iter){
            voltage_idx = get_index(iteration_number_for_row_access, loop_iter, num_cols_in_matrix);
            weight_idx = get_index(gid, loop_iter, num_cols_in_matrix);
            
            // Try to save ourselves from pointless memory access / operations
            if(cs_matrix[weight_idx] == 0.0 && gj_matrix[weight_idx] == 0.0)
                continue;
            
            // Get v_current for the incoming spurt / shocker
            float their_v_curr = fake_voltages_matrix[voltage_idx];
            
            // Make sure we actually have a connection
            float cs_weight = cs_matrix[weight_idx];
            if(cs_weight != 0.0){
                if(my_polarity == 1){ // excite
                    reversal_potential = 0.f;
                }
                // FIXME: Don't think this is necessary....should be same for full row
                else{
                    reversal_potential = -0.065;
                }
                float this_isnt_right_till_we_pass_polarities = reversal_potential - my_v_current;
                cs_incoming += cs_weight * chemical_synapse_conductance * this_isnt_right_till_we_pass_polarities;
            }
            float gj_weight = gj_matrix[weight_idx];
            if(gj_weight != 0.0){
                // Determine weight between connection
                float v_cur_gj_local_wtf_is_this_var_name = their_v_curr - my_v_current;
                gj_incoming += gj_weight * gap_junction_conductance * v_cur_gj_local_wtf_is_this_var_name;
            }
        }
        
        total_incoming_current = gj_incoming + cs_incoming + external_current_in;
        total_incoming_voltage = memb_resist * total_incoming_current;
        float v_curr_finished = (my_v_current - v_leak) + total_incoming_voltage;
        
        if(v_curr_finished < v_min)
            v_curr_finished = v_min;
        
        if(v_curr_finished > 0.15)
            v_curr_finished = 0.15;
        
        
        // Reset the value in the voltage vec
        // Write the value back to the fake_voltages_matrix, but 1 row ahead
        size_t w_idx = get_index(iteration_number_for_row_access + 1, gid, num_cols_in_matrix);
        fake_voltages_matrix[w_idx] = v_curr_finished;
        
        size_t fin_idx = get_index(iteration_number_for_row_access, gid, num_cols_in_matrix);
        
        fake_matrix_for_return_voltages[fin_idx] = v_curr_finished;
        
    }
}
