//
//  AblationStation.hpp
//  delegans
//
//  Created by onyx on 10/18/16.
//  Copyright © 2016 Andrew W.E. McDonald. All rights reserved.
//

#ifndef AblationStation_hpp
#define AblationStation_hpp

#include <stdio.h>
#include "ElementInfoModule.hpp"
#include <string>

class AblationStation {
   
public:
    AblationStation();
    void initDataArrays();
    bool setAblationStatus(ElementInfoModule* eim);
    static int NUM_NEURONS_FOR_ABLATION;
    static int NUM_MUSCLES_FOR_ABLATION;
    static int NUM_PARTIALS_FOR_ABLATION;
    static int NUM_TYPES_FOR_ABLATION;
    // exact matches for neurons
    static std::string* NEURONS_FOR_ABLATION;
    // exact matches for muscles
    static std::string* MUSCLES_FOR_ABLATION;
    // partial matches for neurons ane muscles -- see bottom of file for explanation
    static std::string* PARTIALS_FOR_ABLATION;
    // type matches
    static ElementType* TYPES_FOR_ABLATION;
};

/* source: http://worms.aecom.yu.edu/pages/muscle_abbreviations.htm

 adp - anal depressor
 ail - anterior inner longitudinal
 aob - anterior oblique
 aol - anterior outer longitudinal
 bm - basement membrane
 cdl - caudal longitudinal
 dBWM - dorsal body wall muscle
 dgl - diagonal
 dsp - dorsal spicule protractor
 dsr -dorsal spicule retractor
 e - epithelial
 g- gland
 gec - gubernacular erector
 grt - gubernacular retractor
 hyp - hypodermis
 int - stomatointestinal
 mc - marginal cell
 Phsh - phasmid sheath Cell
 Phso - phasmid socket Cell
 pil - posterior inner longitudinal
 pm - pharangeal muscle
 pob - posterior oblique
 pol - posterior outer longitudinal
 rect - rectum
 sph - sphincter
 vBWM - ventral body wall muscle
 vm - vulval muscle
 vsp - ventral spicule protractor
 vsr - ventral spicule retractor
*/

#endif /* AblationStation_hpp */
