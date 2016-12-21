//
//  BodyParameterSet.cpp
//  PhunWithPhysics
//
//  Created by onyx on 3/17/16.
//  Copyright © 2016 AweM. All rights reserved.
//

#include "BodyParameterSet.hpp"

// default behavior is to use default parameter set.

BodyParameterSet::BodyParameterSet(){
    
    evolve = false;
    useBest = false;
    useBest_n = 0; // use best config by default
    executableSessionNumber = 0; // used when running multiple instances at once.
    num_muscles = 95;
    num_cc = 50;
    num_sections = 24;
    cylinderLen = 1.25;
    
    //----- Genetic parameters ---------------------------------------------------------------------
    reproduce = false;
    if(reproduce){
        generationNumber = GeneticUtils::findGenerationNumber(executableSessionNumber);
    }
    doing_darwin = false;
    //-----
    
    // http://www.ucl.ac.uk/~ucbtdag/McCulloch.pdf
    float wormVolumeMM = .0064; // mm^3
    float waterDensity = 1000; // kg/m^3
    organicWormMass =  (wormVolumeMM*1e-9)*waterDensity; // convert mm^3 to m^3, then multiply by water density, get kg
    lengthScale = 10000; // worm is 1.2 mm (0.0012 m), our worm is 120 m, so. scaled by 10,000
    volumeScale = lengthScale*lengthScale*lengthScale;
    scaledWormMass = volumeScale * organicWormMass;
    eachPartMass = scaledWormMass/(num_muscles+num_cc);
    // we're going to do 10%-80%-10% to divvy up the muscle mass across the two outer spheres, and center sphere.
    ///////////// FAKE
    //eachPartMass = 1.f;
    muscleCenterSphereMass = eachPartMass*.6f;
    muscleOuterSphereMass = eachPartMass*.2f; // there are two of these in every muscle
    centerCylinderMass = eachPartMass;
   
    saveDefaultConfigAndUtility = false;
    
    std::random_device rd;
    twister = std::mt19937(rd()); // use random_device to seed mersenne twister
    centerCylinderChainConSprings = new BodyParameterSet::SpringConfig[6];
    muscleConSprings = new BodyParameterSet::SpringConfig[6];
    centerToMuscleConSprings = new BodyParameterSet::SpringConfig[6];
    centerMuscleSupportSprings = new BodyParameterSet::SpringConfig[6];
    muscleEndToCenterCylinderSprings = new BodyParameterSet::SpringConfig[6];
    endsOfMusclesSupportSprings = new BodyParameterSet::SpringConfig[6];
    
    ///////////////////////////
    currentUtilityIndex = 0.f;
    ////////////// SECTION
    centerSphereAnisotropicFriction = btVector3(2.f, 1.f, .1f);
    groundRestitution = .5;// randFloatInRange(.3,.7); //.5;
    groundFriction = .5;// randFloatInRange(.2,1); // .5
    muscleRestitution = .5;//randFloatInRange(.1,1);//.65;
    ////////////// SECTION 1 !!!!!!!!!
    for (int i = 0; i < 6; i++){
        // setting all to not enabled, so values don't actually matter,
        // except for 3 and 4, which are enabled below --
        centerCylinderChainConSprings[i].stiffness =  5e6;
        centerCylinderChainConSprings[i].damping = 1;
        centerCylinderChainConSprings[i].bounce = 0;
        centerCylinderChainConSprings[i].enabled = false;
        centerCylinderChainConSprings[i].lowerLim = 0.0f;
        centerCylinderChainConSprings[i].upperLim = 0.0f;
    }
    centerCylinderChainConSprings[4].lowerLim = -M_PI/2.0f;
    centerCylinderChainConSprings[4].upperLim = M_PI/2.0f;
    centerCylinderChainConSprings[4].enabled = true;
    ////////////// SECTION 2 !!!!!!!!!!!!!!!!!!!
    for (int i = 0; i < 6; i++){ //lateral
        muscleConSprings[i].stiffness = 5e5;
        muscleConSprings[i].damping = 1.0f;
        muscleConSprings[i].bounce = 0;
        muscleConSprings[i].enabled = false;
        muscleConSprings[i].lowerLim = 0;//randFloatInRange(-4.0f,-2.0f);//-3.0f;
        muscleConSprings[i].upperLim = 0;//randFloatInRange(2.0f,4.0f);//3.0f;
    }
    // x
    muscleConSprings[0].lowerLim = -.5f;
    muscleConSprings[0].upperLim = .5f;
    muscleConSprings[0].enabled = true;
    // z
    muscleConSprings[2].lowerLim = -1.f;
    muscleConSprings[2].upperLim = 3.5f;
    muscleConSprings[2].enabled = true;
    // y rotational
    muscleConSprings[4].lowerLim = -M_PI/3.f;
    muscleConSprings[4].upperLim = M_PI/3.f;
    muscleConSprings[4].enabled = true;
    /////////////// SECTION 3 !!!!!!!!!
    for (int i = 0; i < 6; i++){
        centerToMuscleConSprings[i].stiffness = 1e6;
        centerToMuscleConSprings[i].damping = 1.0f;
        centerToMuscleConSprings[i].bounce = 0;
        centerToMuscleConSprings[i].enabled = false;
        centerToMuscleConSprings[i].lowerLim = 0;//randFloatInRange(-1.0f,-.01f);//-0.1f;
        centerToMuscleConSprings[i].upperLim = 0;//randFloatInRange(.01f,1.0f);//0.1f;
    }
    //////////////// SECTION 4 !!!!!!!!!
    for (int i = 0; i < 6; i++){ // setting all to false, will be enabled based upon quadrant
        centerMuscleSupportSprings[i].stiffness = 1e6;
        centerMuscleSupportSprings[i].damping =  1.0f;
        centerMuscleSupportSprings[i].bounce = 0.0f;
        centerMuscleSupportSprings[i].enabled = false;
        centerMuscleSupportSprings[i].lowerLim = 0; //randFloatInRange(-1.0f,-.01f);//-0.1f;
        centerMuscleSupportSprings[i].upperLim = 0; //randFloatInRange(.01f,1.0f);//0.1f;
    }
    /////////////// SECTION 5
    for (int i = 0; i < 6; i++){
        muscleEndToCenterCylinderSprings[i].stiffness = 1e6;
        muscleEndToCenterCylinderSprings[i].damping = 1.0f;
        muscleEndToCenterCylinderSprings[i].bounce = 0.0f;
        muscleEndToCenterCylinderSprings[i].enabled = false;
        muscleEndToCenterCylinderSprings[i].lowerLim = 0;//randFloatInRange(-1.0f,-.01f);//-.1f;
        muscleEndToCenterCylinderSprings[i].upperLim = 0;//randFloatInRange(.01f,1.0f);//.1f;
    }
    /////////////// SECTION 6
    for (int i = 0; i < 6; i++){ // go through all 6, then fix last 3 later
        endsOfMusclesSupportSprings[i].stiffness =  1e6;
        endsOfMusclesSupportSprings[i].damping =  1.0f;
        endsOfMusclesSupportSprings[i].bounce =  0.0f;
        endsOfMusclesSupportSprings[i].enabled = false;
        endsOfMusclesSupportSprings[i].lowerLim = 0;//randFloatInRange(-1.0f,-.01f);//-0.1f;
        endsOfMusclesSupportSprings[i].upperLim = 0;//randFloatInRange(.01f,1.0f);//0.1f;
    }
    endsOfMusclesSupportSprings[2].lowerLim = -1;//randFloatInRange(-2.0f,-.01f);//-1.0f;
    endsOfMusclesSupportSprings[2].upperLim = 1;//randFloatInRange(.01f,2.0f);//1.0f;
    endsOfMusclesSupportSprings[2].enabled = true;//randFloatInRange(.01f,2.0f);//1.0f;
    
    wave_amp = 75.f;//randFloatInRange(100.f, 500.f);
    wave_freq = 1.f;//randFloatInRange(.,3.f);
    numStates = 500;
    leftRightOffsetPercent = 30.f; // percent
    muscleToMuscleOffsetPercent = 5.f; // percent
    forceMultiplierOffset = 27.f;

    if (!FileUtils::fexists(paramFile)){ // if we don't have any parameters saved, let's save the default set.
        saveDefaultConfigAndUtility = true;
    }
}



