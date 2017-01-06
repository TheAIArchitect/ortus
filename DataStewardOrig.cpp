//
//  DataSteward.cpp
//  delegans
//
//  Created by Sean Grimes on 11/10/15.
//  Copyright © 2015 delegans group. All rights reserved.
//

#include "DataSteward.hpp"

int MAX_MUSCLES = 200;// with allllll muscles, im pretty sure it's 150 (149, actually), but right now, it doesn't much matter if we say 200 or 150... NOTE: this is taking into account muscles in addition to body wall
vector<unordered_map<string,float>> DataSteward::muscleActivationsGJ;
vector<unordered_map<string,float>> DataSteward::muscleActivationsCS;
unsigned int DataSteward::NUM_ELEMS = 0;
unsigned int DataSteward::NUM_ROWS = 0;
unsigned int DataSteward::LEN = 0; // this isn't a very good variable name... it refers to the length of the 1D array for opencl (created when prepping the connection matricies for opencl)

DataSteward::DataSteward(){}





void DataSteward::init(){
    // set up initial elements and set gaps and chems
    // NOTE: we should separate the element creation from connection creation
    get_conns(elements);
    NUM_ELEMS = elements.size();
    NUM_ROWS = NUM_ELEMS;
    LEN = NUM_ROWS * NUM_ELEMS;
    // set the mass points in the elements, and also keep separate references to the mass points for easy access
    // NOTE NOTE NOTE -- FIXME -- change this 
    //FileAssistant::get_neuron_data(&neuronMassPoints, &num_neuron_mass_points, &num_neurons_in_json, &elements);
    //geometrician.straighten(neuronMassPoints, num_neuron_mass_points, &bezPts, &numBezPts);
    // NOTE: come back to this stuff later... it's going to have to be merged into TheRealDeal.cpp 
    //generateBodyMassPoints();
    //generateCenterCylinderMassPoints();
    //addElementInfoToModules();
}

void DataSteward::addElementInfoToModules(){
    for (int k = 0; k < elements.size(); k++){
        NeuronInfoModule* tnimp = (NeuronInfoModule*) elements[k];
        //(*neuronMassPoints)[massPoint_index] = new MassPoint(glm::vec3(-x,z , y), tnimp->name); //mirror x, swap y and z.
        //tnimp->massPoint_id = (*neuronMassPoints)[massPoint_index]->id;
    }
}

size_t DataSteward::get_1d_index(size_t arr_width, int row_num, int col_num){
    return arr_width * row_num + col_num;
}

void DataSteward::set_1d_element(float *arr, size_t arr_width, int row_num, int col_num, float value){
    arr[get_1d_index(arr_width, row_num, col_num)] = value;
}

float *DataSteward::get_init_vector(size_t num_rows){
    float *vec = new float[num_rows]();
    int numElems = elements.size();
    for( int i = 0; i < numElems; ++i){
        vec[i] = elements[i]->v_0;
    }
    return vec;
}

float *DataSteward::convert_2d_to_1d(float **matrix, size_t num_rows, size_t num_cols){
    size_t len = num_rows * num_cols;
    float *flat_arr = new float[len](); // Note the "()", init to zeroed.
    for(int i = 0; i < num_rows; ++i){
        for(int j = 0; j < num_cols; ++j){
            set_1d_element(flat_arr, num_cols, i, j, matrix[i][j]);
        }
    }
    return flat_arr;
}

void DataSteward::saveCurrentConnectome(){
    std::vector<std::string> elementNames;
    int numEN = NUM_ELEMS;
    for (int i = 0; i < numEN; ++i){
        elementNames.push_back(elements[i]->name);
    }
    write_connectome("poooOOoOOOoOOOop.csv", gj_matrix, cs_matrix, elementNames);
}

float* DataSteward::get_polarity_vector(size_t num_rows){
    PolarityConfigurator polarityConfigurator;
    return polarityConfigurator.makePolarityVector(&elements,num_rows);
}

