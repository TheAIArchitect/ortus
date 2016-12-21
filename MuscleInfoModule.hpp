//
//  MuscleInfoModule.hpp
//  LearningOpenGL
//
//  Created by onyx on 12/16/15.
//  Copyright © 2015 AweM. All rights reserved.
//

#ifndef MuscleInfoModule_hpp
#define MuscleInfoModule_hpp

#include <stdio.h>

//#include "std_graphics_header.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <string>
#include <vector>

#include "ElementInfoModule.hpp"

#include "MassPoint.hpp"

class MuscleInfoModule : public ElementInfoModule {
    
public:
    MuscleInfoModule();
    glm::vec3 getCenterPoint();
    
public:
    
    float rotAng;
    glm::vec3 rotAxis;
    MassPoint* anteriorMassPoint;
    MassPoint* centerMassPoint;
    MassPoint* posteriorMassPoint;
    static float MUSCLE_SIZE_SCALE;
    static float MUSCLE_TRANS_SCALE;
    static float length_z_scale;
    static float trans_z_scale;
    int muscle_id = -1; // set this to  -1 so that we can test to see if we've set the muscle_id yet -- this is used for ordering the muscles
};


#endif /* MuscleInfoModule_hpp */