void BodyParameterSet::assignRandomizedValues(){
    
    currentUtilityIndex = 0.f;
    ////////////// SECTION
    float anisotropic_x = randFloatInRange(4,7);
    float anisotropic_y = 1.f;//randFloatInRange(.01,3);
    float anisotropic_z = randFloatInRange(.01,.1);
    centerSphereAnisotropicFriction = btVector3(anisotropic_x,anisotropic_y,anisotropic_z);
    
    std::vector<float> stiffStiffnessVector = {1e7, 5e7, 1e8, 5e8, 1e9, 5e9, 1e10, 5e10, 1e11, 5e11, 1e12, 5e12, 1e13, 5e13};
    std::vector<float> mediumStiffnessVector = {1e4, 5e4, 1e5, 5e5, 1e6, 5e6, 1e7, 5e7, 1e8, 5e8, 1e9, 5e9, 1e10, 5e10};
    
    std::vector<float> plushStiffnessVector = { 1e3, 5e3, 1e4, 5e4, 1e5, 5e5, 1e6, 5e6, 1e7, 5e7, 1e8, 5e8, 1e9, 5e9};
    std::vector<float> dampingVector = {.2, .3, .4, .5, .6, .7, .8, .9, 1, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2, 2.1, 2.2, 2.3, 2.4, 2.5, 2.6, 2.7, 2.8, 2.9, 3, 3.1, 3.2, 3.3, 3.4, 3.5};
    // y rotational
    centerCylinderChainConSprings[4].stiffness = stiffStiffnessVector[randInt((int)stiffStiffnessVector.size())];
    centerCylinderChainConSprings[4].damping = dampingVector[randInt((int)dampingVector.size())];
    
    // x lateral
    muscleConSprings[0].stiffness = stiffStiffnessVector[randInt((int)stiffStiffnessVector.size())];
    muscleConSprings[0].damping = dampingVector[randInt((int)dampingVector.size())];
    float muscleConLateralX = randFloatInRange(0.2,1.f);
    muscleConSprings[0].lowerLim = -muscleConLateralX; // -.5
    muscleConSprings[0].upperLim = muscleConLateralX; // .5
    
    // z lateral
    muscleConSprings[2].stiffness = mediumStiffnessVector[randInt((int)stiffStiffnessVector.size())];
    muscleConSprings[2].damping = dampingVector[randInt((int)dampingVector.size())];
    muscleConSprings[2].lowerLim = randFloatInRange(-5.f,-1.f); // -1
    muscleConSprings[2].upperLim = randFloatInRange(1.f,5.f); // 3.5
    
    // y rotational
    muscleConSprings[4].stiffness = stiffStiffnessVector[randInt((int)stiffStiffnessVector.size())];
    muscleConSprings[4].damping = dampingVector[randInt((int)dampingVector.size())];
    
    // z lateral
    endsOfMusclesSupportSprings[2].stiffness = plushStiffnessVector[randInt((int)plushStiffnessVector.size())];
    endsOfMusclesSupportSprings[2].damping = dampingVector[randInt((int)dampingVector.size())];
    
    float endOfMuscleLateralZ = randFloatInRange(0.25f,4.f);
    endsOfMusclesSupportSprings[2].lowerLim = -endOfMuscleLateralZ; //-1
    endsOfMusclesSupportSprings[2].upperLim = endOfMuscleLateralZ; //-1
    
    
    std::vector<float> waveAmpVector = {75, 80, 85, 90, 95, 100, 105, 110, 115, 120, 125, 130, 135, 140, 145, 150, 155, 160, 165, 170, 175};
    std::vector<int> numStatesVector = {50, 55, 60, 65, 70, 75, 80, 85, 90, 95, 100, 105, 110, 115, 120, 125, 130, 135, 140, 145, 150, 155, 160, 165, 170, 175, 180, 185, 190, 195, 200};
    wave_amp = waveAmpVector[randInt((int)waveAmpVector.size())];
    wave_freq = 1.f;//randFloatInRange(.,3.f);
    numStates = numStatesVector[randInt((int)numStatesVector.size())];
    std::vector<float> LROffsetPercentsVector = {15, 17.5, 20, 22.5, 25, 27.5, 30, 32.5, 35, 37.5, 40, 42.5, 45, 47.5, 50, 52.5, 55, 57.5, 60, 62.5, 65, 67.5, 70, 72.5, 75, 77.5, 80, 82.5, 85};
    std::vector<float> muscleOffsetPercentsVector = { 2.5, 5, 7.5, 10, 12.5, 15, 17.5, 20, 22.5, 25, 27.5, 30, 32.5, 35, 37.5, 40, 42.5, 45};
    leftRightOffsetPercent = LROffsetPercentsVector[randInt((int)LROffsetPercentsVector.size())];
    muscleToMuscleOffsetPercent = muscleOffsetPercentsVector[randInt((int)muscleOffsetPercentsVector.size())];
    forceMultiplierOffset = 20 + randInt(21);
}

