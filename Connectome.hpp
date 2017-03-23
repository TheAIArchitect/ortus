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

class Connectome {
    
public:
    Connectome();
    void Connectome::initializeData();
    
    
    ortus::element_map* elementMap;
    
    // want a vector of all ElementRelations
    // want a map of each ElementRelationType, with element index as key
}


#endif /* Connectome_hpp */
