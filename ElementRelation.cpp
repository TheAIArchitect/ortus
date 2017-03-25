//
//  ElementRelation.cpp
//  delegans
//
//  Created by onyx on 10/13/16.
//  Copyright © 2016 Andrew W.E. McDonald. All rights reserved.
//

#include "ElementRelation.hpp"


ElementRelation::ElementRelation(){
    polarity = 0.f;
    fDirection = 0.f;
    fAge = 0.f;
    sAge = "";
    sThresh = "normal";
    fThresh = 0.f;
};


float ElementRelation::getWeight(){
    return weight;
}

float ElementRelation::getPolarity(){
    return polarity;
}

void ElementRelation::setAge(std::string age){
    sAge = age;
    //fAge =
}

void ElementRelation::setThresh(std::string thresh){
    sThresh = thresh;
    //fThresh =
}

std::string ElementRelation::toString(){
    int max = 512;
    char buffer[max];
    switch (type) {
        case CORRELATED:
            snprintf(buffer, max,"<CORRELATED> (%s->%s, weight: %.2f, age: %f)",preName.c_str(),postName.c_str(),weight, fAge);
            break;
        case CAUSES:
            snprintf(buffer, max,"<CAUSES> (%s->%s, weight: %.2f, polarity: %.2f, age: %f, thresh: %s)",preName.c_str(),postName.c_str(),weight,polarity, fAge, sThresh.c_str());
            break;
        case DOMINATES:
            snprintf(buffer, max,"<DOMINATES> (%s->%s)",preName.c_str(),postName.c_str());
            break;
        case OPPOSES:
            snprintf(buffer, max,"<OPPOSES> (%s->%s, thresh: %s)",preName.c_str(),postName.c_str(), sThresh.c_str());
            break;
        default:
            snprintf(buffer,max,"-- ERROR -- CAN'T PRINT ELEMENT RELATION TYPE '%d'",type);
            break;
    }
    return std::string(buffer);
}