std::string vecToCommaDelimitedString( std::vector<std::string>& vec){
    std::string s = "";
    auto num_entries = vec.size();
    for (auto i = 0; i < num_entries; i++){
        s += vec[i]+",";
    }
    s.pop_back(); // remove trailing comma
    return s;
}

/* this also chops off the configuration index (index 0)
 */
void BodyParameterSet::readMetaData(){
    std::vector<std::string> metaData;
    if(reproduce){
        metaData = FileUtils::readLineByLine(evolveMetaFile);
    }
    else{
        metaData = FileUtils::readLineByLine(paramMetaData);
    }
    
    auto config_index_from_file = NumUtils::strToInt(metaData[0]);
    //int config_index_from_file = (int)NumUtils::strToDouble(StrUtils::parseOnCharDelim(metaData[0],',')[0]);
    metaData.erase(metaData.begin());
    auto num_entries = metaData.size();
    // need to populate utilityIndexList
    for (int i = 0; i < num_entries; i++){
        std::vector<std::string> temp_s = StrUtils::parseOnCharDelim(metaData[i],',');
        std::vector<float> temp_vec;
        float t_config_index = (float)NumUtils::strToDouble(temp_s[0]);
        //if (reproduce && (t_config_index > numWormsInGeneration)) continue;
        if (t_config_index > configIndex){// configIndex defaults to -1
            configIndex = (int)t_config_index;
        }
        temp_vec.push_back(t_config_index); // entry index
        temp_vec.push_back((float)NumUtils::strToDouble(temp_s[1])); // corresponding utility index
        utilityIndexAndEntryPairing.push_back(temp_vec);
    }
    if (config_index_from_file != configIndex){ // then something potentially undesireable has happened
        log_.fatal("config_index_from_file != configIndex", FUNC);
        index_set = false;
        throw std::runtime_error("Oh brother! config_index_from_file != configIndex ... :( ("+std::to_string(config_index_from_file)+"), ("+std::to_string(configIndex)+")");
    }
    index_set = true;
}

