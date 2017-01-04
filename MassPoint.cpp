//
//  MassPoint.cpp
//  LearningOpenGL
//
//  Created by onyx on 1/19/16.
//  Copyright © 2016 AweM. All rights reserved.
//

#include "MassPoint.hpp"


MassPoint::MassPoint(glm::vec3 center, std::string name, float mass): center {center}, name {name}, mass {mass}, id {++static_id}{
        // Nothing to do!!!
}

//  copy constructor
MassPoint::MassPoint(MassPoint* oldMP){
    center = glm::vec3(oldMP->center);
    name = oldMP->name;
    mass = oldMP->mass;
    id = ++static_id;// don't copy id
    radius = oldMP->radius;
    
}

int MassPoint::static_id = -1;//c'tor increments prior to setting 'id' variable.
