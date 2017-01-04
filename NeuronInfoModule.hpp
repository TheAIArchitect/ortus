//
//  NeuronInfoModule.hpp
//  LearningOpenGL
//
//  Created by onyx on 12/1/15.
//  Copyright © 2016 Andrew W.E. McDonald. All rights reserved.
//

#ifndef NeuronInfoModule_hpp
#define NeuronInfoModule_hpp

#include <stdio.h>

//#include "std_graphics_header.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <string>
#include <vector>

#include "ElementInfoModule.hpp"

#include "MassPoint.hpp"

class NeuronInfoModule : public ElementInfoModule {
    
public:
    NeuronInfoModule();
    glm::vec3 getCenterPoint();
    
public:
    
    //glm::vec3 soma_position; // now massPoint->center
    MassPoint* massPoint;
    float soma_diameter;
    int neuron_id;
    static int NUM_NIMS; // number of neuron info modules. gets set in BioData::makeNeuronModules()
    static float AVG_NEURON_DIAM; // this is set in FileShit, when neurons are read in. Used to allow the bezier drawing function to draw the bezier 'points' roughly the same size as the neurons
    
    
    static float NEURON_TRANS_SCALE;
    static float NEURON_SIZE_SCALE;
};

#endif /* NeuronInfoModule_hpp */
