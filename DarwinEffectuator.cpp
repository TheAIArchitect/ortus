//
//  DarwinEffectuator.cpp
//  PhunWithPhysics
//
//  Created by onyx on 4/21/16.
//  Copyright © 2016 AweM. All rights reserved.
//

#include "DarwinEffectuator.hpp"

/*
 * returns a 
 */
std::vector<float> DarwinEffectuator::computeStatistics(std::vector<float>& vec){
    float total = 0;
    float mean = 0;
    float std_deviation = 0;
    float median = 0;
    auto num_entries = vec.size();
    // first get mean
    for (int i = 0; i < num_entries; i++){
        total += vec[i];
    }
    mean = ((float)total)/num_entries;
    // now do std deviation
    total = 0;
    float temp = 0;
    for (int i = 0; i < num_entries; i++){
        temp = vec[i] - mean;
        total += temp*temp;
    }
    std_deviation = std::sqrt(((float)total)/num_entries);
    // finally, get the median
    median = NumUtils::getMedian(vec);
    std::vector<float> results = {mean, std_deviation, median};
    return results;
}


int DarwinEffectuator::getConstraintData(BodyParameterSet::SpringConfig* sc, int next_slot, std::vector<std::vector<float>>& dat){
    int ns = next_slot - 1;
    for(int i = 0; i < 6; i++){
        dat[++ns].push_back(sc[i].stiffness);
        dat[++ns].push_back(sc[i].damping);
        dat[++ns].push_back(sc[i].bounce);
        dat[++ns].push_back(sc[i].enabled);
        dat[++ns].push_back(sc[i].lowerLim);
        dat[++ns].push_back(sc[i].upperLim);
    }
    return ++ns;
}


/**
 * exactracts the data from the paramsets, then calculates mean, median, and std deviation of top samples
 */
std::vector<std::vector<float>> DarwinEffectuator::extractData(BodyParameterSet** paramsets, int num_paramsets){
    std::vector<std::vector<float>> dat;
    std::vector<std::vector<float>> results;
    auto num_dats = 227;// 11 + (6*6*6) -- 6 constraints, each with 6 DOF, each with 6 fields
    auto next_slot = 0;
    for (auto i = 0; i < num_dats; ++i){
        dat.push_back(std::vector<float>());
    }
    for (auto i = 0; i < num_paramsets; ++i){
        next_slot = 0;
        dat[0].push_back(paramsets[i]->centerSphereAnisotropicFriction[0]);
        dat[1].push_back(paramsets[i]->centerSphereAnisotropicFriction[1]);
        dat[2].push_back(paramsets[i]->centerSphereAnisotropicFriction[2]);
        dat[3].push_back(paramsets[i]->groundRestitution);
        dat[4].push_back(paramsets[i]->groundFriction);
        dat[5].push_back(paramsets[i]->muscleRestitution);
        next_slot = getConstraintData(paramsets[i]->centerCylinderChainConSprings, 6, dat);
        next_slot = getConstraintData(paramsets[i]->muscleConSprings, next_slot, dat);
        next_slot = getConstraintData(paramsets[i]->centerToMuscleConSprings, next_slot, dat);
        next_slot = getConstraintData(paramsets[i]->centerMuscleSupportSprings, next_slot, dat);
        next_slot = getConstraintData(paramsets[i]->muscleEndToCenterCylinderSprings, next_slot, dat);
        next_slot = getConstraintData(paramsets[i]->endsOfMusclesSupportSprings, next_slot, dat);
        dat[next_slot++].push_back(paramsets[i]->wave_amp);
        dat[next_slot++].push_back(paramsets[i]->wave_freq);
        dat[next_slot++].push_back(paramsets[i]->numStates);
        dat[next_slot++].push_back(paramsets[i]->leftRightOffsetPercent);
        dat[next_slot++].push_back(paramsets[i]->muscleToMuscleOffsetPercent);
        dat[next_slot].push_back(paramsets[i]->forceMultiplierOffset);
        
    }
    for (auto i = 0; i < num_dats; ++i){
        results.push_back(computeStatistics(dat[i]));
    }
    return results;
}

/*
 * Used by setHighOrLow, sets spring configs for all axes for a given config to param. 
 * 
 * note: enabled is always false
 */
void DarwinEffectuator::setSpringConfigHighOrLow(BodyParameterSet::SpringConfig* sc, float param){
    for(int i = 0; i < 6; i++){
        sc[i].stiffness = param; // stiffness
        sc[i].damping = param; // damping
        sc[i].bounce = param; // bounce
        sc[i].enabled = false; // enabled
        sc[i].lowerLim = param; // lower limit
        sc[i].upperLim = param; // upper limit
    }
}


