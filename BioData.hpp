//
//  BioDataDecoder.hpp
//  LearningOpenGL
//
//  Created by onyx on 12/16/15.
//  Copyright © 2015 AweM. All rights reserved.
//

#ifndef BioData_hpp
#define BioData_hpp

#include <stdio.h>
#include "std_graphics_header.hpp"
#include "ElementInfoModule.hpp"
#include "MuscleInfoModule.hpp"
#include "WormwiringCSVToSQLite.hpp"
#include "MassPoint.hpp"

#include "FileShit.hpp"

#include "WormwiringCSVToSQLite.hpp"

#include "Geometrician.hpp"

class BioData {
    
    public:
    //void makeGraph(std::vector<GapJunction> gaps, std::vector<ChemSynapse> chems, NeuronInfoModule* nim, MuscleInfoModule* mim, int nim_count, int mim_count);
    BioData();
    void makeModules();
    void makeMuscleModules();
    void makeNeuronModules();
    void generateBodyMassPoints();
    void generateCenterCylinderMassPoints();
    
    float* neuron_colors;
    float* neuron_positions;
    
    MuscleInfoModule** mim;
    NeuronInfoModule** nim;
    MassPoint** bodyMassPoints;
    int num_body_mass_points;
    MassPoint** neuronMassPoints;
    int num_neuron_mass_points;
    int num_neurons_in_json;
    std::vector<ElementInfoModule*> elements;
    
    const int NUM_MUSCLES = 95;
    const int NUM_CC = 26;
    const float CENTER_CYLINDER_HEIGHT = .5; // meters, so, 50cm
    const float CENTER_CYLINDER_DIAM = .05; // meters, so, 5cm

    
    int num_cc_mass_points;
    MassPoint** centerCylinderMassPoints;
    
};

#endif /* BioData_hpp */
