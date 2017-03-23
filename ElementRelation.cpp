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
    snprintf(buffer, max,"(%s->%s : %.2f : %.2f)",preName.c_str(),postName.c_str(),weight(),polarity());
    return std::string(buffer);
}
