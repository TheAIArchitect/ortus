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
    relationAttributeMap.resize(Ort::NUM_RELATION_ATTRIBUTES);
    attributeTracker.resize(Ort::NUM_RELATION_ATTRIBUTES);
    // should be a fast way to ensure all float* point to something valid
    // from SO, seems like gcc unrolls the loop a bit
    std::fill(relationAttributeMap.begin(), relationAttributeMap.end(),&ZEROF);
    std::fill(attributeTracker.begin(), attributeTracker.end(), false);
};

ElementRelation::~ElementRelation(){
    //for (int i = 0; i < Ort::NUM_ATTRIBUTES; ++i){
        //if (attributeTracker[i]){
            //delete relationAttributeMap[i];
        //}
    //}
    if (freeWeights){
        delete csWeight;
        delete gjWeight;
    }
}


float ElementRelation::getAttribute(RelationAttribute rAttribute){
    return *relationAttributeMap[static_cast<int>(rAttribute)];
}

void ElementRelation::setCSWeight(float weight){
    *csWeight[0] = weight;
}

void ElementRelation::setGJWeight(float weight){
    *gjWeight[0] = weight;
}

float ElementRelation::getCSWeight(int fromTimestepsAgo){
    if (Ort::WEIGHT_HISTORY_SIZE <= fromTimestepsAgo){
        printf("(ElementRelation) Error: trying to access CS Weight from '%d' timesteps ago, when limit is '%d'.\n", fromTimestepsAgo, Ort::WEIGHT_HISTORY_SIZE-1);
        exit(18);
    }
    return *csWeight[fromTimestepsAgo];
}

float ElementRelation::getGJWeight(int fromTimestepsAgo){
    if (Ort::WEIGHT_HISTORY_SIZE <= fromTimestepsAgo){
        printf("(ElementRelation) Error: trying to access GJ Weight from '%d' timesteps ago, when limit is '%d'.\n", fromTimestepsAgo, Ort::WEIGHT_HISTORY_SIZE-1);
        exit(18);
    }
    return *gjWeight[fromTimestepsAgo];
}

/** NOTE: this must only be called *afteR* calling 'setDataPointers()' */

void ElementRelation::setAttribute(RelationAttribute rAttribute, float value){
    corruptionCheck("Pre set");
    *relationAttributeMap[static_cast<int>(rAttribute)] = value;
    // below (attributeTracker) might be totally unnecessary.
    //attributeTracker[static_cast<int>(rAttribute)] = true;
    printf("(set attribute) %s->%s -- attribute #%d: %f\n",preName.c_str(),postName.c_str(),static_cast<int>(rAttribute),*relationAttributeMap[static_cast<int>(rAttribute)]);
    corruptionCheck("Post set");
}

/** Note: for this to work, preId and postId must be set...
 *
 * The effect of setDataPointers() is:
 *   # if there is a Blade in attributeBladeMap for a given attribute in relationAttributeMap,
 *      the address of &ZEROF will be replaced with the appropriate address to access
 *      **THIS** relation's value, based upon the preId and postID,
 *      for any given attribute.
 *      That is, relationAttributeMap becomes a map of pointers that all point to the position 
 *      in each attribute's relative Blade, which is where the data actually resides.
 *   # if there is no Blade (that would be quite odd...), then the address remains
 */
void ElementRelation::setAttributeDataPointers(std::unordered_map<RelationAttribute, Blade<cl_float>*>& relationAttributeBladeMap){
    for (auto entry : relationAttributeBladeMap){
        // entry.second is a Blade*
        if (Ort::STORE_RELATIONS_TRANSPOSED){ // SWAPS preId and PostId
            relationAttributeMap[static_cast<int>(entry.first)] = entry.second->getp(postId, preId);
            //printf("(set data pointer -- transposed) %s (id: %d) ->%s (id: %d)-- attribute #%d: %f\n",preName.c_str(), preId, postName.c_str(), postId, static_cast<int>(entry.first),relationAttributeMap[static_cast<int>(entry.first)]);
        }
        else {
            relationAttributeMap[static_cast<int>(entry.first)] = entry.second->getp(preId, postId);
        //attributeTracker[static_cast<int>(entry.first)] = true;
            //printf("(set data pointer -- transposed) %s (id: %d) ->%s (id: %d)-- attribute #%d: %f\n",preName.c_str(), preId, postName.c_str(), postId, static_cast<int>(entry.first),relationAttributeMap[static_cast<int>(entry.first)]);
        }
    }
}

