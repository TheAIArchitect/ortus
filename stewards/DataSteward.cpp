//
//  DataSteward.cpp
// ortus
//
// Andrew W.E. McDonald
// Sean Grimes
// Copyright © 2017
//


/* we want everything stored in opencl-compatible datatypes. data will go directly from here, to opencl buffers.
 
  ** At the moment, we are going to ignore the issue of physical proximity with respect to how it impacts computation (e.g., greater time delay for neuron C because it's farther from A than B is, despite both being directly connected to A), or signal decay.
    *** This is because it will add complexity to the computation, as well as the system that enables neurons to be added.
 
  * we will have extra space at the end of each row, in 2D arrays (from now on, 2D means a 1D array being used as a 2D array), meaning that under normal operation, there will effectively be 'blanks' at the end of each 'row' in the 1D array representation. 
    ** This is where we'll put the weights for the new neurons. This is how we will grow the network without slowing things down significantly. Upon writing the new connectome, we'll first re-organize.
 
 * This class will hold all of the real data. 
 ******** UNLESS WE ARE NO LONGER USING THE DATA (e.g. it's historic voltage data per neuron),
  EVERYTHING ELSE IN ORTUS WILL HOLD A --REFERENCE-- TO THE DATA HELD HERE. THAT INCLUDES WEIGHTS, POLARITY, NEUROTRANSMITTERS, NEURONS, ETC..
    --> the reason for this, is so that we can pass data to and from opencl without constantly having to re-intitalize the 1D representations of data. It keeps things simple.
 
 
 
   -> fix bad pointer usage: http://stackoverflow.com/questions/22146094/why-should-i-use-a-pointer-rather-than-the-object-itself
 
 */



#include "DataSteward.hpp"


int DataSteward::CSV_ROWS;
int DataSteward::CSV_COLS;
int DataSteward::CONNECTOME_ROWS;
int DataSteward::CONNECTOME_COLS;

unsigned int DataSteward::NUM_ELEMS = 0;
unsigned int DataSteward::NUM_ROWS = 0;
unsigned int DataSteward::NUM_NEURONS_CLOSEST_LARGER_MULTIPLE_OF_8 = 0;


DataSteward::DataSteward(){}

DataSteward::~DataSteward(){
    for (int i = 0; i < NUM_ELEMS; ++i){
        delete officialNamepVector[i];
        delete elements[i];
    }
    delete inputVoltages;
    delete outputVoltages;
    delete gaps;
    delete chems;
    delete chemContrib;
    delete gapContrib;
    delete rowCount;
    delete colCount;
    delete gapNormalizer;
    delete chemNormalizer;
    
    delete probe;
    
}

void DataSteward::init(){
    initializeData();
}

void DataSteward::setKernelp(cl_kernel* kp){
    kernelp = kp;
}

void DataSteward::setCLHelperp(CLHelper* clhp){
    clHelperp = clhp;
}

void DataSteward::executePostRunMemoryTransfers(){
    readOpenCLBuffers();
    updateOutputVoltageVector();
    // copy the ouput data into the input data
    // NOTE: FIX THIS SO THAT WE USE ONE BUFFER!!!
    inputVoltages->copyData(outputVoltages);
}


void DataSteward::readOpenCLBuffers(){
    // get output voltages
    outputVoltages->readCLBuffer();
    
}

void DataSteward::growConnectome(){
    // this will take care of adding the required entries to the blades,
    // and adding the required weights
}

/* we need another blade:
 

 
 =========> 'active state', cut down on this.
 -----------> 'sleeping state', or 'reflective state', maybe go back and do something?
 
 
 
 
 so, just as the connectome is a square matrix, and each entry is a weight,
 in this blade, each entry will be a crossCorrelation
 
 maybe -- pass in two, three, etc.. historic voltage arrays, and compute the xcorr in the kernel,
 and then alter the weight based upon that:
    -> if postiviely correlated at 0 offset (signals are on top of eachother):
        -> if positive change in correlation (so, more positive):
            -> increase strength of excititory synapse
        -> if negative change in correlation (so, less positive):
            -> decrease strength of excitatory synapse
    -> if inversely correlated at 0 offset (signals are on top of eachother)::
        -> if positive change in correlation (so, more negative):
            -> increase strength of inhibitory synapse
        -> if negative change in correlation (so, less negative):
            -> decrease strength of inhibitory synapse
    -> if postiviely correlated at offset > 0, where presynaptic neuron's signal is offset:
        -> if positive change in correlation (so, more positive):
            -> increase strength of excititory synapse
        -> if negative change in correlation (so, less positive):
            -> decrease strength of excitatory synapse
    -> if inversely correlated at offset > 0, where presynaptic neuron's signal is offset:
        -> if positive change in correlation (so, more negative):
            -> increase strength of inhibitory synapse
        -> if negative change in correlation (so, less negative):
            -> decrease strength of inhibitory synapse
 
 
    => note: 'change' refers to xcorr value from previous time-step (iteration)
 
    So, how do gap junctions factor into this? 
        -> 
            * if neuron A, at time n (A[n]) has an activation, x, and
            * if neuron B, at time n (B[n]) has an activation, y, and
            * if A[n+1] has an activation x+dX, then:
                * if B[n+1] has an activation y+dY
                => we have a correlation
                * if B[n+1] has an activation,  y, and if B[n+2] has an activation y+dY
                => we have a causal relationship
    
    
            
    
 */

