//
//  DataSteward.cpp
//  delegans
//
//  Created by Sean Grimes on 11/10/15.
//  Copyright © 2015 delegans group. All rights reserved.
//

/* we want everything stored in opencl-compatible datatypes. data will go directly from here, to opencl buffers.
 
  ** At the moment, we are going to ignore the issue of physical proximity with respect to how it impacts computation (e.g., greater time delay for neuron C because it's farther from A than B is, despite both being directly connected to A), or signal decay.
    *** This is because it will add complexity to the computation, as well as the system that enables neurons to be added. 
 
  * we will have extra space at the end of each row, in 2D arrays (from now on, 2D means a 1D array being used as a 2D array), meaning that under normal operation, there will effectively be 'blanks' at the end of each 'row' in the 1D array representation. 
    ** This is where we'll put the weights for the new neurons. This is how we will grow the network without slowing things down significantly. Upon writing the new connectome, we'll first re-organize.
 
 * This class will hold all of the real data. 
 ******** UNLESS WE ARE NO LONGER USING THE DATA (e.g. it's historic voltage data per neuron),
  EVERYTHING ELSE IN ORTUS WILL HOLD A --REFERENCE-- TO THE DATA HELD HERE. THAT INCLUDES WEIGHTS, POLARITY, NEUROTRANSMITTERS, NEURONS, ETC..
    --> the reason for this, is so that we can pass data to and from opencl without constantly having to re-intitalize the 1D representations of data. It keeps things simple.
 
 
 //////////////// NOTE: MUST GO BACK TO WORMWIRINGCSVTOSQLITE.CPP AND FINISH get_conns(..) and writeConnectome(..) /////////////////
 
 */



#include "DataSteward.hpp"

int MAX_MUSCLES = 200;// with allllll muscles, im pretty sure it's 150 (149, actually), but right now, it doesn't much matter if we say 200 or 150... NOTE: this is taking into account muscles in addition to body wall
int DataSteward::CSV_OFFSET = 2;// first two rows and columns in both csv files are column/row information
int DataSteward::CSV_ROWS = 0;
int DataSteward::CSV_COLS = 0;
vector<unordered_map<string,float>> DataSteward::muscleActivationsGJ;
vector<unordered_map<string,float>> DataSteward::muscleActivationsCS;
unsigned int DataSteward::NUM_ELEMS = 0;
unsigned int DataSteward::NUM_ROWS = 0;
unsigned int DataSteward::LEN = 0; // this isn't a very good variable name... it refers to the length of the 1D array for opencl (created when prepping the connection matricies for opencl)

DataSteward::DataSteward(){}





