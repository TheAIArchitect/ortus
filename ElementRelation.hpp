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
    float getPolarity();
    float getWeight();
    void setAge(std::string sAge);
    void setThresh(std::string thresh);
    static float ZEROF;
    
    ElementInfoModule* pre;
    std::string preName;
    int preId;
    ElementInfoModule* post;
    std::string postName;
    int postId;
    ElementRelationType type;
    std::string sType;
    
    float polarity;
    // used for CAUSES and CORRELATED ERT
    std::string sDirection;
    float fDirection;
    
    std::string sAge;
    float fAge;
    // need measure of habituation... here??
    
    std::string toString();

    float weight;
    
    // right now, high or low, needs to change...
    std::string sThresh;
    float fThresh;
    
    std::string weightLabel;
    bool marked = false; // used for search
    int path_len = -1; // used for search
    glm::vec3 midpoint; // used to for the label's location
    
    
    
};

#endif /* ElementRelation_hpp */