std::vector<float> DataSteward::getOutputVoltageVector(){
    return outputVoltageVector;
}

/* updates outputVoltageVector with a *COPY* of the voltage data, NOT a reference to the actual data, nor its location. */
void DataSteward::updateOutputVoltageVector(){
    outputVoltageVector.clear();
    for (int i = 0; i < outputVoltages->currentSize; ++i){
        outputVoltageVector.push_back(outputVoltages->data[i]);
    }
    // push current voltage vector into vector of all output voltages
    kernelVoltages.push_back(outputVoltageVector);
}

void DataSteward::setOpenCLKernelArgs(){
    inputVoltages->setCLArgIndex(0, kernelp);
    outputVoltages->setCLArgIndex(1, kernelp);
    gaps->setCLArgIndex(2, kernelp);
    chems->setCLArgIndex(3, kernelp);
    gapContrib->setCLArgIndex(4, kernelp);
    chemContrib->setCLArgIndex(5, kernelp);
    rowCount->setCLArgIndex(6, kernelp);
    colCount->setCLArgIndex(7, kernelp);
    clHelperp->err = clSetKernelArg(*kernelp, 8, sizeof(cl_uint), &Probe::shouldProbe);
    clHelperp->check_and_print_cl_err(clHelperp->err);
    gapNormalizer->setCLArgIndex(9, kernelp);
    chemNormalizer->setCLArgIndex(10, kernelp);
}

void DataSteward::pushOpenCLBuffers(){
    //Timer et;
    //et.start_timer();
    inputVoltages->pushCLBuffer();
    outputVoltages->pushCLBuffer();
    gaps->pushCLBuffer();
    chems->pushCLBuffer();
    chemContrib->pushCLBuffer();
    gapContrib->pushCLBuffer();
    //et.stop_timer();
}


/* initializes the Blades that aren't used for the gj or cs weights */
void DataSteward::initializeBlades(){
    // need a gj and cs contrib (one of each) -- these are NxN
    chemContrib = new Blade<float>(clHelperp, CL_MEM_READ_WRITE, CONNECTOME_ROWS, CONNECTOME_COLS, MAX_ELEMENTS, MAX_ELEMENTS);
    gapContrib = new Blade<float>(clHelperp, CL_MEM_READ_WRITE, CONNECTOME_ROWS, CONNECTOME_COLS, MAX_ELEMENTS, MAX_ELEMENTS);
    // now we initialize the voltage vectors -- one row.
    // ideally, we'd have one that gets read from and written to -- should get around to fixing that shortly.
    inputVoltages = new Blade<float>(clHelperp, CL_MEM_READ_WRITE, CONNECTOME_COLS, MAX_ELEMENTS);
    outputVoltages = new Blade<float>(clHelperp, CL_MEM_READ_WRITE, CONNECTOME_COLS, MAX_ELEMENTS);
    //fillInputVoltageBlade(); // will probably be used at some point
    rowCount = new Blade<cl_uint>(clHelperp, CL_MEM_READ_ONLY);
    rowCount->set(NUM_ELEMS); // cl_uint, for current row count
    colCount = new Blade<cl_uint>(clHelperp, CL_MEM_READ_ONLY);
    colCount->set(NUM_ELEMS); // cl_uint, for current col count
    gapNormalizer = new Blade<cl_float>(clHelperp, CL_MEM_READ_ONLY);
    gapNormalizer->set(1.f); // NOTE: THIS DOESN'T WORK YET... OR GET SET TO ANYTHING OTHER THAN 1!!!!
    chemNormalizer = new Blade<cl_float>(clHelperp, CL_MEM_READ_ONLY);
    chemNormalizer->set(1.f); // NOTE: THIS DOESN'T WORK YET... OR GET SET TO ANYTHING OTHER THAN 1!!!!
    
}

