//
//  ElementRelation.cpp
//  delegans
//
//  Created by onyx on 10/13/16.
//  Copyright © 2016 Andrew W.E. McDonald. All rights reserved.
//

#include "ElementRelation.hpp"

float ElementRelation::ZEROF = 0.f;

ElementRelation::ElementRelation(){
    polarityp = &ZEROF; // if it's a gap junction, nothing is assigned on creation.
    directionp = &ZEROF;
    thresh = "normal";
};


float ElementRelation::weight(){
    return *weightp;
}

float ElementRelation::polarity(){
    return *polarityp;
}

std::string ElementRelation::toString(){
    int max = 512;
    char buffer[max];
    switch (type) {
        case CORRELATED:
            snprintf(buffer, max,"<CORRELATED> (%s->%s, weight: %.2f)",preName.c_str(),postName.c_str(),weight());
            break;
        case CAUSES:
            snprintf(buffer, max,"<CAUSES> (%s->%s, weight: %.2f, polarity: %.2f, thresh: %s)",preName.c_str(),postName.c_str(),weight(),polarity());
            break;
        case DOMINATES:
            snprintf(buffer, max,"<DOMINATES> (%s->%s)",preName.c_str(),postName.c_str());
            break;
        case OPPOSES:
            snprintf(buffer, max,"<OPPOSES> (%s->%s, thresh: %s)",preName.c_str(),postName.c_str(), thresh.c_str());
            break;
        default:
            snprintf(buffer,max,"-- ERROR -- CAN'T PRINT ELEMENT RELATION TYPE '%d'",type);
            break;
    }
    return std::string(buffer);
}
