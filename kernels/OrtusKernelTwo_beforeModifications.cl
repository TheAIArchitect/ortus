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
 
 each block has 'numElements' rows, and 'numXcorrEntries' cols (well, if the scratch pad weren't a 1D buffer it would).
 
 so, we multiply the lId (local id), by the block size, which is 'numElements*numXCorrEntries', to get the starting index (thus, offset), for a thread in a work-group 
 
int getLocalScratchPadStartingOffset(int lId, int numElements, int numXcorrEntries){
    return lId*(numElements*numXcorrEntries);
}

int getScratchPadIndex(int startingScratchPadOffset, int elementId, int xcorrEntry, int numXcorrEntries){
    return startingScratchPadOffset+get2DIndexAs1D(elementId, xcorrEntry, numXcorrEntries);
}
 */

/*
 note: shouldProbe, chemContrib, and gapContrib:
 used for the Probe to collect information about which neurons/muscles are passing what to other neurons/muscles.
 
 if shouldProbe == 0, nothing happens with the *Contrib vars.
 if shouldProbe == 1, we'll fill the *Contrib vars in the same way as the cs_ and gj_ matricies,
 but instead of each column holding the  pre synaptic conn weight of that neuron/muscle to the post synaptic neuron/muscle (the row),
 each column will hold the voltage/activation contribution of that synapse/gap junction to the neuron/muscle in that row.
 */
__kernel void OrtusKernel( __global float *voltages, // read and write
                         __global float *outputVoltageHistory, // read and write
                         __global float *gapWeights, // read and write
                         __global float *chemWeights, // read and write
                          __global float* gapContrib, // read and write
                          __global float* chemContrib, // read and write
                          const int shouldProbe, // read only
                          float gapNormalizer, // read and write
                          float chemNormalizer, // read and write
                          __global int* metadata,// read only... for now?
                          __local float* xcorrScratchPad){// access rows by work-group id!
    
    // iteration_number_for_row_access:
    //  Tells us what iteration we're on, 0 indexed.
    // gid:
    //  Determine the row of the CS / GJ matrix we're working on
    // loop_iter:
    //  The column numbers of the CS / GJ matrix that we get v_current values from
    //  on row = gid. So any given v_current comes from:
    //      idx = getIndex(iteration_number_for_row_access, loop_iter, colCount)
    //      float v_current = voltages[idx]
    // Any given connection weight comes from:
    //  idx = getIndex(gid, loop_iter, colCount)
    //  {cs, gj}_matrix[idx]
    
    // current row
    __local unsigned int dimidx;
    dimidx = 0; // dimension index. we only have one dimension (with index 0).
    int gid = get_global_id(dimidx); // gives id of current work-item
    //size_t get_global_size(dimidx);// gives total number of work-items (for specified dimension)
    //size_t get_group_id(dimidx); // gives current work-group id (from 0 to (# of work-groups -1))
    //size_t get_local_id(dimidx); // gives id of work-item *within* its work group (so, from 0 to (local_size - 1))
    //size_t get_local_size(dimidx); // gives number of work-items in a work-group
    //size_t get_num_groups(dimidx); // gives total number of work-groups
    
    
    // can i declare these as global??
    __local int rowCount;
    __local int colCount;
    __local int kernelIterationNum;
    __local int voltageHistorySize;
    rowCount = metadata[0];
    colCount = metadata[1];
    kernelIterationNum = metadata[2];
    voltageHistorySize = metadata[3]; // this is one larger than we use for the xcorr -- the last one is the staging one.
    
    ///////////// *********** replace rowCount and colCount with numElments, and gid with elementId !!!!!!!!!!!!!!!!
    /* BEGIN CHECKS -- comment out for speed, but leave uncommented for development */
    // assign
    // rowCount == colCount
    // then set numElements = rowCount;
    
    
    /* END CHECKS */
    
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
    //printf("rowCount, colCount: %u, %u\n", rowCount, colCount);
    // Don't do anything if we're out of range.
    if(gid <= (rowCount - 1)){
        // using 0 for the row_num, because right now voltages is just a vector
//        size_t my_v_curr_idx = getIndex(0, gid, colCount); // ROW MAJOR
        size_t my_v_curr_idx = getVoltageIndex(gid);
        float my_v_current = voltages[my_v_curr_idx];
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
//            voltage_idx = getIndex(0, loop_iter, colCount); // ROW MAJOR
            voltage_idx = getVoltageIndex(loop_iter);
            weight_idx = getConnectomeIndex(gid, loop_iter, colCount); // ROW MAJOR
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
        //size_t w_idx = getIndex(iteration_number_for_row_access + 1, gid, colCount);
        //voltages[w_idx] = v_curr_finished;
        
//        size_t fin_idx = getIndex(0, gid, colCount); // ROW MAJOR
        size_t fin_idx = getVoltageIndex(gid);
        
        //voltages[fin_idx] = v_curr_finished;
        voltages[fin_idx] = v_curr_finished;
        
        // update the outputVoltageHistory
        int i;
        int ovhSizeMinusOne = voltageHistorySize - 1;
        // TEST THIS LINE BELOW!!! <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<!!!!!!!!!!!!!!!!!!!!!!!!!!!!>>>>>>>>>>>>>>>>>>>>>>>>>>>>
        // size_t currentOVHIndex =  getIndex(gid, 0, voltageHistorySize);// the first (oldest) entry for this element. for next entry, just add one.
        // push all the existing output voltages down one (losing the oldest)
        for(i = 0; i < ovhSizeMinusOne; ++i){
            // NOTE: calling getIndex with row and col flipped, because we're doing row-by-row (e.g., element 0 uses 0-5, if we have an ovh of size 6).
//            size_t ovhIndex = getIndex(gid, i, voltageHistorySize); // ROW MAJOR (each row contains all voltages for that element, so for each element, we need a new row -- row indexing is same as connectome)
            size_t ovhIndex = getOutputVoltageHistoryIndex(gid, i, voltageHistorySize); // ROW MAJOR (each row contains all voltages for that element, so for each element, we need a new row -- row indexing is same as connectome)
//            size_t ovhIndexPlusOne = getIndex(gid, i+1, voltageHistorySize); // ROW MAJOR
            size_t ovhIndexPlusOne = getOutputVoltageHistoryIndex(gid, i+1, voltageHistorySize); // ROW MAJOR
            outputVoltageHistory[ovhIndex] = outputVoltageHistory[ovhIndexPlusOne];
        }
        // then set the most recent one in the 'staging' area
        outputVoltageHistory[getIndex(gid,ovhSizeMinusOne, voltageHistorySize)] = v_curr_finished;
        
        /*
        if (kernelIterationNum >= ovhSizeMinusOne){ // then we know we have a full outputVoltageHistory
            // do xcorr
            // do gap and chem work
            if (gid == 0){
                printf("ovhs: %d, v_curr_finished: %.2f\n", voltageHistorySize, v_curr_finished);
            printf("%.2f, %.2f, %.2f, %.2f, %.2f, %.2f\n",outputVoltageHistory[0],outputVoltageHistory[1],outputVoltageHistory[2],outputVoltageHistory[3],outputVoltageHistory[4],outputVoltageHistory[5]);
            }
        }
         */
        
        /*
        if (gid == 1){
            printf("START CLPRINT:\ninputs: ");
            int i;
            for (i = 0; i < rowCount; ++i){
                printf("%.2f, ",voltages[i]);
            }
            printf("\noutputs: ");
            for (i = 0; i < rowCount; ++i){
                printf("%.2f, ",voltages[i]);
            }
            printf("\nEND CLPRINT\n");
        }
         */
    }
    
}

/*
 *
void computeXCorr(float* outputVoltageHistory, int voltageHistorySize, int gId, int wkGpId, float* xcorrScratchPad ){
    // 
}

float xcorrMultiply(float* A, int aOffset, float* B, int bOffset, int len, int windowNum){
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

 */
 

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