void DataSteward::fillInputVoltageBlade(){
   // this doesn't do anything at the moment, because v_0 is 0...
}



void DataSteward::createConnections(){
    printf("connectome_rows, connectome_cols: %d. %d\n",CONNECTOME_ROWS, CONNECTOME_COLS);
    gaps = new Blade<float>(clHelperp, CL_MEM_READ_ONLY, CONNECTOME_ROWS, CONNECTOME_COLS, MAX_ELEMENTS, MAX_ELEMENTS);
    chems = new Blade<float>(clHelperp, CL_MEM_READ_ONLY, CONNECTOME_ROWS, CONNECTOME_COLS, MAX_ELEMENTS, MAX_ELEMENTS);
    int start_at = CSV_OFFSET; // it's a square matrix, with the same information on each side of the diagonal (but flipped)... could do one 'side' and add same connection pointer to each pre, but let's hold off... would need to switch the out_conns vector to hold Connection object pointers.
    for (int i = CSV_OFFSET; i < CSV_ROWS; i++){
        
        int pre_id = i - CSV_OFFSET;// there's no offset in our Element vector
        int post_id = -1;
        
        for (int j = CSV_OFFSET; j < CSV_COLS; j++){
            //printf("(%d, %d) >>%s<<\n",i,j,csvDat[i][j].c_str());
            
            if (csvDat[i][j] == ""){
                continue;// no connection between i and j
            }
            post_id = j - CSV_OFFSET;
            
            // we don't want to create a connection if either the pre or post neuron/muscle has been 'ablated'... we don't want a reference to an object we'll delete later, because that can cause issues
            if (elements[pre_id]->ablated || elements[post_id]->ablated){
                continue;
            }
            // index 0 will be the gap, index 1 will be the chem
            // either one, as well as both, may be empty, or not empty.
            // only 1 may have a negative value. If negative, it's an inhibitory connection.
            // row -> col => pre -> post
            // and, gaps should be symmetric (if i->j, then also j->i)
            std::vector<std::string> strVecWeights = StrUtils::parseOnCharDelim(csvDat[i][j], '/');
            int numEntries = strVecWeights.size();
            float gapWeight = 0; // we'll know if we have a gap or chem by the weight not being 0
            float chemWeight = 0;
            // we know if it's empty, we won't hit it (see above)
            // in addition, as long as if a gap or chem (or both) is/are added, a 0 is added
            // for the connection that doesn't exist, this will be fine, e.g.:
            //      -> if a gap is added: '1/0' would be fine, but '1', or '1/' would not.
            //      -> if a chem is added: '0/-1' would be fine, but '/-1', or '-1' would not.
            // and, obvously, if there is a gap and a chem, a weight is given for both.
            if (numEntries == 2){
                gapWeight = stof(strVecWeights[0],NULL);
                chemWeight = stof(strVecWeights[1],NULL);
            }
            else {
                printf("Oh no. This isn't good... connectome data seems corrupted.\n");
                exit(3);
            }
            
            if (gapWeight > 0){ // can only be positive
                gaps->set(post_id, pre_id, gapWeight); // swap post and pre so that we have row as post and col as pre
                Connection c;
                c.pre = elements[pre_id];
                c.preName = elements[pre_id]->name();
                c.post = elements[post_id];
                c.postName = elements[post_id]->name();
                c.type = GAP;
                c.weightp = gaps->getp(post_id, pre_id);
                printf("%s\n",c.toString().c_str());
                elements[pre_id]->out_conns.push_back(c);
            }
            if (chemWeight != 0){ // could be negative or positive
                chems->set(post_id, pre_id, chemWeight);
                Connection c;
                c.pre = elements[pre_id];
                c.preName = elements[pre_id]->name();
                c.post = elements[post_id];
                c.postName = elements[post_id]->name();
                c.type = CHEM;
                c.weightp = chems->getp(post_id, pre_id);
                printf("%s\n",c.toString().c_str());
                elements[pre_id]->out_conns.push_back(c);
            }
        }
        //printf("i = %d\n",i);
        ++start_at;
    }
}









