//
//  BioDataDecoder.cpp
//  LearningOpenGL
//
//  Created by onyx on 12/16/15.
//  Copyright © 2015 AweM. All rights reserved.
//

#include "BioData.hpp"

int MAX_MUSCLES = 200;// with allllll muscles, i think it's 150, but right now, it doesn't much matter if we say 200 or 150...

/*
void BioDataDecoder::makeGraph(std::vector<GapJunction> gaps, std::vector<ChemSynapse> chems, NeuronInfoModule* nim, MuscleInfoModule* mim, int nim_count, int mim_count){
   
    
    
}
*/

BioData::BioData(){
    get_conns(elements);
    FileShit::get_neuron_data(&neuronMassPoints, &num_neuron_mass_points, &num_neurons_in_json, &elements);
    generateBodyMassPoints();
    generateCenterCylinderMassPoints();
}

void BioData::makeModules(){
    makeNeuronModules();
    makeMuscleModules();
}

void BioData::makeNeuronModules(){
    
    int num_elements = elements.size();
    int num_neurons = 0;
    for (int i = 0; i < num_elements; i++){
        if (elements[i]->element_type != MUSCLE){
            ++num_neurons;
        }
    }
    nim = new NeuronInfoModule*[num_neurons];
    int neuron_count = 0;
    for (int i = 0; i < num_elements; i++){
        if (elements[i]->element_type != MUSCLE){
            nim[neuron_count] = (NeuronInfoModule*) elements[i];
            ++neuron_count;
        }
    }
    NeuronInfoModule::NUM_NIMS = neuron_count;
}

void BioData::makeMuscleModules(){
    
    mim = new MuscleInfoModule*[MAX_MUSCLES];
    MuscleInfoModule* temp_mim = new MuscleInfoModule[MAX_MUSCLES];
    int num_elements = (int) elements.size();
    int muscle_id = 0;
    int muscle_count = 0;
    int i;
    MuscleInfoModule tmim;
    for (i = 0; i < num_elements; i++){
        if (elements[i]->element_type == MUSCLE && elements[i]->name.substr(1,3) == "BWM"){// only focusing on body wall muscles now
            tmim = MuscleInfoModule();
            tmim.name = elements[i]->name;
            tmim.element_type = elements[i]->element_type;
            tmim.element_id = i;
            temp_mim[muscle_count] = tmim;
            muscle_count++;
        }
    }
    if (NUM_MUSCLES != muscle_count){
        printf("HOLD THE fuck ON! Are you using more than just the bodywall muscles? in BioData::makeMuscleModules, muscle_count != NUM_MUSCLES... NUM_MUSCLES = 95.\n");
        exit(0);
    }
//    printf("counted muscles, actual muscles: %d, %d\n\n",muscle_count, NUM_MUSCLES);
    std::regex r("(d|v)(BWM)(L|R)([[:digit:]]+)");
    std::smatch regres;
    int quadrant = 0; // quandrants: 0 -> DL, 1 -> DR, 2 -> VR, 3 -> VL. Round robin ordering.
    int round = 1;
    std::string opts[4] = {"dL", "dR", "vR", "vL"}; // accessed by quadrant variable
    for ( i = 0; i < NUM_MUSCLES; i++){
        //printf("DAMN (%d): %s\n",i, temp_mim[i].name.c_str());
        regex_search(temp_mim[i].name,regres, r);
        if (regres.size() == 5){
            // group 0 -> full match, group 1 -> either d or v, group 2 -> BWM, group 3 -> L or R, group 4 -> number (1 or 2 digits)
            if (round == std::stoi(regres[4].str()) && (regres[1].str()+regres[3].str()) == opts[quadrant]){
            //std::string in = regres[1].str() + regres[3].str() + regres[4].str();
                mim[muscle_id] = (MuscleInfoModule*) elements[temp_mim[i].element_id];
                mim[muscle_id]->muscle_id = muscle_id;
                ++muscle_id;
                ++quadrant;
                if (quadrant >= 4 && round < 24){
                    quadrant = 0;
                    ++round;
                }
                else if (quadrant >= 3 && round >= 24){
                    break; // there is no VL24.
                }
                i = 0;
            }
            
        }
    }
    for (i = 0; i < NUM_MUSCLES; i++){
        //printf("DAMN: %s\n", mim[i]->name.c_str());
        mim[i]->anteriorMassPoint = bodyMassPoints[i*3];
        mim[i]->centerMassPoint = bodyMassPoints[i*3+1];
        mim[i]->posteriorMassPoint = bodyMassPoints[i*3+2];
        mim[i]->anteriorMassPoint->name = mim[i]->name;
        mim[i]->centerMassPoint->name = mim[i]->name;
        mim[i]->posteriorMassPoint->name = mim[i]->name;
    }
    
    
    //std::regex r("(.*)(0)([[:digit:]])(.*)");
    // group 0 -> full match, group 1 -> before zero, group 2 -> zero, group 3 -> digit after zero, group 4 -> everything after digit
    
}

