//
//  Connectome.cpp
//  ortus
//
//  Created by andrew on 3/22/17.
//  Copyright © 2017 Andrew W.E. McDonald. All rights reserved.
//

#include "Connectome.hpp"


Connectome::Connectome(std::string ortFileName){
    
    // parse the .ort file, and create the elements and element relations specified
    std::vector<std::string> theLines;
    ortUtil.getLines(ortFileName, theLines);
    
    
    buildAdditionalDataStructures();
}

/**
 * Once the .ort file has been parsed,
 * and the elements and element relations have been created,
 * we use this function to create:
 *
 * - nameMap ==> mapping between name and index in elementModules vector
 * - indexMap ==> mapping between index in elementModules vector and name
 * - correlatedRelations ==> mapping between the index of the pre element and all ElementRelations (pointers) that have post elements correlated with the pre
 * - causesRelations ==> mapping between the index of the pre element and all ElementRelations (pointers) that have post elements effected by the pre
 * - dominatesRelations ==> mapping between the index of the pre element and all ElementRelations (pointers) that have post elements dominates by the pre
 * - opposesRelations ==> mapping between the index of the pre element and all ElementRelations (pointers) that have post elements opposed by the pre
 *
 */
void Connectome::buildAdditionalDataStructures(){
    printf(">> Element Modules (%d)\n",elementModules.size());
    for (auto element : elementModules){
        nameMap[element->name] = element->id;
        indexMap[element->id] = element->name;
        printf("%s\n",element->toString().c_str());
    }
    printf(">> Element Relations (%d)\n", elementRelations.size());
    for (auto relation : elementRelations){
        
        
        printf("%s\n",relation->toString().c_str());
    }
    
    
    return;
    
}



/**
 * This is only used for when parsing the .ort file. 
 * To add relation attributes during execution, (.....FIXME.......) is called
 */
void Connectome::addRelationAttributesFromOrt(std::unordered_map<std::string,std::string>& preAttributeMapStrings, std::unordered_map<std::string,std::string>& postAttributeMapStrings, ElementRelation* elrel){
    std::string tempAttrib = "";
    bool isElement = false;
    
    ortus::enum_string_unordered_map<RelationAttribute> preAttributeMap = ortUtil.getAttributeEnumsFromStrings<RelationAttribute>(preAttributeMapStrings, isElement);
    
    ortus::enum_string_unordered_map<RelationAttribute> postAttributeMap = ortUtil.getAttributeEnumsFromStrings<RelationAttribute>(postAttributeMapStrings, isElement);
    
    for (auto attrib : preAttributeMap){
        elrel->setAttribute(attrib.first,std::stof(attrib.second));
    }
    
    for (auto attrib : postAttributeMap){
        elrel->setAttribute(attrib.first,std::stof(attrib.second));
    }
}

ElementRelation* Connectome::addRelation(ElementInfoModule* ePre, ElementInfoModule* ePost, ElementRelationType ert){
        ElementRelation* elrel = new ElementRelation();
        elrel->pre = ePre;// NOTE: if 'major' has attributes to set, they must be re-set for each ElementRelation
        elrel->preName = ePre->name;
        elrel->preId = ePre->id;
        elrel->post = ePost;
        elrel->postName = ePost->name;
        elrel->postId = ePost->id;
        // NOTE: as this grows, it might make more sense to have a function take an array of attributes to check for for each case.
        switch(ert){ 
            case CORRELATED:
                elrel->rtype = CORRELATED;
                correlatedRelations[ePre->id].push_back(elrel);
                break;
            case CAUSES:
                elrel->rtype = CAUSES;
                causesRelations[ePre->id].push_back(elrel);
                break;
            case DOMINATES:
                elrel->rtype = DOMINATES;
                dominatesRelations[ePre->id].push_back(elrel);
                break;
            case OPPOSES:
                elrel->rtype = OPPOSES;
                opposesRelations[ePre->id].push_back(elrel);
                break;
            default:
                printf("Error: unknown ElementRelationType, '%d'\n",ert);
                break;
        }
        elementRelations.push_back(elrel);
    return elrel;
}

/**
 * now that
void addToRelationMap(ortus::relation_map& remap, int preIndex, ElementRelation* elrel){
    remap[preIndex].push_back(elrel);
}
*/

//void Connectome::initRelationMap(


/**
 * This is only used for when parsing the .ort file. 
 * 
 * The first entry in the vector is the 'pre' element and its attirbutes,
 * and the rest (could be only one) are/is the 'post' element/s. 
 *
 * To add a relation during execution, (.....FIXME.......) is called
 */