float **DataSteward::convert_1d_to_2d(float *matrix, size_t num_rows, size_t num_cols){
    float **matrix_re = new float*[num_rows];
    for(int i = 0; i < num_rows; ++i){
        matrix_re[i] = new float[num_cols]();
    }
    
    for(int i = 0; i < num_rows; ++i){
        for(int j = 0; j < num_cols; ++j){
            matrix_re[i][j] = matrix[get_1d_index(num_cols, i, j)];
        }
    }
    
    return matrix_re;
}

void DataSteward::transpose(float **matrix, float **matrix_t, size_t num_rows, size_t num_cols){
    size_t i, j;
    for(i = 0; i < num_rows; ++i){
        for(j = 0; j < num_cols; ++j){
            matrix_t[i][j] = matrix[j][i];
        }
    }
}

float **DataSteward::transpose(float **matrix, size_t num_rows, size_t num_cols){
    float **trans;
    trans = new float*[num_rows];
    for(int i = 0; i < num_rows; ++i){
        trans[i] = new float[num_cols]();
    }
    
    transpose(matrix, trans, num_rows, num_cols);
    return trans;
}

/**
 * Also creates the neuron name to index map, stored in Core.cpp
 */
void DataSteward::populate_cs_gj_matricies(float **cs_matrix, float **gj_matrix, unordered_map<string,int>& neuron_names_to_index_map, size_t num_rows, size_t num_cols){
    for(int i = 0; i < num_rows; ++i){
        for(int j = 0; j < num_cols; ++j){
            cs_matrix[i][j] = 0.0f;
            gj_matrix[i][j] = 0.0f;
        }
    }
    
    float maxGapWeight = -1.f;
    float maxChemWeight = -1.f;
    // Get a <map> that allows one to look up an id for a neuron based on name for easy array indexing
    unordered_map<string, int> name_to_id = map_name_to_id();
   
    for (int i = 0; i < NeuronInfoModule::NUM_NIMS; ++i){
        printf("neuron id - %d, neuron name - %s\n", nim[i]->neuron_id, nim[i]->name.c_str());
    }
    
    int numElems = elements.size();
    for (int i = 0; i < numElems; ++i){
        int numConns = elements[i]->out_conns.size();
        std::vector<Connection> tempConns = elements[i]->out_conns;
        for (int j = 0; j < numConns; ++j){
            if ((tempConns[j].pre->ablated == true) || (tempConns[j].post->ablated == true)){
                // then something is very wrong, because in the CSV importer that created the connections, conns with one or more ablations were supposed to be removed.
                printf("(%d, %d) %s (ablated - %d) <=> %s (ablated - %d) -- no ablated cells should have connections...\n",i,j, tempConns[j].pre->name.c_str(), tempConns[j].pre->ablated, tempConns[j].post->name.c_str(), tempConns[j].post->ablated);
                exit(81);
            }
            int pre_id = tempConns[j].pre->element_id;
            int post_id = tempConns[j].post->element_id;
            if (tempConns[j].conntype == GAP){
                gj_matrix[pre_id][post_id] = tempConns[j].weight;
                if (fabs(tempConns[j].weight) > maxGapWeight){
                    maxGapWeight = fabs(tempConns[j].weight);
                }
            }
            else if (tempConns[j].conntype == CHEM){
                cs_matrix[pre_id][post_id] = tempConns[j].weight;
                if (fabs(tempConns[j].weight) > maxChemWeight){
                    maxChemWeight = fabs(tempConns[j].weight);
                }
            }
            
        }
        
        
    }
   
    // these are just going to be hard-coded into the kernel for now....
    this->maxGapWeight = maxGapWeight;
    this->maxChemWeight = maxChemWeight;
    printf("maximum gap weight: %.2f, maximum chem weight: %.2f\n", maxGapWeight, maxChemWeight);
    this->gj_matrix = gj_matrix;
    this->cs_matrix = cs_matrix;
    n2idxMap = name_to_id;
    neuron_names_to_index_map = name_to_id;
    // so now we have our complete connectome in the form of an X by X matrix.
    // now we need to:
    // - pull out all motor neurons and put them in a map (name -> id),
    // - pull out all muscles and put them in a map (name -> id)
    // - find all connections between motor neurons and muscles and put them in a map (mn name -> muscle id list)
    // - figure out which muscles should get feedback from mechanosensory neurons and put them in a map (muscle name -> sensory neuron id)
    // as well as other mappings of that nature.
    
    
    
    
}

