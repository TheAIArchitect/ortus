//
//  NeuronInfoModule.hpp
//  LearningOpenGL
//
//  Created by onyx on 12/1/15.
//  Copyright © 2015 AweM. All rights reserved.
//

#ifndef ElementInfoModule_hpp
#define ElementInfoModule_hpp

#include <stdio.h>
#include <vector>
#include <string>
#include "MassPoint.hpp"
#include "Connection.hpp"


enum ElementType { SENSORY, MOTOR, INTER, MUSCLE, WFK, UH_OH};




class ElementInfoModule{
    
public:
    
    
    ElementInfoModule();
    ~ElementInfoModule();
    void setElementType(ElementType etype);
    ElementType getEType();
    std::string getSType();
    float getFType();
    glm::vec3 getCenterPoint(); // allows us to use muscles and neurons for creating graphical connections
    
    std::string name;
    std::string graphicalName; // name used in graphics -- with 'graphical identifier": either "-(I)", "-(S)", "-(M)", or "-<M>" -- inter, sensory, motor, or muscle
    int element_id; // the id of this element with respect to all other elements
    int massPoint_id = -1; // -1 means no mass point
    
    std::vector<Connection> out_conns; // will all have same pre (this)
    
    bool marked = false; // used for stack
    bool ablated = false; // if true, we treat the element as non-existent
    float v_0 = 0; // initial voltage
    float v_c; // current voltage
    
private:
    ElementType element_type;
    std::string stype; // string type
    float ftype; // float type
    
};



#endif /* NeuronInfoModule_hpp */
