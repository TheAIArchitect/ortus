//
//  DataSetup.hpp
// ortus
//
// Andrew W.E. McDonald
// Sean Grimes
//

#pragma once

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <sstream>
#include <unordered_map>
#include <fstream>
#include <time.h>
#include "NumUtils.hpp"
#include "StrUtils.hpp"
#include "ElementInfoModule.hpp"
#include "FileAssistant.hpp"
#include "AblationStation.hpp"
#include "Blade.hpp"
#include "Probe.hpp"
#include "CLHelper.hpp"
#include "OrtusStd.hpp"
#include "Connectome.hpp"
#include "KernelArg.hpp"

// only used to make it clear that it's irrelevant,
// like when creating KernelArgs without keys (so, 1 Blade to 1 CLBuffer)
enum class DummyType { Dummy };

class ElementInfoModule;
class ElementRelation;
class ComputeSteward;


class DataSteward{
    
public: /** NEW */
    DataSteward();
    ~DataSteward();
    
    int* kernelIterationNumberp;
    size_t* openCLWorkGroupSize;
    Connectome* connectomep;
    bool connectomeNewed;
    
    
    void init();
    void setComputeStewardPointers(ComputeSteward* computeStewardp);
    void initializeConnectome(std::string ortFile);
    void createElementsAndElementRelations();
    void executePostRunMemoryTransfers();
    
    
    ElementInfoModule* addElement(std::string name);
    ElementRelation* addRelation(std::unordered_map<RelationAttribute, cl_float> newRelationAttributes, ElementInfoModule* ePre, ElementInfoModule* ePost, ElementRelationType ert);
    
    void initializeKernelArgsAndBlades(CLHelper* clHelper, cl_kernel* kernelp);
    void updateMetadataBlades();
    void executePreRunOperations();
    void pushOpenCLBuffers();
    void readOpenCLBuffers();
    void setKernelArgInfo(std::vector<std::vector<int>>& tempKernelArgInfo);
    size_t calculateScratchpadRows(std::string request);
    
    // Blade maps -- this is where the data is *actually* held...
    // Everything else just points here.
    
    /** NOTE: change the template in ortus:: to bladeMap, then fix naming issue in KernelArg, 
     then the name issue isn't, and it's a bit shorter... then again, who cares? */
    // 1d -- r/w
    std::unordered_map<ElementAttribute, Blade<cl_float>*> elementAttributeBladeMap;
    // 2d -- r/w
    std::unordered_map<Scratchpad, Blade<cl_float>*> scratchpadBladeMap;
    std::unordered_map<RelationAttribute, Blade<cl_float>*> relationAttributeBladeMap;
    Blade<cl_float>* activationBlade;
    
    // scalar -- r/w
    std::unordered_map<GlobalAttribute, Blade<cl_float>*> globalAttributeBladeMap;
    std::unordered_map<MetadataAttribute, Blade<cl_float>*> metadataBladeMap;
    
    // 3D
    std::unordered_map<WeightAttribute, Blade<cl_float>*> weightBladeMap;
    
    // last kernelArg
    Blade<cl_int>* kernelArgInfoBlade;
    
    
    std::vector<float> outputActivationVector;
    std::vector<std::vector<float>> fullActivationHistory;
    
    
    const static std::vector<WeightAttribute> WEIGHT_KEYS;
    const static std::vector<ElementAttribute> ELEMENT_KEYS;
    const static std::vector<RelationAttribute> RELATION_KEYS;
    const static std::vector<GlobalAttribute> GLOBAL_KEYS;
    const static std::vector<MetadataAttribute> METADATA_KEYS;
    const static std::vector<Scratchpad> SCRATCHPAD_KEYS;
    
    
    
    
    
    
};
