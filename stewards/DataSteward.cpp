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

#include "ComputeSteward.hpp"

//unsigned int DataSteward::NUM_NEURONS_CLOSEST_LARGER_MULTIPLE_OF_8 = 0;


const std::vector<WeightAttribute> DataSteward::WEIGHT_KEYS = { WeightAttribute::CSWeight, WeightAttribute::GJWeight };
const std::vector<ElementAttribute> DataSteward::ELEMENT_KEYS = { ElementAttribute::Type, ElementAttribute::Affect, ElementAttribute::Thresh};
const std::vector<RelationAttribute> DataSteward::RELATION_KEYS = { RelationAttribute::Type, RelationAttribute::Polarity, RelationAttribute::PreDirection, RelationAttribute::PostDirection, RelationAttribute::Age, RelationAttribute::Thresh, RelationAttribute::Decay, RelationAttribute::Mutability};
const std::vector<GlobalAttribute> DataSteward::GLOBAL_KEYS = { GlobalAttribute::ChemNormalizer, GlobalAttribute::GapNormalizer };
const std::vector<MetadataAttribute> DataSteward::METADATA_KEYS = { MetadataAttribute::NumElements, MetadataAttribute::MaxElements, MetadataAttribute::KernelIterationNum, MetadataAttribute::ActivationHistorySize, MetadataAttribute::NumXCorrComputations, MetadataAttribute::XCorrSize, MetadataAttribute::NumSlopeComputations, MetadataAttribute::SlopeSize} ;
const std::vector<Scratchpad> DataSteward::SCRATCHPAD_KEYS = { Scratchpad::XCorr, Scratchpad::Slope};


