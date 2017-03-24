//
//  NeuronInfoModule.cpp
//  LearningOpenGL
//
//  Created by onyx on 12/1/15.
//  Copyright © 2016 Andrew W.E. McDonald. All rights reserved.
//

#include "ElementInfoModule.hpp"

ElementInfoModule::ElementInfoModule(){
};

ElementInfoModule::~ElementInfoModule(){};

ElementType ElementInfoModule::getEType(){
    return eType;
}

std::string ElementInfoModule::getSType(){
    return sType;
}

float ElementInfoModule::getFType(){
    return fType;
}

std::string ElementInfoModule::getName(){
    //return *namep;
    return name;
}

int ElementInfoModule::getId(){
    //return *idp;
    return id;
}

// this needs to change to call it from the blade.
// should also have another function that allows calling for a specific time window,
// that would come from the voltage history... wherever that's being kept.
float ElementInfoModule::vCurr(){
    return *vCurrp;
}

/**
 * Sets the affect of the element; input may be 'pos', or 'neg'.
 */
void ElementInfoModule::setAffect(std::string affect){
    sAffect = affect;
    if (affect == "pos"){
        fAffect= 1.f;
    }
    else if (affect == "neg"){
        fAffect= -1.f;
    }
    else {
        printf("Error: attempting to set non-recognized affect '%s'.\n", affect.c_str());
        exit(58);
    }
}

/*
 * Sets the ElementType enum, the string type (stype), and the float type, (ftype), of the element
 */
void ElementInfoModule::setType(std::string type){
    sType = type;
    if (sType == "sense"){
        eType = SENSE;
        fType = 0.f;
    }
    else if (sType == "emotion"){
        eType = EMOTION;
        fType = 1.f;
    }
    else if (sType == "inter"){
        eType = INTER;
        fType = 2.f;
    }
    else if (sType == "motor"){
        eType = MOTOR;
        fType = 3.f;
    }
    else if (sType == "muscle"){
        eType = MUSCLE;
        fType = 4.f;
    }
    else {
        printf("Error: attempting to set non-recognized element type '%s'.\n", type.c_str());
        exit(58);
    }
    return;
}


std::string ElementInfoModule::toString(){
    int max = 512;
    char buffer[max];
    switch (eType) {
        case SENSE:
            snprintf(buffer, max,"<SENSE> (%s, affect: %s)",name.c_str(),sAffect.c_str());
            break;
        case EMOTION:
            snprintf(buffer, max,"<EMOTION> (%s, affect: %s)",name.c_str(),sAffect.c_str());
            break;
        case INTER:
            snprintf(buffer, max,"<INTER> (%s, affect: %s)",name.c_str(),sAffect.c_str());
            break;
        case MOTOR:
            snprintf(buffer, max,"<MOTOR> (%s, affect: %s)",name.c_str(),sAffect.c_str());
            break;
        case MUSCLE:
            snprintf(buffer, max,"<MUSCLE> (%s, affect: %s)",name.c_str(),sAffect.c_str());
            break;
        default:
            snprintf(buffer,max,"-- ERROR -- CAN'T PRINT ELEMENT TYPE '%d'",eType);
            break;
    }
    return std::string(buffer);
}