/* converts an array of 6 SpringConfig objects and a name, to a string
 
    format (i = 0 .. 5):
    identifier,stiffness_i,damping_i,bounce_i,enabled_i,lower_limit_i,upper_limit_i,(repeat 5 more times),-1*identifier
 
    NOTE: 'identifier' is an enum, such that the value passed in to springConfigArrayToString should be negative --
    that is, the enum should start with "BEGIN_". Once multiplied by -1, it will match the corresponding identifier
    that starts with "END_". For example, "BEGIN_MUSCLE_CON" is -5, and "END_MUSCLE_CON" is 5.
 
    NOTE 2: NO COMMAS OR SPACES ARE ADDED TO THE BEGINNING OR END OF THE STRING!!!
 */
std::string BodyParameterSet::springConfigArrayToString(SpringConfig* sc, BodyParameterSet::SpringConfigOrdering identifier){
    // 6 string config objects
    int num_configs = 6;
    std::string theString = std::to_string(identifier)+",";
    for (auto i = 0; i < num_configs; i++){
        theString +=
        std::to_string(sc[i].stiffness) + ","
        + std::to_string(sc[i].damping) + ","
        + std::to_string(sc[i].bounce) + ","
        + std::to_string(sc[i].enabled) + ","
        + std::to_string(sc[i].lowerLim) + ","
        + std::to_string(sc[i].upperLim) + ",";
    }
    theString += std::to_string((-1*identifier));// let the loop put the comma on before 'end...'
    return theString;
}