/**
 * This populates the n2m map, and adds muscles to the typesMap.
 * 
 * Should be called after read_data_csv_files
 * 
 * NOTE NOTE: this needs to be redone using the ElementInfoModules (elements)
 */
bool DataSteward::mapNeuronsToMuscles(){
    unordered_map<string, int>::iterator iter;
    iter = n2idxMap.begin();
    muscles;
    int i;
    while (iter != n2idxMap.end()){
        string elem_name = iter->first;
        int idx = iter->second;
        if (elem_name.find("BWM") != std::string::npos){ // NOTE: map these to enum'ed muscles for faster traversal.
            muscles.push_back(elem_name);
            for (i = 0; i < NUM_ELEMS; ++i){
                if (gj_matrix[i][idx] > 0){ // then we have a gj connection
                    MuscleWeight tempMW = {elem_name, gj_matrix[i][idx]};
                    n2mGjMap[idx2nMap[i]].push_back(tempMW);
                }
                if (cs_matrix[i][idx] > 0){ // then we have a gj connection
                    MuscleWeight tempMW = {elem_name, gj_matrix[i][idx]};
                    n2mCsMap[idx2nMap[i]].push_back(tempMW);
                }
            }
        }
        iter++;
    }
    printf("MUSCLES:\n");
    for (auto m : muscles){
        printf("\t%s\n",m.c_str());
    }
    unordered_map<string, vector<MuscleWeight>>::iterator iterTwo;
    iterTwo = n2mGjMap.begin();
    printf("GJ to Muscles:\n");
    while (iterTwo != n2mGjMap.end()){
        string name = iterTwo->first;
        vector<MuscleWeight> vec = iterTwo->second;
        printf("\t%s -> ",name.c_str());
        for (int i = 0; i < vec.size(); ++i){
            printf("(%s, %.2f) ",vec[i].name.c_str(), vec[i].weight);
        }
        printf("\n");
        iterTwo++;
    }
    return true;
}


bool DataSteward::push_neuron_data(vector<int> post_syn_ids, float fake_weight_from_sections, int syn_type, int pre_id){
    vec_of_post_syn_ids.push_back(post_syn_ids);
    vec_of_fake_weights.push_back(fake_weight_from_sections);
    vec_of_syn_types.push_back(syn_type);
    vec_of_pre_ids.push_back(pre_id);
    return true;
}


/*
0|id|integer|0||1
1|name|text|0||0
2|description|text|0||0
3|post_list|text|0||0
4|num_post_synapses|integer|0||0
5|num_synapses|integer|0||0
6|num_sections|integer|0||0
7|synapse_type|integer|0||0  --> CS == 1, other == GJ
8|synapse_id|integer|0||0
9|add_date|text|0||0
*/


// Private
/** Deprecated **/
vector<vector<string>> DataSteward::query_wdm(){
    WormWiringDatabaseManager wdm;
    vector<vector<string>> wdm_res;
    wdm_res = wdm.select_records("STILL_DOES_NOT_FUCKING_MATTER");
    return wdm_res;
}

unordered_map<string, int> DataSteward::map_name_to_id(){
    unordered_map<string, int> name_to_id_map;

    int unique_id_counter = 0;
    // Run through the items and put together a name --> id mapping
    int numElems = elements.size();
    for(int i = 0; i < numElems; ++i){
        // Grab the name from the DB return
        string name = elements[i]->name;
        unordered_map<string, int>::iterator it;
     
        // Check to see if the name is already in the map
        it = name_to_id_map.find(name);
        if(it == name_to_id_map.end()){
            // The name does not exist in the map yet, so add it and an id
            name_to_id_map[name] = elements[i]->element_id;
        }
        else{
            printf("Why do you have duplicates in the elements array? '%s' exists at least twice... \n",name.c_str());
            exit(82);
        }
    }
    return name_to_id_map;
}