/* 
 * Sets all BodyParameterSet parameters to either high or low, and enbaled always to false
 */
void DarwinEffectuator::setHighOrLow(BodyParameterSet* param, bool setHigh){
    float hiLowParam = FLT_MAX;
    if (setHigh){
        hiLowParam = FLT_MIN;
    }
    param->centerSphereAnisotropicFriction = btVector3(hiLowParam, hiLowParam, hiLowParam);
    param->groundRestitution = hiLowParam;
    param->groundFriction = hiLowParam;
    param->muscleRestitution = hiLowParam;
    setSpringConfigHighOrLow(param->centerCylinderChainConSprings, hiLowParam);
    setSpringConfigHighOrLow(param->muscleConSprings, hiLowParam);
    setSpringConfigHighOrLow(param->centerToMuscleConSprings, hiLowParam);
    setSpringConfigHighOrLow(param->centerMuscleSupportSprings, hiLowParam);
    setSpringConfigHighOrLow(param->muscleEndToCenterCylinderSprings, hiLowParam);
    setSpringConfigHighOrLow(param->endsOfMusclesSupportSprings, hiLowParam);
    param->wave_amp = hiLowParam;
    param->wave_freq = hiLowParam;
    param->numStates = hiLowParam;
    param->leftRightOffsetPercent = hiLowParam;
    param->muscleToMuscleOffsetPercent = hiLowParam;
    param->forceMultiplierOffset = hiLowParam;
}


void DarwinEffectuator::minimizeSpringConfig(BodyParameterSet::SpringConfig* newSC, BodyParameterSet::SpringConfig* oldSC){
    for(int i = 0; i < 6; i++){
        if(newSC[i].stiffness > oldSC[i].stiffness)
            newSC[i].stiffness = oldSC[i].stiffness;
        if(newSC[i].damping > oldSC[i].damping)
            newSC[i].damping = oldSC[i].damping;
        if(newSC[i].bounce > oldSC[i].bounce)
            newSC[i].bounce = oldSC[i].bounce;
        if(oldSC[i].enabled == true) // all "oldSC" will have the same state of enabled/disabled
            newSC[i].enabled = true;
        if(newSC[i].lowerLim > oldSC[i].lowerLim)
            newSC[i].lowerLim = oldSC[i].lowerLim;
        if(newSC[i].upperLim > oldSC[i].upperLim)
            newSC[i].upperLim = oldSC[i].upperLim;
    }
}

void DarwinEffectuator::maximizeSpringConfig(BodyParameterSet::SpringConfig* newSC, BodyParameterSet::SpringConfig* oldSC){
    for(int i = 0; i < 6; i++){
        if(newSC[i].stiffness < oldSC[i].stiffness)
            newSC[i].stiffness = oldSC[i].stiffness;
        if(newSC[i].damping < oldSC[i].damping)
            newSC[i].damping = oldSC[i].damping;
        if(newSC[i].bounce < oldSC[i].bounce)
            newSC[i].bounce = oldSC[i].bounce;
        if(oldSC[i].enabled == true) // all "oldSC" will have the same state of enabled/disabled
            newSC[i].enabled = true;
        if(newSC[i].lowerLim < oldSC[i].lowerLim)
            newSC[i].lowerLim = oldSC[i].lowerLim;
        if(newSC[i].upperLim < oldSC[i].upperLim)
            newSC[i].upperLim = oldSC[i].upperLim;
    }
}