void BodyParameterSet::saveConfiguration(){
    
    if (!index_set){
        if (FileUtils::fexists(paramMetaData)){
            readMetaData(); // this will set the configIndex
        }
        else { // there's no file, so we create it, indexing starts at 0.
            configIndex = 0;
            index_set = true;
        }
    }
    std::ofstream params;
    if(reproduce){
        params.open(evolveFileNextGeneration, std::ofstream::app);
    }
    else{
        params.open(paramFile,std::ofstream::app);
    }
    
    // now we create a string from our param set
    if(!reproduce)     // Darwin needs to control the config index during reproduction
        configIndex++; // before this line, configIndex was the last one... now we're adding one, so we increment
    std::string theConfig = std::to_string(configIndex) + "," + std::to_string(currentUtilityIndex) + ",";
    theConfig += std::to_string(centerSphereAnisotropicFriction[0]) + ",";
    theConfig += std::to_string(centerSphereAnisotropicFriction[1]) + ",";
    theConfig += std::to_string(centerSphereAnisotropicFriction[2]) + ",";
    theConfig += std::to_string(groundRestitution) + ",";
    theConfig += std::to_string(groundFriction) + ",";
    theConfig += std::to_string(muscleRestitution) + ",";
    theConfig += springConfigArrayToString(centerCylinderChainConSprings,SpringConfigOrdering::BEGIN_CENTER_CYLINDER_CHAIN_CON) + ",";
    theConfig += springConfigArrayToString(muscleConSprings,SpringConfigOrdering::BEGIN_MUSCLE_CON) + ",";
    theConfig += springConfigArrayToString(centerToMuscleConSprings,SpringConfigOrdering::BEGIN_CENTER_TO_MUSCLE) + ",";
    theConfig += springConfigArrayToString(centerMuscleSupportSprings,SpringConfigOrdering::BEGIN_CENTER_MUSCLE_SUPPORT) + ",";
    theConfig += springConfigArrayToString(muscleEndToCenterCylinderSprings,SpringConfigOrdering::BEGIN_MUSCLE_END_TO_CENTER_CYLINDER) + ",";
    theConfig += springConfigArrayToString(endsOfMusclesSupportSprings,SpringConfigOrdering::BEGIN_ENDS_OF_MUSCLES_SUPPORT) + ",";
    theConfig += std::to_string(wave_amp) + ",";
    theConfig += std::to_string(wave_freq) + ",";
    theConfig += std::to_string(numStates) + ", ";
    theConfig += std::to_string(leftRightOffsetPercent) + ", ";
    theConfig += std::to_string(muscleToMuscleOffsetPercent) + ", ";
    theConfig += std::to_string(forceMultiplierOffset) + ", ";
    theConfig += "\n";
    // now we write this string to our params file
    params.write(theConfig.c_str(), theConfig.size());
    params.close();
    // now we have to update the metadata file with the index and utility (and keep it in sorted order)
    std::vector<float> temp_vec;
    temp_vec.push_back(configIndex);
    temp_vec.push_back(currentUtilityIndex);
    utilityIndexAndEntryPairing.push_back(temp_vec);
    // now re-sort
    std::sort(utilityIndexAndEntryPairing.begin(), utilityIndexAndEntryPairing.end(),
              [](const std::vector<float> a, const std::vector<float> b){
                  return (a[1] < b[1]);
              });
    //and re-write the utility pairings
    std::ofstream paramMeta;
#warning this sucks
    if(reproduce){
        paramMeta.open(evolveMetaFileNextGeneration, std::ofstream::trunc);
        std::vector<std::vector<float>> tmp;
        for(auto i = 0; i < numWormsInGeneration; ++i){
            tmp.push_back(utilityIndexAndEntryPairing[i]);
            tmp[i][0] = i;
        }
        utilityIndexAndEntryPairing = tmp;
    }
    else{
        paramMeta.open(paramMetaData,std::ofstream::trunc);
    }
    std::string metaData = std::to_string(configIndex) + "\n";
    auto num_pairings = utilityIndexAndEntryPairing.size();
    for (auto i = 0; i < num_pairings; i++){
        metaData += std::to_string(utilityIndexAndEntryPairing[i][0]) + "," + std::to_string(utilityIndexAndEntryPairing[i][1]) + "\n";
    }
    paramMeta.write(metaData.c_str(), metaData.size());
    paramMeta.close();
}

