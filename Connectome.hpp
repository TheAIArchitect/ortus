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

class Connectome {
    
public:
    Connectome(std::string ortFileName);
    void buildAdditionalDataStructures();
    
     // primary element pointer holder -- the index in this vector is the element's 'id'
    std::vector<ElementInfoModule*> elementModules;
    std::vector<ElementRelation*> elementRelations; // primary relation pointer holder
    ortus::element_map elementMap; // name to element pointer
    
    
    ortus::name_map nameMap; // name to index
    ortus::index_map indexMap; // index to name
   
    
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