void DarwinEffectuator::cminimize(BodyParameterSet* paramNew, BodyParameterSet* paramOld){
    if (paramNew->centerSphereAnisotropicFriction[0] > paramOld->centerSphereAnisotropicFriction[0])
        paramNew->centerSphereAnisotropicFriction[0] = paramOld->centerSphereAnisotropicFriction[0];
    if (paramNew->centerSphereAnisotropicFriction[1] > paramOld->centerSphereAnisotropicFriction[1])
        paramNew->centerSphereAnisotropicFriction[1] = paramOld->centerSphereAnisotropicFriction[1];
    if (paramNew->centerSphereAnisotropicFriction[2] > paramOld->centerSphereAnisotropicFriction[2])
        paramNew->centerSphereAnisotropicFriction[2] = paramOld->centerSphereAnisotropicFriction[2];
    if (paramNew->groundRestitution > paramOld->groundRestitution)
        paramNew->groundRestitution = paramOld->groundRestitution;
    if (paramNew->groundFriction > paramOld->groundFriction)
        paramNew->groundFriction = paramOld->groundFriction;
    if (paramNew->muscleRestitution > paramOld->muscleRestitution)
        paramNew->muscleRestitution = paramOld->muscleRestitution;
    minimizeSpringConfig(paramNew->centerCylinderChainConSprings, paramOld->centerCylinderChainConSprings);
    minimizeSpringConfig(paramNew->muscleConSprings, paramOld->muscleConSprings);
    minimizeSpringConfig(paramNew->centerToMuscleConSprings, paramOld->centerToMuscleConSprings);
    minimizeSpringConfig(paramNew->centerMuscleSupportSprings, paramOld->centerMuscleSupportSprings);
    minimizeSpringConfig(paramNew->muscleEndToCenterCylinderSprings, paramOld->muscleEndToCenterCylinderSprings);
    minimizeSpringConfig(paramNew->endsOfMusclesSupportSprings, paramOld->endsOfMusclesSupportSprings);
    if (paramNew->wave_amp > paramOld->wave_amp)
        paramNew->wave_amp = paramOld->wave_amp;
    if (paramNew->wave_freq > paramOld->wave_freq)
        paramNew->wave_freq = paramOld->wave_freq;
    if (paramNew->numStates > paramOld->numStates)
        paramNew->numStates = paramOld->numStates;
    if (paramNew->leftRightOffsetPercent > paramOld->leftRightOffsetPercent)
        paramNew->leftRightOffsetPercent = paramOld->leftRightOffsetPercent;
    if (paramNew->muscleToMuscleOffsetPercent > paramOld->muscleToMuscleOffsetPercent)
        paramNew->muscleToMuscleOffsetPercent = paramOld->muscleToMuscleOffsetPercent;
     if (paramNew->forceMultiplierOffset > paramOld->forceMultiplierOffset)
        paramNew->forceMultiplierOffset = paramOld->forceMultiplierOffset;
   
}

void DarwinEffectuator::cmaximize(BodyParameterSet* paramNew, BodyParameterSet* paramOld){
    if (paramNew->centerSphereAnisotropicFriction[0] < paramOld->centerSphereAnisotropicFriction[0])
        paramNew->centerSphereAnisotropicFriction[0] = paramOld->centerSphereAnisotropicFriction[0];
    if (paramNew->centerSphereAnisotropicFriction[1] < paramOld->centerSphereAnisotropicFriction[1])
        paramNew->centerSphereAnisotropicFriction[1] = paramOld->centerSphereAnisotropicFriction[1];
    if (paramNew->centerSphereAnisotropicFriction[2] < paramOld->centerSphereAnisotropicFriction[2])
        paramNew->centerSphereAnisotropicFriction[2] = paramOld->centerSphereAnisotropicFriction[2];
    if (paramNew->groundRestitution < paramOld->groundRestitution)
        paramNew->groundRestitution = paramOld->groundRestitution;
    if (paramNew->groundFriction < paramOld->groundFriction)
        paramNew->groundFriction = paramOld->groundFriction;
    if (paramNew->muscleRestitution < paramOld->muscleRestitution)
        paramNew->muscleRestitution = paramOld->muscleRestitution;
    maximizeSpringConfig(paramNew->centerCylinderChainConSprings, paramOld->centerCylinderChainConSprings);
    maximizeSpringConfig(paramNew->muscleConSprings, paramOld->muscleConSprings);
    maximizeSpringConfig(paramNew->centerToMuscleConSprings, paramOld->centerToMuscleConSprings);
    maximizeSpringConfig(paramNew->centerMuscleSupportSprings, paramOld->centerMuscleSupportSprings);
    maximizeSpringConfig(paramNew->muscleEndToCenterCylinderSprings, paramOld->muscleEndToCenterCylinderSprings);
    maximizeSpringConfig(paramNew->endsOfMusclesSupportSprings, paramOld->endsOfMusclesSupportSprings);
    if (paramNew->wave_amp < paramOld->wave_amp)
        paramNew->wave_amp = paramOld->wave_amp;
    if (paramNew->wave_freq < paramOld->wave_freq)
        paramNew->wave_freq = paramOld->wave_freq;
    if (paramNew->numStates < paramOld->numStates)
        paramNew->numStates = paramOld->numStates;
    if (paramNew->leftRightOffsetPercent < paramOld->leftRightOffsetPercent)
        paramNew->leftRightOffsetPercent = paramOld->leftRightOffsetPercent;
    if (paramNew->muscleToMuscleOffsetPercent < paramOld->muscleToMuscleOffsetPercent)
        paramNew->muscleToMuscleOffsetPercent = paramOld->muscleToMuscleOffsetPercent;
    if (paramNew->forceMultiplierOffset < paramOld->forceMultiplierOffset)
        paramNew->forceMultiplierOffset = paramOld->forceMultiplierOffset;
    
}