void DataSteward::createElements(){
    csvDat = std::vector<std::vector<std::string>>();
    FileAssistant::readConnectomeCSV(FileAssistant::ortus_basic_connectome,csvDat);
    int count = csvDat.size(); // total row count
    int len = csvDat[0].size(); // total col count
    /*
     * First we want to go through and create structs for all the neurons and muscles
     */
    // absolute_index_mapping takes the original index in the csv file for the neuron or muscle,
    // and gives the index in the neuron or muscle struct vector. if negative, it's a muscle index.
    std::vector<int> absolute_index_mapping;
    
    int csv_rows =(int) csvDat.size()-1;// last row is the same as row 1 (not row 0... row 1)
    // all columns should be the same length if we've gotten here. NOTE: last column is same as col 1 (not col 0..)
    int csv_cols = len - 1;
    if (csv_rows != csv_cols){
        printf("ERROR! Connectome row count != col count.\n");
        exit(8);
    }
    CSV_ROWS = csv_rows;
    CSV_COLS = csv_cols;
    CONNECTOME_ROWS = csv_rows - CSV_OFFSET;
    CONNECTOME_COLS = csv_cols - CSV_OFFSET;
    
    std::string curr_type = "";
    ElementType etype;
    std::string graphicalIdentifier;
    // NOTE: All NIM and MIM objects are EIMs, so the element_id increases each loop.
    // neuron_id increases if it's a neuron, muscle_id increases if it's a muscle
    int element_id = 0;
    int neuron_id = 0;
    int muscle_id = 0;
    for (int i = CSV_OFFSET; i < csv_rows; i++){
        if (!(csvDat[i][0] == "")){ // else, we keep the same etype
            etype = FileAssistant::string_to_etype(csvDat[i][0], graphicalIdentifier);
        }
        std::string* temp_namep = new std::string(csvDat[i][1]);
        FileAssistant::remove_leading_zero_from_anywhere(temp_namep);
        officialNamepVector.push_back(temp_namep);
        officialNameToIndexMap[*temp_namep] = element_id;
        if (etype != MUSCLE){
            NeuronInfoModule* nim = new NeuronInfoModule();
            nim->namep = officialNamepVector[element_id];
            nim->graphicalName = *temp_namep+graphicalIdentifier;
            nim->idp = &officialNameToIndexMap[*temp_namep];
            nim->setElementType(etype);
            ablator.setAblationStatus(nim);
            if (!nim->ablated){ // only keep it if it isn't ablated
                nim->neuron_id = neuron_id;
                neuron_id++;
            }
            elements.push_back(nim);// must add ablated neurons to elements array so that we can find them when we do chems
        }
        else {
            MuscleInfoModule* mim = new MuscleInfoModule();
            mim->namep = officialNamepVector[element_id];
            mim->graphicalName = *temp_namep+graphicalIdentifier;
            mim->idp = &officialNameToIndexMap[*temp_namep];
            mim->setElementType(etype);
            mim->centerMassPoint = NULL;
            ablator.setAblationStatus(mim);
            if (!mim->ablated){ // only keep it if it isn't ablated
                mim->muscle_id = muscle_id;
                muscle_id++;
            }
            elements.push_back(mim); // must add ablated muscles to elements array so that we can find them when we do chems
        }
        element_id++;
    }
    for(int i = 0; i < officialNamepVector. size(); i++){
        printf("%s -> ",(*officialNamepVector[i]).c_str());
        printf("%d\n",officialNameToIndexMap[*officialNamepVector[i]]);
    }
}



void DataSteward::initializeData(){
    createElements();
    createConnections();
    NUM_ELEMS = elements.size();
    NUM_ROWS = NUM_ELEMS;
    int modEight = NUM_ELEMS % 8;
    NUM_NEURONS_CLOSEST_LARGER_MULTIPLE_OF_8 = DataSteward::NUM_ELEMS + (8 - modEight);
    initializeBlades();
    probe = new Probe(); // NOTE: probe not working right now. needs to be re-worked.
}

/* writes the connectome. as of this comment, it will write it by **DE-TRANSPOSING** it.
 * NOTE: currently, this assumes neurons are ordered. */
