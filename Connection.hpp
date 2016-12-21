//
//  Connection.hpp
//  delegans
//
//  Created by onyx on 10/13/16.
//  Copyright © 2016 AweM. All rights reserved.
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

enum NeuroTransmitter {POS, NEG};

class ElementInfoModule;

class Connection {
    
public:
    Connection();
    ElementInfoModule* pre;
    std::string preName;
    ElementInfoModule* post;
    std::string postName;
    ConnectionType conntype;
    std::string sConntype;
    int polarity;
    std::vector<NeuroTransmitter> neroTransmitters;
    std::string toString();

    float weight;
    
    std::string weightLabel;
    bool marked = false; // used for search
    int path_len = -1; // used for search
    glm::vec3 midpoint; // used to for the label's location
    
};

#endif /* Connection_hpp */