std::string DarwinEffectuator::getStatString(std::vector<std::vector<float>>& stats, int row){
    return "    => (Mean: "+std::to_string(stats[row][0])+", Std. Deviation: +/- "+std::to_string(stats[row][1])+", Median: "+std::to_string(stats[row][2])+")";
}

std::string DarwinEffectuator::writeMinVsMaxSpringConfig(std::string name, BodyParameterSet::SpringConfig* minsc, BodyParameterSet::SpringConfig* maxsc, std::vector<std::vector<float>>& stats, int& next_index){
    std::string s = "[SPRING CONFIG: "+name+"]\n";
    std::vector<std::string> axes = {"X-lateral","Y-lateral","Z-lateral","X-rotational","Y-rotational","Z-rotational"};
    int ns = next_index - 1;
    for(int i = 0; i < 6; i++){
        if (minsc[i].enabled == false)
            s += "  > "+axes[i]+" ((SPRING DISABLED)):\n";
        else
            s += "  > "+axes[i]+":\n";
        s += "      stiffness: "+ std::to_string(minsc[i].stiffness) + " -- " + std::to_string(maxsc[i].stiffness) + getStatString(stats,++ns)+"\n";
        s += "      damping: "+ std::to_string(minsc[i].damping) + " -- " + std::to_string(maxsc[i].damping) + getStatString(stats,++ns)+"\n";
        s += "      bounce: "+ std::to_string(minsc[i].bounce) + " -- " + std::to_string(maxsc[i].bounce) + getStatString(stats,++ns)+"\n";
        s += "      enabled: "+ std::to_string(minsc[i].enabled) + " -- " + std::to_string(maxsc[i].enabled) + getStatString(stats,++ns)+"\n";
        s += "      lowerLim: "+ std::to_string(minsc[i].lowerLim) + " -- " + std::to_string(maxsc[i].lowerLim) + getStatString(stats,++ns)+"\n";
        s += "      upperLim: "+ std::to_string(minsc[i].upperLim) + " -- " + std::to_string(maxsc[i].upperLim) + getStatString(stats,++ns)+"\n";
    }
    next_index = ++ns;
    return s;
}

void DarwinEffectuator::writeMinVsMax(BodyParameterSet* min, BodyParameterSet* max, std::vector<std::vector<float>>& stats){
    std::string s = "min -- max:\n";
    s += "Anisotropic Friction, x: " + std::to_string(min->centerSphereAnisotropicFriction[0]) + " -- " + std::to_string(max->centerSphereAnisotropicFriction[0]) + getStatString(stats,0)+"\n";
    s += "Anisotropic Friction, y: " + std::to_string(min->centerSphereAnisotropicFriction[1]) + " -- " + std::to_string(max->centerSphereAnisotropicFriction[1]) + getStatString(stats,1)+"\n";
    s += "Anisotropic Friction, z: " + std::to_string(min->centerSphereAnisotropicFriction[2]) + " -- " + std::to_string(max->centerSphereAnisotropicFriction[2]) + getStatString(stats,2)+"\n";
    s += "Ground Restitution: " + std::to_string(min->groundRestitution) + " -- " + std::to_string(max->groundRestitution) + getStatString(stats,3)+"\n";
    s += "Ground Friction: " + std::to_string(min->groundFriction) + " -- " + std::to_string(max->muscleRestitution) + getStatString(stats,4)+"\n";
    s += "Muscle Restitution: " + std::to_string(min->muscleRestitution) + " -- " + std::to_string(max->groundRestitution) + getStatString(stats,5)+"\n";
    int next_index = 6;
    s += writeMinVsMaxSpringConfig("centerCylinderChainConSprings",min->centerCylinderChainConSprings, max->centerCylinderChainConSprings, stats, next_index);
    s += writeMinVsMaxSpringConfig("muscleConSprings",min->muscleConSprings, max->muscleConSprings, stats, next_index);
    s += writeMinVsMaxSpringConfig("centerToMuscleConSprings",min->centerToMuscleConSprings, max->centerToMuscleConSprings, stats, next_index);
    s += writeMinVsMaxSpringConfig("centerMuscleSupportSprings",min->centerMuscleSupportSprings, max->centerMuscleSupportSprings, stats, next_index);
    s += writeMinVsMaxSpringConfig("endsOfMusclesSupportSprings",min->endsOfMusclesSupportSprings, max->endsOfMusclesSupportSprings, stats, next_index);
    s += writeMinVsMaxSpringConfig("muscleEndToCenterCylinderSprings",min->muscleEndToCenterCylinderSprings, max->muscleEndToCenterCylinderSprings, stats, next_index);
    s += "Wave Amplitude: " + std::to_string(min->wave_amp) + " -- " + std::to_string(max->wave_amp) + getStatString(stats,next_index++)+"\n";
    s += "Wave Frequency: " + std::to_string(min->wave_freq) + " -- " + std::to_string(max->wave_freq) + getStatString(stats,next_index++)+"\n";
    s += "Number of Muscle Activation States: " + std::to_string(min->numStates) + " -- " + std::to_string(max->numStates) + getStatString(stats,next_index++)+"\n";
    s += "Left/Right Offset Percent: " + std::to_string(min->leftRightOffsetPercent) + " -- " + std::to_string(max->leftRightOffsetPercent) + getStatString(stats,next_index++)+"\n";
    s += "Muscle to Muscle Offset Percent: " + std::to_string(min->muscleToMuscleOffsetPercent) + " -- " + std::to_string(max->muscleToMuscleOffsetPercent) + getStatString(stats,next_index++)+"\n";
    s += "Force Multiplier Offset: " + std::to_string(min->forceMultiplierOffset) + " -- " + std::to_string(max->forceMultiplierOffset) + getStatString(stats,next_index++)+"\n";
    // now write the string
    std::ofstream minMaxString;
    minMaxString.open("Worm_Params-min_max.txt");
    minMaxString.write(s.c_str(), s.size());
    minMaxString.close();
    
}

