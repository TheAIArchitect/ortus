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

#include "OrtusNamespace.hpp"
#include <stdio.h>
#include <vector>
#include <string>
#include "KernelBuddy.hpp"
#include "Blade.hpp"


enum class ElementType { SENSE, EMOTION, INTER, MOTOR, MUSCLE, NUM_ELEMENT_TYPES};


enum class ElementAffect { NEG = -1, NEUTRAL, POS};



/**
 * the data here should actually be held in blades, so that it is stored centrally, and can easily be pushed to opencl, but can also easily be used to generate graphical results.
 */
class ElementInfoModule{
    
public:
    
    
    ElementInfoModule();
    ~ElementInfoModule();
    
    void setAttributeDataPointers(KernelBuddy* kbp);
    void setActivationDataPointer(KernelBuddy* kbp);
    //glm::vec3 getCenterPoint(); // allows us to use muscles and neurons for creating graphical connections
    
    //std::string* namep;
    std::string name;
    std::string graphicalName; // name used in graphics -- with 'graphical identifier": either "-(I)", "-(S)", "-(M)", or "-<M>" -- inter, sensory, motor, or muscle
    int* idp; // the id of this element with respect to all other elements
    int id;
    int massPoint_id = -1; // -1 means no mass point
    
    bool marked = false; // used for stack
    bool ablated = false; // if true, we treat the element as non-existent
    
    cl_float* activationp;
    cl_float* affectp;
    cl_float* typep;
    
    
    
    
    std::string getName();
    float vCurr();
    int getId();
    
    
    void setAffect(std::string affect);
    std::string getSAffect();
    float getFAffect();
    
        
    void setType(std::string type);
    ElementType getEType();
    std::string getSType();
    float getFType();
    
    std::string toString();
    
    std::string sAffect;
    ElementType eType;
    std::string sType; // string type
    
private:
    float activation;
    float fAffect;
    float fType;
    
};



#endif /* NeuronInfoModule_hpp */