void Connectome::addRelationsFromOrt(std::vector<std::unordered_map<std::string, std::string>>& vecOfAttributeMaps, ElementRelationType ert){
    int numMaps = -1;
    if ((numMaps = vecOfAttributeMaps.size()) < 2){
        printf("Error: must have at least one opposing element, only found '%d' attribute maps, one of which should be the 'pre' element.\n",numMaps);
        exit(59);
    }
    // the first one is always the 'pre' (we'll call it major here, just for fun)
    std::unordered_map<std::string, std::string> preAttributeMapStrings = vecOfAttributeMaps[0];
    std::unordered_map<std::string, std::string> postAttributeMapStrings;
    ElementInfoModule* ePre = ortUtil.checkMapAndGetElementPointer(preAttributeMapStrings, elementMap);
    ElementInfoModule* ePost;
    std::string tempAttrib;
    // start at 1; we already took care of 0
    for (int i = 1; i < numMaps; ++i){
        tempAttrib = "";
        postAttributeMapStrings = vecOfAttributeMaps[i];
        ePost = ortUtil.checkMapAndGetElementPointer(postAttributeMapStrings, elementMap);
        ElementRelation* elrel = addRelation(ePre, ePost, ert);
        addRelationAttributesFromOrt(preAttributeMapStrings, postAttributeMapStrings, elrel);
    }
}



/**
 * new's an ElementInfoModule, and fills it according to the data in the attributeMap
 * 
 * NOTE: This is sets the id for each element.
 *
 * list of recognized attributes (all others are ignored):
 *  # type -- either sense, emotion, motor, or muscle
 *  # affect -- either pos or neg
 *  # opposite -- the name of an element that has an opposing funciton (e.g, CO2 and O2 sensors)
 *
 * NOTE: to add more attributes, add a block, similar to the blocks for currently recognized attributes,
 * and update the above list.
 */
void Connectome::addElement(std::unordered_map<std::string, std::string> attributeMap){
    
    if (attributeMap.find("name") == attributeMap.end()){
        printf("Error: missing 'name' attribute in attribute map.\n");
        exit(56);
    }
    std::string name = attributeMap["name"];
    if (elementMap.find(name) == elementMap.end()){// that's good, and we'll add it.
        elementMap[name] = new ElementInfoModule();
        elementMap[name]->name = name;
        elementMap[name]->id = elementModules.size();// new index will always be 1 more than current largest index, which will always be equal to the size of the vector.
        elementModules.push_back(elementMap[name]);
    }
    else {
        printf("Error: attempting to add duplicate element '%s' to elementMap.\n");
        exit(57);
    }
    ElementInfoModule* eim = elementMap[name];
    
    // these are spe
    if (attributeMap.find("type") != attributeMap.end()){
        
        eim->setType(attributeMap["type"]);
    }
    if (attributeMap.find("affect") != attributeMap.end()){
        eim->setAffect(attributeMap["affect"]);
    }
}


void Connectome::setElements(std::vector<std::string>& theLines){
    //    fixedLine = StrUtils::trim(fixedLine); // remove any remaining whitespace
    /*
     definitionLevel = 0: elements => neurons/areas/muscles, with attributes (type, +affect, etc.)
     definitionLevel = 1: causes => how increases or decreases in neurons/areas impact other things
     definitionLevel = 2: dominates => defines hierarchy of neurons/areas
    */
    int definitionLevel = -1;
    std::string trimmedLine = "";
    int indentationLevel = 0;
    // NOTE: at end of level 0, update NUM_ELEMENTS!!!
    int numLines = theLines.size();
    int lineNum = 0;
    std::vector<std::unordered_map<std::string, std::string>> vecOfAttributeMaps;
    std::string line;
    ElementRelationType ert;
    while (lineNum < numLines){
        //printf("lineNum: %d/%d\n", lineNum, numLines);
        vecOfAttributeMaps.clear();
        line = theLines[lineNum];
        trimmedLine = ortUtil.determineIndentationAndStripWhitespace(line, indentationLevel);
        if (indentationLevel == 0){
            definitionLevel++; // move to new definition level if we drop back to no indentation
            lineNum++;
            continue; // nothing else on this line
        }
        if ("elements" == OrtUtil::ORT_DEFINITION_LEVELS[definitionLevel]){ // just one line
            std::unordered_map<std::string, std::string> attributeMap = ortUtil.createAttributeMapStrings(trimmedLine);
            addElement(attributeMap);
        }
        else {
            vecOfAttributeMaps = ortUtil.createVecOfAttributeMapsContainingRelevantLines(theLines, lineNum);
            ert = NONE;
            if ("opposes" == OrtUtil::ORT_DEFINITION_LEVELS[definitionLevel]){ // 0 or at least 2 lines -- 0 unchecked
                ert = OPPOSES;
                
            }
            else if ("causes" == OrtUtil::ORT_DEFINITION_LEVELS[definitionLevel]){ // 0 or at least 2 lines -- 0 unchecked
                ert = CAUSES;
                
            }
            else if ("dominates" == OrtUtil::ORT_DEFINITION_LEVELS[definitionLevel]){ // 0 or at least 2 lines
                ert = DOMINATES;
            }
            addRelationsFromOrt(vecOfAttributeMaps, ert);
            //addRelation(vecOfAttributeMaps, elementRelations, elementMap, ert);
        }
        lineNum++;
    }
}
