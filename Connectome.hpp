//
//  Connectome.hpp
//  ortus
//
//  Created by andrew on 3/22/17.
//  Copyright © 2017 Andrew W.E. McDonald. All rights reserved.
//

#ifndef Connectome_hpp
#define Connectome_hpp

#include <stdio.h>
#include "ElementInfoModule.hpp"
#include "ElementRelation.hpp"
#include "OrtusNamespace.hpp"
#include "OrtUtil.hpp"
#include <vector>
#include <string>
#include "Attribute.hpp"



class Connectome {
    
public:
    Connectome(std::string ortFileName);
    void buildAdditionalDataStructures();
    ElementRelation* addRelation(ElementInfoModule* ePre, ElementInfoModule* ePost, ElementRelationType ert);
    
    void addRelationAttributesFromOrt(std::unordered_map<std::string,std::string>& preAttributeMapStrings, std::unordered_map<std::string,std::string>& postAttributeMapStrings, ElementRelation* elrel);
    
    void addRelationsFromOrt(std::vector<std::unordered_map<std::string, std::string>>& vecOfAttributeMaps, ElementRelationType ert);
    
    void addElement(std::unordered_map<std::string, std::string> attributeMap);
    
    friend void addToRelationMap(ortus::relation_map& remap, int preIndex, ElementRelation* elrel, ElementRelationType ert);
    
    void setElements(std::vector<std::string>& theLines);
    
     // primary element pointer holder -- the index in this vector is the element's 'id'
    std::vector<ElementInfoModule*> elementModules;
    std::vector<ElementRelation*> elementRelations; // primary relation pointer holder
    ortus::element_map elementMap; // name to element pointer
    
    
    ortus::name_map nameMap; // name to index
    ortus::index_map indexMap; // index to name... but it's acutally a vector. because after i named it, i realized it would be stupid for this to be a map, when a vector would do the same thing, more quickly.
   
    
    ortus::relation_map correlatedRelations;
    // for the below 3 maps, the key is the index of the 'pre' element,
    // and relationships are created such that "pre" <ElementRelationType> "post"
    // e.g., "pre" CAUSES "post" (along with
    ortus::relation_map causesRelations;
    ortus::relation_map dominatesRelations;
    ortus::relation_map opposesRelations;
    
    
    
    std::vector<std::string> odrVec;
    OrtUtil ortUtil;
};

#endif /* Connectome_hpp */
