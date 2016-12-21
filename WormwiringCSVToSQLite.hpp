//
//  WormwiringCSVToSQLite.hpp
//  LearningOpenGL
//
//  Created by onyx on 11/10/15.
//  Copyright © 2015 AweM. All rights reserved.
//

#ifndef WormwiringCSVToSQLite_hpp
#define WormwiringCSVToSQLite_hpp

#include <stdio.h>
#include "FileShit.hpp"
#include <fstream>
#include "ElementInfoModule.hpp"
#include "NeuronInfoModule.hpp"
#include "MuscleInfoModule.hpp"


using std::string;





void get_conns(std::vector<ElementInfoModule*>& elements);

#endif /* WormwiringCSVToSQLite_hpp */
