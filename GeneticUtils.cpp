//
//  GeneticUtils.cpp
//  delegans
//
//  Created by Sean Grimes on 6/13/16.
//  Copyright © 2016 Sean Grimes. All rights reserved.
//

#include "GeneticUtils.hpp"


int GeneticUtils::findGenerationNumber(int eSN){
    return findGenerationNumber(std::to_string(eSN));
}

int GeneticUtils::findGenerationNumber(const std::string &eSN){
    auto nums = readAndParseFile(eSN);
    return NumUtils::strToInt(nums[generationPos]);
}

int GeneticUtils::updateGenerationNumber(int eSN, int gen_number){
    return updateGenerationNumber(std::to_string(eSN), gen_number);
}

int GeneticUtils::updateGenerationNumber(const std::string &eSN, int gen_number){
    auto old_gen_num = findGenerationNumber(eSN);
    auto cur_worm_num = findWormNumber(eSN);
    std::ofstream out{getGenFilePath(eSN)};
    out << gen_number << "," << cur_worm_num;
    return old_gen_num;
}

int GeneticUtils::findWormNumber(int eSN){
    return findWormNumber(std::to_string(eSN));
}

int GeneticUtils::findWormNumber(const std::string &eSN){
    auto nums = readAndParseFile(eSN);
    return NumUtils::strToInt(nums[wormPos]);
}

int GeneticUtils::updateWormNumber(int eSN, int worm_number){
    return updateWormNumber(std::to_string(eSN), worm_number);
}

int GeneticUtils::updateWormNumber(const std::string &eSN, int worm_number){
    auto cur_gen_num = findGenerationNumber(eSN);
    auto old_worm_num = findWormNumber(eSN);
    std::ofstream out{getGenFilePath(eSN)};
    out << cur_gen_num << "," << worm_number;
    return old_worm_num;
}

std::vector<std::string> GeneticUtils::readAndParseFile(const std::string &eSN){
    const std::string gen_file = getGenFilePath(eSN);
    if(!FileUtils::fexists(gen_file)){
        // Make sure the directory exists
        FileUtils::mkdirWrapper("evolved_worms/gen_files");
        std::ofstream out(gen_file);
        out << "0,0";
        out.close();
    }
    return StrUtils::parseOnCharDelim(StrUtils::trim(FileUtils::readFullFile(gen_file)),',');
}

std::string GeneticUtils::getGenFilePath(const std::string &eSN){
    return std::string{"evolved_worms/gen_files/generation_and_current_worm_"+eSN+".txt"};
}

std::string GeneticUtils::getParamsFilePath(const std::string &eSN){
    std::string dir;
    std::string params;
    auto gen_num = findGenerationNumber(eSN);
    if(gen_num != 0){
        dir = "evolved_worms/generation_"+std::to_string(gen_num)+"/";
        params = dir+"worm_body_params_"+eSN+".txt";
    }
    else{
        dir = "evolved_worms/generation_0/";
        params = dir+"merged_wbp.txt";
    }
    return params;
}

std::string GeneticUtils::getMetaFilePath(const std::string &eSN){
    std::string dir;
    std::string meta;
    auto gen_num = findGenerationNumber(eSN);
    if(gen_num != 0){
        dir = "evolved_worms/generation_"+std::to_string(gen_num)+"/";
        meta = dir+"worm_body_params_meta_"+eSN+".txt";
    }
    else{
        dir = "evolved_worms/generation_0/";
        meta = dir+"merged_wbp_meta.txt";
    }
    return meta;
}

std::string GeneticUtils::getNextGenParamsPath(const std::string &eSN){
    std::string dir;
    std::string params;
    auto next_gen_num = findGenerationNumber(eSN) + 1;
    dir = "evolved_worms/generation_"+std::to_string(next_gen_num)+"/";
    params = dir+"worm_body_params_"+eSN+".txt";
    return params;
}

std::string GeneticUtils::getNextGenMetaPath(const std::string &eSN){
    std::string dir;
    std::string meta;
    auto next_gen_num = findGenerationNumber(eSN) + 1;
    dir = "evolved_worms/generation_"+std::to_string(next_gen_num)+"/";
    meta = dir+"worm_body_params_meta_"+eSN+".txt";
    return meta;
}