void DarwinEffectuator::getMinAndMaxVals(){
    std::vector<std::string> mainfiles;
    std::vector<std::string> metafiles;
    for (int i = 1; i <= 4; i++){ // file naming starts at 1
        //std::string temp = "worm_body_params_"+std::to_string(i)+".txt";
        //std::string tempmeta = "worm_body_params_meta_"+std::to_string(i)+".txt";
        std::string temp = "merged_wbp.txt";
        std::string tempmeta = "merged_wbp_meta.txt";
        mainfiles.push_back(temp);
        metafiles.push_back(tempmeta);
    }
    // -1 at end to signal that we're done with the file
    std::vector<int> temp1 = {100,130,150,-1};
    // below (indices): [0][2] would be file 0, 3rd best
    std::vector<std::vector<int>> indices = {temp1};
    
    int num_paramsets = 3;
    BodyParameterSet* paramsets[num_paramsets];
    int file_index = 0;
    int per_file_param_index = 0;
    for (int i = 0; i < num_paramsets; i++){
        paramsets[i] = new BodyParameterSet();
        if(indices[file_index][per_file_param_index] == -1){
            file_index += 1;
            per_file_param_index = 0;
        }
        paramsets[i]->paramFile = mainfiles[file_index];
        paramsets[i]->paramMetaData = metafiles[file_index];
        paramsets[i]->evolve = false;
        paramsets[i]->useBest = true; // middle precedence
        paramsets[i]->useBest_n = indices[file_index][per_file_param_index];
        paramsets[i]->executableSessionNumber = file_index;
        paramsets[i]->loadParams();
        per_file_param_index++;
    }
    // now we have all relevant paramsets loaded.
    // so, we want to create two new ones, one with all of the lowest values, and one with the highest
    BodyParameterSet* lowestparams = new BodyParameterSet();
    setHighOrLow(lowestparams, false);
    BodyParameterSet* greatestparams = new BodyParameterSet();
    setHighOrLow(greatestparams, true);
    std::vector<float*> dat;
    // each entry in dat corresponds to
    for(int i = 0; i < num_paramsets; i++){
        cminimize(lowestparams, paramsets[i]);
        cmaximize(greatestparams, paramsets[i]);
    }
    std::vector<std::vector<float>> stats = extractData(paramsets, num_paramsets);
    writeMinVsMax(lowestparams,greatestparams,stats);
    
}

void DarwinEffectuator::survivalOfTheFittest(size_t random_worms_in_gen, size_t total_worms_in_gen, int eSN){
    survivalOfTheFittest(random_worms_in_gen, total_worms_in_gen, std::to_string(eSN));
}

