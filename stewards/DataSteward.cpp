//
//  DataSteward.cpp
// ortus
//
// Andrew W.E. McDonald
// Sean Grimes
// Copyright © 2017
//


/*
  * we will have extra space at the end of each row, in 2D arrays (from now on, 2D means a 1D array being used as a 2D array), meaning that under normal operation, there will effectively be 'blanks' at the end of each 'row' in the 1D array representation.
    ** This is where we'll put the weights for the new neurons. This is how we will grow the network without slowing things down significantly. Upon writing the new connectome, we'll first re-organize.
 
 * This class will hold all of the real data. 
 ******** UNLESS WE ARE NO LONGER USING THE DATA (e.g. it's historic voltage data per neuron),
  EVERYTHING ELSE IN ORTUS WILL HOLD A --REFERENCE-- TO THE DATA HELD HERE. THAT INCLUDES WEIGHTS, POLARITY, NEUROTRANSMITTERS, NEURONS, ETC..
    --> the reason for this, is so that we can pass data to and from opencl without constantly having to re-intitalize the 1D representations of data. It keeps things simple.
 
   -> fix bad pointer usage: http://stackoverflow.com/questions/22146094/why-should-i-use-a-pointer-rather-than-the-object-itself
 
 */



#include "DataSteward.hpp"

// these two headers need to be included here,
// and these two classes are 'declared' above
// DataSteward's definition in DataSteward.hpp.
//
// Both of the below classes do the same sort of thing with
// DataSteward and its header, so that DataSteward can access
// ElementInfoModules and ElementRelations,
// and they can both access DataSteward.
#include "ElementInfoModule.hpp"
#include "ElementRelation.hpp"

//unsigned int DataSteward::NUM_NEURONS_CLOSEST_LARGER_MULTIPLE_OF_8 = 0;


DataSteward::DataSteward(){
    connectomeNewed = false;
}

DataSteward::~DataSteward(){
    delete voltages;
    delete outputVoltageHistory;
    delete gaps;
    delete chems;
    delete chemContrib;
    delete gapContrib;
    //delete rowCount;
    //delete colCount;
    delete gapNormalizer;
    delete chemNormalizer;
    delete metadata;
    
    delete probe;
    if (connectomeNewed){
        delete connectomep;
        connectomeNewed = false;
    }
    
}

void DataSteward::init(size_t openCLWorkGroupSize){
    this->openCLWorkGroupSize = openCLWorkGroupSize;
    initializeData();
}

void DataSteward::loadConnectome(std::string connectomeFile){
    connectomep = new Connectome(connectomeFile);
    connectomeNewed = true;
}

void DataSteward::initializeData(){
    
    /*
    createElements();
    createConnections();
    NUM_ELEMS = elements.size();
    NUM_ROWS = NUM_ELEMS;
    int modEight = NUM_ELEMS % 8;
    NUM_NEURONS_CLOSEST_LARGER_MULTIPLE_OF_8 = DataSteward::NUM_ELEMS + (8 - modEight);
    initializeBlades();
    probe = new Probe(); // NOTE: probe not working right now. needs to be re-worked.
     */
}

void DataSteward::executePostRunMemoryTransfers(){
    //readOpenCLBuffers();
    updateOutputVoltageVector();
    // copy the ouput data into the input data
    // NOTE: FIX THIS SO THAT WE USE ONE BUFFER!!!
    //inputVoltages->copyData(outputVoltages);
}


//void DataSteward::readOpenCLBuffers(){
    ////for (auto blade : attributeBladeMap){
    ////    blade.second->readDataFromDevice();
    ////}

    // get output voltages
    /*
    voltages->readCLBuffer();
    outputVoltageHistory->readCLBuffer();
    chems->readCLBuffer();
    gaps->readCLBuffer();
     */
//}


std::vector<float> DataSteward::getOutputVoltageVector(){
    return outputVoltageVector;
}

