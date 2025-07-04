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
#include "OrtusStd.hpp"
#include "OrtUtil.hpp"
#include <vector>
#include <string>
#include "StrUtils.hpp"

class DataSteward;

class Connectome {

public:
    DataSteward* dataStewardp;
    
public:
    Connectome(DataSteward* dataStewardp);
    void initialize(std::string ortFileName);
    void createElementsAndElementRelations();
    
    void buildAdditionalDataStructures();
    
    ElementRelation* addRelation(ElementInfoModule* ePre, ElementInfoModule* ePost, ElementRelationType ert);
    
    void addRelationAttributesFromOrt(std::unordered_map<std::string,std::string>& preAttributeMapStrings, std::unordered_map<std::string,std::string>& postAttributeMapStrings, ElementRelation* elrel);
    
    void addRelationsFromOrt(std::vector<std::unordered_map<std::string, std::string>>& vecOfAttributeMaps, ElementRelationType ert);
    
    void addElementFromOrt(std::unordered_map<std::string, std::string> attributeMap);
    
    friend void addToRelationMap(ortus::relation_map& remap, int preIndex, ElementRelation* elrel, ElementRelationType ert);
    
    void setElements();
    
    void cat();
    
    std::vector<std::string> theLines;
    
     // primary element pointer holder -- the index in this vector is the element's 'id'
    std::vector<ElementInfoModule*> elementModules;
    std::vector<ElementRelation*> elementRelations; // primary relation pointer holder
    ortus::element_map elementMap; // name to element pointer
    
    // actual sensory neurons -- to be kept separate from seiElements, which extend each sensory neuron, and are interneurons
    std::vector<ElementInfoModule*> primarySensoryElements;
    // and the seiElements
    std::vector<ElementInfoModule*> seiElements;
    
    // index 0 is the 'primary', so, for 'fear', FEAR_0, but the _0 isn't there.
    // after that, just use fearElements.size() BEFORE adding the new element,
    // to get the new element's name. e.g:
    // name = fearElements[0]->name + std::to_string(fearElements.size());
    std::vector<ElementInfoModule*> fearElements;
    std::vector<ElementInfoModule*> pleasureElements;
    
    
    
    //// NOTE: below needs more work for sensor array...
    // Sensory Consolidatory Interneurons to link to emotional extension interneurons (EEIs)
    //
    // inputs to SCIs have causal relationships, with each input weight equal to:
    // <total desired weight>/<number of inputs>
    //
    // HOWEVER: there will be 1 causal input from the EEI it inputs to, for "emotional feedback"
    std::vector<ElementInfoModule*> sciElements;
    
    
    ortus::name_map nameMap; // name to index
    ortus::index_map indexMap; // index to name... but it's acutally a vector. because after i named it, i realized it would be stupid for this to be a map, when a vector would do the same thing, more quickly.
   
    
    ortus::relation_map correlatedRelations;
    // for the below 3 maps, the key is the index of the 'pre' element,
    // and relationships are created such that "pre" <ElementRelationType> "post"
    // e.g., "pre" CAUSES "post" (along with
    ortus::relation_map causesRelations;
    ortus::relation_map dominatesRelations;
    ortus::relation_map opposesRelations;
    
    //// might want to create a map for muscles that are supposed to 'cause' sensory input
        //// e.g., Lung... it should cause O2 to rise when excited, under normal circumstances.
    
    
    
    std::vector<std::string> odrVec;
    OrtUtil ortUtil;
    
};

#endif /* Connectome_hpp */
