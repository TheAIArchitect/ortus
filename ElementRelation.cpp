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
    attributeMap.reserve(ortus::NUM_ATTRIBUTES);
    attributeTracker.reserve(ortus::NUM_ATTRIBUTES);
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


float ElementRelation::getAttribute(Attribute attribute){
    return *attributeMap[static_cast<int>(attribute)];
}

/** NOTE: this must only be called *afteR* calling 'setDataPointers()' */
void ElementRelation::setAttribute(Attribute attribute, float value){
    attributeMap[static_cast<int>(attribute)] = new float(value);
    attributeTracker[static_cast<int>(attribute)] = true;
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
void ElementRelation::setDataPointers(DataSteward* dsp){
    for (auto blade : dsp->attributeBladeMap){
        attributeMap[static_cast<int>(blade.first)] = blade.second->getp(preId, postId);
    }
    
}

std::string ElementRelation::toString(){
    int max = 512;
    char buffer[max];
    switch (rtype) {
        case CORRELATED:
            snprintf(buffer, max,"<CORRELATED> (%s->%s, weight: %.2f, age: %f)",preName.c_str(),postName.c_str(),
                     getAttribute(Attribute::Weight),
                     getAttribute(Attribute::Age));
            break;
        case CAUSES:
            snprintf(buffer, max,"<CAUSES> (%s->%s, weight: %.2f, polarity: %.2f, age: %f, thresh: %f)",preName.c_str(),postName.c_str(),getAttribute(Attribute::Weight),
                     getAttribute(Attribute::Polarity),
                    getAttribute(Attribute::Age),
                     getAttribute(Attribute::RThresh));
            break;
        case DOMINATES:
            snprintf(buffer, max,"<DOMINATES> (%s->%s)",preName.c_str(),postName.c_str());
            break;
        case OPPOSES:
            snprintf(buffer, max,"<OPPOSES> (%s->%s, thresh: %f)",preName.c_str(),postName.c_str(), getAttribute(Attribute::RThresh));
            break;
        default:
            snprintf(buffer,max,"-- ERROR -- CAN'T PRINT ELEMENT RELATION TYPE '%d'",rtype);
            break;
    }
    return std::string(buffer);
}