DataSteward::DataSteward(){
    connectomeNewed = false;
    init();
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

void DataSteward::init(){
    outputActivationVector.resize(Ort::MAX_ELEMENTS); // need to do this, so we don't run into issues doing things like plotting the data -- elements that don't exist in the beginning, but exist later, should have 0 activation.
    fullActivationHistory.reserve(Ort::NUM_ITERATIONS);
}

/* yeah, this is kind of screwy. */
void DataSteward::setComputeStewardPointers(ComputeSteward* computeStewardp){
    kernelIterationNumberp = &computeStewardp->currentIteration;
    openCLWorkGroupSize = &computeStewardp->workGroupSize;
}

void DataSteward::initializeConnectome(std::string ortFile){
    // Connectome holds the elements, and DataSteward holds the Connectome, but it also holds the Blades, which the Connectome has to link to the Elements, because it's processing the data as creates the Elements and ElementRelations as it sets the data (in the Blades).
    connectomep = new Connectome(this);
    connectomeNewed = true;
    connectomep->initialize(ortFile);
   
}

void DataSteward::createElementsAndElementRelations(){
    connectomep->createElementsAndElementRelations();
    
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
    readOpenCLBuffers();
    //updateOutputVoltageVector();
   
    /**
     NEED TO PUSH VOLTAGES DOWN IN BLADE -- LATER ON,
     TEST TO SEE IF A NEW KERNEL WOULD BE FASTER
     
     or a thread pool?
     */
    // look into using swap()
    /*
     the columns of the activation blade look like this:
     [0][1]...[historySize-1][historySize]
     where [0] was the most up-to-date activation prior to the last kernel call,
     historySize-1 is the oldest activation we have in the blade,
     and historySize is the current (new) activation, newly updated on the last kernel call.
     
     So, the value in historySize goes to [0] (and our full history vector), [0] goes to [1], etc., and [historySize-1] goes away.
     */
    
    
    int historySize = Ort::ACTIVATION_HISTORY_SIZE - 1; // the last one goes to index 0, and is then left empty, to be filled during the next iteration
    
    int i,j;
    // first we want to add the newest activation to our vector that will be pushed to our full activation history
    std::fill(outputActivationVector.begin(), outputActivationVector.end(), 0);
    for (i = 0; i < Ort::NUM_ELEMENTS; ++i){
        outputActivationVector[i] = activationBlade->getv(i,historySize);
    }
    for (j = historySize-1; j > 0; --j){ // init j to historySize-1 to get last *history* *index*
        for (i = 0; i < Ort::NUM_ELEMENTS; ++i){
            activationBlade->set(i, j, activationBlade->getv(i,j-1));
        }
    }
    
    for (i = 0; i < Ort::NUM_ELEMENTS; ++i){
        activationBlade->set(i, 0, activationBlade->getv(i, historySize));
    }
    
    fullActivationHistory.push_back(outputActivationVector);
    
    // now shift the weights. same idea as activations, but this is 3D.
    int k;
    historySize = Ort::WEIGHT_HISTORY_SIZE - 1;
    for (k = historySize-1; k > 0; --k){
        for (i = 0; i < Ort::NUM_ELEMENTS; ++i){
            for (j = 0; j < Ort::NUM_ELEMENTS; ++j){
                weightBladeMap[WeightAttribute::CSWeight]->set(i, j, k, weightBladeMap[WeightAttribute::CSWeight]->getv(i, j, k-1));
                weightBladeMap[WeightAttribute::GJWeight]->set(i, j, k, weightBladeMap[WeightAttribute::GJWeight]->getv(i, j, k-1));
            }
        }
    }
    for (i = 0; i < Ort::NUM_ELEMENTS; ++i){
        for (j = 0; j < Ort::NUM_ELEMENTS; ++j){
            weightBladeMap[WeightAttribute::CSWeight]->set(i, j, 0, weightBladeMap[WeightAttribute::CSWeight]->getv(i, j, k));
            weightBladeMap[WeightAttribute::GJWeight]->set(i, j, 0, weightBladeMap[WeightAttribute::GJWeight]->getv(i, j, k));
        }
    }
   
}


void DataSteward::readOpenCLBuffers(){
    // unrolling this might be faster...
    
    for (auto entry : elementAttributeBladeMap){
        entry.second->readDataFromDevice();
    }
    for (auto entry : relationAttributeBladeMap){
        entry.second->readDataFromDevice();
    }
    activationBlade->readDataFromDevice();
    weightBladeMap[WeightAttribute::CSWeight]->readDataFromDevice();
    weightBladeMap[WeightAttribute::GJWeight]->readDataFromDevice();
}


//std::vector<float> DataSteward::getOutputVoltageVector(){
//    return outputVoltageVector;
//}

/* updates outputVoltageVector with a *COPY* of the voltage data, NOT a reference to the actual data, nor its location. */
//void DataSteward::updateOutputVoltageVector(){
//    outputVoltageVector.clear();
//    for (int i = 0; i < voltages->currentSize; ++i){
//        outputVoltageVector.push_back(voltages->data[i]);
//    }
    // push current voltage vector into vector of all output voltages
//    kernelVoltages.push_back(outputVoltageVector);
//}

void DataSteward::updateMetadataBlades(){
    // really only need to run this for the kernel iteration pointer,
    // but for now, who cares.
    metadataBladeMap[MetadataAttribute::NumElements]->set(Ort::NUM_ELEMENTS);
    printf("NUM ELEMENTS: %d\n",Ort::NUM_ELEMENTS);
    metadataBladeMap[MetadataAttribute::MaxElements]->set(Ort::MAX_ELEMENTS);
    metadataBladeMap[MetadataAttribute::KernelIterationNum]->set(*kernelIterationNumberp);
    metadataBladeMap[MetadataAttribute::ActivationHistorySize]->set(Ort::ACTIVATION_HISTORY_SIZE);
    metadataBladeMap[MetadataAttribute::NumXCorrComputations]->set(Ort::XCORR_COMPUTATIONS);
    metadataBladeMap[MetadataAttribute::XCorrSize]->set(Ort::XCORR_SIZE);
    metadataBladeMap[MetadataAttribute::NumSlopeComputations]->set(Ort::SLOPE_COMPUTATIONS);
    metadataBladeMap[MetadataAttribute::SlopeSize]->set(Ort::SLOPE_SIZE);
}

void DataSteward::executePreRunOperations(){
    updateMetadataBlades();
    pushOpenCLBuffers();
}

void DataSteward::pushOpenCLBuffers(){
    
    //printf("pushing element attributes...\n");
    for (auto entry : elementAttributeBladeMap){
        entry.second->pushDataToDevice();
    }
    
    //printf("pushing relation attributes...\n");
    for (auto entry : relationAttributeBladeMap){
        entry.second->pushDataToDevice();
    }
    
    //printf("pushing weights...\n");
    for (auto entry : weightBladeMap){
        entry.second->pushDataToDevice();
    }
    
    //printf("pushing activations...\n");
    activationBlade->pushDataToDevice();
    
    //printf("pushing metadata...\n");
    for (auto entry : metadataBladeMap){
        //printf("\t%d... ",static_cast<int>(entry.first));
        //printf("with offset %d\n", entry.second->clBufferOffset);
        entry.second->pushDataToDevice();
    }
    
    //printf("pushing kernel arg info...\n");
    kernelArgInfoBlade->pushDataToDevice();
    
    //printf("all Blades pushed.\n");

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


/** NOTE: THIS CAN ONLY BE USED FOR INTERNEURONS!
 * you can't use this to add a new sensor,
 * motorneuron, or muscle. Those must be added in the .ort file.
 *
 * Further, only Architect should call this.
 *
 * returns NULL if operation failed (e.g. already at Ort::MAX_ELEMENTS)
 *
 * calling function should set the element's attributes
 */
ElementInfoModule* DataSteward::addElement(std::string name){
    if (Ort::NUM_ELEMENTS == Ort::MAX_ELEMENTS){
        printf("(DataSteward) Error: cannot create new element, because max number of elements has been reached.\n");
        return NULL;
    }
    // name can't be empty
    if (name == ""){
        printf("(DataSteward) Error: cannot create element without a name.\n");
        return NULL;
    }
    // element can't be in the map already
    if (connectomep->elementMap.find(name) != connectomep->elementMap.end()){
        printf("(DataSteward) Error: element with name '%s' already in elementMap.\n", name.c_str());
        return NULL;
    }
    
    ElementInfoModule* newElement = new ElementInfoModule();
    newElement->name = name;
    newElement->id = connectomep->elementModules.size();
    connectomep->elementMap[name] = newElement;
    connectomep->elementModules.push_back(newElement);
    Ort::NUM_ELEMENTS += 1;
    int newColCountTest = Ort::NUM_ELEMENTS; // should be this
    int newRowCountTest = Ort::NUM_ELEMENTS; // should be this (for 2D or 3D Blades)
    int updateResult = -1;
    
    // increase the relevant Blades' sizes,
    // set data pointers (where applicable), and
    // update kernel arguments
    //
    // element attributes
    for (auto entry : elementAttributeBladeMap){
        if (newColCountTest != (updateResult = entry.second->addCol())){
            printf("(DataSteward) Error: new col count in attribute blade %d not equal to %d (%d returned).\n",entry.first, newColCountTest, updateResult);
            return NULL;
        }
    }
    newElement->setAttributeDataPointers(elementAttributeBladeMap);
    // ka0 -- element attributes => cols per blade (index 4)
    kernelArgInfoBlade->set(0, 4, Ort::NUM_ELEMENTS);
    //
    // relation attributes
    for (auto entry : relationAttributeBladeMap){
        if (newColCountTest != (updateResult = entry.second->addCol())){
            printf("(DataSteward) Error: new col count in relation blade %d not equal to %d (%d returned).\n",entry.first, newColCountTest, updateResult);
            return NULL;
        }
        if (newRowCountTest != (updateResult = entry.second->addRow())){
            printf("(DataSteward) Error: new row count in relation blade %d not equal to %d (%d returned).\n",entry.first, newRowCountTest, updateResult);
            return NULL;
        }
    }
    // ka1 -- relation attributes => rows and cols per blade (indices 3, 4)
    kernelArgInfoBlade->set(1, 3, Ort::NUM_ELEMENTS);
    kernelArgInfoBlade->set(1, 4, Ort::NUM_ELEMENTS);
    //
    // weights
    for (auto entry : weightBladeMap){
       if (newColCountTest != (updateResult = entry.second->addCol())){
            printf("(DataSteward) Error: new col count in weight blade %d not equal to %d (%d returned).\n",entry.first, newColCountTest, updateResult);
            return NULL;
        }
        if (newRowCountTest != (updateResult = entry.second->addRow())){
            printf("(DataSteward) Error: new row count in weight blade %d not equal to %d (%d returned).\n",entry.first, newRowCountTest, updateResult);
            return NULL;
        }
    }
    // ka2 -- weight attributes => rows and cols per blade (indices 3, 4)
    kernelArgInfoBlade->set(2, 3, Ort::NUM_ELEMENTS);
    kernelArgInfoBlade->set(2, 4, Ort::NUM_ELEMENTS);
    //
    // activations
    if (newRowCountTest != (updateResult = activationBlade->addRow())){
        printf("(DataSteward) Error: new row count in activation blade not equal to %d (%d returned).\n",  newRowCountTest, updateResult);
        return NULL;
    }
    newElement->setActivationDataPointer(activationBlade);
    // ka3 -- activations => rows per blade (index 3)
    kernelArgInfoBlade->set(3, 4, Ort::NUM_ELEMENTS);
    //
    // scratchpads
    int newScratchpadRowCountTest = Ort::NUM_ELEMENTS * (*openCLWorkGroupSize);
    for (auto entry : scratchpadBladeMap){
        if (newScratchpadRowCountTest != (updateResult = entry.second->addRows(*openCLWorkGroupSize))){
            printf("(DataSteward) Error: new row count in scratchpad blade %d not equal to %d (%d returned).\n",entry.first, newScratchpadRowCountTest, updateResult);
        }
    }
    // ka5 -- scratchpads => rows per blade (index 3)
    kernelArgInfoBlade->set(5, 3, calculateScratchpadRows("current"));
    //
    // ka4 -- metadata blades get updated just before kernel args get pushed (updateMetadataBlades)
    
    // add new element (and its info) to the connectome data structures
    connectomep->nameMap[name] = newElement->id;
    connectomep->indexMap[newElement->id] = name;
    
    return newElement;
}

/** only call this from Architect.
 *
 * the main reason this is here, is because 'addElement' is here,
 * and it made sense to keep the two together.
 *
 * returns NULL if operation failed.
 */
ElementRelation* DataSteward::addRelation(std::unordered_map<RelationAttribute, cl_float> newRelationAttributes, ElementInfoModule* ePre, ElementInfoModule* ePost, ElementRelationType ert){
    
    // create the new relation (this also sets the data pointers,
    // and adds the relation to the connectome datastructures)
    ElementRelation* newElrel = connectomep->addRelation(ePre, ePost, ert);
    // then add the attributes
    for (auto attrib : newRelationAttributes){
        newElrel->setAttribute(attrib.first, attrib.second);
    }
    return newElrel;
}


/** creates KernelArgs, CLBuffers, and Blades
 **/
void DataSteward::initializeKernelArgsAndBlades(CLHelper* clHelper, cl_kernel* kernelp){
    
    // THE LAST KERNEL ARGUMENT will be the kernelArgInfo (kernel arg metadata),
    // no relation to the 'metadata' blade, which is 'ortus' metadata.
    //
    // The kernelArgInfo will be formatted like so:
    // 'row' is kernel arg, and there are 6 'col's for each kernel arg:
    //      [0] => kernel arg number (sanity check)
    //      [1] => number of Blades / different 'types' (e.g., age, polarity, etc.) of data in that buffer / at that kernel argument locaiton
    //      [2] => offset from start of one, to start of another (so, the max size):
    //              -> if each blade is 100, 1st starts at 0, 2nd at 100, 3rd at 200, and so on.
    //      [3] => rows per Blade
    //      [4] => cols per Blade
    //      [5] => pages per Blade
    //
    // NOTE: THERE IS NO kernelArgInfo FOR THE kernelArgInfo arg!!!
    //      => it will only have 'NUM_KERNEL_ARGS-1' entries
    //      => The kernel *must* know (have hard--coded):
    //          * the number of kernel args
    //          * the number of metadata entries for each kernel arg (e.g., 6)
    //          * the significance/meaning of each of the metadata entries
    
    cl_uint NUM_KERNEL_ARGS = 7;
    int maxKernelIndex = NUM_KERNEL_ARGS - 1;
    int numKernelArgMetadataCols = 6;
    int numBlades;
    int maxSize;// the offset from the start of one blade to the next (so, maxSize)
    // note: these 3 measures are what is currently in there, **not** the max.
    int rowsPerBlade;
    int colsPerBlade;
    int pagesPerBlade;
    std::vector<std::vector<int>> tempKernelArgInfo;
    cl_uint currentKernelArgNum = 0;
    // END variable initialization.
    
    
    // KERNEL ARG #0!!! Element (neuron, muscle) attributes
    printf("Creating KernelArg # %d of %d\n",currentKernelArgNum, maxKernelIndex);
    KernelArg<cl_float, ElementAttribute> ka0(clHelper, kernelp, currentKernelArgNum, ELEMENT_KEYS);
    elementAttributeBladeMap = *(ka0.addKernelArgWithBufferAndBlades(Ort::NUM_ELEMENTS, Ort::MAX_ELEMENTS, CL_MEM_READ_WRITE));
    // now collect metadata info for our 'kernelArgInfo' (Blade) arg that we'll add at the end:
    numBlades = (int) elementAttributeBladeMap.size();
    maxSize = (int) elementAttributeBladeMap[ELEMENT_KEYS[0]]->maxSize;
    rowsPerBlade = 1;
    colsPerBlade = Ort::NUM_ELEMENTS;
    pagesPerBlade = 1;
    // add the data to our temporary vector (we can't set a kernel arg out of order, and we want it at the end)
    // it also makes it easier to collect the data, in a way. 
    tempKernelArgInfo.push_back(std::vector<int>{(int)currentKernelArgNum, numBlades, maxSize, rowsPerBlade, colsPerBlade, pagesPerBlade});
    // finally, increment the kernel arg number
    currentKernelArgNum++;
    // END KERNEL ARG #0
    
    // KERNEL ARG #1!!! Relation (connection) attributes
    printf("Creating KernelArg # %d of %d\n",currentKernelArgNum, maxKernelIndex);
    KernelArg<cl_float, RelationAttribute> ka1(clHelper, kernelp, currentKernelArgNum, RELATION_KEYS);
    relationAttributeBladeMap = *(ka1.addKernelArgWithBufferAndBlades(Ort::NUM_ELEMENTS, Ort::NUM_ELEMENTS, Ort::MAX_ELEMENTS, Ort::MAX_ELEMENTS, CL_MEM_READ_WRITE));
    // collect metadata
    numBlades = (int) relationAttributeBladeMap.size();
    maxSize = (int) relationAttributeBladeMap[RELATION_KEYS[0]]->maxSize;
    rowsPerBlade = Ort::NUM_ELEMENTS;
    colsPerBlade = Ort::NUM_ELEMENTS;
    pagesPerBlade = 1;
    // add the data to temp vector
    tempKernelArgInfo.push_back(std::vector<int>{(int)currentKernelArgNum, numBlades, maxSize, rowsPerBlade, colsPerBlade, pagesPerBlade});
    // increment kernel arg number
    currentKernelArgNum++;
    // END KERNEL ARG #1
    
    
    // KERNEL ARG #2!!! Weights (CS and GJ) -- this is 2 3D blades.
    printf("Creating KernelArg # %d of %d\n",currentKernelArgNum, maxKernelIndex);
    // NOTE: specifying 'false' at the end is important (for now), because if that doesn't happen,
    // the cl_mem_flags enum will register 'true' for the funciton simplification that creates a device scratch pad...
    KernelArg<cl_float, WeightAttribute> ka2(clHelper, kernelp, currentKernelArgNum, WEIGHT_KEYS);
    weightBladeMap = *(ka2.addKernelArgWithBufferAndBlades(Ort::NUM_ELEMENTS, Ort::NUM_ELEMENTS, Ort::WEIGHT_HISTORY_SIZE, Ort::MAX_ELEMENTS, Ort::MAX_ELEMENTS, Ort::WEIGHT_HISTORY_SIZE, CL_MEM_READ_WRITE, false));
    // collect metadata
    numBlades = (int) weightBladeMap.size();
    maxSize = (int) weightBladeMap[WEIGHT_KEYS[0]]->maxSize;
    rowsPerBlade = Ort::NUM_ELEMENTS;
    colsPerBlade = Ort::NUM_ELEMENTS;
    pagesPerBlade = Ort::WEIGHT_HISTORY_SIZE;
    // add the data to temp vector
    tempKernelArgInfo.push_back(std::vector<int>{(int)currentKernelArgNum, numBlades, maxSize, rowsPerBlade, colsPerBlade, pagesPerBlade});
    // increment kernel arg number
    currentKernelArgNum++;
    // END KERNEL ARG #2
    
    // KERNEL_ARG #3!!! Current and historic activations -- col 0 holds the current activations, cols 1+ hold the historic activations
    printf("Creating KernelArg # %d of %d\n",currentKernelArgNum, maxKernelIndex);
    KernelArg<cl_float, DummyType> ka3(clHelper, kernelp, currentKernelArgNum);
    activationBlade = ka3.addKernelArgWithBufferAndBlade(Ort::NUM_ELEMENTS, Ort::ACTIVATION_HISTORY_SIZE, Ort::MAX_ELEMENTS, Ort::ACTIVATION_HISTORY_SIZE, CL_MEM_READ_WRITE);
    // collect metadata
    numBlades = 1;// just one blade here, no map
    maxSize = (int) activationBlade->maxSize;
    rowsPerBlade = Ort::NUM_ELEMENTS;
    colsPerBlade = Ort::ACTIVATION_HISTORY_SIZE;
    pagesPerBlade = 1;
    // add the data to temp vector
    tempKernelArgInfo.push_back(std::vector<int>{(int)currentKernelArgNum, numBlades, maxSize, rowsPerBlade, colsPerBlade, pagesPerBlade});
    // increment kernel arg number
    currentKernelArgNum++;
    // END KERNEL ARG #3
    
    
    // ignoring global_keys for now...
    
    
    // KERNEL ARG #4!!! Metadata
    printf("Creating KernelArg # %d of %d\n",currentKernelArgNum, maxKernelIndex);
    KernelArg<cl_float, MetadataAttribute> ka4(clHelper, kernelp, currentKernelArgNum, METADATA_KEYS);
    metadataBladeMap = *(ka4.addKernelArgWithBufferAndBlades(CL_MEM_READ_WRITE));
    // collect metadata (again, this metadata has nothing to do with the metadataBladeMap that we are collecting metadata from)
    numBlades = (int) metadataBladeMap.size();
    maxSize = (int) metadataBladeMap[METADATA_KEYS[0]]->maxSize;
    rowsPerBlade = 1;
    colsPerBlade = Ort::BLADE_METADATA_COUNT;
    pagesPerBlade = 1;
    // add the data to temp vector
    tempKernelArgInfo.push_back(std::vector<int>{(int)currentKernelArgNum, numBlades, maxSize, rowsPerBlade, colsPerBlade, pagesPerBlade});
    // increment kernel arg number
    currentKernelArgNum++;
    // END KERNEL ARG #4
    
    
    // KERNEL ARG #5!!!  Scratchpads (temporary storage of info in kernel)
    printf("Creating KernelArg # %d of %d\n",currentKernelArgNum, maxKernelIndex);
    // CHECK THIS!!!
    size_t scratchpadRows = calculateScratchpadRows("current");
    size_t scratchpadMaxRows = calculateScratchpadRows("max");
    KernelArg<cl_float, Scratchpad>  ka5(clHelper, kernelp, currentKernelArgNum, SCRATCHPAD_KEYS);
    scratchpadBladeMap = *(ka5.addKernelArgWithBufferAndBlades(scratchpadRows, Ort::SCRATCHPAD_COMPUTATION_SLOTS, 1, scratchpadMaxRows, Ort::SCRATCHPAD_COMPUTATION_SLOTS, 1, true));
    // collect metadata 
    numBlades = (int) scratchpadBladeMap.size();
    maxSize = (int) scratchpadBladeMap[SCRATCHPAD_KEYS[0]]->maxSize;
    rowsPerBlade = (int) scratchpadRows;
    colsPerBlade = Ort::SCRATCHPAD_COMPUTATION_SLOTS;
    pagesPerBlade = 1;
    // add the data to temp vector
    tempKernelArgInfo.push_back(std::vector<int>{(int)currentKernelArgNum, numBlades, maxSize, rowsPerBlade, colsPerBlade, pagesPerBlade});
    // increment kernel arg number
    currentKernelArgNum++;
    // END KERNEL ARG #5
    
    
    printf("Creating KernelArg # %d of %d\n",currentKernelArgNum, maxKernelIndex);
    // kernel arg info - this will be the last kernel arg
    KernelArg<cl_int, DummyType> kaLast(clHelper, kernelp, currentKernelArgNum);
    // might be able to simplify this constructor so if only one set (rows, cols, pages)
    // is specified, it knows to duplicate them so current == max.
    // NOTE: again, using NUM_KERNEL_ARGS - 1 because the kernelArgInfo arg doesn't have kernelArgInfo
    kernelArgInfoBlade = kaLast.addKernelArgWithBufferAndBlade(NUM_KERNEL_ARGS-1, numKernelArgMetadataCols, NUM_KERNEL_ARGS-1, numKernelArgMetadataCols, CL_MEM_READ_ONLY);
    
    // now we transfer our kernelArgInfo from the temp vector into the Blade.
    setKernelArgInfo(tempKernelArgInfo);
    
    printf("All KernelArgs created.\n");
}
     

/** Transfers the temporary kernel arg info from the holding vector into the Blade for "permanent" storage */
void DataSteward::setKernelArgInfo(std::vector<std::vector<int>>& tempKernelArgInfo){
    int i, j;
    int numArgsWithInfo = (int) tempKernelArgInfo.size();
    int numInnerIndices;
    for (i = 0; i < numArgsWithInfo; ++i){
        numInnerIndices = (int) tempKernelArgInfo[i].size();
        for (j = 0; j < numInnerIndices; ++j){
            kernelArgInfoBlade->set(i, j, tempKernelArgInfo[i][j]);
        }
    }
}

/**
 * using a string here to make it clear what's being computed/return
 */
size_t DataSteward::calculateScratchpadRows(std::string request){
    if (request == "current"){
        return Ort::NUM_ELEMENTS * (*openCLWorkGroupSize);
    }
    else if (request == "max"){
        return Ort::MAX_ELEMENTS * (*openCLWorkGroupSize);
    }
    else return -1;
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
