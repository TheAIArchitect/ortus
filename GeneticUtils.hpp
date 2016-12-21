//
//  GeneticUtils.hpp
//  delegans
//
//  Created by Sean Grimes on 6/13/16.
//  Copyright © 2016 Sean Grimes. All rights reserved.
//

#pragma once

#include <iostream>
#include <fstream>
#include "FileUtils.hpp"
#include "StrUtils.hpp"
#include "NumUtils.hpp"

class GeneticUtils{
public:
    // Reads the generation_and_current_worm.txt file so we can determine which generation we're on
    // between runs
    static int findGenerationNumber(int eSN);
    static int findGenerationNumber(const std::string &eSN);
    
    // Updates the generation_and_current_worm.txt file. Returns the old generation number that was
    // in the file incase it's needed for something that I'm about to write
    static int updateGenerationNumber(int eSN, int gen_number);
    static int updateGenerationNumber(const std::string &eSN, int gen_number);
    
    // Reads the generation_and_current_worm.txt file and returns the current worm number
    static int findWormNumber(int eSN);
    static int findWormNumber(const std::string &eSN);
    
    // Updates the generation_and_current_worm.txt file and resutns the old number for the worm
    static int updateWormNumber(int eSN, int worm_number);
    static int updateWormNumber(const std::string &eSN, int worm_number);
    
    static std::string getParamsFilePath(const std::string &eSN);
    static std::string getMetaFilePath(const std::string &eSN);
    static std::string getNextGenParamsPath(const std::string &eSN);
    static std::string getNextGenMetaPath(const std::string &eSN);
    
private:
    static std::vector<std::string> readAndParseFile(const std::string &eSN);
    static std::string getGenFilePath(const std::string &eSN);
    
private:
    static const int generationPos{0};
    static const int wormPos{1};
};
