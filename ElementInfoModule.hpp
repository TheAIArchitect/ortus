//
//  NeuronInfoModule.hpp
//  LearningOpenGL
//
//  Created by onyx on 12/1/15.
//  Copyright © 2016 Andrew W.E. McDonald. All rights reserved.
//

#ifndef ElementInfoModule_hpp
#define ElementInfoModule_hpp

#include <stdio.h>
#include <vector>
#include <string>
#include "MassPoint.hpp"
#include "Connection.hpp"


enum ElementType { SENSE, EMOTION, INTER, MOTOR, MUSCLE};




/**
 * the data here should actually be held in blades, so that it is stored centrally, and can easily be pushed to opencl, but can also easily be used to generate graphical results.
 */
class ElementInfoModule{
    
public:
    
    
    ElementInfoModule();
    ~ElementInfoModule();
    
    glm::vec3 getCenterPoint(); // allows us to use muscles and neurons for creating graphical connections
    
    std::string* namep;
    std::string graphicalName; // name used in graphics -- with 'graphical identifier": either "-(I)", "-(S)", "-(M)", or "-<M>" -- inter, sensory, motor, or muscle
    int* idp; // the id of this element with respect to all other elements
    int massPoint_id = -1; // -1 means no mass point
    
    bool marked = false; // used for stack
    bool ablated = false; // if true, we treat the element as non-existent
    float v_0 = 0; // initial voltage
    float* vCurrp; // current voltage
    
    std::string name();
    float vCurr();
    int id();
    
    
    void setAffect(std::string affect);
    std::string getSAffect();
    float getFAffect();
    
        
    void setType(std::string type);
    ElementType getEType();
    std::string getSType();
    float getFType();
    
private:
    std::string sAffect;
    float fAffect;
    ElementType eType;
    std::string sType; // string type
    float fType; // float type
    
};



#endif /* NeuronInfoModule_hpp */
