//
//  ElementRelation.hpp
//  delegans
//
//  Created by onyx on 10/13/16.
//  Copyright © 2016 Andrew W.E. McDonald. All rights reserved.
//

#ifndef ElementRelation_hpp
#define ElementRelation_hpp

#include <stdio.h>
#include <string>
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "ElementInfoModule.hpp"

//enum ElementRelationType {GAP, CHEM};
// idea is: pre <ElementRelationType> post, e.g., pre CAUSES post
// if CORRELATES_WITH or CAUSES, 'Direction' must be specified (-1 or 1)
// note, "ERT" <==> "ElementRelationType"
enum ElementRelationType {CORRELATED, CAUSES, DOMINATES, OPPOSES, NONE};


class ElementRelation {
    
public:
    ElementRelation();
    float polarity();
    float weight();
    static float ZEROF;
    
    ElementInfoModule* pre;
    std::string preName;
    ElementInfoModule* post;
    std::string postName;
    ElementRelationType type;
    std::string sType;
    
    float* polarityp;
    float* directionp; // used for CAUSES ERT
    float* agep;
    // need measure of habituation, but not here, i don't think.
    
    std::string toString();

    float* weightp;
    
    // right now, high or low, needs to change...
    std::string thresh;
    
    std::string weightLabel;
    bool marked = false; // used for search
    int path_len = -1; // used for search
    glm::vec3 midpoint; // used to for the label's location
    
    
    
};

#endif /* ElementRelation_hpp */