void BioData::generateBodyMassPoints(){
    /**
     * set up positions
     */
    num_body_mass_points = NUM_MUSCLES*3;
    bodyMassPoints = new MassPoint*[num_body_mass_points];
    float xshift = .06f; // always either pushing in or pulling out to create offset
    float yshift = .4f;
    float zshift = .5f;
    float x_sign_coeff[4] = {-1.0f, 1.0f, 1.0f, -1.0f};
    float y_coeff[4] = {1.0f, 1.0f, 0.0f, 0.0f};
    int round = 1;
    float quadrant_translations = .15; // for x and y;
    int push_pull = -1;
    int quadrant = 0; // quandrants: 0 -> DL, 1 -> DR, 2 -> VR, 3 -> VL. Round robin ordering.
    float temp_shift = 0.0f;
    int index = 0;
    for (int i = 0; i < NUM_MUSCLES; i++){
        quadrant = i % 4;// 0,1, 2, 3
        push_pull = round % 2; // if 1, we pull, if 0, we push --> round 1, muscles are closer to inside, round 2, closer to outside, etc..
        // x
        float x;
        if (push_pull == 1){
            x = quadrant_translations - xshift;
        }
        else{
            x = quadrant_translations + xshift;
        }
        x = x*x_sign_coeff[quadrant];
        // y
        float y = yshift*y_coeff[quadrant];
        // z
        float z = (round * zshift) - zshift;
        //(*mim)[i]->muscle_position_offset = glm::vec3(x, y, z);
        bodyMassPoints[3*i] = new MassPoint(glm::vec3(x,y,z+.005));
        bodyMassPoints[3*i+1] = new MassPoint(glm::vec3(x,y,z+zshift));
        bodyMassPoints[3*i+2] = new MassPoint(glm::vec3(x,y,z+(2*zshift)-.005));
        //(*mim)[i]->muscle_position_offset = glm::vec3(2.0f, 2.0f, 2.0f);
        //printf("%s\n",(*mim)[i].name.c_str());
        if (quadrant >= 3){
            ++round;
        }
    }
    /*
    for (int i = 0; i < 95; i++){
       printf("nim %d: %s\n",i, (*mim)[i]->name.c_str());
    }
    printf("shit.\n");
     */
}

void BioData::generateCenterCylinderMassPoints(){
    /**
     * set up positions
     */
    num_cc_mass_points = NUM_CC*3;
    centerCylinderMassPoints = new MassPoint*[num_cc_mass_points];
    float xshift = 0.0f; // always either pushing in or pulling out to create offset
    float yshift = .2f;
    float zshift = CENTER_CYLINDER_HEIGHT;
    //float x_sign_coeff[4] = {-.10f, .10f, .10f, -.10f};
    //float y_coeff[4] = {1.0f, 1.0f, 0.0f, 0.0f};
    int round = 0;
    //float quadrant_translations = .05; // for x and y;
    //int push_pull = -1;
    //int quadrant = 0; // quandrants: 0 -> DL, 1 -> DR, 2 -> VR, 3 -> VL. Round robin ordering.
    //float temp_shift = 0.0f;
    int index = 0;
    for (int i = 0; i < NUM_CC; i++){
        float x = xshift;
        // y
        float y = yshift;
        // z
        float z = (round * zshift);// + zshift;
        //(*mim)[i]->muscle_position_offset = glm::vec3(x, y, z);
        centerCylinderMassPoints[i*3] = new MassPoint(glm::vec3(x,y,z));
        centerCylinderMassPoints[i*3+1] = new MassPoint(glm::vec3(x,y,(z+zshift)/2.0f));
        centerCylinderMassPoints[i*3+2] = new MassPoint(glm::vec3(x,y,z+zshift));
        //(*mim)[i]->muscle_position_offset = glm::vec3(2.0f, 2.0f, 2.0f);
        //printf("%s\n",(*mim)[i].name.c_str());
        ++round;
    }
    /*
    for (int i = 0; i < 95; i++){
       printf("nim %d: %s\n",i, (*mim)[i]->name.c_str());
    }
    printf("shit.\n");
     */
}