void DarwinEffectuator::survivalOfTheFittest(size_t random_worms_in_gen, size_t total_worms_in_gen, const std::string &eSN){
    // Determine which data folder to go get the data from
    generation_ = GeneticUtils::findGenerationNumber(eSN);
    
    // How many were randomized, not used at the moment
    randomWorms_ = random_worms_in_gen;
    
    // Ideally this would be an even number...
    wormsInGen_ = total_worms_in_gen;
    /*
    if(wormsInGen_ % 2 != 0){
        log_.error("wormsInGen_ not an even number...", FUNC);
        --wormsInGen_;
    }
     */
    
    std::string dir_next_gen = "evolved_worms/generation_"+std::to_string(GeneticUtils::findGenerationNumber(eSN) + 1);
    FileUtils::mkdirWrapper(dir_next_gen);
    
    executableSessionNumber_ = NumUtils::strToInt(eSN);
    dataFile_ = GeneticUtils::getParamsFilePath(eSN);
    metaFile_ = GeneticUtils::getMetaFilePath(eSN);
    data_nextgen_ = GeneticUtils::getNextGenParamsPath(eSN);
    meta_nextgen_ = GeneticUtils::getNextGenMetaPath(eSN);
    
    
    // Load the worms into memory
    loadWorms();
    
    // Move the best worms into maps to get ready for reproduction
    // NOTE: right now this is an unnecessary step, but once we find out we can't just use the best
    // 40 worms and have to find ones that are 'close' in their default params this function will
    // actually matter
    selectWormsForReproduction();
    
    // Make some new worms
    reproduce();
    
    saveWorms();
    
}

void DarwinEffectuator::loadWorms(){
    for(auto i = 0; i < wormsInGen_; ++i){
        paramSets_.emplace_back(new BodyParameterSet());
        paramSets_[i]->evolveFile = dataFile_;
        paramSets_[i]->evolveMetaFile = metaFile_;
        paramSets_[i]->evolveFileNextGeneration = data_nextgen_;
        paramSets_[i]->evolveMetaFileNextGeneration = meta_nextgen_;
        paramSets_[i]->evolve = true;
        paramSets_[i]->reproduce = true;
        paramSets_[i]->numWormsInGeneration = static_cast<int>(wormsInGen_);
        paramSets_[i]->currentWormNumber = i;
        paramSets_[i]->generationNumber = static_cast<int>(generation_);
        paramSets_[i]->executableSessionNumber = static_cast<int>(executableSessionNumber_);
        paramSets_[i]->configIndex = i;
        paramSets_[i]->doing_darwin = true;
        paramSets_[i]->loadEvolvedWorm();
        //paramSets_[i]->configIndex = i;
    }
    
    if(paramSets_.size() != wormsInGen_){
        log_.fatal("Paramsets does not contain "+std::to_string(wormsInGen_)+" worms.", FUNC);
        throw std::logic_error("Paramsets does not contain "+std::to_string(wormsInGen_)+ " worms.");
    }
}

/**
 * selected{Left, Right} is a map<int, worm>, load it up with the best numWormsForReproduction
 * so we can start some sexual reproduction
 */
void DarwinEffectuator::selectWormsForReproduction(){
    for(auto i = 0; i < numWormsForReproduction_; ++i){
        selectedLeft_[i] = paramSets_[i];
        selectedRight_[i] = paramSets_[i];
    }
}

void DarwinEffectuator::reproduce(){
    log_.debug("wormsInGen_: "+std::to_string(wormsInGen_), FUNC);
    log_.debug("generationNUmber_: "+std::to_string(generation_), FUNC);
    log_.debug("eSN: "+std::to_string(executableSessionNumber_), FUNC);
   
    /**
     * A bit of explanation:
     *  At this point in the code we have 2 "selected_map" objects, one for the left parent ('mother')
     *  and one for the right parent ('father'). They have been filled with numWormsForReproduction_
     *  number of worms (right now [6.20.16], the best 40). We also have 2 vector<int> objects which
     *  have been filled with [0, numWormsForReproduction_) ints, then shuffled. At this point we
     *  loop numWormsForReproduction_ times to create that many worms. The parents vectors are 
     *  reshuffled and we do it again...giving us 80 newly created worms.
     *
     *  We now have our 100 worms for our next generation, 80 new worms + the best 20 without change
     *  from the past generation. 
     */
    std::cout << "reproducing\n";
    goForthAndMultiply();
}

void DarwinEffectuator::goForthAndMultiply(){
    shuffleParents();
    makeBabies();
    shuffleParents();
    makeBabies();
}

