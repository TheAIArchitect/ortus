//
//  DarwinEffectuator.hpp
//  PhunWithPhysics
//
//  Created by onyx on 4/21/16.
//  Copyright © 2016 AweM. All rights reserved.
//

#pragma once

#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <cmath>
#include <unordered_map>
#include <algorithm>
#include <random>
#include "GeneticUtils.hpp"
#include "NumUtils.hpp"
#include "BodyParameterSet.hpp"
#include "Logger.hpp"

using paramVec_p = std::vector<BodyParameterSet*>;
using selected_map = std::unordered_map<int, BodyParameterSet*>;


class DarwinEffectuator{
    
    public:
    
        std::vector<float> computeStatistics(std::vector<float>& vec);
        int getConstraintData(BodyParameterSet::SpringConfig* sc, int next_slot, std::vector<std::vector<float>>& dat);
        std::vector<std::vector<float>> extractData(BodyParameterSet** paramsets, int num_params);
        void getMinAndMaxVals();
        void setHighOrLow(BodyParameterSet* evo, bool setHigh);
        void setSpringConfigHighOrLow(BodyParameterSet::SpringConfig* sc, float param);
        void minimizeSpringConfig(BodyParameterSet::SpringConfig* newSC, BodyParameterSet::SpringConfig* oldSC);
        void maximizeSpringConfig(BodyParameterSet::SpringConfig* newSC, BodyParameterSet::SpringConfig* oldSC);
        void cminimize(BodyParameterSet* evoNew, BodyParameterSet* evoOld);
        void cmaximize(BodyParameterSet* evoNew, BodyParameterSet* evoOld);
        std::string getStatString(std::vector<std::vector<float>>& stats, int row);
        void writeMinVsMax(BodyParameterSet* min, BodyParameterSet* max, std::vector<std::vector<float>>& stats);
        std::string writeMinVsMaxSpringConfig(std::string name, BodyParameterSet::SpringConfig* minsc, BodyParameterSet::SpringConfig* maxsc, std::vector<std::vector<float>>& stats, int& next_index);
    
public:
    void survivalOfTheFittest(size_t random_worms_in_gen, size_t total_worms_in_gen, const std::string &eSN);
    void survivalOfTheFittest(size_t random_worms_in_gen, size_t total_worms_in_gen, int eSN);
    
private:
    void reproduce();
    void selectWormsForReproduction();
    void loadWorms();
    void fillParents();
    void shuffleParents();
    void goForthAndMultiply();
    void makeBabies();
    bool is_even();
    void saveWorms();
    void writeParamFiles(BodyParameterSet *worm);
    
private:
    size_t generation_;
    size_t randomWorms_;
    size_t wormsInGen_;
    std::string dataFile_;
    std::string metaFile_;
    std::string data_nextgen_;
    std::string meta_nextgen_;
    size_t executableSessionNumber_;
    selected_map selectedRight_;
    selected_map selectedLeft_;
    paramVec_p paramSets_;
    Logger log_;
    std::vector<int> mother_;
    std::vector<int> father_;
    paramVec_p children_;
    std::vector<std::vector<float>> utilityAndPairing_local_;
    
    const size_t numWormsForReproduction_{40};
    const size_t numWormsToMoveOn_{20};
};
