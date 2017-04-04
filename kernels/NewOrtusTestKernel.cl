/*
 * @author Andrew W.E. Mcdonald, awm32@cs.drexel.edu
 * @author Sean Grimes, spg63@cs.drexel.edu
 *
 * What about: http://www.izhikevich.org/publications/spikes.htm
 */

/* GENERAL (kernel) NOTES
 note: shouldProbe, chemContrib, and gapContrib:
 used for the Probe to collect information about which neurons/muscles are passing what to other neurons/muscles.
 
 if shouldProbe == 0, nothing happens with the *Contrib vars.
 if shouldProbe == 1, we'll fill the *Contrib vars in the same way as the cs_ and gj_ matricies,
 but instead of each column holding the  pre synaptic conn weight of that neuron/muscle to the post synaptic neuron/muscle (the row),
 each column will hold the voltage/activation contribution of that synapse/gap junction to the neuron/muscle in that row.
 
 */


/* declarations */
void computeXCorr(__global float* outputVoltageHistory, int numElements, int voltageHistorySize, int gid, int lid, __local float* XCorrScratchPad, int startingScratchPadOffset, int numXCorrEntries);
float XCorrMultiply(__global float* outputVoltageHistories, int aOffset, int bOffset, int len);
float computeVoltageRateOfChange(__global float* outputVoltageHistory, int numElements, int voltageHistorySize, int gid, int lid, __local float* voltageRateOfChangeScratchPad, int startingScratchPadOffset, int numXCorrEntries);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// this is the number of kernel args -1
constant int NUM_KERNEL_ARGS_WITH_INFO = 6;
constant int NUM_COLS_PER_KERNEL_ARG_INFO = 6;

int getIndex(int row, int col, int page, int maxRows, int maxCols, int maxPages){
    int pageSize = maxRows * maxCols;
    return (page * pageSize) +  (row * maxCols) + col;
}


/* BEGIN kernelArgInfo info: (see DataSteward::initializeKernelArgsAndBlades for more info)*/
// for each 'row':
// [0] => kernel arg # (sanity check)
// [1] => number of Blades
// [2] => stride (from start to one Blade, to start to next -- essentially the max size)
// [3] => rows per Blade
// [4] => cols per Blade
// [5] => pages per Blade
// Note: there is no entry for kernelArgInfo, so there are only "number of kernel args"-1 rows in kernelArgInfo.
/* END kernelArgInfo info */
void getKernelArgInfo(global int* kernelArgInfo, int kernelArgNum, int* kernelArgBladeCount, int* kernelArgStride, int* kernelArgRows, int* kernelArgCols, int* kernelArgPages){
    // to get row, just set col to 0.
    int kArgRow = getIndex(kernelArgNum, 0, 0, NUM_KERNEL_ARGS_WITH_INFO, NUM_COLS_PER_KERNEL_ARG_INFO, 0);
    if (kernelArgInfo[kArgRow] != kernelArgNum){
        printf("Oh no! KernelArgInfo seems to be incorrectly formatted!\n");
    }
    // now we add 1, 2, 3, etc. to kArgRow to fill our variables.
    *kernelArgBladeCount = kernelArgInfo[kArgRow + 1];
    *kernelArgStride = kernelArgInfo[kArgRow + 2];
    *kernelArgRows = kernelArgInfo[kArgRow + 3];
    *kernelArgCols = kernelArgInfo[kArgRow + 4];
    *kernelArgPages = kernelArgInfo[kArgRow + 5];
}

void printKernelArgInfo(int kernelArgNum, int kernelArgBladeCount, int kernelArgStride, int kernelArgRows, int kernelArgCols, int kernelArgPages){
    printf("kernel arg #%d {blades: %d, stride: %d, rows: %d, cols: %d, pages: %d}\n", kernelArgNum, kernelArgBladeCount, kernelArgStride, kernelArgRows, kernelArgCols, kernelArgPages);
}