void DarwinEffectuator::makeBabies(){
//#error select groups and also some mutation stuff
    std::cout << "making babies\n";
    for(auto i = 0; i < numWormsForReproduction_; ++i){
        BodyParameterSet *mom = selectedLeft_[mother_[i]];
        BodyParameterSet *dad = selectedRight_[father_[i]];
        BodyParameterSet *child = new BodyParameterSet();
        child->evolveFileNextGeneration = data_nextgen_;
        child->evolveMetaFileNextGeneration = meta_nextgen_;
        child->doing_darwin = true;
        
        if(is_even())
            child->centerSphereAnisotropicFriction = mom->centerSphereAnisotropicFriction;
        else
            child->centerSphereAnisotropicFriction = dad->centerSphereAnisotropicFriction;
        
        if(is_even())
            child->centerCylinderChainConSprings[4].stiffness = mom->centerCylinderChainConSprings[4].stiffness;
        else
            child->centerCylinderChainConSprings[4].stiffness = dad->centerCylinderChainConSprings[4].stiffness;
        
        if(is_even())
            child->centerCylinderChainConSprings[4].damping = mom->centerCylinderChainConSprings[4].damping;
        else
            child->centerCylinderChainConSprings[4].damping = dad->centerCylinderChainConSprings[4].damping;
        
        if(is_even())
            child->muscleConSprings[0].stiffness = mom->muscleConSprings[0].stiffness;
        else
            child->muscleConSprings[0].stiffness = dad->muscleConSprings[0].stiffness;
        
        if(is_even())
            child->muscleConSprings[0].damping = mom->muscleConSprings[0].damping;
        else
            child->muscleConSprings[0].damping = dad->muscleConSprings[0].damping;
        
        if(is_even())
            child->muscleConSprings[0].lowerLim = mom->muscleConSprings[0].lowerLim;
        else
            child->muscleConSprings[0].lowerLim = dad->muscleConSprings[0].lowerLim;
        
        if(is_even())
            child->muscleConSprings[0].upperLim = mom->muscleConSprings[0].upperLim;
        else
            child->muscleConSprings[0].upperLim = dad->muscleConSprings[0].upperLim;
        
        if(is_even())
            child->muscleConSprings[2].stiffness = mom->muscleConSprings[2].stiffness;
        else
            child->muscleConSprings[2].stiffness = dad->muscleConSprings[2].stiffness;
        
        if(is_even())
            child->muscleConSprings[2].damping = mom->muscleConSprings[2].damping;
        else
            child->muscleConSprings[2].damping = dad->muscleConSprings[2].damping;
        
        if(is_even())
            child->muscleConSprings[2].lowerLim = mom->muscleConSprings[2].lowerLim;
        else
            child->muscleConSprings[2].lowerLim = dad->muscleConSprings[2].lowerLim;
        
        if(is_even())
            child->muscleConSprings[2].upperLim = mom->muscleConSprings[2].upperLim;
        else
            child->muscleConSprings[2].upperLim = dad->muscleConSprings[2].upperLim;
        
        if(is_even())
            child->muscleConSprings[4].stiffness = mom->muscleConSprings[4].stiffness;
        else
            child->muscleConSprings[4].stiffness = dad->muscleConSprings[4].stiffness;
        
        if(is_even())
            child->muscleConSprings[4].damping = mom->muscleConSprings[4].damping;
        else
            child->muscleConSprings[4].damping = dad->muscleConSprings[4].damping;
        
        if(is_even())
            child->endsOfMusclesSupportSprings[2].stiffness = mom->endsOfMusclesSupportSprings[2].stiffness;
        else
            child->endsOfMusclesSupportSprings[2].stiffness = dad->endsOfMusclesSupportSprings[2].stiffness;
        
        if(is_even())
            child->endsOfMusclesSupportSprings[2].damping = mom->endsOfMusclesSupportSprings[2].damping;
        else
            child->endsOfMusclesSupportSprings[2].damping = dad->endsOfMusclesSupportSprings[2].damping;
        
        if(is_even())
            child->endsOfMusclesSupportSprings[2].lowerLim = mom->endsOfMusclesSupportSprings[2].lowerLim;
        else
            child->endsOfMusclesSupportSprings[2].lowerLim = dad->endsOfMusclesSupportSprings[2].lowerLim;
        
        if(is_even())
            child->endsOfMusclesSupportSprings[2].upperLim = mom->endsOfMusclesSupportSprings[2].upperLim;
        else
            child->endsOfMusclesSupportSprings[2].upperLim = dad->endsOfMusclesSupportSprings[2].upperLim;
        
        if(is_even())
            child->wave_amp = mom->wave_amp;
        else
            child->wave_amp = dad->wave_amp;
        
        if(is_even())
            child->wave_freq = mom->wave_freq;
        else
            child->wave_freq = dad->wave_freq;
        
        if(is_even())
            child->numStates = mom->numStates;
        else
            child->numStates = dad->numStates;
 
        if(is_even())
            child->leftRightOffsetPercent = mom->leftRightOffsetPercent;
        else
            child->leftRightOffsetPercent = dad->leftRightOffsetPercent;
        
        if(is_even())
            child->muscleToMuscleOffsetPercent = mom->muscleToMuscleOffsetPercent;
        else
            child->muscleToMuscleOffsetPercent = dad->muscleToMuscleOffsetPercent;
        
        if(is_even())
            child->forceMultiplierOffset = mom->forceMultiplierOffset;
        else
            child->forceMultiplierOffset = dad->forceMultiplierOffset;
        
        children_.push_back(child);
    }
}