/**
 * Sets the pointers for cs and gj weight blades -- not only the current weights, but previous ones as well (as many as the blades have)
 */
void ElementRelation::setWeightDataPointers(Blade<cl_float>* csWeightBlade, Blade<cl_float>* gjWeightBlade){
    csWeight = new cl_float*[Ort::WEIGHT_HISTORY_SIZE];
    gjWeight = new cl_float*[Ort::WEIGHT_HISTORY_SIZE];
    freeWeights = true;
    int i = 0;
    for (i = 0; i < Ort::WEIGHT_HISTORY_SIZE; ++i){
        if (Ort::STORE_RELATIONS_TRANSPOSED){
            csWeight[i] = csWeightBlade->getp(postId, preId, i); // 0 is the current weight, 1 is current-1 weight, 2 is current-2 weight, etc..
            gjWeight[i] = gjWeightBlade->getp(postId, preId, i); // same for gj
        }
        else {
            csWeight[i] = csWeightBlade->getp(preId, postId, i); // 0 is the current weight, 1 is current-1 weight, 2 is current-2 weight, etc..
            gjWeight[i] = gjWeightBlade->getp(preId, postId, i); // same for gj
        }
    }
}

/**
 * if a value is set in the relationAttributeMap,
 * and a pointer to a location in a Blade hasn't been set,
 * it will change the value of ZEROF from 0.f to something else.
 * (all values in the relationAttributeMap are set to point to ZEROF,
 * so if an attribute is set before reassigning the pointer, 
 * ZEROF will change)
 *
 * this function will exit if that has happened, 
 * because that means that the state of the program is totally unknown.
 */
void ElementRelation::corruptionCheck(std::string location = "ElementRelation"){
    //printf("(%s) -- corruption check ", location.c_str());
    if (ZEROF != 0.f){
        printf("(ElementRelation) Error: Corruption check failed!\n");
        //printf("failed.\n");
        exit(6);
    }
    //printf("OK.\n");
}

std::string ElementRelation::toString(){
    int max = 512;
    char buffer[max];
    switch (rtype) {
        case CORRELATED:
            snprintf(buffer, max,"<CORRELATED> (%d -> %d) (%s->%s,  weight (cs, gj): (%.3f, %.3f), age: %f)",pre->id, post->id, preName.c_str(),postName.c_str(), *csWeight[0], *gjWeight[0],
                     getAttribute(RelationAttribute::Age));
            break;
        case CAUSES: {
            snprintf(buffer, max,"<CAUSES> (%d -> %d) (%s->%s, weight (cs, gj): (%f, %f), polarity: %.2f, age: %f, thresh: %f)",pre->id, post->id, preName.c_str(),postName.c_str(), *csWeight[0], *gjWeight[0],
                     getAttribute(RelationAttribute::Polarity),
                    getAttribute(RelationAttribute::Age),
                     getAttribute(RelationAttribute::Thresh));
            break;
        }
        case DOMINATES:
            snprintf(buffer, max,"<DOMINATES> (%d -> %d) (%s->%s)", pre->id, post->id, preName.c_str(),postName.c_str());
            break;
        case OPPOSES:
            snprintf(buffer, max,"<OPPOSES> (%d -> %d) (%s->%s, thresh: %f)", pre->id, post->id,preName.c_str(),postName.c_str(), getAttribute(RelationAttribute::Thresh));
            break;
        default:
            snprintf(buffer,max,"-- ERROR -- CAN'T PRINT ELEMENT RELATION TYPE '%d'",rtype);
            break;
    }
    return std::string(buffer);
}
