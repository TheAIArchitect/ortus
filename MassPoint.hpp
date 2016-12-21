//
//  MassPoint.hpp
//  LearningOpenGL
//
//  Created by onyx on 1/19/16.
//  Copyright © 2016 AweM. All rights reserved.
//

#ifndef MassPoint_hpp
#define MassPoint_hpp

#include <stdio.h>
#include <string>
#include "glm/glm.hpp"

class MassPoint {
    
public:

    MassPoint(glm::vec3 center, std::string name = "ANONYMOUS", float mass = 1.0f);
    MassPoint(MassPoint* oldMP);
    glm::vec3 center;
    float mass;
    float radius = 1.0f;
    std::string name;
    int id;
    
private:
    static int static_id; // start counting at 0, constructor increments
};

#endif /* MassPoint_hpp */
