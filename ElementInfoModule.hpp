//
//  NeuronInfoModule.hpp
//  LearningOpenGL
//
//  Created by onyx on 12/1/15.
//  Copyright © 2016 Andrew W.E. McDonald. All rights reserved.
//

#ifndef ElementInfoModule_hpp
#define ElementInfoModule_hpp

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#include "OrtusStd.hpp"
#include <stdio.h>
#include <vector>
#include <string>
//#include "DataSteward.hpp"
#include "Blade.hpp"

// a CSO is a 'Consolidated Sensory Output',
// meaning that it is the last step in sensory consolidation for a given set of sensors of the same type (e.g., visual),
// and it's activation signals that 
enum class ElementType { SENSE, EMOTION, INTER, MOTOR, MUSCLE, NUM_ELEMENT_TYPES};


enum class ElementAffect { NEG = -1, NEUTRAL, POS};

class DataSteward;

/**
 * the data here should actually be held in blades, so that it is stored centrally, and can easily be pushed to opencl, but can also easily be used to generate graphical results.
 */
class ElementInfoModule{
    
public:
    
    
    ElementInfoModule();
    ~ElementInfoModule();
    
    void setAttributeDataPointers(std::unordered_map<ElementAttribute, Blade<cl_float>*>& elementAttributeBladeMap);
    void setActivationDataPointer(Blade<cl_float>* activationBladep);
    //glm::vec3 getCenterPoint(); // allows us to use muscles and neurons for creating graphical connections
    
    //std::string* namep;
    std::string name;
    std::string graphicalName; // name used in graphics -- with 'graphical identifier": either "-(I)", "-(S)", "-(M)", or "-<M>" -- inter, sensory, motor, or muscle
    //int* idp; // the id of this element with respect to all other elements
    int id;
    int massPoint_id = -1; // -1 means no mass point
    
    bool isSEI; // Sensory Extension Interneuron
    bool isEEI; // Emotion Extension Interneuron
    
    bool marked = false; // used for stack
    bool ablated = false; // if true, we treat the element as non-existent
    
    cl_float** activationp;
    cl_float* affectp;
    cl_float* typep;
    //cl_float* threshp;
    
    
    void setActivation(float fActivation);
    
    void setAffect(ElementAffect eAffect);
    void setType(ElementType type);
    
    std::string getName();
    float vCurr(int fromTimestepsAgo); //// NOTE: change this to 'getActivation()' -- held of because of potential errors it would generate
    int getId();
    
    
    void setAffect(std::string affect);
    std::string getSAffect();
    ElementAffect eAffect;
    ElementAffect getEAffect();
    float getFAffect();
    
        
    void setType(std::string type);
    ElementType getEType();
    std::string getSType();
    float getFType();
    
    std::string toString();
    
    std::string sAffect;
    ElementType eType;
    std::string sType; // string type
    
};



#endif /* NeuronInfoModule_hpp */
