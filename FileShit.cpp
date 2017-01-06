//
//  FileShit.cpp
//  LearningOpenGL
//
//  Created by onyx on 11/10/15.
//  Copyright © 2015 AweM. All rights reserved.
// @author Sean Grimes (variable and class namer)
//
#include "FileShit.hpp"
#include "NeuronInfoModule.hpp"
#include <iostream>

//std::string FileShit::wormwiring_db = "db/wormwiring.db";
std::string FileShit::new_wormwiring_db = "db/new_wormwiring.db";
std::string FileShit::neuron_physical_info_json = "resources/neuron_physical_info.json";
//std::string FileShit::neuron_physical_info_json = "resources/neuron_physical_info_ZERO.json";
std::string FileShit::chemical_syns_csv = "csv/herm_chemical.csv";
std::string FileShit::gap_juncts_csv = "csv/herm_gap.csv";
std::string FileShit::open_sans_ttf = "fonts/open-sans/OpenSans-Regular.ttf";
std::string FileShit::mushroomObjFile = "resources/meshes/mushroom/Mushroom.obj";
std::string FileShit::centerCylinderObjFile = "resources/meshes/worm_parts/CenterCylinder.obj";
std::string FileShit::muscleObjFile = "resources/meshes/worm_parts/Muscle.obj";
std::string FileShit::petriDishObjFile = "resources/meshes/worm_parts/petriDish.obj";
std::string FileShit::sphereObjFile = "resources/meshes/worm_parts/Sphere.obj";
std::string FileShit::cameraConfigFile = "resources/camera_config.cfg";

std::string FileShit::ortus_basic_connectome = "csv/ortus_basic_connectome.csv";
std::string FileShit::ortus_basic_connectome_test = "csv/ortus_basic_connectome_test.csv";


std::vector<std::string> FileShit::parse_on_comma(std::string line){
    std::vector<std::string> shit_fuck;
    std::stringstream ss{line};
    std::string tmp;
    while(getline(ss, tmp, ',')){
        shit_fuck.push_back(tmp);
    }
    return shit_fuck;
}

/**
 * returns an open ifstream object
 */
std::ifstream FileShit::open(std::string s){
    std::ifstream input;
    input.open(s);
    if (!input.is_open()){
        throw std::runtime_error("file unable to be opened: "+s);
    }
    return input;
}

std::ofstream FileShit::wopen(std::string s){
    std::ofstream output;
    output.open(s);
    if (!output.is_open()){
        throw std::runtime_error("file unable to be opened: "+s);
    }
    return output;
}


void FileShit::readConnectomeCSV(std::string csv_name, std::vector<std::vector<std::string>>& dat){
    std::ifstream input = FileShit::open(csv_name);
    std::string line;
    //std::vector<std::vector<std::string>> dat;
    int count = 0;
    int len = 0, plen = 0;
    while (getline(input,line)){
        //printf("%s\n",line.c_str());
        std::vector<std::string> split = FileShit::parse_on_comma(line);
        len = (int) split.size(); //
        if (count && (plen != len)){ // makes sure we're not at count == 0, and then cehcks prev len against current len
            throw std::runtime_error("incorrect row size in "+csv_name+"!!!\n");
        }
        plen = len;
        split = StrUtils::trimStringVector(split);
        //printf("(count: %d) >%s\n", split.size(), line.c_str());
        dat.push_back(split);
        count++;
    }
}

/*
 * graphical identifier is either "-(I)", "-(S)", "-(M)", or "-<M>" -- inter, sensory, motor, or muscle
 */
ElementType FileShit::string_to_etype(std::string s, std::string& graphical_identifier){
    std::string string_types[] = {"SENSORY", "INTER",  "MOTOR", "MUSCLE"};
    if (s.find(string_types[0]) != std::string::npos){
        //printf("stype: %s\n",string_types[0].c_str());
        graphical_identifier = "-(S)";
        return SENSORY;
    }
    if (s.find(string_types[1]) != std::string::npos){
        //printf("stype: %s\n",string_types[1].c_str());
        graphical_identifier = "-(I)";
        return INTER;
    }
    if (s.find(string_types[2]) != std::string::npos){
        //printf("stype: %s\n",string_types[2].c_str());
        graphical_identifier = "-(M)";
        return MOTOR;
    }
    if (s.find(string_types[3]) != std::string::npos){
        //printf("stype: %s\n",string_types[3].c_str());
        graphical_identifier = "-<M>";
        return MUSCLE;
    }
    throw std::runtime_error("Unable to convert string to ElementType");
}

float FileShit::str_type_to_float(std::string str){
    const char* in_str = str.c_str();
    if (!strcmp(in_str,"inter"))
        return 0.0f; // blue
    if (!strcmp(in_str,"sensory"))
        return 1.0f; // green
    if (!strcmp(in_str,"motor"))
        return 2.0f; // red
    if (!strcmp(in_str,"poly"))
        return 3.0f; // purple
    if (!strcmp(in_str,"wfk"))
        return 4.0f; // gray
    return -1.0f;
}

ElementType FileShit::str_type_to_enum(std::string str){
    const char* in_str = str.c_str();
    if (!strcmp(in_str,"inter"))
        return INTER; // blue
    if (!strcmp(in_str,"sensory"))
        return SENSORY; // green
    if (!strcmp(in_str,"motor"))
        return MOTOR; // red
    if (!strcmp(in_str,"poly"))
        return WFK; // purple // NOTE -- FIX THIS
    if (!strcmp(in_str,"wfk"))
        return WFK; // gray
    return UH_OH;
}

void FileShit::remove_leading_zero_from_anywhere(std::string* in){
    // any character, any number of times, followed by a zero, then a digit, and then any character any number of times
    std::regex r("(.*)(0)([[:digit:]])(.*)");
    std::smatch regres;
    regex_search(*in,regres, r);
    if (regres.size() == 5){ // then there's a zero
        // group 0 -> full match, group 1 -> before zero, group 2 -> zero, group 3 -> digit after zero, group 4 -> everything after digit
        *in = regres[1].str() + regres[3].str() + regres[4].str();
    }
    return;
}

std::vector<std::string> FileShit::readCameraConfig(){
    std::ifstream input =  FileShit::open(FileShit::cameraConfigFile);
    std::string theLine;
    std::vector<std::string> configVec;
    while(getline(input, theLine)){
        configVec.push_back(theLine);
    }
    return configVec;
}

void FileShit::writeCameraConfig(std::vector<std::string> cameraConfigVec){
    std::ofstream output = wopen(cameraConfigFile);
    int numEntries = cameraConfigVec.size();
    for (int i = 0; i < numEntries; ++i){
        std::string oot = (cameraConfigVec[i]+"\n");
        output.write(oot.c_str(), oot.size());
    }
    output.close();
    
}