bool DataSteward::writeConnectome(std::string csv_name){
    
    // the first row is complicated, it starts with "gap/chem" (as a 'key' of sorts),
    // then has an empty column, and then 'SENSORY', because we start with sensory neurons.
    std::string firstRow = "gap/chem,,";
    // before we continue, we need to create a map that holds the indices that the SENSORY, INTER, MOTOR, etc..
    // elements start at, so that we know what columns to put the SENSORY/INTER/MOTOR headings in on the firstRow
    std::unordered_map<std::string,int> columnHeadingMap;
    // start at 2, (see 'firstRow' to see why)
    int colOffset = 2;
    int currentColIndex = colOffset;
    int numElements = NUM_ELEMS;
    std::string headings[4] = {"SENSORY", "INTER", "MOTOR","BUFFER"}; // "BUFFER" allows us to keep accessing the array after we've used all headings (after 'MOTOR', we have to add all motor neurons, but headingNum has been incremented, and we still check the array)
    int headingNum = 0;
    //columnHeadingMap[headings[headingNum]] = currentColIndex;
    //headingNum++;
    // now we run through the list of element names (elements) until the first letter stops being "S",
    // and becomes "I", (and then "M", for motor). At the time of this, motors and muscles are used as the same thing.
    std::string nextKey = headings[headingNum];
    headingNum++;
    char firstLetter = nextKey[0];
    // we can also build the second row, which is the offset, and then all the element names
    std::string secondRow = ",,";
    for (int i = 0; i < numElements; ++i){
        // if the element's name starts with firstLetter and nextKey isn't in the map
        if ((*officialNamepVector[i])[0] == firstLetter && !columnHeadingMap.count(nextKey)){
            columnHeadingMap[nextKey] = i; // don't add the colOffset to this, because we'll use it to check against the elements' actual indices later on
            firstRow += nextKey + ",";
            nextKey = headings[headingNum];
            headingNum++;
            firstLetter = nextKey[0];
        }
        else {
            firstRow += ",";
        }
        secondRow += (*officialNamepVector[i]) + ",";
        
    }
    firstRow += "placeholder\n";
    secondRow += "placeholder\n";
    // AND, we can do the last row, which is the same as the secondRow
    std::string lastRow = secondRow;
    // now we have something that looks somewhat like:
    // (for firstRow): "gap/chem,,SENSORY,,,,,,INTER,,,,,,MOTOR,,,,,,placeholder"
    // (for secondRow): ",,e1,e2,e3,...,eN,placeholder"
    
    // next, we create the 'main' rows, that are the actual connectome.
    // formula: <space>,<name>,<either connection or blank for all elements>,<name>,<newline>
    // NOTE: this assumes the connectome has NOT been transposed!!!
    std::string actualConnectome = "";
    // also, we need to set headingNum to zero, because we need to check if a heading is supposed to be there
    headingNum = 0;
    for (int i = 0; i < NUM_ELEMS; ++i){
        // check to see if we're at a point that we need to add a 'type' heading
        if (columnHeadingMap[headings[headingNum]] == i){
            actualConnectome += headings[headingNum] + ",";
            headingNum++;
        }
        else {
            actualConnectome += ",";
        }
        actualConnectome += (*officialNamepVector[i]) + ",";
        for (int j = 0; j < numElements; ++j){
            /* NOTE NOTE NOTE: THIS IS ONE SPOT WHERE THE 'i' and 'j' ARE SWITCHED, TO DE-TRANSPOSE */
            if (gaps->getv(j,i) == 0 && chems->getv(j,i) == 0){
                actualConnectome += ","; // there's no connection
            }
            else {
                /* NOTE NOTE NOTE: THIS IS THE OTHER SPOT WHERE THE 'i' and 'j' ARE SWITCHED, TO DE-TRANSPOSE */
                actualConnectome += std::to_string((int)gaps->getv(j,i)) + "/" + std::to_string((int)chems->getv(j,i)) + ",";
            }
        }
        actualConnectome += (*officialNamepVector[i]) + "\n";
    }
    std::ofstream newConnectome = FileAssistant::wopen(FileAssistant::ortus_basic_connectome_test);
    newConnectome.write(firstRow.c_str(), firstRow.size());
    newConnectome.write(secondRow.c_str(), secondRow.size());
    newConnectome.write(actualConnectome.c_str(), actualConnectome.size());
    newConnectome.write(secondRow.c_str(), secondRow.size());
    newConnectome.close();
    return true;
}


void DataSteward::feedProbe(){
    /*
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
     
     */
}