bool DarwinEffectuator::is_even(){
    if((NumUtils::getRandomInt(0, 12000) % 2) == 0)
        return true;
    return false;
}

void DarwinEffectuator::fillParents(){
    if(mother_.size() != 0)
        mother_.clear();
    if(father_.size() != 0)
        father_.clear();
    for(auto i = 0; i < numWormsForReproduction_; ++i){
        mother_.push_back(i);
        father_.push_back(i);
    }
}

void DarwinEffectuator::shuffleParents(){
    if(mother_.size() == 0 || father_.size() == 0)
        fillParents();
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(std::begin(mother_), std::end(mother_), g);
    std::shuffle(std::begin(father_), std::end(father_), g);
}

void DarwinEffectuator::saveWorms(){
    for(auto i = 0; i < numWormsToMoveOn_; ++i){
        writeParamFiles(paramSets_[i]);
    }
    
    auto csz = children_.size();
    for(auto i = 0; i < csz; ++i){
        writeParamFiles(children_[i]);
    }
}

void DarwinEffectuator::writeParamFiles(BodyParameterSet *worm){
    static int config_index = 0;
    std::cout << "config_index: " << config_index << std::endl;
    std::string theConfig = std::to_string(config_index) + "," + std::to_string(worm->currentUtilityIndex) + ",";
    theConfig += std::to_string(worm->centerSphereAnisotropicFriction[0]) + ",";
    theConfig += std::to_string(worm->centerSphereAnisotropicFriction[1]) + ",";
    theConfig += std::to_string(worm->centerSphereAnisotropicFriction[2]) + ",";
    theConfig += std::to_string(worm->groundRestitution) + ",";
    theConfig += std::to_string(worm->groundFriction) + ",";
    theConfig += std::to_string(worm->muscleRestitution) + ",";
    theConfig += worm->springConfigArrayToString(worm->centerCylinderChainConSprings,worm->SpringConfigOrdering::BEGIN_CENTER_CYLINDER_CHAIN_CON) + ",";
    theConfig += worm->springConfigArrayToString(worm->muscleConSprings,worm->SpringConfigOrdering::BEGIN_MUSCLE_CON) + ",";
    theConfig += worm->springConfigArrayToString(worm->centerToMuscleConSprings,worm->SpringConfigOrdering::BEGIN_CENTER_TO_MUSCLE) + ",";
    theConfig += worm->springConfigArrayToString(worm->centerMuscleSupportSprings,worm->SpringConfigOrdering::BEGIN_CENTER_MUSCLE_SUPPORT) + ",";
    theConfig += worm->springConfigArrayToString(worm->muscleEndToCenterCylinderSprings,worm->SpringConfigOrdering::BEGIN_MUSCLE_END_TO_CENTER_CYLINDER) + ",";
    theConfig += worm->springConfigArrayToString(worm->endsOfMusclesSupportSprings,worm->SpringConfigOrdering::BEGIN_ENDS_OF_MUSCLES_SUPPORT) + ",";
    theConfig += std::to_string(worm->wave_amp) + ",";
    theConfig += std::to_string(worm->wave_freq) + ",";
    theConfig += std::to_string(worm->numStates) + ", ";
    theConfig += std::to_string(worm->leftRightOffsetPercent) + ", ";
    theConfig += std::to_string(worm->muscleToMuscleOffsetPercent) + ", ";
    theConfig += std::to_string(worm->forceMultiplierOffset) + ", ";
    theConfig += "\n";

    std::ofstream params(worm->evolveFileNextGeneration, std::ofstream::app);
    params << theConfig;
    params.close();
    
    std::vector<float> tmp_vec;
    tmp_vec.push_back(config_index);
    tmp_vec.push_back(worm->currentUtilityIndex);
    utilityAndPairing_local_.push_back(tmp_vec);
    
    std::sort(std::begin(utilityAndPairing_local_),
              std::end(utilityAndPairing_local_),
              [](const std::vector<float> &a, const std::vector<float> &b){
                  return (a[1] < b[1]);
              });
    
    std::ofstream meta(worm->evolveMetaFileNextGeneration, std::ofstream::trunc);
    auto metaData = std::to_string(config_index) + "\n";
    auto num_pairings = utilityAndPairing_local_.size();
    for(auto i = 0; i < num_pairings; ++i){
        metaData += std::to_string(utilityAndPairing_local_[i][0]) + "," + std::to_string(utilityAndPairing_local_[i][1]) + "\n";
    }
    
    meta << metaData;
    meta.close();
    
    ++config_index;
}
