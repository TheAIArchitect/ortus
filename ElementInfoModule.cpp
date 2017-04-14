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
    isSEI = false;
    isEEI = false;
    isSCI = false;
};

ElementInfoModule::~ElementInfoModule(){};



void ElementInfoModule::setAttributeDataPointers(std::unordered_map<ElementAttribute, Blade<cl_float>*>& elementAttributeBladeMap){
    typep = elementAttributeBladeMap[ElementAttribute::Type]->getp(id); // types correspond to ElementType enum class
    *typep = -1.f; // easy way to tell if a type has been set
    affectp = elementAttributeBladeMap[ElementAttribute::Affect]->getp(id); // types correspond to ElementAffect enum class
    *affectp = (float) ElementAffect::NEUTRAL; // 0, at least, when this was written it was...
}

/**
 * sets the pointers for the activation history (index 0 is the current activation,
 * and indices 1+ are previous activations, such that [1] is current time - 1,
 * [2] is current time - 2, and so on).
 */
void ElementInfoModule::setActivationDataPointer(Blade<cl_float>* activationBladep){
    activationp = new cl_float*[Ort::ACTIVATION_HISTORY_SIZE];
    int i = 0;
    for (i = 0; i < Ort::ACTIVATION_HISTORY_SIZE; ++i){
        activationp[i] = activationBladep->getp(i, id); // activation blade holds the current activation at row 0, historic activations in rows 1+
        *activationp = 0; // default to 0
    }
}

/** don't ever, for any reason, do anything, to this function, for any reason, ever,
 * no matter what, no matter where, or who, or who you are with or where you are going,
 * before you have called 'setActivationDataPointer' (or, setAttributeDataPointers, for that matter)
 *
 * this function sets the activation level of this element (in the Blade) 
 *
 * NOTE: this sets the activation for the 'current' window (so, index 0);
 */
void ElementInfoModule::setActivation(float fActivation){
    *activationp[0] = fActivation;
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

ElementAffect ElementInfoModule::getEAffect(){
    return eAffect;
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
float ElementInfoModule::vCurr(int fromTimestepsAgo){
 //   return *activation;
    if (Ort::ACTIVATION_HISTORY_SIZE <= fromTimestepsAgo){
        printf("(ElementInfoModule) Error: trying to access activation from '%d' timesteps ago, when limit is '%d'.\n", fromTimestepsAgo, Ort::ACTIVATION_HISTORY_SIZE-1);
        exit(18);
    }
    return *activationp[fromTimestepsAgo*Ort::MAX_ELEMENTS];// anything other than 0 might not be good... check this.
}



/**
 * Sets the affect of the element; input may be 'pos', or 'neg'.
 * NOTE: must be set *AFTER* setting typep to point to the right address (via setDataPointers())
 */
void ElementInfoModule::setAffect(std::string affect){
    sAffect = affect;
    if (affect == "pos"){
        *affectp = 1.f;
        eAffect = ElementAffect::POS;
    }
    else if (affect == "neg"){
        *affectp = -1.f;
        eAffect = ElementAffect::NEG;
    }
    else if (affect == "neutral"){
        *affectp = 0.f;
        eAffect = ElementAffect::NEUTRAL;
    }
    else {
        printf("Error: attempting to set non-recognized affect '%s'.\n", affect.c_str());
        exit(58);
    }
}


/**
 * Sets the affect of the element; input may be 'pos', or 'neg'.
 * NOTE: must be set *AFTER* setting typep to point to the right address (via setDataPointers())
 */
void ElementInfoModule::setAffect(ElementAffect eAffect){
    this->eAffect = eAffect;
    switch (eAffect){
        case ElementAffect::NEG:{
           sAffect = "neg";
           break;
        }
        case ElementAffect::NEUTRAL:{
           sAffect = "neutral";
           break;
        }
        case ElementAffect::POS:{
            sAffect = "pos";
            break;
        }
        default:
            printf("Error: invalid ElementAffect enum value.\n");
            exit(58);
    }
    *affectp = static_cast<cl_float>(eAffect);
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

/*
 * Sets the ElementType enum, the string type (stype), and the float type, (ftype), of the element
 * NOTE: must be set *AFTER* setting typep to point to the right address (via setDataPointers())
 */
void ElementInfoModule::setType(ElementType type){
    eType = type;
    switch (type){
        case ElementType::SENSE:
            sType = "sense";
            break;
        case ElementType::EMOTION:
            sType = "emotion";
            break;
        case ElementType::INTER:
            sType = "inter";
            break;
        case ElementType::MOTOR:
            sType = "motor";
            break;
        case ElementType::MUSCLE:
            sType = "muscle";
            break;
        default:
            printf("Error: invalid ElementAffect enum value.\n");
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
