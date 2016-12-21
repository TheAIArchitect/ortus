//
//  AblationStation.cpp
//  delegans
//
//  Created by onyx on 10/18/16.
//  Copyright © 2016 Andrew W.E. McDonald. All rights reserved.
//

#include "AblationStation.hpp"

std::string* AblationStation::NEURONS_FOR_ABLATION;
std::string* AblationStation::MUSCLES_FOR_ABLATION;
std::string* AblationStation::PARTIALS_FOR_ABLATION;
ElementType* AblationStation::TYPES_FOR_ABLATION;

int AblationStation::NUM_NEURONS_FOR_ABLATION = -1;
int AblationStation::NUM_MUSCLES_FOR_ABLATION = -1;
int AblationStation::NUM_PARTIALS_FOR_ABLATION = -1;
int AblationStation::NUM_TYPES_FOR_ABLATION = -1;

AblationStation::AblationStation(){
    initDataArrays();
}



/*
 we simply go through our lists and check to see if it fits any of the ablation criteria.
 
 If it does, we set ablated = true, and return true.
 
 If it doesn't, we leave it alone, and return false. EIM default is ablated = false
*/
bool AblationStation::setAblationStatus(ElementInfoModule* eim){
    int i;
    for (i = 0; i < NUM_NEURONS_FOR_ABLATION; ++i){
        if (NEURONS_FOR_ABLATION[i] == eim->name){
            eim->ablated = true;
            return true;
        }
    }
    for (i = 0; i < NUM_MUSCLES_FOR_ABLATION; ++i){
        if (MUSCLES_FOR_ABLATION[i] == eim->name){
            eim->ablated = true;
            return true;
        }
    }
    for (i = 0; i < NUM_PARTIALS_FOR_ABLATION; ++i){
        std::string wtf(PARTIALS_FOR_ABLATION[i]);
        size_t found = eim->name.find(wtf);
        size_t huh = std::string::npos;
        if (found != std::string::npos){
            eim->ablated = true;
            return true;
        }
        
    }
    for (i = 0; i < NUM_TYPES_FOR_ABLATION; ++i){
        if (TYPES_FOR_ABLATION[i] == eim->getEType()){
            eim->ablated = true;
            return true;
        }
    }
    return false;
}



/* this is written like this just so the things to ablate can be added and removed easily, without the need to count or update a count or anyting like that...
 */
void AblationStation::initDataArrays(){
    // exact neuron matches
    std::string NEURONS_FOR_ABLATION[] = {};
    AblationStation::NUM_NEURONS_FOR_ABLATION = sizeof(NEURONS_FOR_ABLATION)/sizeof(*NEURONS_FOR_ABLATION);
    memcpy(AblationStation::NEURONS_FOR_ABLATION, NEURONS_FOR_ABLATION, NUM_NEURONS_FOR_ABLATION*sizeof(std::string));
    

    // exact muscle matches
    std::string MUSCLES_FOR_ABLATION[] = {};
    AblationStation::NUM_MUSCLES_FOR_ABLATION = sizeof(MUSCLES_FOR_ABLATION)/sizeof(*MUSCLES_FOR_ABLATION);
    memcpy(AblationStation::MUSCLES_FOR_ABLATION, MUSCLES_FOR_ABLATION, NUM_MUSCLES_FOR_ABLATION*sizeof(std::string));

    // partial matches -- see bottom of header file for explanation
    std::string PARTIALS_FOR_ABLATION[] = {
        "adp",
        "ail",
        "aob",
        "aol",
        "bm",
        "cdl",
        "dgl",
        "dsp",
        "dsr",
        "e",
        "g",
        "gec",
        "grt",
        "hyp",
        "int",
        "mc",
        "Phsh",
        "Phso",
        "pil",
        "pm",
        "pob",
        "pol",
        "rect",
        "sph",
        "vm",
        "vsp",
        "vsr",
        "um",/*added due to observation (there were some um's in the muscle list... we dont' want those right now)*/
        "anal" /* added due to observation */
    };
    AblationStation::NUM_PARTIALS_FOR_ABLATION = sizeof(PARTIALS_FOR_ABLATION)/sizeof(*PARTIALS_FOR_ABLATION);
    AblationStation::PARTIALS_FOR_ABLATION = new std::string[NUM_PARTIALS_FOR_ABLATION];
    memcpy(AblationStation::PARTIALS_FOR_ABLATION,PARTIALS_FOR_ABLATION,NUM_PARTIALS_FOR_ABLATION*sizeof(std::string));

    // types -- muscle, motor, sensory, inter,...
    ElementType TYPES_FOR_ABLATION[] = {};
    AblationStation::NUM_TYPES_FOR_ABLATION = sizeof(TYPES_FOR_ABLATION)/sizeof(*TYPES_FOR_ABLATION);
    memcpy(AblationStation::TYPES_FOR_ABLATION,TYPES_FOR_ABLATION,NUM_TYPES_FOR_ABLATION*sizeof(ElementType));
}


