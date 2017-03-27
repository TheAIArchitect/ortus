//
//  ElementRelation.cpp
//  delegans
//
//  Created by onyx on 10/13/16.
//  Copyright © 2016 Andrew W.E. McDonald. All rights reserved.
//

#include "ElementRelation.hpp"


ElementRelation::ElementRelation(){
    attributeMap.reserve(NUM_ATTRIBUTES);
    attributeTracker.reserve(NUM_ATTRIBUTES);
    // should be a fast way to ensure all float* point to something valid
    // from SO, seems like gcc unrolls the loop a bit
    std::fill(attributeMap.begin(), attributeMap.end(),&ZEROF);
};

ElementRelation::~ElementRelation(){
    for (int i = 0; i < NUM_ATTRIBUTES; ++i){
        if (attributeTracker[i]){
            delete attributeMap[i];
        }
    }
}


float ElementRelation::getAttribute(Attribute attribute){
    return *attributeMap[static_cast<int>(attribute)];
}

void ElementRelation::setAttribute(Attribute attribute, float value){
    attributeMap[static_cast<int>(attribute)] = new float(value);
    // with the blades, this will still be used, but will be used to detect what things have been set/are being used
    attributeTracker[static_cast<int>(attribute)] = true;
}

std::string ElementRelation::toString(){
    int max = 512;
    char buffer[max];
    switch (type) {
        case CORRELATED:
            snprintf(buffer, max,"<CORRELATED> (%s->%s, weight: %.2f, age: %f)",preName.c_str(),postName.c_str(),
                     getAttribute(Attribute::Weight),
                     getAttribute(Attribute::Age));
            break;
        case CAUSES:
            snprintf(buffer, max,"<CAUSES> (%s->%s, weight: %.2f, polarity: %.2f, age: %f, thresh: %f)",preName.c_str(),postName.c_str(),getAttribute(Attribute::Weight),
                     getAttribute(Attribute::Polarity),
                    getAttribute(Attribute::Age),
                     getAttribute(Attribute::Thresh));
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