/* updates outputVoltageVector with a *COPY* of the voltage data, NOT a reference to the actual data, nor its location. */
void DataSteward::updateOutputVoltageVector(){
    outputVoltageVector.clear();
    for (int i = 0; i < voltages->currentSize; ++i){
        outputVoltageVector.push_back(voltages->data[i]);
    }
    // push current voltage vector into vector of all output voltages
    kernelVoltages.push_back(outputVoltageVector);
}

//// create new blade for this (all scalars, or one that rarely changes, and one that changes all the time, just like the other blades), and access it via enum or something
/* this must be called for each kernel iteration... */
void DataSteward::updateMetadataBlade(int kernelIterationNum){
    /*
    metadata->set(0, CONNECTOME_ROWS);
    metadata->set(1, CONNECTOME_COLS);
    metadata->set(2, kernelIterationNum);
    metadata->set(3, VOLTAGE_HISTORY_SIZE);
    metadata->set(4, XCORR_COMPUTATIONS);
     */
}

//void DataSteward::setOpenCLKernelArgs(){
    /*
    voltages->setCLArgIndex(0, kernelp);
    outputVoltageHistory->setCLArgIndex(1, kernelp);
    gaps->setCLArgIndex(2, kernelp);
    chems->setCLArgIndex(3, kernelp);
    gapContrib->setCLArgIndex(4, kernelp);
    chemContrib->setCLArgIndex(5, kernelp);
    clHelperp->err = clSetKernelArg(*kernelp, 6, sizeof(cl_int), &Probe::shouldProbe);
    clHelperp->check_and_print_cl_err(clHelperp->err);
    gapNormalizer->setCLArgIndex(7, kernelp);
    chemNormalizer->setCLArgIndex(8, kernelp);
    metadata->setCLArgIndex(9, kernelp);
    deviceScratchPadXCorr->setCLArgIndex(10,kernelp);
    deviceScratchPadVoltageROC->setCLArgIndex(11,kernelp);
     */
    
//}

void DataSteward::executePreRunOperations(){
    pushOpenCLBuffers();
}

void DataSteward::pushOpenCLBuffers(){
    for (auto blade : attributeBladeMap){
        blade.second->pushDataToDevice();
    }


    //Timer et;
    //et.start_timer();
    /*
    voltages->pushCLBuffer();
    outputVoltageHistory->pushCLBuffer();
    gaps->pushCLBuffer();
    chems->pushCLBuffer();
    chemContrib->pushCLBuffer();
    gapContrib->pushCLBuffer();
    metadata->pushCLBuffer();
     */
    //et.stop_timer();
}




/** creates KernelArgs, CLBuffers, and Blades
 **/