kernel void OrtusKernel(global float* elementAttributes,
                        global float* relationAttributes,
                        global float* weights,
                        global float* activations,
                        global float* metadata,
                        local float* scratchpad,
                        global int* kernelArgInfo){
    local unsigned int dimidx;
    dimidx = 0;
    int gId = get_global_id(dimidx); // gives id of current work-item
    int lId = get_local_id(dimidx);

    // BEGIN vars for extracting necessary data from kernel args
    int kernelArgNum;
    int kernelArgBladeCount;
    int kernelArgStride;
    int kernelArgRows;
    int kernelArgCols;
    int kernelArgPages;
    // END vars for extracting necessary data from kernel args
    // (kernel arg 0): elementAttributes
    kernelArgNum = 0;
    getKernelArgInfo(kernelArgInfo, kernelArgNum, &kernelArgBladeCount, &kernelArgStride, &kernelArgRows, &kernelArgCols, &kernelArgPages);
    // print
    if (gId == 1) {
     printKernelArgInfo(kernelArgNum, kernelArgBladeCount, kernelArgStride, kernelArgRows, kernelArgCols, kernelArgPages);   
    }
    // (kernel arg 1): relationAttributes
    // uncomment 94-99 to make opencl crash on mac, i think 
    //kernelArgNum = 1;
    //getKernelArgInfo(kernelArgInfo, kernelArgNum, &kernelArgBladeCount, &kernelArgStride, &kernelArgRows, &kernelArgCols, &kernelArgPages);
    //if (gId == 1){
    //    printKernelArgInfo(kernelArgNum, kernelArgBladeCount, kernelArgStride, kernelArgRows, kernelArgCols, kernelArgPages);
    //}
    
    
    
    /* stopping point: continue this (see note above)... finish redoing kernel. then do structure. boom. done. haha. */
    
    // get all indices we'll need:
    
    // element attributes: type, affect, thresh
    // relation attributes: type, polarity, direction, age, thresh, decay, mutability
    // activations: just activations, with the activation history size in 'metadata' (see below)
    // metadata: numElements, kernelIterationNum, activationHistorySize,  numXCorrComputations, numSlopeComputations
    
    
    
    // how to deal with transposed??? 
    
    
    
    // PRIVATE VARS
    
    int offset = 0;
    int len = 100;
    int bigLen = 100*100;

    //printf("gid, lid: %d, %d\n",gid, lid);
    if (gId == 1){
        printf("nice.\n");
    }
    
}


/******************************************************************************
 ******************************************************************************
 ***********************************  OLD *************************************
 ***********************************  \/  *************************************
 *****************************************************************************/
 


float computeDecay(float v_curr, float v_decay, float v_init){
    float decay = v_curr*v_decay;// this works because v_init is 0.
    return decay;
}


/* skips 'row' rows, before adding 'col' columns -- row major */
int get2DIndexAs1D(int row, int col, int numCols){
    return (row * numCols) + col;
}

/* skips 'col' cols, before adding 'row' rows -- col major.
int get2DIndexAs1D_colMajor(int col, int row, int numRows){
    return (col * numRows) + row;
}
*/

/* yeah, this is stupid. */
int getVoltageIndex(int elementId){
    return elementId;
}

/* yeah, this is also pretty stupid. It's more for readability. */
int getConnectomeIndex(int postId, int preId, int numElements){
    return get2DIndexAs1D(postId, preId, numElements);
}

/* idx is the 'inverted age', essentially.  0 would give the oldest, and 'voltageHistorySize-1' would give the newest (the 'staging area' one) */
int getOutputVoltageHistoryIndex(int elementId, int idx, int voltageHistorySize){
    return get2DIndexAs1D(elementId, idx, voltageHistorySize);
}

/* for local scratch pad, each thread in a work-group gets a block:
  _
 |_| <- Thread 0's block
 |_| <- Thread 1's block
 ...
 |_| <- Thread N's block
 
 each block has 'numElements' rows, and 'numXCorrEntries' cols (well, if the scratch pad weren't a 1D buffer it would).
 
 so, we multiply the lid (local id), by the block size, which is 'numElements*numXCorrEntries', to get the starting index (thus, offset), for a thread in a work-group 
 
 **VERY IMPORTANT NOTE: ACCESS THE voltageRateOfChangeScratchPad IN THE SAME WAY -- each rate of change corresponds to the set of values used to compute the xcorr value at the same relative index in the XCorrScratchPad. (e.g, if using sets of 4 voltage history values for the xcorr: [0,1,2,3], [1,2,3,4], [2,3,4,5], and [3,4,5,6] would be the indices of from the voltageHistory to compute the rate of change for voltageRateOfChangeScratchPad indices 0, 1, 2, and 3.
 */