/** NOTE: Below, from BioData */


void DataSteward::makeModules(){
    makeNeuronModules();
    makeMuscleModules();
}

void DataSteward::makeNeuronModules(){
    
    int num_elements = elements.size();
    int num_neurons = 0;
    for (int i = 0; i < num_elements; i++){
        if (elements[i]->getEType() != MUSCLE){
            ++num_neurons;
        }
    }
    nim = new NeuronInfoModule*[num_neurons];
    int neuron_count = 0;
    for (int i = 0; i < num_elements; i++){
        if (elements[i]->getEType() != MUSCLE){
            nim[neuron_count] = (NeuronInfoModule*) elements[i];
            ++neuron_count;
        }
    }
    NeuronInfoModule::NUM_NIMS = neuron_count;
}

void DataSteward::makeMuscleModules(){
    
    mim = new MuscleInfoModule*[MAX_MUSCLES];
    MuscleInfoModule* temp_mim[MAX_MUSCLES];
    int num_elements = (int) elements.size();
    int muscle_id = 0;
    int muscle_count = 0;
    int i;
    MuscleInfoModule tmim;
    for (i = 0; i < num_elements; i++){
        if (elements[i]->getEType() == MUSCLE && !elements[i]->ablated){
            temp_mim[muscle_count] = (MuscleInfoModule*) elements[i];
            muscle_count++;
        }
    }
    /*
    if (NUM_MUSCLES != muscle_count){
        printf("HOLD THE fuck ON! Are you using more than just the bodywall muscles? in DataSteward::makeMuscleModules, muscle_count != NUM_MUSCLES... NUM_MUSCLES = 95.\n");
        exit(0);
    }
     */
    //    printf("counted muscles, actual muscles: %d, %d\n\n",muscle_count, NUM_MUSCLES);
    std::regex r("(d|v)(BWM)(L|R)([[:digit:]]+)");
    std::smatch regres;
    int quadrant = 0; // quandrants: 0 -> DL, 1 -> DR, 2 -> VR, 3 -> VL. Round robin ordering.
    int round = 1;
    std::string opts[4] = {"dL", "dR", "vR", "vL"}; // accessed by quadrant variable
    for ( i = 0; i < NUM_MUSCLES; i++){
        //printf("DAMN (%d): %s\n",i, temp_mim[i].name.c_str());
        regex_search(temp_mim[i]->name,regres, r);
        if (regres.size() == 5){
            // group 0 -> full match, group 1 -> either d or v, group 2 -> BWM, group 3 -> L or R, group 4 -> number (1 or 2 digits)
            if (round == std::stoi(regres[4].str()) && (regres[1].str()+regres[3].str()) == opts[quadrant]){
                //std::string in = regres[1].str() + regres[3].str() + regres[4].str();
                mim[muscle_id] = (MuscleInfoModule*) elements[temp_mim[i]->element_id];
                mim[muscle_id]->muscle_id = muscle_id;
                ++muscle_id;
                ++quadrant;
                if (quadrant >= 4 && round < 24){
                    quadrant = 0;
                    ++round;
                }
                else if (quadrant >= 3 && round >= 24){
                    break; // there is no VL24.
                }
                i = 0;
            }
            
        }
    }
    for (i = 0; i < NUM_MUSCLES; i++){
        //printf("DAMN: %s\n", mim[i]->name.c_str());
        mim[i]->anteriorMassPoint = bodyMassPoints[i*3];
        mim[i]->centerMassPoint = bodyMassPoints[i*3+1];
        mim[i]->posteriorMassPoint = bodyMassPoints[i*3+2];
        mim[i]->anteriorMassPoint->name = mim[i]->name;
        mim[i]->centerMassPoint->name = mim[i]->name;
        mim[i]->posteriorMassPoint->name = mim[i]->name;
    }
    
    
    //std::regex r("(.*)(0)([[:digit:]])(.*)");
    // group 0 -> full match, group 1 -> before zero, group 2 -> zero, group 3 -> digit after zero, group 4 -> everything after digit
    
}

