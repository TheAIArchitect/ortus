//
//  Architect.hpp
//  ortus
//
//  Created by andrew on 4/7/17.
//  Copyright © 2017 Andrew W.E. McDonald. All rights reserved.
//

#ifndef Architect_hpp
#define Architect_hpp

#include <stdio.h>
#include "Connectome.hpp"
#include "DataSteward.hpp"
#include "ElementInfoModule.hpp"
#include "ElementRelation.hpp"
#include "OrtusStd.hpp"
#include "Statistician.hpp"


class Architect {
public:
    
    Architect();
    
    Connectome* connectomep;
    DataSteward* dataStewardp;
    
    void setConnectome(Connectome* cp);
    void setDataSteward(DataSteward* ds);
    
    ElementInfoModule* createSEI(ElementInfoModule* pre, std::unordered_map<RelationAttribute, cl_float>& attribs);
    void designConnectome();
    
    long nCr(int n, int r);
    long fact(int f);
};



#endif /* Architect_hpp */
