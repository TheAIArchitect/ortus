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
#include "OrtusStd.hpp"
#include <unordered_map>
#include <Blade.hpp>


//enum ElementRelationType {GAP, CHEM};
// idea is: pre <ElementRelationType> post, e.g., pre CAUSES post
// if CORRELATES_WITH or CAUSES, 'Direction' must be specified (-1 or 1)
// note, "ERT" <==> "ElementRelationType"
enum ElementRelationType {CORRELATED, CAUSES, DOMINATES, OPPOSES, NONE};

class DataSteward;

class ElementRelation {
    
public:
    ElementRelation();
    ~ElementRelation();
    static float ZEROF;
    static void corruptionCheck(std::string location);
    
    
    
    
    bool freeWeights;
    void setAttributeDataPointers(std::unordered_map<RelationAttribute, Blade<cl_float>*>& relationAttributeBladeMap);
    void setWeightDataPointers(Blade<cl_float>* csWeightBlade, Blade<cl_float>* gjWeightBlade);
    
    
    void setCSWeight(float weight);
    void setGJWeight(float weight);
    float getCSWeight(int fromTimestepsAgo);
    float getGJWeight(int fromTimestepsAgo);
    float getAttribute(RelationAttribute rAttribute);
    void setAttribute(RelationAttribute rAttribute, float value);
    
    
    ElementInfoModule* pre;
    std::string preName;
    int preId;
    ElementInfoModule* post;
    std::string postName;
    int postId;
    ElementRelationType rtype;
    std::string sType;
    
    
    std::string toString();

    
    std::string weightLabel;
    bool marked = false; // used for search
    int path_len = -1; // used for search
    glm::vec3 midpoint; // used to for the label's location
    
private:
    // this is actually a vector!
    ortus::attribute_map relationAttributeMap;
    // NOTE: CS, GJ Weights are **NOT** 'RelationAttributes'
    cl_float** csWeight;
    cl_float** gjWeight;
    
    // probably acts like a bitvector!
    std::vector<bool> attributeTracker; // keeps track of floats that have been 'newed'
    
};

#endif /* ElementRelation_hpp */
