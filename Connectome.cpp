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
    ortUtil.makeAndSetElements(ortFileName, elementModules, elementRelations, elementMap);
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
    printf("Element Modules (%d)\n",elementModules.size());
    for (auto thing : elementModules){
        printf("%s\n",thing->toString().c_str());
    }
    printf("Element Relations (%d)\n", elementRelations.size());
    for (auto thing : elementRelations){
        printf("%s\n",thing->toString().c_str());
    }
    
    
    return;
    
}

