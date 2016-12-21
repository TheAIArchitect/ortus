//
//  BodyParameterSet.hpp
//  PhunWithPhysics
//
//  Created by onyx on 3/17/16.
//  Copyright © 2016 AweM. All rights reserved.
//

#pragma once

#include <stdio.h>
#include <iostream>
#include "Logger.hpp"
#include "FileUtils.hpp"
#include "StrUtils.hpp"
#include "NumUtils.hpp"
#include <random>
#include "BulletDynamics/btBulletDynamicsCommon.h"
#include <algorithm>
#include <vector>
#include "Logger.hpp"
#include "GeneticUtils.hpp"

class BodyParameterSet{
    
public:
    BodyParameterSet();
    std::mt19937 twister;
    
    //----- Genetic parameters ---------------------------------------------------------------------
    bool reproduce;
    std::string evolveFile;
    std::string evolveMetaFile;
    std::string generationNumberFile;
    std::string evolveFileNextGeneration;
    std::string evolveMetaFileNextGeneration;
    int generationNumber;
    int numWormsInGeneration;
    int currentWormNumber;
    size_t numRandomWormsGeneratedInCurrentGeneration{0};
    bool seans_lambda_failed(const std::vector<float> &a);
    bool doing_darwin;
    //----- End genetic params ---------------------------------------------------------------------
    
    
    bool evolve;
    bool useBest;
    int useBest_n;
    int executableSessionNumber;
    bool saveDefaultConfigAndUtility;
    int num_sections;
    int num_muscles;
    int num_cc;
    float wave_amp;
    float wave_freq;
    float wave_offset_left;
    float wave_offset_right;
    std::string paramFile;
    std::string paramMetaData;
    
    float organicWormMass;
    float lengthScale;
    float volumeScale;
    float eachPartMass;
    float muscleCenterSphereMass;
    float muscleOuterSphereMass;
    float centerCylinderMass;
    float scaledWormMass;
    float cylinderLen;
    int numStates;
    float leftRightOffsetPercent; // percent
    float muscleToMuscleOffsetPercent; // percent
    float forceMultiplierOffset;
    
    bool index_set = false;
    int configIndex = -1;
    std::vector<std::vector<float>> utilityIndexAndEntryPairing;
    void readMetaData();
    
    /* the utility value of the current configuration */
    float currentUtilityIndex = 2.0f;
    
    struct SpringConfig {
        float stiffness;
        float damping;
        float bounce;
        float lowerLim;
        float upperLim;
        float enabled;
    };
    
    typedef enum SpringConfigOrdering {
        BEGIN_CENTER_CYLINDER_CHAIN_CON = -999,
        BEGIN_MUSCLE_CON = -998,
        BEGIN_CENTER_TO_MUSCLE = -997,
        BEGIN_CENTER_MUSCLE_SUPPORT = -996,
        BEGIN_MUSCLE_END_TO_CENTER_CYLINDER = -995,
        BEGIN_ENDS_OF_MUSCLES_SUPPORT = -994,
        NONE = 0,
        END_ENDS_OF_MUSCLES_SUPPORT = 994,
        END_MUSCLE_END_TO_CENTER_CYLINDER = 995,
        END_CENTER_MUSCLE_SUPPORT = 996,
        END_CENTER_TO_MUSCLE = 997,
        END_MUSCLE_CON = 998,
        END_CENTER_CYLINDER_CHAIN_CON = 999,
        
    } springConfigOrdering;
    
    /* bounciness of muslces (anterior, center, and posterior all same)*/
    float muscleRestitution;
    
    /* bounciness of ground (anterior, center, and posterior all same)*/
    float groundRestitution;
    
    /* air density. just kidding. it's ground friction. */
    float groundFriction;
    
    /* center sphere anisotropic friction */
    btVector3 centerSphereAnisotropicFriction;
    
    /* ground friction. kidding again. gravity. m/s */
    float gravity = -9.8f; //m/s2
    
    /* from centers of muscles to each end, all 6 DOF */
    SpringConfig* muscleConSprings;
    
    /* from centers of muscles to center cylinder chain, all 6 DOF */
    SpringConfig* centerToMuscleConSprings;
    
    /* center cylinder chain joints, all 6 DOF */
    SpringConfig* centerCylinderChainConSprings;
    
    /* ends of muscles to center cylinder chain, all 6 DOF */
    SpringConfig* muscleEndToCenterCylinderSprings;
    
    /* centers of muscles to eachother (in square-like manner), all 6 DOF */
    SpringConfig* centerMuscleSupportSprings;
    
    /* ends of muscles to centers of adjacent muscles generally like "/\",
     where the bottom represents the ends of two different muscles,
     , and the top represents the center sphere of another muscle, all 6 DOF */
    SpringConfig* endsOfMusclesSupportSprings;
    
public:
    /* within a range, of course */
    void assignRandomizedValues();
    
    /* returns a random float within a specified range */
    float randFloatInRange(float LO, float HI);
    
    /* random integer, NOT inclsive*/
    int randInt(int OUTER);
    
    /* loads parameters */
    void loadParams();
    
    /* saves current parameter configuation */
    void saveConfiguration();
    
    /* loads the nth best configuration based upon utility */
    void load_nth_BestConfiguration(std::vector<std::vector<std::string>>& cells, int n);
    
    /* gets the entry index of the nth best configuration from the metadata */
    int find_nth_BestConfiguration(int n);
    
    /* converts an array of SpringConfig objects, representing a single spring configuration, to a string */
    std::string springConfigArrayToString(SpringConfig* sc, SpringConfigOrdering identifier);
    
    /* sets saved spring configuations */
    int setSavedSpringConfigs(std::vector<float> vec, int startIndex);
    
    float stof(std::string s);
    
    /* sets a single config */
    int setSavedSpringConfig(SpringConfig* sc, std::vector<float>& vec, int startIndex);
    
    void loadEvolvedWorm();
    
    Logger log_;
    
};