bool BodyParameterSet::seans_lambda_failed(const std::vector<float> &a){
    return a[0] >= numWormsInGeneration;
}

/* sets all configs for a single spring type (all 6 DOF, that is.)
 *
 * returns the last index that it used
 */
int BodyParameterSet::setSavedSpringConfig(SpringConfig* sc, std::vector<float>& vec, int startIndex){
    auto cur = startIndex;
    auto num_configs = 6;
    for (auto i = 0; i < num_configs; i++){
        sc[i].stiffness = vec[cur]; // stiffness
        sc[i].damping = vec[cur+1]; // damping
        sc[i].bounce = vec[cur+2]; // bounce
        sc[i].enabled = (int) vec[cur+3]; // enabled
        sc[i].lowerLim = vec[cur+4]; // lower limit
        sc[i].upperLim = vec[cur+5]; // upper limit
        cur += 6;// add 6 because we added 5 for the entries, so by adding one extra, we can start at cur again for the next loop
    }
    return cur; // return 'cur', because it points to the "END_..." enum for this set of configs, and that's effectively 'used' now, so, we really are returning the last used.
}

/* takes the current loaded row of parameters, and loads the spring configurations.
 
 returns the index of the last used value in the input array (so that if there's more data to proess,
 we know where to start doing so.
 */
int BodyParameterSet::setSavedSpringConfigs(std::vector<float> vec, int startIndex){
    if ((int)vec[startIndex] != SpringConfigOrdering::BEGIN_CENTER_CYLINDER_CHAIN_CON){
        log_.fatal("ruh roh raggy", FUNC);
        throw std::runtime_error("ruh rou raggy! Your spring configs don't look quite right, "+std::to_string((int)vec[startIndex])+"... (starting at index"+std::to_string(startIndex)+")");
    }
    int cur = startIndex+1;
    int last_used = setSavedSpringConfig(centerCylinderChainConSprings, vec, cur);
    cur = last_used + 2; // add 2 because we have to get past the last used one, and the "BEGIN_..." enum
    last_used = setSavedSpringConfig(muscleConSprings, vec, cur);
    cur = last_used + 2;
    last_used = setSavedSpringConfig(centerToMuscleConSprings, vec, cur);
    cur = last_used + 2;
    last_used = setSavedSpringConfig(centerMuscleSupportSprings, vec, cur);
    cur = last_used + 2;
    last_used = setSavedSpringConfig(muscleEndToCenterCylinderSprings, vec, cur);
    cur = last_used + 2;
    last_used = setSavedSpringConfig(endsOfMusclesSupportSprings, vec, cur);
    if ((int)vec[last_used] != SpringConfigOrdering::END_ENDS_OF_MUSCLES_SUPPORT){
        log_.fatal("ruh roh raggy", FUNC);
        throw std::runtime_error("ruh rou raggy! Your spring configs don't look quite right... (ended at index"+std::to_string(cur)+")");
    }
    return last_used; // return 'last_used', because it points to the last "END_.." enum, so, effectively, it is the last used... in order to use more data in 'vec', that value will have to be incremented.
}

