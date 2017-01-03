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

void FileShit::get_neuron_data(MassPoint*** neuronMassPoints, int* num_neuron_mass_points, int* num_neurons_in_json, std::vector<ElementInfoModule*>* elements) {
    // 1. Parse a JSON string into DOM.
    //const char* json = "{\"project\":\"rapidjson\",\"stars\":10}";
    
    std::ifstream input =  FileShit::open(FileShit::neuron_physical_info_json);
    std::string poo;
    std::string json_in;
    while(getline(input, poo)){
        json_in.append(poo);
    }
    //std::cout << json_in << std::endl;
    
    rapidjson::Document d;
    d.Parse(json_in.c_str());
    rapidjson::Value& v = d[0];
    int num_members = d.Size();
    *num_neurons_in_json = num_members;
    *neuronMassPoints = new MassPoint*[num_members];
    int massPoint_index = 0;
    *num_neuron_mass_points = num_members;
    //(*neuron_info) = new NeuronInfoModule[num_members];
    std::cout << "Member count: " << d.Size() << std::endl;
    int member_number = 0;
    float total_soma_diam = 0.f;
    int num_somas = 0;
    for (int i = 0; i < num_members; i++){
        rapidjson::Value& v = d[member_number];
        rapidjson::Value::ConstMemberIterator itr = v.MemberBegin();
        std::string neuron_name = itr->name.GetString();
        printf("%s\n",neuron_name.c_str());
        rapidjson::Value& neuron_dat = v[neuron_name.c_str()];
        float x = (float) neuron_dat[0]["soma"]["proximal"][0].GetDouble();
        float y = (float) neuron_dat[0]["soma"]["proximal"][1].GetDouble();
        float z = (float) neuron_dat[0]["soma"]["proximal"][2].GetDouble();
        float diam = (float) neuron_dat[0]["soma"]["proximal"][3].GetDouble();
        std::string stype = neuron_dat[2]["type"].GetString();
        float ftype = str_type_to_float(stype);
        ElementType etype = str_type_to_enum(stype);
        // std::string temp_s =
        //        neuron_dat[0]["soma"]["id"][0];
        //std::cout << neuron_dat[1]["segments"][0]["id"][0].GetInt() << std::endl;
        //float color_index = str_type_to_int(temp_s);
        //glm::vec4 temp_neuron = glm::vec4(x, y, z, diam);
        //neuron_positions[i] = temp_neuron;
        
        for (int k = 0; k < elements->size(); k++){
            if ((*elements)[k]->name == neuron_name){
                NeuronInfoModule* tnimp = (NeuronInfoModule*) (*elements)[k];
                //(*neuronMassPoints)[massPoint_index] = new MassPoint(glm::vec3(x, y, z), tnimp->name);
                (*neuronMassPoints)[massPoint_index] = new MassPoint(glm::vec3(-x,z , y), tnimp->name); //mirror x, swap y and z.
                //tnimp->soma_position = glm::vec3(x, y, z);
                tnimp->massPoint = (*neuronMassPoints)[massPoint_index];
                tnimp->soma_diameter = diam;
                total_soma_diam += diam;
                num_somas++;
                tnimp->name = neuron_name;
                tnimp->setElementType(etype);
                tnimp->neuron_id = i;// NOTE: ortus might need this
                tnimp->massPoint_id = (*neuronMassPoints)[massPoint_index]->id;
                massPoint_index++;
                break;
            }
        }
        member_number++;
        //(*neuron_info)[member_number] = nim;
        //(*neuron_positions)[i+4] = color_index;
        //printf("%d\n",(int)color_index);
        //neuron_dat["id"];
        //neuron_dat["proximal"];
    }
    //for (Value::ConstMemberIterator itr = v.MemberBegin(); itr != v.MemberEnd(); ++itr){
    //    std::cout << itr->name.GetString()  << std::endl;
    //<< itr->value.GetType()
    //}
    /*
     // 2. Modify it by DOM.
     Value& s = d["stars"];
     s.SetInt(s.GetInt() + 1);
     
     // 3. Stringify the DOM
     StringBuffer buffer;
     Writer<StringBuffer> writer(buffer);
     d.Accept(writer);
     
     // Output {"project":"rapidjson","stars":11}
     std::cout << buffer.GetString() << std::endl;
     */
    NeuronInfoModule::AVG_NEURON_DIAM = total_soma_diam/num_somas;
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

void FileShit::remove_leading_zero_from_anywhere(std::string& in){
    // any character, any number of times, followed by a zero, then a digit, and then any character any number of times
    std::regex r("(.*)(0)([[:digit:]])(.*)");
    std::smatch regres;
    regex_search(in,regres, r);
    if (regres.size() == 5){ // then there's a zero
        // group 0 -> full match, group 1 -> before zero, group 2 -> zero, group 3 -> digit after zero, group 4 -> everything after digit
        in = regres[1].str() + regres[3].str() + regres[4].str();
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
