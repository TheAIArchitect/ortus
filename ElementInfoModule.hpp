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


enum ElementType { SENSORY, MOTOR, INTER, MUSCLE, WFK, UH_OH};




/**
 * the data here should actually be held in blades, so that it is stored centrally, and can easily be pushed to opencl, but can also easily be used to generate graphical results.
 */
class ElementInfoModule{
    
public:
    
    
    ElementInfoModule();
    ~ElementInfoModule();
    void setElementType(ElementType etype);
    ElementType getEType();
    std::string getSType();
    float getFType();
    glm::vec3 getCenterPoint(); // allows us to use muscles and neurons for creating graphical connections
    
    std::string* namep;
    std::string graphicalName; // name used in graphics -- with 'graphical identifier": either "-(I)", "-(S)", "-(M)", or "-<M>" -- inter, sensory, motor, or muscle
    int* idp; // the id of this element with respect to all other elements
    int massPoint_id = -1; // -1 means no mass point
    
    std::vector<Connection> out_conns; // will all have same pre (this)
    
    bool marked = false; // used for stack
    bool ablated = false; // if true, we treat the element as non-existent
    float v_0 = 0; // initial voltage
    float* vCurrp; // current voltage
    
    std::string name();
    float vCurr();
    int id();
    
    
private:
    ElementType element_type;
    std::string stype; // string type
    float ftype; // float type
    
};



#endif /* NeuronInfoModule_hpp */
