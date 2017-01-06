/*
 * @author Andrew W.E. Mcdonald, awm32@cs.drexel.edu
 * @author Sean Grimes, spg63@cs.drexel.edu
 *
 * What about: http://www.izhikevich.org/publications/spikes.htm
 */

float computeDecay(float v_curr, float v_decay, float v_init){
    float decay = v_curr*v_decay;// this works because v_init is 0.
    return decay;
}

int getIndex(int row_num, int col_num, int arr_width){
    return arr_width * row_num + col_num;
}

/*
 note: the last three arguments, shouldProbe, chemContrib, and gapContrib,
 are for the Probe to collect information about which neurons/muscles are passing what to other neurons/muscles.
 
 if shouldProbe == 0, nothing happens with the *_contribs.
 if shouldProbe == 1, we'll fill the *_contribs in the same way as the cs_ and gj_ matricies,
 but instead of each column holding the  pre synaptic conn weight of that neuron/muscle to the post synaptic neuron/muscle (the row),
 each column will hold the voltage/activation contribution of that synapse/gap junction to the neuron/muscle in that row.
 
 *** THIS WILL NEED TO BE MODIFIED IF WE DO MORE THAN ONE 'ITERATION' PER CALL ***
 */
__kernel void OrtusKernel( __global float *inputVoltages,
                         __global float *outputVoltages,
                         __global float *gapWeights,
                         __global float *chemWeights,
                          __global float* gapContrib,
                          __global float* chemContrib,
                         const unsigned int rowCount,
                         const unsigned int colCount,
                          const unsigned int shouldProbe,
                          float gapNormalizer,
                          float chemNormalizer){
    
    // iteration_number_for_row_access:
    //  Tells us what iteration we're on, 0 indexed.
    // gid:
    //  Determine the row of the CS / GJ matrix we're working on
    // loop_iter:
    //  The column numbers of the CS / GJ matrix that we get v_current values from
    //  on row = gid. So any given v_current comes from:
    //      idx = getIndex(iteration_number_for_row_access, loop_iter, colCount)
    //      float v_current = inputVoltages[idx]
    // Any given connection weight comes from:
    //  idx = getIndex(gid, loop_iter, colCount)
    //  {cs, gj}_matrix[idx]
    
    // current row
    int gid = get_global_id(0);
    
    // NOTE: Apparently you can't declare and initialize in 1 statement...huh.
    // Well, regardless, these should be shared amoung the work-groups
    __local float v_decay_constant;
    v_decay_constant = .2; // loses % of its charge (multiply by 100 for %)
    __local float v_init;
    v_init = 0;
    __local float v_min;
    v_min = -100.f;
    __local float v_max;
    v_max = 100.f;
    __local float thresh;
    thresh = 5.f;
    __local float max_cs_weight;
    max_cs_weight = 1.0f;// NOTE: THIS WILL USE 'chemNormalizer'
    __local float max_gj_weight;
    max_gj_weight = 1.0f; // NOTE: THIS WILL USE 'gapNormalizer'
    // These should be kept in private address space as they are unique to each 'neuron'
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
    if(gid <= (rowCount - 1)){
        // using 0 for the row_num, because right now inputVoltages is just a vector
        size_t my_v_curr_idx = getIndex(0, gid, colCount);
        float my_v_current = inputVoltages[my_v_curr_idx];
        float v_decay = computeDecay(my_v_current, v_decay_constant, v_init);
        size_t voltage_idx;
        size_t weight_idx;
        voltage_idx = 0;
        weight_idx = 0;
        //float cs_amount_passed_on = .5;//
        //float gj_amount_passed_on = .65;
        float cs_amount_passed_on = 1;
        float gj_amount_passed_on = 1;
        float inhibitRevPot = -10.0f;
        float exciteRevPot = 30.0f;
        float vRange = exciteRevPot - inhibitRevPot;
        
        // We are going to run across the full row of the cs/gj matrix and play with the v_curr value
        int loop_iter;
        for(loop_iter = 0; loop_iter < colCount; ++loop_iter){
            if (loop_iter == gid){ // don't want to try to connect with ourselves.
                continue;
            }
            voltage_idx = getIndex(0, loop_iter, colCount);
            weight_idx = getIndex(gid, loop_iter, colCount);
            // Try to save ourselves from pointless memory access / operations
            if(chemWeights[weight_idx] == 0.0 && gapWeights[weight_idx] == 0.0)
                continue;
            // Get v_current for the incoming spurt / shocker
            float their_v_curr = inputVoltages[voltage_idx];
            if (fabs(their_v_curr) < thresh){ // nothing happens if it's less than thresh.
                continue;
            }
            
            // Make sure we actually have a connection
            // use log(weight + 1)/log(max weight) -- this gives a logarithmic gain, a weight of 1 is .176, and a max weight is 1.
            // input equation here to see: https://www.desmos.com/calculator
            //float cs_weight = log(chemWeights[weight_idx] + 1)/log(max_cs_weight);// normalize to max CS weight (this was based off of a cursory glance)
            float cs_weight = chemWeights[weight_idx]/max_cs_weight;// normalize to max CS weight (this was based off of a cursory glance)
            float added_v = 0;
            float conductance = 0;
            if(cs_weight != 0.0){
                /*************************
                 *********** NOTE: maybe we should multiply the added_v by '(1-(v_max - v_cur))*<some constant>' -- that way, as voltage increases, it absorbs less... we should make this nonlinear though. (this is to simulate conductance)
                 
                 ... potentially something we should do for both GJ and CS...
                 *********************/
                /*
                 
                 
                 - starting conductance is .5 -- similar to wicks' work, just simplified.
                 -> according to sigmoidal function it increases or decreases with neuron's potential
                    -> as it nears max, conductance tends to 0, and as it nears min, conductance tends to 1
                 
                 */
                // if their_v_curr == 0, conductance will be .5.... also, the -5 comes from wicks' paper... almost.
                 //no conductance at the moment...
                //added_v = cs_weight * conductance * their_v_curr;
                //added_v = cs_weight * their_v_curr;
                if(cs_weight < 0){ // if inhibit
                    cs_weight *= -1; // make positive, for below computation
                    float preToEq = fabs(their_v_curr - inhibitRevPot);
                    float vDiff = inhibitRevPot - my_v_current;
                    conductance = (2.0f - (2.0f/(1.0f + exp(-5.0f*(preToEq/vRange))))) - .0134;
                    added_v = cs_weight * conductance * vDiff;
                }
                else { // excite
                    float preToEq = fabs(their_v_curr - exciteRevPot);
                    float vDiff = exciteRevPot - my_v_current;
                    conductance = (2.0f - (2.0f/(1.0f + exp(-5.0f*(preToEq/vRange))))) - .0134;
                    added_v = cs_weight * conductance * vDiff;
                }
                if (shouldProbe == 1){
                    chemContrib[weight_idx] = added_v;
                }
                cs_incoming += added_v;
            }
            //float gj_weight = log(gapWeights[weight_idx] + 1)/log(max_gj_weight);// normalize to max GJ weight (again, cursory glance)
            added_v = 0; // reset this, because we use the same variable for cs and gj incoming...
            float gj_weight = gapWeights[weight_idx]/max_gj_weight;// normalize to max GJ weight (again, cursory glance)
            if(gj_weight != 0.0){ // response is always the same as the source -- depolariziation of one, causes same in other, and vice versa.
                added_v = gj_weight * gj_amount_passed_on * (their_v_curr - my_v_current);
                if (shouldProbe == 1){
                    gapContrib[weight_idx] = added_v;
                }
                gj_incoming += added_v;
            }
        }
        total_incoming_voltage = gj_incoming + cs_incoming;
        // is this the right way to do this, to compute based upon current activation, and then at the end decay it?
        // maybe we should decay it earlier on, prior to computing...
        float v_curr_finished = (my_v_current - v_decay) + total_incoming_voltage;
        
        if(v_curr_finished < v_min)
            v_curr_finished = v_min;
        
        if(v_curr_finished > v_max)
            v_curr_finished = v_max;
        
        
        // Reset the value in the voltage vec
        // Write the value back to the inputVoltages, but 1 row ahead
        //size_t w_idx = getIndex(iteration_number_for_row_access + 1, gid, colCount);
        //inputVoltages[w_idx] = v_curr_finished;
        
        size_t fin_idx = getIndex(0, gid, colCount);
        
        outputVoltages[fin_idx] = v_curr_finished;
        
        /*
        if (gid == 1){
            printf("START CLPRINT:\ninputs: ");
            int i;
            for (i = 0; i < rowCount; ++i){
                printf("%.2f, ",inputVoltages[i]);
            }
            printf("\noutputs: ");
            for (i = 0; i < rowCount; ++i){
                printf("%.2f, ",outputVoltages[i]);
            }
            printf("\nEND CLPRINT\n");
        }
        */
    }
}