void DataSteward::initializeKernelArgsAndBlades(){
    
     /* these are the kernel args we want */
    /* NOTE: 'static' prefix suggests set items should rarely change,
     * while 'dynamic' suggests items in the set may change value every iteration */
    // staticElement = {Attribute::ElementType, Attribute::Affect};//, Attribute::EThresh}
    // dynamicElement = {Attribute::Activation}; // this one needs to have the history
    // staticRelation = {Attribute::RType, Attribute::Polarity, Attribute::Direction, Attribute::RThresh};
    // dynamicRelation = {Attribute::a
    /*
     In addition to the above, we need:
        # metadata (see metadata from old DataSteward)
        # scalars -- separate from metadata scalars
        # offset info
        # scratchpads
     */
    // way to approach offset info:
        // 1) each kernel arg *must* only have data that is seperable by the same offset (so, either scalar, 1D, or 2D, but not a combination)
        // 2) there will be 1 '2d' kernel arg formatted like so (kai <=> kernel arg index):
            // [kai][0] => number of 'keys', [kai][1] => offset size between keys,
            // it will be up to the implementor to ensure that the proper order of attributes/etc. are being accessed.
    

    
    // MOVE THESE!!!
    elementThings = { Attribute::EType, Attribute::Affect, Attribute::Activation };
    relationThings = { Attribute::Weight, Attribute::Polarity, Attribute::RThresh };
    
    
    std::unordered_map<Attribute, Blade<cl_float>*>* tempAttribToFloatBladeMap;
    tempAttribToFloatBladeMap = kernelBuddyp->addKernelArgAndBlades<cl_float,Attribute>(0, elementThings, 1, ortus::NUM_ELEMENTS, ortus::MAX_ELEMENTS, CL_MEM_READ_WRITE);
    for (auto entry : *tempAttribToFloatBladeMap){
        attributeBladeMap[entry.first] = entry.second;
    }
    
    tempAttribToFloatBladeMap = kernelBuddyp->addKernelArgAndBlades<cl_float,Attribute>(1, relationThings, 2, ortus::NUM_ELEMENTS, ortus::MAX_ELEMENTS, CL_MEM_READ_WRITE);
    for (auto entry : *tempAttribToFloatBladeMap){
        attributeBladeMap[entry.first] = entry.second;
    }
    /*
    // need a gj and cs contrib (one of each) -- these are NxN
    chemContrib = new Blade<float>(clHelperp, CL_MEM_READ_WRITE, CONNECTOME_ROWS, CONNECTOME_COLS, MAX_ELEMENTS, MAX_ELEMENTS);
    gapContrib = new Blade<float>(clHelperp, CL_MEM_READ_WRITE, CONNECTOME_ROWS, CONNECTOME_COLS, MAX_ELEMENTS, MAX_ELEMENTS);
    // now we initialize the voltage Blade (one row)
    voltages = new Blade<float>(clHelperp, CL_MEM_READ_WRITE, CONNECTOME_COLS, MAX_ELEMENTS);
    // then the output voltage history Blade -- THIS IS ORGANIZED SUCH THAT EACH ROW CONTAINS A NEURON'S VOLTAGES
    outputVoltageHistory = new Blade<float>(clHelperp, CL_MEM_READ_WRITE, CONNECTOME_ROWS, VOLTAGE_HISTORY_SIZE, MAX_ELEMENTS, VOLTAGE_HISTORY_SIZE);// can't grow column-wise
    //fillInputVoltageBlade(); // will probably be used at some point
    //rowCount = new Blade<cl_uint>(clHelperp, CL_MEM_READ_ONLY);
    //rowCount->set(NUM_ELEMS); // cl_uint, for current row count
    //colCount = new Blade<cl_uint>(clHelperp, CL_MEM_READ_ONLY);
    //colCount->set(NUM_ELEMS); // cl_uint, for current col count
    gapNormalizer = new Blade<cl_float>(clHelperp, CL_MEM_READ_WRITE);
    gapNormalizer->set(1.f); // NOTE: THIS DOESN'T WORK YET... OR GET SET TO ANYTHING OTHER THAN 1!!!!
    chemNormalizer = new Blade<cl_float>(clHelperp, CL_MEM_READ_WRITE);
    chemNormalizer->set(1.f); // NOTE: THIS DOESN'T WORK YET... OR GET SET TO ANYTHING OTHER THAN 1!!!!
    metadata = new Blade<cl_int>(clHelperp, CL_MEM_READ_ONLY, METADATA_COUNT, METADATA_COUNT);
    // this doesn't create a buffer -- just a spot in local memory for the kernel to use as a scratch pad.
    deviceScratchPadXCorr = new Blade<cl_float>(clHelperp, openCLWorkGroupSize*CONNECTOME_ROWS,XCORR_COMPUTATIONS, MAX_ELEMENTS, XCORR_COMPUTATIONS);
    deviceScratchPadVoltageROC = new Blade<cl_float>(clHelperp, openCLWorkGroupSize*CONNECTOME_ROWS,XCORR_COMPUTATIONS, MAX_ELEMENTS, XCORR_COMPUTATIONS);
     */
}
     



// the probe.
/*
 void DataSteward::feedProbe(){
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
 */
