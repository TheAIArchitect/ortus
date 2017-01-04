//
//  Connection.hpp
//  delegans
//
//  Created by onyx on 10/13/16.
//  Copyright © 2016 Andrew W.E. McDonald. All rights reserved.
//

#ifndef Connection_hpp
#define Connection_hpp

#include <stdio.h>
#include <string>
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

enum ConnectionType {GAP, CHEM};

class ElementInfoModule;

class Connection {
    
public:
    Connection();
    float polarity();
    float weight();
    
    
    ElementInfoModule* pre;
    std::string preName; // leave it for this for now
    ElementInfoModule* post;
    std::string postName;
    ConnectionType type;
    std::string sType;
    float* polarityp; // perhaps we can 'model' different neurotransmitters by non-integer valued polarities?
    std::string toString();

    float* weightp;
    
    std::string weightLabel;
    bool marked = false; // used for search
    int path_len = -1; // used for search
    glm::vec3 midpoint; // used to for the label's location
    
    
    
};

#endif /* Connection_hpp */