float BodyParameterSet::stof(std::string s){
    return ((float)NumUtils::strToDouble(s));
}

/* searches the metadata file for the nth best, and returns its entry index in the parameter file
 
 NOTE: the parameter file cannot be assumed to have entries in order of entry index...
 the first column of the nth best configuration in the parameter file will match with the
 value returned by find_nth_BestConfiguration (this function)
 */
int BodyParameterSet::find_nth_BestConfiguration(int n){
    if(reproduce && doing_darwin){
        paramFile = evolveFileNextGeneration;
        paramMetaData = evolveMetaFileNextGeneration;
    }
    else if(reproduce && (!doing_darwin)){
        paramFile = evolveFile;
        paramMetaData = evolveMetaFile;
    }
    if (!FileUtils::fexists(paramMetaData)){
        log_.fatal("No parameter metadata file", FUNC);
        throw std::runtime_error("... There is no parameter metadata file. Unless some jenius (yeah, with a j) delete it, there are probably no saved configurations");
    }
    if (!index_set){ // if index is set, we have the data already, in utilityIndexAndEntryPairing
        readMetaData();
    }
    // since that pairing/list is already sorted, we just go down to the nth entry, and get the first index - the entry index (second is utility index)
    if (n > utilityIndexAndEntryPairing.size()-1){ // ruh-roh, we don't have an nth best!
        log_.fatal("Don't have that many configs", FUNC);
        throw std::runtime_error("We don't even have that many configurations");
    }
    printf ("Found configuration at position %d -- utility value is: %f\n",n,utilityIndexAndEntryPairing[n][1]);
    return ((int)utilityIndexAndEntryPairing[n][0]);
}

/* NOTE NOTE NOTE NOTE NOTE: start at 0. 0 is the best. not 1. 0. zero. */
void BodyParameterSet::load_nth_BestConfiguration(std::vector<std::vector<std::string>>& cells, int n){
    auto nth_best_entry_index = find_nth_BestConfiguration(n);
    auto num_entries = cells.size();
    std::cout << "nth_best_en idx: " << nth_best_entry_index << "\n";
    std::cout << "num entries: " << num_entries << std::endl;
    int cell_index = -1;
    for (auto i = 0; i < num_entries; i++){
        if (((int)NumUtils::strToDouble(cells[i][0])) == nth_best_entry_index){
            cell_index = i;
            break;
        }
    }
    if (cell_index == -1){
        log_.fatal("Unable to locate desired config", FUNC);
        throw std::runtime_error("Unfortunately, I was unable to locate the configuration you desire");
    }
    // now we load the configuration... indices 0 and 1 are the config/entry index and utility indices.
    //std::vector<string>
    /* STOPPING POINT: need to convert contents of cell row to floats, figure out how to deal with bools */
    std::vector<float> currentRow;
    auto num_cells = cells[cell_index].size();
    // convert entire row to floats
    for(auto i = 0; i < num_cells; i++){
        currentRow.push_back(stof(cells[cell_index][i]));
    }
    // not saving the entry index at the moment... don't think we need to load it. it is in index 0.
    currentUtilityIndex = currentRow[1];
    centerSphereAnisotropicFriction = btVector3(currentRow[2],currentRow[3], currentRow[4]);
    groundRestitution = currentRow[5];
    groundFriction = currentRow[6];
    muscleRestitution = currentRow[7];
    // so, index 9 has the start of the first set of spring configs.
    int lastUsedIndex = setSavedSpringConfigs(currentRow,8);
    // now we load the signal information
    wave_amp = currentRow[lastUsedIndex+1];
    wave_freq = currentRow[lastUsedIndex+2];
    numStates = currentRow[lastUsedIndex+3];
    leftRightOffsetPercent = currentRow[lastUsedIndex+4];
    muscleToMuscleOffsetPercent = currentRow[lastUsedIndex+5];
    forceMultiplierOffset = currentRow[lastUsedIndex+6];
    // NOTE: for later, we'll put the indices of parameter sets that were combined, and a 'method index' at the end,
    // saying how they were combined.
}