int getLocalScratchPadStartingOffset(int lid, int numElements, int numXCorrEntries){
    return lid*(numElements*numXCorrEntries);
}

int getScratchPadIndex(int startingScratchPadOffset, int elementId, int XCorrEntry, int numXCorrEntries){
    return startingScratchPadOffset+get2DIndexAs1D(elementId, XCorrEntry, numXCorrEntries);
}


__kernel void refOrtusKernel( __global float *voltages, // read and write
                         __global float *outputVoltageHistory, // read and write
                         __global float *gapWeights, // read and write
                         __global float *chemWeights, // read and write
                          __global float* gapContrib, // read and write
                          __global float* chemContrib, // read and write
                          const int shouldProbe, // read only
                          float gapNormalizer, // read and write
                          float chemNormalizer, // read and write
                          __global int* metadata,// read only... for now?
                          __local float* XCorrScratchPad,// access rows by work-group id!
                          __local float* voltageRateOfChangeScratchPad){// access rows by work-group id!
    
    
    // __locals -- THESE ARE SHARED AMONG MEMBERS OF WORK-GROUP
    __local unsigned int dimidx;
    __local int rowCount;
    __local int colCount;
    __local int numElements;
    __local int kernelIterationNum;
    __local int voltageHistorySize;
    __local int numXCorrEntries;
    __local float v_decay_constant;
    __local float v_init; // starting voltage/activation
    __local float v_min; // minimum possible voltage/activation
    __local float v_max; // maximum possible voltage/activation
    __local float thresh; // don't pass on signals from neurons with activaiton below thresh
    __local float max_cs_weight;
    __local float max_gj_weight;
    dimidx = 0; // dimension index. we only have one dimension (with index 0).
    
    // PRIVATE VARS
    int gid = get_global_id(dimidx); // gives id of current work-item
    int lid = get_local_id(dimidx);
    int elementId = gid;
    float gj_incoming = 0.0f;
    float cs_incoming = 0.0f;
    float total_incoming_current = 0.0f;
    float total_incoming_voltage = 0.0f;
    
    //size_t get_global_size(dimidx);// gives total number of work-items (for specified dimension)
    //size_t get_group_id(dimidx); // gives current work-group id (from 0 to (# of work-groups -1))
    //size_t get_local_id(dimidx); // gives id of work-item *within* its work group (so, from 0 to (local_size - 1))
    //size_t get_local_size(dimidx); // gives number of work-items in a work-group
    //size_t get_num_groups(dimidx); // gives total number of work-groups
    
    
    // __locals -- THESE ARE SHARED AMONG MEMBERS OF WORK-GROUP
    rowCount = metadata[0];
    colCount = metadata[1];
    kernelIterationNum = metadata[2];
    voltageHistorySize = metadata[3]; // this is one larger than we use for the XCorr -- the last one is the staging one.
    numXCorrEntries = metadata[4];
    numElements = rowCount;
    v_decay_constant = .2; // loses % of its charge (multiply by 100 for %)
    v_init = 0;
    v_min = -100.f;
    v_max = 100.f;
    thresh = 5.f;
    max_cs_weight = 1.0f;// NOTE: THIS WILL USE 'chemNormalizer'
    max_gj_weight = 1.0f; // NOTE: THIS WILL USE 'gapNormalizer'
    
    ///////////// *********** replace rowCount and colCount with numElments, and gid with elementId !!!!!!!!!!!!!!!!
    /* BEGIN CHECKS -- comment out for speed, but leave uncommented for development */
    //if (rowCount != colCount){
    //    //printf("OpenCL Kernel Error: rowCount != colCount\n"); // uncomment as test.
    //    return;
    //}
    // PAST HERE,  THERE SHOULD BE NO REFENCE TO 'rowCount' or 'colCount' (this reduces confusion; it's clear then, that rowCount == colCount)
    
    /* END CHECKS */
    
    // Don't do anything if we're out of range.
    if(gid <= (numElements - 1)){
        //printf("gid: %d\n",gid);
        size_t my_v_curr_idx = getVoltageIndex(gid);
        float my_v_current = voltages[my_v_curr_idx];
        float v_decay = computeDecay(my_v_current, v_decay_constant, v_init);
        size_t voltage_idx;
        size_t weight_idx;
        voltage_idx = 0;
        weight_idx = 0;
        float cs_amount_passed_on = 1;
        float gj_amount_passed_on = 1;
        
        // these three can probably be __local
        float inhibitRevPot = -10.0f;
        float exciteRevPot = 30.0f;
        float vRange = exciteRevPot - inhibitRevPot;
        
        // We are going to run across the full row of the cs/gj matrix and play with the v_curr value
        int loop_iter; // NOTE: RENAME THIS VARIABLE
        for(loop_iter = 0; loop_iter < numElements; ++loop_iter){
            if (loop_iter == gid){ // don't want to try to connect with ourselves.
                continue;
            }
            //voltage_idx = getIndex(0, loop_iter, numElements); // ROW MAJOR
            voltage_idx = getVoltageIndex(loop_iter);
            weight_idx = getConnectomeIndex(gid, loop_iter, numElements); // ROW MAJOR
            //weight_idx = getIndex(gid, loop_iter, numElements); // ROW MAJOR
            // Try to save ourselves from pointless memory access / operations
            if(chemWeights[weight_idx] == 0.0 && gapWeights[weight_idx] == 0.0)
                continue;
            // Get v_current for the incoming spurt / shocker
            float their_v_curr = voltages[voltage_idx];
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
                 
                // - starting conductance is .5 -- similar to wicks' work, just simplified.
                // -> according to sigmoidal function it increases or decreases with neuron's potential
                //    -> as it nears max, conductance tends to 0, and as it nears min, conductance tends to 1
                 
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
        // Write the value back to the voltages, but 1 row ahead
        //size_t w_idx = getIndex(iteration_number_for_row_access + 1, gid, numElements);
        //voltages[w_idx] = v_curr_finished;
        
        //size_t fin_idx = getIndex(0, gid, numElements); // ROW MAJOR
        size_t fin_idx = getVoltageIndex(gid);
        
        //voltages[fin_idx] = v_curr_finished;
        voltages[fin_idx] = v_curr_finished;
        
        // update the outputVoltageHistory
        int i;
        int ovhSizeMinusOne = voltageHistorySize - 1;
        size_t currentOVHIndex =  getOutputVoltageHistoryIndex(gid, 0, voltageHistorySize);// the first (oldest) entry for this element. for next entry, just add one.
        // push all the existing output voltages down one (losing the oldest)
        for(i = 0; i < ovhSizeMinusOne; ++i){
            outputVoltageHistory[currentOVHIndex] = outputVoltageHistory[currentOVHIndex + 1];
            currentOVHIndex++;
        }
        // then set the most recent one in the 'staging' area (which is currentOVHIndex, because it was just incremented)
        outputVoltageHistory[currentOVHIndex] = v_curr_finished;
        
        
        if (kernelIterationNum >= ovhSizeMinusOne){ // then we know we have a full outputVoltageHistory
            // do XCorr
            // do gap nd chem work
            //if (gid == 0){
            //   printf("ovhs: %d, v_curr_finished: %.2f\n", voltageHistorySize, v_curr_finished);
            //printf("%.2f, %.2f, %.2f, %.2f, %.2f, %.2f\n",outputVoltageHistory[0],outputVoltageHistory[1],outputVoltageHistory[2],outputVoltageHistory[3],outputVoltageHistory[4],outputVoltageHistory[5]);
            //}
            int startingScratchPadOffset = getLocalScratchPadStartingOffset(lid, numElements,numXCorrEntries);
            computeXCorr(outputVoltageHistory, numElements, voltageHistorySize, gid, lid, XCorrScratchPad, startingScratchPadOffset, numXCorrEntries);
            computeVoltageRateOfChange(outputVoltageHistory, numElements, voltageHistorySize, gid, lid, voltageRateOfChangeScratchPad, startingScratchPadOffset, numXCorrEntries);
            if (gid == 2){ // H20
                int otherElement = 7; // IFEAR
                int spiOne = getScratchPadIndex(startingScratchPadOffset, otherElement, 0, numXCorrEntries);
                int spiTwo = getScratchPadIndex(startingScratchPadOffset, otherElement, 1, numXCorrEntries);
                int spiThree = getScratchPadIndex(startingScratchPadOffset, otherElement, 2, numXCorrEntries);
                int spiFour = getScratchPadIndex(startingScratchPadOffset, otherElement, 3, numXCorrEntries);
                float xcorrOne = XCorrScratchPad[spiOne];
                float xcorrTwo = XCorrScratchPad[spiTwo];
                float xcorrThree = XCorrScratchPad[spiThree];
                float xcorrFour = XCorrScratchPad[spiFour];
                float voltageRocOne = voltageRateOfChangeScratchPad[spiOne];
                float voltageRocTwo = voltageRateOfChangeScratchPad[spiTwo];
                float voltageRocThree = voltageRateOfChangeScratchPad[spiThree];
                float voltageRocFour = voltageRateOfChangeScratchPad[spiFour];
                //printf("kernel iteration %d: elementIDs %d vs. %d... XCorr: (0: %.2f, 1: %.2f, 2: %.2f, 3: %.2f) VROC: (0: %.2f, 1: %.2f, 2: %.2f, 3: %.2f)\n",kernelIterationNum, gid, otherElement, xcorrOne, xcorrTwo, xcorrThree, xcorrFour, voltageRocOne, voltageRocTwo, voltageRocThree, voltageRocFour);
            }
            
            // Now, this is where we adjust the connectome weights.
            
            // first rule we want to (try) to implement is:
            // if xcorr indices 0, 1, and 2 are all greater than .75, AND, 0 < 1 < 2
            // then, we increase weight by some rule
            // possibly: += learningRate
            // -------> learningRate will (very shortly) be different for every synapse, based upon age (maybe have learning rate slightly decrease each time its used to adjust the synaptic weight?
            float learningRate = .1;
            for ( i = 0; i < numElements; ++i){
                if (i == gid){
                    continue;
                }
                int spiOne = getScratchPadIndex(startingScratchPadOffset, i, 0, numXCorrEntries);
                int spiTwo = getScratchPadIndex(startingScratchPadOffset, i, 1, numXCorrEntries);
                int spiThree = getScratchPadIndex(startingScratchPadOffset, i, 2, numXCorrEntries);
                int spiFour = getScratchPadIndex(startingScratchPadOffset, i, 3, numXCorrEntries);
                float xcorrOne = XCorrScratchPad[spiOne];
                float xcorrTwo = XCorrScratchPad[spiTwo];
                float xcorrThree = XCorrScratchPad[spiThree];
                float xcorrFour = XCorrScratchPad[spiFour];
                float voltageRocOne = voltageRateOfChangeScratchPad[spiOne];
                float voltageRocTwo = voltageRateOfChangeScratchPad[spiTwo];
                float voltageRocThree = voltageRateOfChangeScratchPad[spiThree];
                float voltageRocFour = voltageRateOfChangeScratchPad[spiFour];
                float vrocMin = .3;
                //float vrocMax = 2.0;
                float xcorrMin = .95;
                
                //float avgVROC = (voltageRocOne+voltageRocTwo+voltageRocThree+voltageRocFour)/4.0;
//                if ((voltageRocOne >= vrocMin && voltageRocTwo >= vrocMin && voltageRocThree >= voltageRocFour && voltageRocFour >= vrocMin) && (xcorrOne >= xcorrMin && xcorrTwo >= xcorrOne && xcorrThree >= xcorrTwo && xcorrFour >= xcorrThree)){
                if ((voltageRocOne >= voltageRocTwo && voltageRocTwo >= voltageRocThree && voltageRocThree >= voltageRocFour && voltageRocFour >= vrocMin) && (xcorrOne >= xcorrMin && xcorrTwo >= xcorrOne && xcorrThree >= xcorrTwo && xcorrFour >= xcorrThree)){
                //if ((avgVROC >= vrocMin && avgVROC <= vrocMax) && (xcorrOne >= xcorrMin && xcorrTwo >= xcorrOne && xcorrThree >= xcorrTwo && xcorrFour >= xcorrThree)){
                    // then we increase the CHEM weight (different rule for GJ)
                    //printf("kernel iteration %d: elementIDs %d vs. %d... XCorr: (0: %.2f, 1: %.2f, 2: %.2f, 3: %.2f) VROC: (0: %.2f, 1: %.2f, 2: %.2f, 3: %.2f)\n",kernelIterationNum, gid, i, xcorrOne, xcorrTwo, xcorrThree, xcorrFour, voltageRocOne, voltageRocTwo, voltageRocThree, voltageRocFour);
                    
                    weight_idx = getConnectomeIndex(gid, i, numElements); // ROW MAJOR
                    if (chemWeights[weight_idx] < 0){
                        // it's inhibitory, leave it be for now (and in this section)
                        continue;
                    }
                    chemWeights[weight_idx] = chemWeights[weight_idx] + ((max_cs_weight - chemWeights[weight_idx]) * learningRate);
                }
            }
            
        }
        
        /*
        if (gid == 1){
            printf("START CLPRINT:\ninputs: ");
            int i;
            for (i = 0; i < numElements; ++i){
                printf("%.2f, ",voltages[i]);
            }
            printf("\noutputs: ");
            for (i = 0; i < numElements; ++i){
                printf("%.2f, ",voltages[i]);
            }
            printf("\nEND CLPRINT\n");
        }
         */
    }
    
}

/*
 *
 */
void computeXCorr(__global float* outputVoltageHistory, int numElements, int voltageHistorySize, int gid, int lid, __local float* XCorrScratchPad, int startingScratchPadOffset, int numXCorrEntries){
    //printf("gid, lid: %d, %d\n", gId, lid);
    // 'a' refers to the array, 'A' in XCorrMulitply. That is, we do a xcorr between 'A', and 'B'.
    // 'a' is the voltage history for 'gid', and 'b' starts at '0', and goes to numElements-1.
    // at some point, 'a' will be the same as 'b'. That's fine, we will know that xcorr,
    // because when it is in the scratch pad, it will have index equal to 'gid'
    // (in the scratch pad, 'a' with element 0 will be at 0, 'a' with element 1 will be at 1, and so on.
    //
    // XCorrScratchPad will have a row for each set of xcorr computations between 'a' and a given 'b'
    // (of course, offset by the 'startingScratchPadOffset' to account for the specific thread's 'lid' (local id)),
    // This means that:
    //      - index 0 of that row will hold xcorr(a, b), stacked.
    //      - index 1 of that row will hold xcorr(a, b), with 'b' advanced by 1
    //      - index 2 of that row will hold xcorr(a, b), with 'b' advanced by 2
    //
    // 'a' will always have an offset of 0, and we will always use indices 0, 1, and 2.
    // 'b' will 'slide', and will start off with an offset of 0, so that it is stacked directly on-top of 'a'.
    // then, on the next 'xcorrIteration', 'b' will be offset by 1, so we use, 1, 2, and 3.
    // finally, on the 3rd iteration, (when xcorrIteration = 2), 'b' will be offset by 2, and we will use 2, 3, and 4.
    // That, is why outputVoltageHistory (currently, as of this writing), has a size of 6 (5 indices that we use, and 1 for the 'current' [staging] ouput voltage)
    //
    // we do two auto xcorrs, one for 'a', and one for 'b' (each b), with them stacked on top of themselves,
    int i;
    int j;
    int xcorrLength = numXCorrEntries; // This means that we use an array of this length (3, as of this writing) to compute the xcorr (e.g., indices 0, 1, and 2 of 'a')
    int aOffset = (voltageHistorySize-1) - xcorrLength; // we want to start at the 'end' (farthest back possible), to look at A's most recent 'window'
    int aIndex = getOutputVoltageHistoryIndex(gid, aOffset, voltageHistorySize);
    int bIndex = 0;
    int windowNum = 0; // because of
    int xcorrIteration = 0; // we'll use this to offset 'b' by 0, 1, and 2
    int bOffset = aOffset; // start off with A and B lined up in time, and then decrement bOffset on each loop.
    // seems to make the most sense to use the full length of each signal for the autocorr. note, we are subtracting 1 from the voltageHistorySize due to the 1 'staging' index.
    //int autoCorrSize = voltageHistorySize - 1;
    float autoCorrA = XCorrMultiply(outputVoltageHistory,aIndex,aIndex, xcorrLength);
    float autoCorrB = 0;
    float divisor = 0; // this will be the sqrt(autocorrA + autoCorrB), which will divide (so, normalize) each xcorr
    float xcorrResult = 0; 
    int scratchPadIndex = 0;
    for (i = 0; i < numElements; ++i){ // loop through outputVoltageHistory,
        xcorrIteration = 0;
        bIndex = getOutputVoltageHistoryIndex(i, bOffset,voltageHistorySize); // the beginning of element 'i's voltage history
        for (xcorrIteration = 0; xcorrIteration < xcorrLength; ++xcorrIteration){
            autoCorrB = XCorrMultiply(outputVoltageHistory, bIndex, bIndex, xcorrLength); // last arg used to be 'autoCorrSize', but now it's the same as xcorrLength
            divisor = sqrt(autoCorrA * autoCorrB);
            scratchPadIndex = getScratchPadIndex(startingScratchPadOffset, i, xcorrIteration, xcorrLength);
            if (divisor == 0){ // then correlation between two signals is zero
                XCorrScratchPad[scratchPadIndex] = 0.f;
            }
            else{
                xcorrResult = XCorrMultiply(outputVoltageHistory, aIndex, bIndex, xcorrLength);
                XCorrScratchPad[scratchPadIndex] = xcorrResult/divisor;
            }
            /*
            if (scratchPadIndex == 117 || scratchPadIndex == 118 || scratchPadIndex == 119)
                printf("xcorr: %2.f, divisor: %.2f, total: %.2f (idx: %d)\n",xcorrResult, divisor,XCorrScratchPad[scratchPadIndex], scratchPadIndex);
             */
            bIndex--;
        }
        
    }
    
}

float XCorrMultiply(__global float* outputVoltageHistories, int aOffset, int bOffset, int len){
    float result = 0;
    for (int i = 0; i < len; ++i){
        result += outputVoltageHistories[aOffset+i] * outputVoltageHistories[bOffset+i];
    }
    return result;
}


float computeVoltageRateOfChange(__global float* outputVoltageHistory, int numElements, int voltageHistorySize, int gid, int lid, __local float* voltageRateOfChangeScratchPad, int startingScratchPadOffset, int numXCorrEntries){
    int i;
    int j;
    int xcorrLength = numXCorrEntries;
    float timeSteps = xcorrLength;
    int elementBaseIndex = -1;
    int elementLastIndex = -1;
    float rateOfChange = -1.f;
    int scratchPadIndex = -1;
    int voltageHistoryIndexBase = 0;
    for (i = 0; i < numElements; ++i){
        voltageHistoryIndexBase = (voltageHistorySize-1) - xcorrLength;
        for (j = 0; j < numXCorrEntries; ++j){
            elementBaseIndex = getOutputVoltageHistoryIndex(i, voltageHistoryIndexBase, voltageHistorySize);
            elementLastIndex = elementBaseIndex + xcorrLength;
            rateOfChange = (outputVoltageHistory[elementLastIndex] - outputVoltageHistory[elementBaseIndex])/timeSteps;
            scratchPadIndex = getScratchPadIndex(startingScratchPadOffset, i, j, xcorrLength);
            voltageRateOfChangeScratchPad[scratchPadIndex] = rateOfChange;
            voltageHistoryIndexBase--;
        }
    }
}

/* c++ xcorr implementation for reference when doing an opencl implementation: */
/*
#include <stdio.h>
#include <string>
#include <cmath>

// created by Andrew McDonald, 1/10/17, Copyright 2017, All Rights Reserved

// implements the window part of the sliding window.
//
// NOTE: this requires A and B have the same length.
//
float xcorrMultiply(float* A, float* B, int len, int windowNum){
    if ((windowNum < 0) || ((windowNum + 1) >= (2 * len))){// see "aStartingIndex = (windowNum - len) + 1" line for reasoning
        return 0.f;  // out of range
    }
    int lastIndex = len - 1;
    int iterations = 0;
    int aStartingIndex = 0;
    int bStartingIndex = 0;
    if (windowNum >= len){ // then we change our approach, because B[0] is under A[1] (assuming we are sliding B)
        bStartingIndex = 0; // always start at the beginning of B
        aStartingIndex = (windowNum - len) + 1; // at (6 - 4) + 1, we're done (if windowNum == 7, we have a problem)... that's the reasoning for the check above.
        iterations = len - aStartingIndex;
    }
    else {
        bStartingIndex = lastIndex - windowNum; // also, windowNum can't be less than 0, or we go past the end of B.
        aStartingIndex = 0; // always start at the beginning of A
        iterations = len - bStartingIndex;
    }
    float result = 0;
    int aIndex = aStartingIndex;
    int bIndex = bStartingIndex;
    //printf("window: %d\n",windowNum);
    for (int i = 0; i < iterations; ++i){
        //printf("\raIndex, bIndex: %d (%.2f), %d (%.2f)\n",aIndex, A[aIndex], bIndex, B[bIndex]);
        result += A[aIndex] * B[bIndex];
        aIndex++;
        bIndex++;
    }
    return result;
}

int computeXCorrNumWindows(int len){
    // see xcorrMulitply for computation reasoning
    return ((2*len)-1);
}

// normalizer should be 1, unless being called form normalizedXCorr, in which case, it will pass the appropriate value 
float* xcorr(float* A, int aLen, float* B, int bLen, float normalizer){
    if ((aLen != bLen) || (aLen == 0)){
        printf("xcorr requires two non-zero arrays of equal length.\n");
    }
    int len = aLen;
    int maxWindows = computeXCorrNumWindows(len);
    float* xcorrResults = new float[maxWindows];
    // note, signals are on top of eachother at (windowNum = len - 1)
    printf("note, signals are on top of eachother at (windowNum = len - 1) => %d:\n",len-1);
    for (int i = 0; i < maxWindows; ++i){
        xcorrResults[i] = (xcorrMultiply(A, B, len, i))/normalizer;
        printf("window %d: %.2f\n",i, xcorrResults[i]);
    }
    return xcorrResults;
}

float* normalizedXCorr(float* A, int aLen, float* B, int bLen){
    if ((aLen != bLen) || (aLen == 0)){
        printf("xcorr requires two non-zero arrays of equal length.\n");
    }
    float autoCorrA = xcorrMultiply(A, A, aLen, aLen - 1 ); // windowNum 'len - 1' (if first windowNum is 0), gives signals on top of each other (time 0)
    float autoCorrB = xcorrMultiply(B, B, bLen, bLen - 1 );
    float divisor = sqrt(autoCorrA*autoCorrB);
    printf("DIVISOR: %.2f\n", divisor);
    return  xcorr(A, aLen, B, bLen, divisor);
}

int main(int argc, char** argv){
    int len = 11;
    float A[] = {0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0};
    float B[] = {0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0};
    //int len = 4;
    //float A[] = {0, 1, 0, 0};
    //float B[] = {0, 1, 0, 0};
    float* results = xcorr(A, len, B, len, 1.f);
    float* normalizedResults = normalizedXCorr(A, len, B, len);
    delete[] results;
    delete[] normalizedResults;
    return 0;
}
*/
