//
//  MuscleInfoModule.cpp
//  LearningOpenGL
//
//  Created by onyx on 12/16/15.
//  Copyright © 2015 AweM. All rights reserved.
//

#include "MuscleInfoModule.hpp"

float MuscleInfoModule::MUSCLE_SIZE_SCALE = .1f;
float MuscleInfoModule::MUSCLE_TRANS_SCALE = 5.f;
float MuscleInfoModule::length_z_scale = 2.f;// make muscles a bit longer
float MuscleInfoModule::trans_z_scale = 1.3f/length_z_scale;

MuscleInfoModule::MuscleInfoModule(){
    rotAng = 0.0f;
    rotAxis = glm::vec3(0.0f);
//    muscle_position_offset = glm::vec3(1.0f);
}

glm::vec3 MuscleInfoModule::getCenterPoint(){
    glm::vec3 tvec = glm::vec3(centerMassPoint->center);
    tvec = tvec*MUSCLE_SIZE_SCALE*glm::vec3(1.f, 1.f, length_z_scale); // scale for size;
 //   tvec = tvec*((glm::vec3(1.f, 1.f,trans_z_scale)*MUSCLE_TRANS_SCALE));
    tvec = tvec*MUSCLE_TRANS_SCALE;
    tvec = tvec*glm::vec3(1.f, 1.f,trans_z_scale);
    tvec = tvec + glm::vec3(0.f, -2.5f, 1.f)*MUSCLE_SIZE_SCALE;
    return tvec;
}

