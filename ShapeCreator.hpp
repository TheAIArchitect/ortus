//
//  SphereCreator.hpp
//  LearningOpenGL
//
//  Created by onyx on 11/3/15.
//  Copyright © 2015 AweM. All rights reserved.
//

#ifndef ShapeCreator_hpp
#define ShapeCreator_hpp
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <cmath>
#include <vector>

class ShapeCreator{
public:
    
    static glm::vec3* genSphere(float radius, int latitudes, int longitudes, int* num_vecs);
    static glm::vec3* genMuscle(float rad_cap, float rad_mid, int latitudes, int longitudes, int* num_verts);
};

#endif /* ShapeCreator_hpp */
