//
//  PolaritySet.cpp
//  delegans
//
//  Created by onyx on 10/13/16.
//

#include "PolarityConfigurator.hpp"



PolarityConfigurator::PolarityConfigurator(){
    initMap();
}

float* PolarityConfigurator::makePolarityVector(std::vector<ElementInfoModule*>* bioElements, int num_rows){
    int numElems = bioElements->size();
    if (num_rows != numElems){
        printf("This is wrong.\n");
        exit(3);
    }
    float *pol_vec = new float[num_rows]();
    srand((int)6107101036);
    //int rand_1_or_0 = rand() % 2;
    // right now, we are just setting 1 for excitatory, -1 for inhibitory
    // we need to change it to set actual NIM objects to have polarity set based upon the individual neuron.
    //
    // EDIT: hack... check the polarityMap... if we have an entry, we use that. if not, sensory are excite, inter are inhibit, and motor are excite.
    //
    for (int i = 0; i < numElems; ++i){
        if (setPolarityIfExists((*bioElements)[i]->name, &pol_vec, i)){ // if true, we just set the polarity, so we just go on. otherwise, we set a generic polarity
            continue;
        }
        switch ((*bioElements)[i]->getEType()){
            case SENSORY:
                pol_vec[i] = 1;
                break;
            case INTER:
                pol_vec[i] = -1;// CHANGE TO WEIGHTED RAND
                break;
            case MOTOR:
                pol_vec[i] = 1;
                break;
            case MUSCLE:
                pol_vec[i] = 1;
                break;
        }
    }
    return pol_vec;
}

/* returns false if not in map, true if found in map. (*polVec)[index] will be set to polarity if found in map. if not found, nothing happens to polVec.
 */
bool PolarityConfigurator::setPolarityIfExists(std::string name,float** polVec, int index){
    std::unordered_map<std::string,int>::iterator iter = polarityMap.find(name);
    if (iter == polarityMap.end()){ // name not in polarityMap
        return false;
    }
    (*polVec)[index] = polarityMap[name];
    return true;
}

// this is not a very good approach... but it'll do for now.
void PolarityConfigurator::initMap(){
    // SENSORY
    polarityMap["ALML"] = 1; // touch, releases Glutamate
    polarityMap["ALMR"] = 1; // touch, releases Glutamate
    polarityMap["AVM"] = 1; // touch, releases Glutamate
    polarityMap["PLML"] = 1; // touch, releases Glutamate
    polarityMap["PLMR"] = 1; // touch, releases Glutamate
    polarityMap["PVDL"] = 1; // touch, releases Glutamate
    polarityMap["PVDR"] = 1; // touch, releases Glutamate
    // INTER
    polarityMap["DVA"] = -1; // also a stretch receptor, releases ACh
    polarityMap["PVCL"] = 1; // harsh touch increases CA^2+, so, excitatory, releases ACh
    polarityMap["PVCR"] = 1; // harsh touch increases CA^2+, so, excitatory, releases ACh
    polarityMap["AVAL"] = -1; // accepts Glutamate, releases ACh
    polarityMap["AVAR"] = -1; // accepts Glutamate, releases ACh
    polarityMap["AVBL"] = -1; // accepts Glutamate, releases ACh
    polarityMap["AVBR"] = -1; // accepts Glutamate, releases ACh
    polarityMap["AVDL"] = -1; // accepts Glutamate, releases ACh
    polarityMap["AVDR"] = -1; // accepts Glutamate, releases ACh
   
}


Polarity PolarityConfigurator::getPolarity(Connection* conn){
    
    return EXCITATORY;
}