/*
void get_conns(std::vector<ElementInfoModule*>& elements){
    std::vector<ElementInfoModule*> tempElements; // after we get the gaps and chems, we need to fill the real elements vector with only the elements we want (as of now, all neurons and body wall muscles)
    //createElements(tempElements);
    //ortus//parse_chems(tempElements);
    // now we need to go through the tempElements and only add the non-ablated ones to the 'real' elements vector.
    int numTemps = tempElements.size();
    int realIndex = 0; // this is the index for the 'real' elements (the ones that we care about, that aren't ablated)
    int round;
    int maxRounds = 2;
    for (round = 0; round < maxRounds; ++round){ // we want to go through the elements array once for neurons, then muscles, so that in the kernel we can apply a different rule if it's a muscle (e.g., if row > 302, do something specific for muscles).
        for( int i = 0; i < numTemps; ++i){
            if (!tempElements[i]->ablated){
                // change the element_id such that the elements we actually use are indexed consecutively. this allows us to use the element_id as the index when working with arrays/vectors (open cl)
                if (round == 0 && tempElements[i]->getEType() != MUSCLE){ // neuron
                    /////////////// NEED OFFICIAL IDS                    tempElements[i]->idp = realIndex;
                    elements.push_back(tempElements[i]);
                    realIndex++;
                }
                else if (round == 1 && tempElements[i]->getEType() == MUSCLE){ // neuron
                    /////////////// NEED OFFICIAL IDS                    tempElements[i]->idp = realIndex;
                    elements.push_back(tempElements[i]);
                    realIndex++;
                }
            }
            else if (tempElements[i]->ablated && round == 1){ // it's safe to delete once we know we won't access the element again (i.e., we're on our second time through)
                delete tempElements[i];
                tempElements[i] = NULL;
            }
        }
    }
    // now we must check to ensure that all neurons are in order of id, as are all muscles
    // this is both a sanity check and a check to ensure that the order of neurons in the elements array is the same as the order in the nim array, and same for mim array and elements array (though the muscles will be offset by the number of neurons in the elements arrray)
    bool muscleFound = false;
    int numNeurons = 0;
    int numMuscles = 0;
    for (int i = 0; i < realIndex; ++i){
        if (elements[i]->getEType() != MUSCLE && muscleFound){ // if we've seen at least one muscle, and then a neuron, that's not good.
            printf("The order of the elements is not such that all neurons come before all muscles.\n");
            exit(5);
        }
        if (elements[i]->getEType() == MUSCLE && !muscleFound){ // if we see a muscle, acknowledge this.
            muscleFound = true;
        }
        if (!muscleFound){// check element_id against neuron_id
            if (((NeuronInfoModule*)elements[i])->neuron_id != elements[i]->id()){
                printf("We have a problem. neuron_id != element_id, during sanity check.\n");
                exit(6);
            }
            numNeurons++;
        }
        if (muscleFound){ // check element_id against numNeurons+muscle_id (due to offset)
            if ((numNeurons + ((MuscleInfoModule*)elements[i])->muscle_id) != elements[i]->id()){
                printf("We have a problem. (numNeurons + muscle_id) != element_id, during sanity check.\n");
                exit(7);
            }
            numMuscles++;
        }
        //printf("EIM: %s, %d\n",elements[i]->name.c_str(),elements[i]->element_type);
    }
    printf("number of neurons in model: %d, number of muscles in model: %d\n",numNeurons, numMuscles);
}
*/











/* // this is code to check ablation status and get the maximium weights for gaps and chems
    int numElems = elements.size();
    for (int i = 0; i < numElems; ++i){
        int numConns = elements[i]->out_conns.size();
        std::vector<Connection> tempConns = elements[i]->out_conns;
        for (int j = 0; j < numConns; ++j){
            if ((tempConns[j].pre->ablated == true) || (tempConns[j].post->ablated == true)){
                // then something is very wrong, because in the CSV importer that created the connections, conns with one or more ablations were supposed to be removed.
                printf("(%d, %d) %s (ablated - %d) <=> %s (ablated - %d) -- no ablated cells should have connections...\n",i,j, tempConns[j].pre->name().c_str(), tempConns[j].pre->ablated, tempConns[j].post->name().c_str(), tempConns[j].post->ablated);
                exit(81);
            }
            int pre_id = tempConns[j].pre->id();
            int post_id = tempConns[j].post->id();
            if (tempConns[j].type == GAP){
                gj_matrix[pre_id][post_id] = tempConns[j].weight();
                if (fabs(tempConns[j].weight()) > maxGapWeight){
                    maxGapWeight = fabs(tempConns[j].weight());
                }
            }
            else if (tempConns[j].type == CHEM){
                cs_matrix[pre_id][post_id] = tempConns[j].weight();
                if (fabs(tempConns[j].weight()) > maxChemWeight){
                    maxChemWeight = fabs(tempConns[j].weight());
                }
            }
            
        }
        
        
    }
     */ // end code to check ablation status
