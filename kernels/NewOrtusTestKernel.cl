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
void computeXCorr(global float* activations, int NUM_ELEMENTS, int ACTIVATION_HISTORY_SIZE, int gId, int lId, local float* XCorrScratchpad, int xcorrScratchpadBaseIndex, int NUM_XCORR_COMPUTATIONS, int XCORR_SIZE);
float XCorrMultiply(global float* activations, int postOffset, int preOffset, int XCORR_SIZE);
void computeSlope(global float* activations, int NUM_ELEMENTS, int ACTIVATION_HISTORY_SIZE, int gId, int lId, local float* SlopeScratchpad, int slopeScratchpadBaseIndex, int NUM_SLOPE_COMPUTATIONS, int SLOPE_SIZE);


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// this is the number of kernel args -1
constant int NUM_KERNEL_ARGS_WITH_INFO = 6;
constant int NUM_COLS_PER_KERNEL_ARG_INFO = 6;
constant float EPSILON = FLT_MIN*2; // for float equivalency check

/* returns true if two floats are effectively equal */
bool fCompare(float a, float b){
    return (fabs(a-b) < EPSILON);
}


/* everything, including blade, is zero indexed */
int getIndex(int row, int col, int page, int blade, int maxRows, int maxCols, int maxPages, int bladeSize){
    int pageSize = maxRows * maxCols;
    int toSkip_pages = page * pageSize;
    int toSkip_blades = blade * bladeSize;
    int toSkip_rows = row * maxCols;
    return toSkip_blades + toSkip_pages +  toSkip_rows + col;
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
    int kArgRow = getIndex(kernelArgNum, 0, 0, 0, NUM_KERNEL_ARGS_WITH_INFO, NUM_COLS_PER_KERNEL_ARG_INFO, 0, 0);
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
    int numWG = get_num_groups(dimidx);
    // BEGIN vars for extracting necessary data from kernel args
    // declare local vars for blade counts, and then assign values
    local int ka0_blades, ka1_blades, ka2_blades, ka3_blades, ka4_blades, ka5_blades;
    ka0_blades = 3; ka1_blades = 8; ka2_blades = 2; ka3_blades = 1; ka4_blades = 8; ka5_blades = 2;
    //if (gId == 1) printf("ka0_blades: %d\n", ka0_blades);
    // for the kernelArgs we only access once, like kernel arg 0, we don't need to worry about saving the parameters, and we can use general ones
    int kernelArgNum, kernelArgBladeCount, kernelArgStride, kernelArgRows, kernelArgCols, kernelArgPages, bladeIndexRelativeToKernelArg;
    // for the kernel args that we'll have to access in the loop, we want to save certain parameters, so we'll name them by kernel arg number
    // don't need bladeCount because we need to have variables coded in for each blade... the bladeCount var above is for sanity checking (see below)
    int kernelArg1Stride, kernelArg1Rows, kernelArg1Cols, kernelArg1Pages;
    int kernelArg2Stride, kernelArg2Rows, kernelArg2Cols, kernelArg2Pages;
    int kernelArg3Stride, kernelArg3Rows, kernelArg3Cols, kernelArg3Pages;
    // END vars for extracting necessary data from kernel args
    
    // BEGIN METADATA COLLECTION (kernel arg 4)
    // very first thing we want to do is collect metadata.
    // (kernel arg 4): elementAttributes -- Scalars 
    kernelArgNum = 4;
    getKernelArgInfo(kernelArgInfo, kernelArgNum, &kernelArgBladeCount, &kernelArgStride, &kernelArgRows, &kernelArgCols, &kernelArgPages);
    if (gId ==1 && ka4_blades != kernelArgBladeCount) printf("Error: incorrect blade count for kernel arg %d (should be %d, actually is %d)\n", kernelArgNum, ka0_blades, kernelArgBladeCount);
    //if (gId == 1) printKernelArgInfo(kernelArgNum, kernelArgBladeCount, kernelArgStride, kernelArgRows, kernelArgCols, kernelArgPages);
    
    local int NUM_ELEMENTS, MAX_ELEMENTS, KERNEL_ITERATION_NUM, ACTIVATION_HISTORY_SIZE, NUM_XCORR_COMPUTATIONS, XCORR_SIZE, NUM_SLOPE_COMPUTATIONS, SLOPE_SIZE;
    int metadataBladeNum = 0;
    NUM_ELEMENTS = (int) metadata[metadataBladeNum];
    // stop if global is out of range:
    if (gId >= NUM_ELEMENTS) return; // no reason to do anything more if global id is >= # elements
    metadataBladeNum = 1;
    MAX_ELEMENTS = (int) metadata[metadataBladeNum];
    metadataBladeNum = 2;
    KERNEL_ITERATION_NUM = (int) metadata[metadataBladeNum];
    metadataBladeNum = 3;
    ACTIVATION_HISTORY_SIZE = (int) metadata[metadataBladeNum];
    metadataBladeNum = 4;
    NUM_XCORR_COMPUTATIONS = (int) metadata[metadataBladeNum];
    metadataBladeNum = 5;
    XCORR_SIZE = (int) metadata[metadataBladeNum];
    metadataBladeNum = 6;
    NUM_SLOPE_COMPUTATIONS = (int) metadata[metadataBladeNum];
    metadataBladeNum = 7;
    SLOPE_SIZE = (int) metadata[metadataBladeNum];
    //if (gId == 1) printf("Ortus Metadata:\nNUM_ELEMENTS: %d\nMAX_ELEMENTS: %d\nKERNEL_ITERATION_NUM: %d\nACTIVATION_HISTORY_SIZE: %d\nNUM_XCORR_COMPUTATIONS: %d\nXCORR_SIZE: %d\nNUM_SLOPE_COMPUTATIONS: %d\nSLOPE_SIZE: %d\n", NUM_ELEMENTS, MAX_ELEMENTS, KERNEL_ITERATION_NUM, ACTIVATION_HISTORY_SIZE, NUM_XCORR_COMPUTATIONS, XCORR_SIZE, NUM_SLOPE_COMPUTATIONS, SLOPE_SIZE);
   
    int WEIGHT_HISTORY_SIZE = 4; // 3 is for the updated values, 0, 1, and 2 are current, historic1, and historic2.
    int LAST_WEIGHT_HISTORY_INDEX = WEIGHT_HISTORY_SIZE - 1;
    
    
    // (kernel arg 0): elementAttributes -- 1D Blades (use row == 0 for getIndex)
    kernelArgNum = 0;
    getKernelArgInfo(kernelArgInfo, kernelArgNum, &kernelArgBladeCount, &kernelArgStride, &kernelArgRows, &kernelArgCols, &kernelArgPages);
    if (gId ==1 && ka0_blades != kernelArgBladeCount) printf("Error: incorrect blade count for kernel arg %d (should be %d, actually is %d)\n", kernelArgNum, ka0_blades, kernelArgBladeCount);
    //if (gId == 1) printKernelArgInfo(kernelArgNum, kernelArgBladeCount, kernelArgStride, kernelArgRows, kernelArgCols, kernelArgPages);
    // assign vars
    int elementTypeIndex, elementAffectIndex, elementThreshIndex;
    float elementType, elementAffect, elementThresh; // NOTE: change first two to int, or compare with epsilon compare function
    // element type
    bladeIndexRelativeToKernelArg = 0;
    elementTypeIndex = getIndex(0, gId, 0, bladeIndexRelativeToKernelArg, kernelArgRows, kernelArgCols, kernelArgPages, kernelArgStride);
    elementType = elementAttributes[elementTypeIndex];
    // element affect
    bladeIndexRelativeToKernelArg = 1;
    elementAffectIndex = getIndex(0, gId, 0, bladeIndexRelativeToKernelArg, kernelArgRows, kernelArgCols, kernelArgPages, kernelArgStride);
    elementAffect = elementAttributes[elementAffectIndex];
    // element thresh
    bladeIndexRelativeToKernelArg = 2;
    elementThreshIndex = getIndex(0, gId, 0, bladeIndexRelativeToKernelArg, kernelArgRows, kernelArgCols, kernelArgPages, kernelArgStride);
    elementThresh = elementAttributes[elementThreshIndex];
    printf("\telement '%d' {type: %f, affect: %f, thresh: %f}\n", gId, elementType, elementAffect, elementThresh);
    
    // (kernel arg 5): scratchpads -- 2D Blades
    // => each instance's base index = local_id * (num_elements * xcorr_computations)
    // -> equal to: lId * (NUM_ELEMENTS * kernelArgCols)
    // => access an element's xcorr row with current opencl gId (post element) = base_index + (gId*kernelArgCols)
    // => then just add xcorr computation number to elements xcorr row to get specific index
    // *** AS LONG AS THE SIZE OF THE SLOPE AND XCORR SCRATCHPADS REMAIN THE SAME ***
    // *** THE SLOPE SCRATCHPAD CAN BE ACCESSED IN EXACTLY THE SAME WAY!!! ***
    kernelArgNum = 5;
    getKernelArgInfo(kernelArgInfo, kernelArgNum, &kernelArgBladeCount, &kernelArgStride, &kernelArgRows, &kernelArgCols, &kernelArgPages);
    if (gId ==1 && ka5_blades != kernelArgBladeCount) printf("Error: incorrect blade count for kernel arg %d (should be %d, actually is %d)\n", kernelArgNum, ka5_blades, kernelArgBladeCount);
    //if (gId == 1) printKernelArgInfo(kernelArgNum, kernelArgBladeCount, kernelArgStride, kernelArgRows, kernelArgCols, kernelArgPages);
    int xcorrScratchpadBaseIndex, slopeScratchpadBaseIndex;
    // we can pretend that each lId is a page, and the page size is NUM_ELEMENTS * kernelArgCols
    int scratchpadEffectivePageSize = MAX_ELEMENTS * kernelArgCols; // kernelArgCols should be the same as NUM_XCORR_COMPUTATIONS
    bladeIndexRelativeToKernelArg = 0;
    xcorrScratchpadBaseIndex = (bladeIndexRelativeToKernelArg * kernelArgStride) + (lId * scratchpadEffectivePageSize);
    bladeIndexRelativeToKernelArg = 1;
    slopeScratchpadBaseIndex = (bladeIndexRelativeToKernelArg * kernelArgStride) + (lId * scratchpadEffectivePageSize);
    int scratchpadBladeStride = kernelArgStride;
    //if (gId == 1) printf("\t(lId: %d) xcorr scratchpad base index: %d, columns per row: %d, rows: %f\n", lId, xcorrScratchpadBaseIndex, kernelArgCols, ((float)kernelArgStride/numWG));
    //if (gId == 1) printf("\t(lId: %d) slope scratchpad base index: %d, columns per row: %d, rows: %f\n", lId, slopeScratchpadBaseIndex, kernelArgCols, ((float)kernelArgStride/numWG));
                         
    
    
    // relationAttribute, weights, and activations all need to be done in a loop, because pre and post elements are involved
    // the 'post' will stay constant -- that is the gId, while the 'pre' will loop through all elements.
    // therefore, these kernel args get accessed in the main loop, so we want to get the info, and put it in more 'permanent' (named, w.r.t. kernel arg number) variables.
    // (kernel arg 1): relationAttributes -- 2D Blades
    kernelArgNum = 1;
    getKernelArgInfo(kernelArgInfo, kernelArgNum, &kernelArgBladeCount, &kernelArg1Stride, &kernelArg1Rows, &kernelArg1Cols, &kernelArg3Pages);
    if (gId ==1 && ka1_blades != kernelArgBladeCount) printf("Error: incorrect blade count for kernel arg %d (should be %d, actually is %d)\n", kernelArgNum, ka1_blades, kernelArgBladeCount);
    //if (gId == 1) printKernelArgInfo(kernelArgNum, kernelArgBladeCount, kernelArg1Stride, kernelArg1Rows, kernelArg1Cols, kernelArg3Pages);
    // (kernel arg 2): weights -- 3D Blades
    kernelArgNum = 2;
    getKernelArgInfo(kernelArgInfo, kernelArgNum, &kernelArgBladeCount, &kernelArg2Stride, &kernelArg2Rows, &kernelArg2Cols, &kernelArg3Pages);
    if (gId ==1 && ka2_blades != kernelArgBladeCount) printf("Error: incorrect blade count for kernel arg %d (should be %d, actually is %d)\n", kernelArgNum, ka2_blades, kernelArgBladeCount);
    //if (gId == 1) printKernelArgInfo(kernelArgNum, kernelArgBladeCount, kernelArg2Stride, kernelArg2Rows, kernelArg2Cols, kernelArg3Pages);
    // (kernel arg 3): activations -- 1D Blades
    kernelArgNum = 3;
    getKernelArgInfo(kernelArgInfo, kernelArgNum, &kernelArgBladeCount, &kernelArg3Stride, &kernelArg3Rows, &kernelArg3Cols, &kernelArg3Pages);
    if (gId ==1 && ka3_blades != kernelArgBladeCount) printf("Error: incorrect blade count for kernel arg %d (should be %d, actually is %d)\n", kernelArgNum, ka3_blades, kernelArgBladeCount);
    //if (gId == 1) printKernelArgInfo(kernelArgNum, kernelArgBladeCount, kernelArg3Stride, kernelArg3Rows, kernelArg3Cols, kernelArg3Pages);
    
    
    // THE ASSUMPTION IS THAT THE ROW IS THE 'post' AND THE COL IS THE 'pre'
    // (that is, the relation and weight matricies are filled in a transposed manner)
    
    // relationAttributes
    int relationTypeIndex, relationPolarityIndex, relationDirectionIndex, relationAgeIndex, relationThreshIndex, relationDecayIndex, relationMutabilityIndex;
    float relationType, relationPolarity, relationDirection, relationAge, relationThresh, relationDecay, relationMutability;
    // weights -- indices are for current weight. add 1 *page* for previous weight, 2 *pages* for weight 2 timesteps ago.
    int csWeightBaseIndex, gjWeightBaseIndex;
    float csWeight, gjWeight, csHistoric1, gjHistoric1, csHistoric2, gjHistoric2;
    // (kernel arg 3) activations -- remember, post is the current 'gId' -- historic activations work the same as for the weights
    // (though number of historical values available are probably not the same -- or, they certainly aren't guaranteed to be)
    int preActivationBaseIndex, postActivationBaseIndex;
    int postElement, preElement;
    float preActivation, postActivation;
    postElement = gId;
    postActivationBaseIndex = postElement * kernelArg3Cols;
    //  NOTE: the LAST index of activations for each element is the slot for the updated activation at the end of the kernel iteration
    // everything from a relative 0 (so, postElementBaseIndex) to (ACTIVATION_HISTORY_SIZE - 1) (so, USABLE_ACTIVATION_HISTORY),
    // may be used for computations, and MUST NOT BE ALTERED IN THIS KERNEL, because it will change what other instances of the kernel see!
    // NO kernel instances should be looking at any relative 'ACTIVATION_HISTORY_SIZE-1' slot, other than their own.
    int USABLE_ACTIVATION_HISTORY = ACTIVATION_HISTORY_SIZE - 2;
    int UPDATED_ACTIVATION_SLOT = postActivationBaseIndex + (ACTIVATION_HISTORY_SIZE - 1);
    
    
    if (KERNEL_ITERATION_NUM >= USABLE_ACTIVATION_HISTORY){ // can't compute anything with historic values until we have historic values.
        // do XCorr and Slope computations
        computeXCorr(activations, NUM_ELEMENTS, ACTIVATION_HISTORY_SIZE, gId, lId, scratchpad, xcorrScratchpadBaseIndex, NUM_XCORR_COMPUTATIONS, XCORR_SIZE);
        computeSlope(activations, NUM_ELEMENTS, ACTIVATION_HISTORY_SIZE, gId, lId, (scratchpad + (sizeof(float)*scratchpadBladeStride)), slopeScratchpadBaseIndex, NUM_SLOPE_COMPUTATIONS, SLOPE_SIZE);
    }
    
    // START main loop
    local float activationDecayConstant, minActivation, maxActivation, inhibitRevPot, exciteRevPot, inhibitExciteRange;
    
    activationDecayConstant = .2; // loses % of its charge (multiply by 100 for %)
    inhibitRevPot = -10.0f;
    exciteRevPot = 30.0f;
    inhibitExciteRange = exciteRevPot - inhibitRevPot;
    minActivation = -100.f;
    maxActivation = 100.f;
    int weightPageSize = kernelArg2Cols * kernelArg2Rows;
    float csIncoming = 0.f;
    float gjIncoming = 0.f;
    float totalIncomingActivation = 0.f;
    for (preElement = 0; preElement < NUM_ELEMENTS; ++preElement){
        
        preActivationBaseIndex = preElement * kernelArg3Cols;
        
        // now that we have the pre and post element indices, set the relation attributes, weights, and activations
        // kernel arg 1 (relationAttributes) -- 2D Blades
        //// NOTE NOTE NOTE: instead of all this shit, just add 1 'blade size' (so, Stride) to each index, to get the next one!!! (try this to verify...)
        // relationType
        bladeIndexRelativeToKernelArg = 0;
        relationTypeIndex = getIndex(postElement, preElement, 0, bladeIndexRelativeToKernelArg, kernelArg1Rows, kernelArg1Cols, kernelArg1Pages, kernelArg1Stride);
        relationType = relationAttributes[relationTypeIndex];
        // relationPolarity
        bladeIndexRelativeToKernelArg = 1;
        relationPolarityIndex = getIndex(postElement, preElement, 0, bladeIndexRelativeToKernelArg, kernelArg1Rows, kernelArg1Cols, kernelArg1Pages, kernelArg1Stride);
        relationPolarity = relationAttributes[relationPolarityIndex];
        // relationPreDirection
        //bladeIndexRelativeToKernelArg = 2;
        ////relationDirectionIndex = getIndex(postElement, preElement, 0, bladeIndexRelativeToKernelArg, kernelArg1Rows, kernelArg1Cols, kernelArg1Pages, kernelArg1Stride);
        ////relationDirection = relationAttributes[relationDirectionIndex];
        // relationPostDirection
        //bladeIndexRelativeToKernelArg = 3;
        // relationAge
        bladeIndexRelativeToKernelArg = 4;
        relationAgeIndex = getIndex(postElement, preElement, 0, bladeIndexRelativeToKernelArg, kernelArg1Rows, kernelArg1Cols, kernelArg1Pages, kernelArg1Stride);
        relationAge = relationAttributes[relationAgeIndex];
        // relationThresh
        bladeIndexRelativeToKernelArg = 5;
        relationThreshIndex = getIndex(postElement, preElement, 0, bladeIndexRelativeToKernelArg, kernelArg1Rows, kernelArg1Cols, kernelArg1Pages, kernelArg1Stride);
        relationThresh = relationAttributes[relationThreshIndex];
        // relationDecay
        bladeIndexRelativeToKernelArg = 6;
        relationDecayIndex = getIndex(postElement, preElement, 0, bladeIndexRelativeToKernelArg, kernelArg1Rows, kernelArg1Cols, kernelArg1Pages, kernelArg1Stride);
        relationDecay = relationAttributes[relationDecayIndex];
        // relationMutability
        bladeIndexRelativeToKernelArg = 7;
        relationMutabilityIndex = getIndex(postElement, preElement, 0, bladeIndexRelativeToKernelArg, kernelArg1Rows, kernelArg1Cols, kernelArg1Pages, kernelArg1Stride);
        relationMutability = relationAttributes[relationMutabilityIndex];
        // kernel arg 2 (weights) -- 3D Blades
        // CS (chemical synapse) weights
        bladeIndexRelativeToKernelArg = 0;
        csWeightBaseIndex = getIndex(postElement, preElement, 0, bladeIndexRelativeToKernelArg, kernelArg2Rows, kernelArg2Cols, kernelArg2Pages, kernelArg2Stride);
        csWeight = weights[csWeightBaseIndex];
        csHistoric1 = weights[csWeightBaseIndex + weightPageSize]; // 2D weight histories are stacked, so, hopefully this works...
        //int csHistoric1Index = getIndex(postElement, preElement, 1, bladeIndexRelativeToKernelArg, kernelArg2Rows, kernelArg2Cols, kernelArg2Pages, kernelArg2Stride);// this is what the historic index would be without the 'shortcut' (that may or may not work...)
        csHistoric2 = weights[csWeightBaseIndex + 2*weightPageSize];
        // GJ (gap junction) weights
        bladeIndexRelativeToKernelArg = 1;
        gjWeightBaseIndex = getIndex(postElement, preElement, 0, bladeIndexRelativeToKernelArg, kernelArg2Rows, kernelArg2Cols, kernelArg2Pages, kernelArg2Stride);
        gjWeight = weights[gjWeightBaseIndex];
        gjHistoric1 = weights[gjWeightBaseIndex + weightPageSize]; // 2D weight histories are stacked, so, hopefully this works...
        gjHistoric2 = weights[gjWeightBaseIndex + 2*weightPageSize];
        // kernel arg 3 (activations) -- 2D Blades
        // we already have the base indices, and as each 'timestep's activations are a 1D vector,
        // to go from the current activation ("index 0", to the 1st, 2nd, 3rd, etc. prior activations, just add the number of columns that many times.
        // NOTE: index 0 is where the newly calculated activation goes, and what was in 0, goes to 1, etc. (at the end),
        // but we use index 0 as the 'current' activation now.
        bladeIndexRelativeToKernelArg = 0;
        preActivation = activations[preActivationBaseIndex];
        postActivation = activations[postActivationBaseIndex];
        
        // print info to make sure we're reading everything properly / data is where we think it is
        if (gId == 1) printf("(gId: %d) pre: %d, post: %d - csWeight: %.2f, gjWeight: %.2f => relation {type: %.1f, polarity: %.1f, age: %.2f, tresh: %.2f, decay: %.2f, mutability: %.2f}\n", gId, preElement, postElement, csWeight, gjWeight, relationType, relationPolarity, relationAge, relationThresh, relationDecay, relationMutability);
    
        if (fabs(postActivation) < relationThresh){ // nothing happens if it's less than thresh.
            continue;
        }
        //////////////////////////////////// TEMP ////////////////////////////
        //////////////////////////////////////////////////////////////////////
        if (fCompare(csWeight,0)) continue;
        
        // use log(weight + 1)/log(max weight) -- this gives a logarithmic gain, a weight of 1 is .176, and a max weight is 1.
        // input equation here to see: https://www.desmos.com/calculator
        //float cs_weight = log(chemWeights[weight_idx] + 1)/log(max_cs_weight);// normalize to max CS weight (this was based off of a cursory glance)
        
        float addedActivation = 0;
        float conductance = 0;
        if(csWeight != 0.0){
            ///// NOTE: CHECK ALL OF THIS ////////////
            //////////////////////////////////////////
            // - starting conductance is .5 -- similar to wicks' work, just simplified.
            // -> according to sigmoidal function it increases or decreases with neuron's potential
            //    -> as it nears max, conductance tends to 0, and as it nears min, conductance tends to 1
            
            // if their_v_curr == 0, conductance will be .5.... also, the -5 comes from wicks' paper... almost.
            //no conductance at the moment...
            //added_v = cs_weight * conductance * their_v_curr;
            //added_v = cs_weight * their_v_curr;
            if(relationPolarity < 0){ // if inhibit
                float preToEq = fabs(preActivation - inhibitRevPot);
                float actDiff = inhibitRevPot - postActivation;
                conductance = (2.0f - (2.0f/(1.0f + exp(-5.0f*(preToEq/inhibitExciteRange))))) - .0134;
                addedActivation = csWeight * conductance * actDiff;
            }
            else { // excite
                float preToEq = fabs(preActivation - exciteRevPot);
                float actDiff = exciteRevPot - postActivation;
                conductance = (2.0f - (2.0f/(1.0f + exp(-5.0f*(preToEq/inhibitExciteRange))))) - .0134;
                addedActivation = csWeight * conductance * actDiff;
            }
            // PROBE
            //if (shouldProbe == 1){
            //    chemContrib[weight_idx] = added_v;
            //}
            csIncoming += addedActivation;
        }
        //float gj_weight = log(gapWeights[weight_idx] + 1)/log(max_gj_weight);// normalize to max GJ weight (again, cursory glance)
        addedActivation = 0; // reset this, because we use the same variable for cs and gj incoming...
        //float gj_weight = gapWeights[weight_idx]/max_gj_weight;// normalize to max GJ weight (again, cursory glance)
        if(gjWeight != 0.0){ // response is always the same as the source -- depolariziation of one, causes same in other, and vice versa.
            addedActivation = gjWeight * (preActivation - postActivation);
            // PROBE
            //if (shouldProbe == 1){
            //    gapContrib[weight_idx] = added_v;
            //}
            gjIncoming += addedActivation;
        }
        
        
        
        // NOTE: UPDATE WEIGHTS HERE
        // but, use weights[<type>WeightBaseIndex + (LAST_WEIGHT_HISTORY_INDEX * weightPageSize)]
    }
    
    totalIncomingActivation = gjIncoming + csIncoming;
    // is this the right way to do this, to compute based upon current activation, and then at the end decay it?
    // maybe we should decay it earlier on, prior to computing...
    float activationDecay = activationDecayConstant * postActivation;
    float updatedActivation = (postActivation - activationDecay) + totalIncomingActivation;
    
    // keep our activation within range... (not necessarily biologially unrealistic)
    if(updatedActivation < minActivation) updatedActivation = minActivation;
    if(updatedActivation > maxActivation) updatedActivation = maxActivation;
   
    
   
   
    
    // now that we're finished, set the updated activation
    // by putting the new value at the end, we can use 0-ACTIVATION_HISTORY_SIZE-2 for computations
    activations[UPDATED_ACTIVATION_SLOT] = updatedActivation;
    
    
    /*
   int startingScratchPadOffset = getLocalScratchPadStartingOffset(lid, numElements,numXCorrEntries);
    computeXCorr(outputVoltageHistory, numElements, voltageHistorySize, gid, lid, XCorrScratchPad, startingScratchPadOffset, numXCorrEntries);
        computeVoltageRateOfChange(outputVoltageHistory, numElements, voltageHistorySize, gid, lid, voltageRateOfChangeScratchPad, startingScratchPadOffset, numXCorrEntries); 
     */
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

     
 /*
 *
 */
void computeXCorr(global float* activations, int NUM_ELEMENTS, int ACTIVATION_HISTORY_SIZE, int gId, int lId, local float* XCorrScratchpad, int xcorrScratchpadBaseIndex, int NUM_XCORR_COMPUTATIONS, int XCORR_SIZE){
    //printf("gid, lid: %d, %d\n", gId, lid);
    // We do a xcorr between 'Post', and 'Pre'.
    // 'post' refers to the voltage history for 'gid', and 'pre' starts at '0', and goes to NUM_ELEMENTS-1.
    // at some point, 'post' will be the same as 'pre'. That's fine, we will know that xcorr,
    // because when it is in the scratch pad, it will have index equal to 'gid'
    // in the scratch pad, 'post' with element 0 will be at 0, 'post' with element 1 will be at 1, and so on.
    //
    // XCorrScratchPad will have a row for each set of xcorr computations between 'post' and a given 'pre'
    // (of course, offset by the 'startingScratchPadOffset' to account for the specific thread's 'lid' (local id)),
    // This means that:
    //      - index 0 of that row will hold xcorr(post, pre), stacked.
    //      - index 1 of that row will hold xcorr(post, pre), with 'post' advanced by 1
    //      - index 2 of that row will hold xcorr(post, pre), with 'post' advanced by 2
    //
    // 'post' will always have an offset of 0, and we will always use indices 0, 1, 2, etc.. to XCORR_SIZE-1
    // 'pre' will 'slide', and will start off with an offset of 0, so that it is stacked directly on-top of 'post'.
    // then, on the next 'xcorrIteration', 'pre' will be offset by 1, so we use, 1, 2, etc.. to XCORR_SIZE-1
    //
    // the effect is: the first computation gives correlation as of XCORR_SIZE timesteps before the current timestep,
    // the second gives the correlation of post w.r.t. pre, looking at post XCORR_SIZE timesteps before, and pre XCORR_SIZE+1 timesteps before,
    // the third is the same as the second, but with pre XCORR_SIZE+2 timesteps before, and so on.
    //
    // => this means that we see correlation 4, 5, 6, etc. timesteps before, *and* potential causation 5, 6, etc. timesteps away
    // e.g: 8. ** if pre was just going down, and post starts going up, and pre goes up:
    //      1. Post could be a pre to pre, but pre is probably not a pre to post.
    //      2. If causal: pre and post should be correlated at 0, and then at 1 and 2 back (pushing pre back), there should still be correlation. This shows pre started, and then post started.
    //
    //
    int i, j;
    int postOffset = 0;
    int postIndex = getIndex(gId, postOffset, 0, 0, NUM_ELEMENTS, ACTIVATION_HISTORY_SIZE, 0, 0);//last two irrelevant at the moment
    int preIndex = 0;
    int windowNum = 0;
    int xcorrIteration = 0; // we'll use this to offset 'b' by 0, 1, and 2
    int preOffset = postOffset; // start off with A and B lined up in time, and then decrement bOffset on each loop.
    // seems to make the most sense to use the full length of each signal for the autocorr.
    float autoCorrPost = XCorrMultiply(activations,postIndex, postIndex, XCORR_SIZE);
    float autoCorrPre = 0;
    float divisor = 0; // this will be the sqrt(autocorrA + autoCorrB), which will divide (so, normalize) each xcorr
    float xcorrResult = 0; 
    int scratchpadIndex = 0;
    //int desired_lId = 1; // for testing to make sure indices were being set properly
    for (i = 0; i < NUM_ELEMENTS; ++i){ // loop through activations
        xcorrIteration = 0;
        // each col is one iteration back, and each row is an element's activation...
        preIndex = getIndex(i, preOffset, 0, 0, NUM_ELEMENTS, ACTIVATION_HISTORY_SIZE, 0, 0);
        // this line was for testing to make sure indices were being set properly.
        //if (lId == desired_lId ) printf("(gId: %d, lId: %d) postIndex: %d, preIndex: %d\n", gId, lId, postIndex, preIndex);
        for (xcorrIteration = 0; xcorrIteration < XCORR_SIZE; ++xcorrIteration){
            scratchpadIndex = xcorrScratchpadBaseIndex + (i * NUM_XCORR_COMPUTATIONS) + xcorrIteration;
            // these 3 lines were for testing to make sure indices were being set properly.
            //if (lId == desired_lId ) printf("\t(gId: %d, lId: %d) postIndex: %d, preIndex: %d\n", gId, lId, postIndex, preIndex);
            //if (lId == desired_lId  ) printf("\t\t(gId: %d, lId: %d) scratchpd index: %d\n", gId, lId, scratchpadIndex);
            //continue;
            autoCorrPre = XCorrMultiply(activations, preIndex, preIndex, XCORR_SIZE);
            divisor = sqrt(autoCorrPost * autoCorrPre);
            if (divisor == 0){ // then correlation between two signals is zero
                XCorrScratchpad[scratchpadIndex] = 0.f;
            }
            else{
                xcorrResult = XCorrMultiply(activations, postIndex, preIndex, XCORR_SIZE);
                XCorrScratchpad[scratchpadIndex] = xcorrResult/divisor;
            }
            preIndex++;
        }
        
    }
    
}

float XCorrMultiply(global float* activations, int postOffset, int preOffset, int XCORR_SIZE){
    float result = 0;
    for (int i = 0; i < XCORR_SIZE; ++i){
        result += activations[postOffset+i] * activations[preOffset+i];
    }
    return result;
}



/** NOTE: can make this much faster by computing 1 set of slopes for each local group, giving them each a block to take care of or something...
 ... as it is now, the same thing gets computed by every instance... some things get recomputed, too, i believe. */
void computeSlope(global float* activations, int NUM_ELEMENTS, int ACTIVATION_HISTORY_SIZE, int gId, int lId, local float* SlopeScratchpad, int slopeScratchpadBaseIndex, int NUM_SLOPE_COMPUTATIONS, int SLOPE_SIZE){
    int i, j;
    float timeSteps = SLOPE_SIZE;
    int elementBaseIndex = -1;
    int elementLastIndex = -1;
    float rateOfChange = -1.f;
    int scratchpadIndex = -1;
    for (i = 0; i < NUM_ELEMENTS; ++i){
        for (j = 0; j < NUM_SLOPE_COMPUTATIONS; ++j){
            // use j for col because on each iteration, it will move forward by 1
            elementBaseIndex = getIndex(i, j, 0, 0, NUM_ELEMENTS, ACTIVATION_HISTORY_SIZE, 0, 0); // like XCorr computation, last two don't matter
            elementLastIndex = elementBaseIndex + timeSteps;
            rateOfChange = (activations[elementLastIndex] - activations[elementBaseIndex])/timeSteps;
            scratchpadIndex = slopeScratchpadBaseIndex + (i * NUM_SLOPE_COMPUTATIONS) + j;
            SlopeScratchpad[scratchpadIndex] = rateOfChange;
        }
    }
}       
        
        
        
        



/*
       
        if (kernelIterationNum >= ovhSizeMinusOne){ // then we know we have a full outputVoltageHistory
            // do XCorr
            // do gap nd chem work
            //if (gid == 0){
            //   printf("ovhs: %d, v_curr_finished: %.2f\n", voltageHistorySize, v_curr_finished);
            //printf("%.2f, %.2f, %.2f, %.2f, %.2f, %.2f\n",outputVoltageHistory[0],outputVoltageHistory[1],outputVoltageHistory[2],outputVoltageHistory[3],outputVoltageHistory[4],outputVoltageHistory[5]);
            //}
            int startingScratchPadOffset = getLocalScratchPadStartingOffset(lid, numElements,numXCorrEntries);
            //computeXCorr(outputVoltageHistory, numElements, voltageHistorySize, gid, lid, XCorrScratchPad, startingScratchPadOffset, numXCorrEntries);
            //computeVoltageRateOfChange(outputVoltageHistory, numElements, voltageHistorySize, gid, lid, voltageRateOfChangeScratchPad, startingScratchPadOffset, numXCorrEntries);
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
