//
//  NeuronInfoModule.cpp
//  LearningOpenGL
//
//  Created by onyx on 12/1/15.
//  Copyright © 2016 Andrew W.E. McDonald. All rights reserved.
//

#include "NeuronInfoModule.hpp"

float NeuronInfoModule::AVG_NEURON_DIAM = 1.f;
int NeuronInfoModule::NUM_NIMS = -1;
float NeuronInfoModule::NEURON_TRANS_SCALE = 10000.f;
float NeuronInfoModule::NEURON_SIZE_SCALE = 1000.f;

NeuronInfoModule::NeuronInfoModule(){
    
}

glm::vec3 NeuronInfoModule::getCenterPoint(){
    return massPoint->center;
}
