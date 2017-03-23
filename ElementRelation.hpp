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

//enum ElementRelationType {GAP, CHEM};
// idea is: pre <ElementRelationType> post, e.g., pre CAUSES post
// if CORRELATES_WITH or CAUSES, 'Direction' must be specified (-1 or 1)
enum ElementRelationType {CORRELATES_WITH, CAUSES, DOMINATES, OPPOSES};

class ElementInfoModule;
    

/* GABA is a primary inhibitory NT
 * Glutamate is a primary excitatory NT
 * Dopamine is responsible for reinforcement/"wanting"
 *      - perhaps we can just do 1 inhibitory and 1 excitatory for now
 *  Not clear on differences between NTs like glutamate and dopamine
 *      - specifically, what one does that the other can't, and why they aren't interchangable (e.g., replace dopamine receptors with glutamate ones, assuming both groups excitatory)
 */
    
class ElementRelation {
    
public:
    ElementRelation();
    float polarity();
    float weight();
    static float ZEROF;
    
    ElementInfoModule* pre;
    std::string preName; // leave it for this for now
    ElementInfoModule* post;
    std::string postName;
    ElementRelationType type;
    std::string sType;
    float* polarityp; // perhaps we can 'model' different neurotransmitters by non-integer valued polarities?
    std::string toString();

    float* weightp;
    
    std::string weightLabel;
    bool marked = false; // used for search
    int path_len = -1; // used for search
    glm::vec3 midpoint; // used to for the label's location
    
    
    
};

#endif /* ElementRelation_hpp */
