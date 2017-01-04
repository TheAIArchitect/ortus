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
    return element_type;
}

std::string ElementInfoModule::getSType(){
    return stype;
}

float ElementInfoModule::getFType(){
    return ftype;
}

std::string ElementInfoModule::name(){
    return *namep;
}

int ElementInfoModule::id(){
    return *idp;
}

float ElementInfoModule::vCurr(){
    return *vCurrp;
}

/*
 * Sets the ElementType enum, the string type (stype), and the float type, (ftype), of the element
 */
void ElementInfoModule::setElementType(ElementType etype){
    element_type = etype;
    switch(element_type){
        case SENSORY:
            stype = "sensory";
            ftype = 0.f;
            break;
        case INTER:
            stype = "inter";
            ftype = 1.f;
            break;
        case MOTOR:
            stype = "motor";
            ftype = 2.f;
            break;
        default:
            stype = "other";
            ftype = -1.f;
            break;
    }
    return;
}