/* loads params.
 *
 * default parameter set is used if neither evolver nor useBest are true.
 *
 */
void BodyParameterSet::loadParams(){
    
    std::vector<std::string> lines;
    std::vector<std::vector<std::string>> cells;
    
    if(evolve){
        if(reproduce)
            loadEvolvedWorm();
        else
            assignRandomizedValues();
    }
    // load csv with saved parameters
    else if(BodyParameterSet::useBest && FileUtils::fexists(paramFile)){
        lines = FileUtils::readLineByLine(paramFile);
        auto num_lines = lines.size();
        
        for(auto i = 0; i < num_lines; i++){
            cells.push_back(StrUtils::parseOnCharDelim(lines[i],','));
        }
        // here we have a vector of vectors, each of strings
        int n = 0;
        log_.info("Loading nth best configuration -- (n == " + std::to_string(n) + ").", FUNC);
        printf("Loading nth best configuration -- (n == %d).\n",n);
        load_nth_BestConfiguration(cells,useBest_n);
    }
    else{
        printf("Using default parameter set.\n");
        log_.debug("Default parameter set", FUNC);
    }
    
}

/* thanks to 'John Dibling':
 * http://stackoverflow.com/questions/686353/c-random-float-number-generation
 *
 */
float BodyParameterSet::randFloatInRange(float LO, float HI){
    return LO + (static_cast <float> (twister()) /( static_cast <float> (twister.max()/(HI-LO))));
}

// 0 - outer, not including outer. uses floats and then returns an int -- this automatically rounds down.
int BodyParameterSet::randInt(int OUTER){
    return (static_cast <float> (twister()) /( static_cast <float> (twister.max()/OUTER)));
}

/**
 * This function keeps track of the worm number that is being evaluated, loads
 * that worm number using load_nth_BestConfiguration and then that worm is
 * evaulated using the fitness function in TheRealDeal
 */
void BodyParameterSet::loadEvolvedWorm(){
    // NOTE: This will return merged_wbp... when generation number == 0, else worm_body_params...
    evolveFile = GeneticUtils::getParamsFilePath(std::to_string(executableSessionNumber));
    evolveMetaFile = GeneticUtils::getMetaFilePath(std::to_string(executableSessionNumber));
    evolveFileNextGeneration = GeneticUtils::getNextGenParamsPath(std::to_string(executableSessionNumber));
    evolveMetaFileNextGeneration = GeneticUtils::getNextGenMetaPath(std::to_string(executableSessionNumber));
    
    if(!FileUtils::fexists(evolveFile))
        throw std::runtime_error(evolveFile + " failed fexists check");
    
    auto lines{FileUtils::readLineByLine(evolveFile)};
    std::vector<std::vector<std::string>> cells;
    for(auto &&line : lines){
        cells.emplace_back(StrUtils::parseOnCharDelim(line, ','));
    }
    
    /*
    printf("Loading worm %d from generation %d for fitness testing\n", currentWormNumber, generationNumber);
    printf("The metafile is: %s\n", evolveMetaFile.c_str());
    printf("The worm comes from: %s\n\n\n", evolveFile.c_str());
    */
    load_nth_BestConfiguration(cells, currentWormNumber);
}
