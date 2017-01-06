//
//  Filenames.h
//  LearningOpenGL
//
//  Created by onyx on 11/10/15.
//  Copyright © 2015 AweM. All rights reserved.
//

#ifndef FileShit_h
#define FileShit_h

#include <string>
#include <sstream>
#include <vector>
#include <exception>
#include <fstream>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "NeuronInfoModule.hpp"
#include "ElementInfoModule.hpp"
#include <regex>
#include "StrUtils.hpp"

#include "MassPoint.hpp"

/*
class BadThings: public std::exception
{
    virtual const char* happend(std::string the_things) const throw()
    {
        std::string all = "Bad things happened: "+the_things;
        return "Bad things happened:";
    }
} badthings;
 */


class FileShit{
    
    public:
        //static std::string wormwiring_db;
        static std::string new_wormwiring_db;
        static std::string neuron_physical_info_json;
        static std::string chemical_syns_csv;
        static std::string gap_juncts_csv;
        static std::string mushroomObjFile;
        static std::string centerCylinderObjFile;
        static std::string muscleObjFile;
        static std::string petriDishObjFile;
        static std::string sphereObjFile;
    
        static std::string ortus_basic_connectome;
        static std::string ortus_basic_connectome_test;
    
    
        static std::string open_sans_ttf;
    
        static std::string cameraConfigFile;
    
    
        /**
         * returns an open ifstream object
         */
        static std::ifstream open(std::string s);
    
        static std::ofstream wopen(std::string s);
    
    
        static void readConnectomeCSV(std::string csv_name, std::vector<std::vector<std::string>>& dat);
        static ElementType string_to_etype(std::string s, std::string& graphical_identifier);
        static void get_neuron_data(MassPoint*** neuronMassPoints, int* num_neuron_mass_points, int* num_neurons_in_json, std::vector<ElementInfoModule*>* elements);
        static std::vector<std::string> parse_on_comma(std::string line);
    
        static float str_type_to_float(std::string str);
        static ElementType str_type_to_enum(std::string str);
        static void remove_leading_zero_from_anywhere(std::string* in);
    
    
        static std::vector<std::string> readCameraConfig();
        static void writeCameraConfig(std::vector<std::string> cameraConfigVec);
};


#endif
