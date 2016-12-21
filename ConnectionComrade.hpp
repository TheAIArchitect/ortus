//
//  ConnectionComrade.hpp
//  delegans
//
//  Created by onyx on 10/12/16.
//  Copyright © 2016 Sean Grimes. All rights reserved.
//

#ifndef ConnectionComrade_hpp
#define ConnectionComrade_hpp

#include <stdio.h>

#include "ElementInfoModule.hpp"
#include "MuscleInfoModule.hpp"
#include "NeuronInfoModule.hpp"
#include <cmath>
#include <stack>

#include <queue>
#include "OptionForewoman.hpp"


class ConnectionComrade {
    
public:
    ConnectionComrade(std::vector<ElementInfoModule*> elements, std::string* startingNeurons,int numStartingNeurons);
    
    
    bool resetConns();
    std::queue<Connection*> enqueueDesiredConns(int arg);
    void generateConns(int arg);
    void populateConnArray(std::queue<Connection*>& cq);
        
    
    std::vector<ElementInfoModule*> elements;
    int numStartingNeurons;
    std::string* startingNeurons;
    
    float* gap_conns;
    int num_total_gaps;
    float* chem_conns;
    int num_total_chems;
    float* all_conns;
    int num_gap_indices;
    int num_chem_indices;
    int num_all_indices;
    std::vector<Connection*> currentlyUsedConns;
    
    static bool doGaps;
    static bool doChems;
    static bool ANY_MUSCLE_WEIGHT;
    
    
    static const int MUSCLES = 0x01;
    static const int STARTING_SET = 0x02;
    static const int MUSCLES_AND_STARTING_SET = 0x04;
    
    
    
public:
    template<class T, class U>
    int addConn(float* connArray, int starting_index, T* premod, U* postmod, float preOffset, float postOffset){
        float pre_trans_scale = 0;
        float post_trans_scale = 0;
        if (premod->getEType() == MUSCLE){
            pre_trans_scale = 1.f;// Artist::MUSCLE_TRANS_SCALE;
        } else{
            pre_trans_scale = NeuronInfoModule::NEURON_TRANS_SCALE;
        }
        if (postmod->getEType() == MUSCLE){
            post_trans_scale = 1.f;//Artist::MUSCLE_TRANS_SCALE;
        } else{
            post_trans_scale = NeuronInfoModule::NEURON_TRANS_SCALE;
        }
        connArray[starting_index] = premod->getCenterPoint().x*pre_trans_scale + preOffset;
        connArray[starting_index+1] = premod->getCenterPoint().y*pre_trans_scale + preOffset;
        connArray[starting_index+2] = premod->getCenterPoint().z*pre_trans_scale + preOffset;
        connArray[starting_index+3] = postmod->getCenterPoint().x*post_trans_scale + postOffset;
        connArray[starting_index+4] = postmod->getCenterPoint().y*post_trans_scale + postOffset;
        connArray[starting_index+5] = postmod->getCenterPoint().z*post_trans_scale + postOffset;
        premod->marked = true;
        postmod->marked = true;
        return 6; // this is the number of indices we 'used up' in connArray -- allows adjusting any indexing variables.
    }
    
    
};

#endif /* ConnectionComrade_hpp */