void DataSteward::generateBodyMassPoints(){
    /**
     * set up positions
     */
    num_body_mass_points = NUM_MUSCLES*3;
    bodyMassPoints = new MassPoint*[num_body_mass_points];
    float xshift = .06f; // always either pushing in or pulling out to create offset
    float yshift = 1.f;
    float zshift = .5f;
    float x_sign_coeff[4] = {-1.0f, 1.0f, 1.0f, -1.0f};
    float y_coeff[4] = {1.0f, 1.0f, 0.0f, 0.0f};
    int round = 1;
    float quadrant_translations = .5; // for x and y;
    int push_pull = -1;
    int quadrant = 0; // quandrants: 0 -> DL, 1 -> DR, 2 -> VR, 3 -> VL. Round robin ordering.
    float temp_shift = 0.0f;
    int index = 0;
    for (int i = 0; i < NUM_MUSCLES; i++){
        quadrant = i % 4;// 0,1, 2, 3
        push_pull = round % 2; // if 1, we pull, if 0, we push --> round 1, muscles are closer to inside, round 2, closer to outside, etc..
        // x
        float x;
        if (push_pull == 1){
            x = quadrant_translations - xshift;
        }
        else{
            x = quadrant_translations + xshift;
        }
        x = x*x_sign_coeff[quadrant];
        // y
        float y = yshift*y_coeff[quadrant];
        // z
        float z = (round * zshift) - zshift;
        //(*mim)[i]->muscle_position_offset = glm::vec3(x, y, z);
        bodyMassPoints[3*i] = new MassPoint(glm::vec3(x,y,z+.005));
        bodyMassPoints[3*i+1] = new MassPoint(glm::vec3(x,y,z));
        bodyMassPoints[3*i+2] = new MassPoint(glm::vec3(x,y,z+(2*zshift)-.005));
        //(*mim)[i]->muscle_position_offset = glm::vec3(2.0f, 2.0f, 2.0f);
        //printf("%s\n",(*mim)[i].name.c_str());
        if (quadrant >= 3){
            ++round;
        }
    }
    /*
     for (int i = 0; i < 95; i++){
     printf("nim %d: %s\n",i, (*mim)[i]->name.c_str());
     }
     printf("shit.\n");
     */
}

void DataSteward::generateCenterCylinderMassPoints(){
    /**
     * set up positions
     */
    num_cc_mass_points = NUM_CC*3;
    centerCylinderMassPoints = new MassPoint*[num_cc_mass_points];
    float xshift = 0.0f; // always either pushing in or pulling out to create offset
    float yshift = .2f;
    float zshift = CENTER_CYLINDER_HEIGHT;
    //float x_sign_coeff[4] = {-.10f, .10f, .10f, -.10f};
    //float y_coeff[4] = {1.0f, 1.0f, 0.0f, 0.0f};
    int round = 0;
    //float quadrant_translations = .05; // for x and y;
    //int push_pull = -1;
    //int quadrant = 0; // quandrants: 0 -> DL, 1 -> DR, 2 -> VR, 3 -> VL. Round robin ordering.
    //float temp_shift = 0.0f;
    int index = 0;
    for (int i = 0; i < NUM_CC; i++){
        float x = xshift;
        // y
        float y = yshift;
        // z
        float z = (round * zshift);// + zshift;
        //(*mim)[i]->muscle_position_offset = glm::vec3(x, y, z);
        centerCylinderMassPoints[i*3] = new MassPoint(glm::vec3(x,y,z));
        centerCylinderMassPoints[i*3+1] = new MassPoint(glm::vec3(x,y,(z+zshift)/2.0f));
        centerCylinderMassPoints[i*3+2] = new MassPoint(glm::vec3(x,y,z+zshift));
        //(*mim)[i]->muscle_position_offset = glm::vec3(2.0f, 2.0f, 2.0f);
        //printf("%s\n",(*mim)[i].name.c_str());
        ++round;
    }
    /*
     for (int i = 0; i < 95; i++){
     printf("nim %d: %s\n",i, (*mim)[i]->name.c_str());
     }
     printf("shit.\n");
     */
}

