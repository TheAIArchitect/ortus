//
//  NeuronInfoModule.cpp
//  LearningOpenGL
//
//  Created by onyx on 12/1/15.
//  Copyright © 2016 Andrew W.E. McDonald. All rights reserved.
//

#include "ElementInfoModule.hpp"
#include "DataSteward.hpp"

ElementInfoModule::ElementInfoModule(){
};

ElementInfoModule::~ElementInfoModule(){};



void ElementInfoModule::setAttributeDataPointers(DataSteward* dsp){
    typep = dsp->attributeBladeMap[Attribute::RType]->getp(id); // types correspond to ElementType enum class
    affectp = dsp->attributeBladeMap[Attribute::Affect]->getp(id); // types correspond to ElementAffect enum class
}

void ElementInfoModule::setActivationDataPointer(DataSteward* dsp){
    activationp = dsp->activationBlade->getp(id);
}

ElementType ElementInfoModule::getEType(){
    return eType;
}

std::string ElementInfoModule::getSType(){
    return sType;
}

float ElementInfoModule::getFType(){
    return *typep;
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
 //   return *activation;
    return *activationp;
}

/**
 * Sets the affect of the element; input may be 'pos', or 'neg'.
 * NOTE: must be set *AFTER* setting typep to point to the right address (via setDataPointers())
 */
void ElementInfoModule::setAffect(std::string affect){
    sAffect = affect;
    if (affect == "pos"){
        *affectp = 1.f;
    }
    else if (affect == "neg"){
        *affectp = -1.f;
    }
    else {
        printf("Error: attempting to set non-recognized affect '%s'.\n", affect.c_str());
        exit(58);
    }
}

/*
 * Sets the ElementType enum, the string type (stype), and the float type, (ftype), of the element
 * NOTE: must be set *AFTER* setting typep to point to the right address (via setDataPointers())
 */
void ElementInfoModule::setType(std::string type){
    sType = type;
    if (sType == "sense"){
        eType = ElementType::SENSE;
    }
    else if (sType == "emotion"){
        eType = ElementType::EMOTION;
    }
    else if (sType == "inter"){
        eType = ElementType::INTER;
    }
    else if (sType == "motor"){
        eType = ElementType::MOTOR;
    }
    else if (sType == "muscle"){
        eType = ElementType::MUSCLE;
    }
    else {
        printf("Error: attempting to set non-recognized element type '%s'.\n", type.c_str());
        exit(58);
    }
    *typep = (float) eType;
    return;
}


std::string ElementInfoModule::toString(){
    int max = 512;
    char buffer[max];
    switch (eType) {
        case ElementType::SENSE:
            snprintf(buffer, max,"<SENSE> (%s, affect: %s)",name.c_str(),sAffect.c_str());
            break;
        case ElementType::EMOTION:
            snprintf(buffer, max,"<EMOTION> (%s, affect: %s)",name.c_str(),sAffect.c_str());
            break;
        case ElementType::INTER:
            snprintf(buffer, max,"<INTER> (%s, affect: %s)",name.c_str(),sAffect.c_str());
            break;
        case ElementType::MOTOR:
            snprintf(buffer, max,"<MOTOR> (%s, affect: %s)",name.c_str(),sAffect.c_str());
            break;
        case ElementType::MUSCLE:
            snprintf(buffer, max,"<MUSCLE> (%s, affect: %s)",name.c_str(),sAffect.c_str());
            break;
        default:
            snprintf(buffer,max,"-- ERROR -- CAN'T PRINT ELEMENT TYPE '%d'",eType);
            break;
    }
    return std::string(buffer);
}
