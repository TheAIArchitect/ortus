//
//  ElementRelation.cpp
//  delegans
//
//  Created by onyx on 10/13/16.
//  Copyright © 2016 Andrew W.E. McDonald. All rights reserved.
//

#include "ElementRelation.hpp"

#include "DataSteward.hpp"

float ElementRelation::ZEROF = 0.0f;

ElementRelation::ElementRelation(){
    relationAttributeMap.reserve(ortus::NUM_ELEMENT_ATTRIBUTES);
    attributeTracker.reserve(ortus::NUM_ELEMENT_ATTRIBUTES);
    // should be a fast way to ensure all float* point to something valid
    // from SO, seems like gcc unrolls the loop a bit
    std::fill(attributeMap.begin(), attributeMap.end(),&ZEROF);
};

ElementRelation::~ElementRelation(){
    //for (int i = 0; i < ortus::NUM_ATTRIBUTES; ++i){
        //if (attributeTracker[i]){
            //delete attributeMap[i];
        //}
    //}
}


float ElementRelation::getAttribute(RelationAttribute rAttribute){
    return *attributeMap[static_cast<int>(rAttribute)];
}

/** NOTE: this must only be called *afteR* calling 'setDataPointers()' */

void ElementRelation::setAttribute(RelationAttribute rAttribute, float value){
    attributeMap[static_cast<int>(rAttribute)] = value;
    // below might be totally unnecessary.
    attributeTracker[static_cast<int>(rAttribute)] = true;
}

/** Note: for this to work, preId and postId must be set...
 *
 * The effect of setDataPointers() is:
 *   # if there is a Blade in attributeBladeMap for a given attribute in attributeMap,
 *      the address of &ZEROF will be replaced with the appropriate address to access
 *      **THIS** relation's value, based upon the preId and postID,
 *      for any given attribute.
 *      That is, attributeMap becomes a map of pointers that all point to the position 
 *      in each attribute's relative Blade, which is where the data actually resides.
 *   # if there is no Blade (that would be quite odd...), then the address remains
 */
void ElementRelation::setDataPointers(std::unordered_map<RelationAttribute, Blade<cl_float>*> relationAttributeBladeMap){
    for (auto entry : relationAttributeBladeMap){
        // entry.second is a Blade*
        attributeMap[static_cast<int>(entry.first)] = entry.second->getp(preId, postId);
    }
}

/**
 * if a value is set in the attributeMap,
 * and a pointer to a location in a Blade hasn't been set,
 * it will change the value of ZEROF from 0.f to something else.
 * (all values in the attributeMap are set to point to ZEROF,
 * so if an attribute is set before reassigning the pointer, 
 * ZEROF will change)
 *
 * this function will exit if that has happened, 
 * because that means that the state of the program is totally unknown.
 */
void ElementRelation::corruptionCheck(){
    if (ZEROF != 0.f){
        exit(6);
    }
}

std::string ElementRelation::toString(){
    int max = 512;
    char buffer[max];
    switch (rtype) {
        case CORRELATED:
            snprintf(buffer, max,"<CORRELATED> (%s->%s, weight: %.2f, age: %f)",preName.c_str(),postName.c_str(),
                     getAttribute(RelationAttribute::Weight),
                     getAttribute(RelationAttribute::Age));
            break;
        case CAUSES:
            snprintf(buffer, max,"<CAUSES> (%s->%s, weight: %.2f, polarity: %.2f, age: %f, thresh: %f)",preName.c_str(),postName.c_str(),getAttribute(RelationAttribute::Weight),
                     getAttribute(RelationAttribute::Polarity),
                    getAttribute(RelationAttribute::Age),
                     getAttribute(RelationAttribute::Thresh));
            break;
        case DOMINATES:
            snprintf(buffer, max,"<DOMINATES> (%s->%s)",preName.c_str(),postName.c_str());
            break;
        case OPPOSES:
            snprintf(buffer, max,"<OPPOSES> (%s->%s, thresh: %f)",preName.c_str(),postName.c_str(), getAttribute(RelationAttribute::Thresh));
            break;
        default:
            snprintf(buffer,max,"-- ERROR -- CAN'T PRINT ELEMENT RELATION TYPE '%d'",rtype);
            break;
    }
    return std::string(buffer);
}