void DataSteward::init(){
    // set up initial elements and set gaps and chems
    // NOTE: we should separate the element creation from connection creation
    //get_conns(elements);
    createElements();
    createConnections();
    NUM_ELEMS = elements.size();
    NUM_ROWS = NUM_ELEMS;
    LEN = NUM_ROWS * NUM_ELEMS;
    
    // set the mass points in the elements, and also keep separate references to the mass points for easy access
    // NOTE NOTE NOTE -- FIXME -- change this 
    //FileShit::get_neuron_data(&neuronMassPoints, &num_neuron_mass_points, &num_neurons_in_json, &elements);
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
        elementNames.push_back(elements[i]->name());
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
        printf("neuron id - %d, neuron name - %s\n", nim[i]->neuron_id, nim[i]->name().c_str());
    }
    
    int numElems = elements.size();
    for (int i = 0; i < numElems; ++i){
        int numConns = elements[i]->out_conns.size();
        std::vector<Connection> tempConns = elements[i]->out_conns;
        for (int j = 0; j < numConns; ++j){
            if ((tempConns[j].pre->ablated == true) || (tempConns[j].post->ablated == true)){
                // then something is very wrong, because in the CSV importer that created the connections, conns with one or more ablations were supposed to be removed.
                printf("(%d, %d) %s (ablated - %d) <=> %s (ablated - %d) -- no ablated cells should have connections...\n",i,j, tempConns[j].pre->name().c_str(), tempConns[j].pre->ablated, tempConns[j].post->name().c_str(), tempConns[j].post->ablated);
                exit(81);
            }
            int pre_id = tempConns[j].pre->id();
            int post_id = tempConns[j].post->id();
            if (tempConns[j].type == GAP){
                gj_matrix[pre_id][post_id] = tempConns[j].weight();
                if (fabs(tempConns[j].weight()) > maxGapWeight){
                    maxGapWeight = fabs(tempConns[j].weight());
                }
            }
            else if (tempConns[j].type == CHEM){
                cs_matrix[pre_id][post_id] = tempConns[j].weight();
                if (fabs(tempConns[j].weight()) > maxChemWeight){
                    maxChemWeight = fabs(tempConns[j].weight());
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



unordered_map<string, int> DataSteward::map_name_to_id(){
    unordered_map<string, int> name_to_id_map;

    int unique_id_counter = 0;
    // Run through the items and put together a name --> id mapping
    int numElems = elements.size();
    for(int i = 0; i < numElems; ++i){
        // Grab the name from the DB return
        string name = elements[i]->name();
        unordered_map<string, int>::iterator it;
     
        // Check to see if the name is already in the map
        it = name_to_id_map.find(name);
        if(it == name_to_id_map.end()){
            // The name does not exist in the map yet, so add it and an id
            name_to_id_map[name] = elements[i]->id();
        }
        else{
            printf("Why do you have duplicates in the elements array? '%s' exists at least twice... \n",name.c_str());
            exit(82);
        }
    }
    return name_to_id_map;
}
//////////////////////////// NEW ////////////////////////

void DataSteward::createElements(){
    FileShit::readConnectomeCSV(FileShit::ortus_basic_connectome,csvDat);
    int count = csvDat.size(); // total row count
    int len = csvDat[0].size(); // total col count
    /*
     * First we want to go through and create structs for all the neurons and muscles
     */
    // absolute_index_mapping takes the original index in the csv file for the neuron or muscle,
    // and gives the index in the neuron or muscle struct vector. if negative, it's a muscle index.
    std::vector<int> absolute_index_mapping;
    
    CSV_ROWS =(int) csvDat.size()-1;// last row is the same as row 1 (not row 0... row 1)
    // all columns should be the same length if we've gotten here. NOTE: last column is same as col 1 (not col 0..)
    CSV_COLS = len - 1;
    if (CSV_ROWS != CSV_COLS){
        printf("ERROR! Connectome row count != col count.\n");
        exit(8);
    }
    
    std::string curr_type = "";
    ElementType etype;
    std::string graphicalIdentifier;
    // NOTE: All NIM and MIM objects are EIMs, so the element_id increases each loop.
    // neuron_id increases if it's a neuron, muscle_id increases if it's a muscle
    int element_id = 0;
    int neuron_id = 0;
    int muscle_id = 0;
    for (int i = CSV_OFFSET; i < CSV_ROWS; i++){
        if (!(csvDat[i][0] == "")){ // else, we keep the same etype
            etype = FileShit::string_to_etype(csvDat[i][0], graphicalIdentifier);
        }
        std::string temp_name = csvDat[i][1];
        FileShit::remove_leading_zero_from_anywhere(temp_name);
        officialNameVector.push_back(temp_name);
        officialNameToIndexMap[temp_name] = element_id;
        if (etype != MUSCLE){
            NeuronInfoModule* nim = new NeuronInfoModule();
            nim->namep = &officialNameVector[i];
            nim->graphicalName = temp_name+graphicalIdentifier;
            nim->idp = &officialNameToIndexMap[temp_name];
            nim->setElementType(etype);
            ablator.setAblationStatus(nim);
            if (!nim->ablated){ // only keep it if it isn't ablated
                nim->neuron_id = neuron_id;
                neuron_id++;
            }
            elements.push_back(nim);// must add ablated neurons to elements array so that we can find them when we do chems
        }
        else {
            MuscleInfoModule* mim = new MuscleInfoModule();
            mim->namep = &officialNameVector[i];
            mim->graphicalName = temp_name+graphicalIdentifier;
            mim->idp = &officialNameToIndexMap[temp_name];
            mim->setElementType(etype);
            mim->centerMassPoint = NULL;
            ablator.setAblationStatus(mim);
            if (!mim->ablated){ // only keep it if it isn't ablated
                mim->muscle_id = muscle_id;
                muscle_id++;
            }
            elements.push_back(mim); // must add ablated muscles to elements array so that we can find them when we do chems
        }
        element_id++;
    }
}

void DataSteward::createConnections(){
    printf("CSV_ROWS, CSV_COLS: %d. %d\n",CSV_ROWS, CSV_COLS);
    gaps = new Blade(CSV_ROWS-CSV_OFFSET, CSV_COLS-CSV_OFFSET,MAX_ELEMENTS, MAX_ELEMENTS);
    chems = new Blade(CSV_ROWS-CSV_OFFSET, CSV_COLS-CSV_OFFSET,MAX_ELEMENTS, MAX_ELEMENTS);
    int start_at = CSV_OFFSET; // it's a square matrix, with the same information on each side of the diagonal (but flipped)... could do one 'side' and add same connection pointer to each pre, but let's hold off... would need to switch the out_conns vector to hold Connection object pointers.
    for (int i = CSV_OFFSET; i < CSV_ROWS; i++){
        
        int pre_id = i - CSV_OFFSET;// there's no offset in our Element vector
        int post_id = -1;
        
        for (int j = CSV_OFFSET; j < CSV_COLS; j++){
            if (csvDat[i][j] == ""){
                continue;// no connection between i and j
            }
            post_id = j - CSV_OFFSET;
            
            // we don't want to create a connection if either the pre or post neuron/muscle has been 'ablated'... we don't want a reference to an object we'll delete later, because that can cause issues
            if (elements[pre_id]->ablated || elements[post_id]->ablated){
                continue;
            }
            // index 0 will be the gap, index 1 will be the chem
            // either one, as well as both, may be empty, or not empty.
            // only 1 may have a negative value. If negative, it's an inhibitory connection.
            // row -> col => pre -> post
            // and, gaps should be symmetric (if i->j, then also j->i)
            std::vector<std::string> strVecWeights = StrUtils::parseOnCharDelim(csvDat[i][j], '/');
            int numEntries = strVecWeights.size();
            float gapWeight = 0; // we'll know if we have a gap or chem by the weight not being 0
            float chemWeight = 0;
            // we know if it's empty, we won't hit it (see above)
            // in addition, as long as if a gap or chem (or both) is/are added, a 0 is added
            // for the connection that doesn't exist, this will be fine, e.g.:
            //      -> if a gap is added: '1/0' would be fine, but '1', or '1/' would not.
            //      -> if a chem is added: '0/-1' would be fine, but '/-1', or '-1' would not.
            // and, obvously, if there is a gap and a chem, a weight is given for both.
            if (numEntries == 2){
                gapWeight = stof(strVecWeights[0],NULL);
                chemWeight = stof(strVecWeights[1],NULL);
            }
            else {
                printf("Oh no. This isn't good... connectome data seems corrupted.\n");
                exit(3);
            }
            
            if (gapWeight > 0){ // can only be positive
                gaps->set(post_id, pre_id, gapWeight); // swap post and pre so that we have row as post and col as pre
                Connection c;
                c.pre = elements[pre_id];
                c.preName = elements[pre_id]->name();
                c.post = elements[post_id];
                c.postName = elements[post_id]->name();
                c.type = GAP;
                c.weightp = gaps->getp(post_id, pre_id);
                elements[pre_id]->out_conns.push_back(c);
            }
            if (chemWeight != 0){ // could be negative or positive
                chems->set(post_id, pre_id, chemWeight);
                Connection c;
                c.pre = elements[pre_id];
                c.preName = elements[pre_id]->name();
                c.post = elements[post_id];
                c.postName = elements[post_id]->name();
                c.type = CHEM;
                c.weightp = chems->getp(post_id, pre_id);
                elements[pre_id]->out_conns.push_back(c);
            }
        }
        //printf("i = %d\n",i);
        ++start_at;
    }
    //kinput.seekg(0);
}
