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


class Architect {
public:
    
    Architect();
    
    Connectome* connectomep;
    DataSteward* dataStewardp;
    
    void setConnectome(Connectome* cp);
    void setDataSteward(DataSteward* ds);
    void designConnectome();
};



#endif /* Architect_hpp */
