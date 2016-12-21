//
//  WormwiringCSVToSQLite.cpp
//  LearningOpenGL
//
//  Created by onyx on 11/10/15.
//  Copyright © 2015 AweM. All rights reserved.
//

#include "WormwiringCSVToSQLite.hpp"
#include "FileShit.hpp"
#include "dbHelper.hpp"
#include "StrUtils.hpp"
#include "Connection.hpp"
#include "AblationStation.hpp"


/****** chemical synapse csv ******/
// row 0 -- column descriptors
// row 1 -- neuron or muscle names
// last row -- copy of row 1
//
// column 0 -- row descriptors
// column 1 -- neuron names


/**
 database:
 
 tables for all neurons and muscles
 
 then a gap junction and chemical synapse table
 
 also, tables for motor neuron to muscle mappings (maybe not necessary -- maybe that's included)
 
 
 */



int offset = 2; // first two rows and columns in both csv files are column/row information


/*
 * graphical identifier is either "-(I)", "-(S)", "-(M)", or "-<M>" -- inter, sensory, motor, or muscle
 */
ElementType string_to_etype(std::string s, std::string& graphical_identifier){
    std::string string_types[] = {"SENSORY", "INTER",  "MOTOR", "MUSCLE"};
    if (s.find(string_types[0]) != string::npos){
        //printf("stype: %s\n",string_types[0].c_str());
        graphical_identifier = "-(S)";
        return INTER;
    }
    if (s.find(string_types[1]) != string::npos){
        //printf("stype: %s\n",string_types[1].c_str());
        graphical_identifier = "-(I)";
        return SENSORY;
    }
    if (s.find(string_types[2]) != string::npos){
        //printf("stype: %s\n",string_types[2].c_str());
        graphical_identifier = "-(M)";
        return MOTOR;
    }
    if (s.find(string_types[3]) != string::npos){
        //printf("stype: %s\n",string_types[3].c_str());
        graphical_identifier = "-<M>";
        return MUSCLE;
    }
    throw std::runtime_error("Unable to convert string to ElementType");
}

bool create_database(){
    dbHelper dbh(FileShit::new_wormwiring_db);
    std::vector<std::string> tables;
    //tables.push_back("types");
    tables.push_back("elements");
    tables.push_back("connections");
    dbh.drop_table("drop tables if exists "+tables[0]);
    dbh.drop_table("drop tables if exists "+tables[1]);
    std::vector<std::string> statements;
    //statements.push_back("create table if not exists "+tables[0]+" id integer auto_increment primary key, type_name text");
    statements.push_back("create table if not exists "+tables[0]+" id integer auto_increment primary key, name text, type text");
    statements.push_back("create table if not exists "+tables[1]+" id integer auto_increment primary key, pre_id integer, post_id integer, weight real, conn_type integer, foreign key(pre_id) references "+tables[1]+"(id), foreign key(post_id) references "+tables[1]+"(id)");
    return true;
}

void read_csv(std::string csv_name, std::vector<std::vector<std::string>>& dat){
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
        dat.push_back(split);
        count++;
    }
}

/*
bool insert_element(Element* element, std::string table){
    std::string statment = "insert into "+table+" (name, type) values ("+element->name+", "+element->etype+")";
}
 */

std::vector<std::vector<std::string>> connDat;
AblationStation ablator;
void createConnections(std::vector<ElementInfoModule*>& elements, int rows, int cols);

void createElements(std::vector<ElementInfoModule*>& elements){
    read_csv(FileShit::ortus_basic_connectome,connDat);
    int count = connDat.size(); // total row count
    int len = connDat[0].size(); // total col count
    /*
     * First we want to go through and create structs for all the neurons and muscles
     */
    // absolute_index_mapping takes the original index in the csv file for the neuron or muscle,
    // and gives the index in the neuron or muscle struct vector. if negative, it's a muscle index.
    std::vector<int> absolute_index_mapping;
    int rows =(int) connDat.size()-1;// last row is the same as row 1 (not row 0... row 1)
    //printf("num rows: %d\n",rows);
    // all columns should be the same length if we've gotten here. NOTE: last column is same as col 1 (not col 0..)
    //NOTE2: this only applied for the worm. NOTE: there are 4 empty columns at the end of the gap data. // int cols = len - (1+4);
    int cols = len - 1; // as per note above, last col == col 1, (not 0)
    
    std::string curr_type = "";
    ElementType etype;
    std::string graphicalIdentifier;
    // NOTE: All NIM and MIM objects are EIMs, so the element_id increases each loop.
    // neuron_id increases if it's a neuron, muscle_id increases if it's a muscle
    int element_id = 0;
    int neuron_id = 0;
    int muscle_id = 0;
    for (int i = offset; i < rows; i++){
        //for (int j = 0; j < cols; j++){
        //printf("gap dat: %s",connDat[i][0].c_str());
        if (!(connDat[i][0] == "")){ // else, we keep the same etype
         //   printf("<< NOT EMPTY!\n");
            etype = string_to_etype(connDat[i][0], graphicalIdentifier);
        }
        //else{
         //   printf("<< EMPTY!\n");
        //}
        std::string temp_name = connDat[i][1];
        FileShit::remove_leading_zero_from_anywhere(temp_name);
        if (etype != MUSCLE){
            NeuronInfoModule* nim = new NeuronInfoModule();
            nim->name = temp_name;
            nim->graphicalName = temp_name+graphicalIdentifier;
            nim->element_id = element_id;
            nim->setElementType(etype);
            ablator.setAblationStatus(nim);
            if (!nim->ablated){ // only keep it if it isn't ablated
                nim->neuron_id = neuron_id;
                neuron_id++;
            }
            elements.push_back(nim);// must add ablated neurons to elements array so that we can find them when we do chems
            //nim->soma_position = glm::vec3(1.0f);
            //printf("NEW NEURON (type: %d): (n-idx: %d) %s\n",etype,nim->neuron_id, nim->name.c_str());
        }
        else {
            MuscleInfoModule* mim = new MuscleInfoModule();
            mim->name = temp_name;
            mim->graphicalName = temp_name+graphicalIdentifier;
            mim->element_id = element_id;
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
    createConnections(elements, rows, cols);
    printf("%d\n",count);
}


void createConnections(std::vector<ElementInfoModule*>& elements, int rows, int cols){
    
    printf("ROWS, COLS: %d. %d\n",rows, cols);
    int start_at = offset; // it's a square matrix, with the same information on each side of the diagonal (but flipped)... could do one 'side' and add same connection pointer to each pre, but let's hold off... would need to switch the out_conns vector to hold Connection object pointers.
    for (int i = offset; i < rows; i++){
        
        int pre_id = i - offset;// there's no offset in our Element vector
        int post_id = -1;
        
        for (int j = offset; j < cols; j++){
            if (connDat[i][j] == ""){
                continue;// no connection between i and j
            }
            post_id = j - offset;
            
            // we don't want to create a connection if either the pre or post neuron/muscle has been 'ablated'... we don't want a reference to an object we'll delete later, because that can cause issues
            if (elements[pre_id]->ablated || elements[post_id]->ablated){
                continue;
            }
            // index 0 will be the gap, index 1 will be the chem
            // either one, as well as both, may be empty, or not empty.
            // only 1 may have a negative value. If negative, it's an inhibitory connection.
            // row -> col => pre -> post
            // and, gaps should be symmetric (if i->j, then also j->i)
            std::vector<std::string> strVecWeights = StrUtils::parseOnCharDelim(connDat[i][j], '/');
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
                Connection c;
                c.pre = elements[pre_id];
                c.preName = elements[pre_id]->name;
                c.post = elements[post_id];
                c.postName = elements[post_id]->name;
                c.conntype = GAP;
                c.weight = gapWeight;
                elements[pre_id]->out_conns.push_back(c);
            }
            if (chemWeight != 0){ // could be negative or positive
                Connection c;
                c.pre = elements[pre_id];
                c.preName = elements[pre_id]->name;
                c.post = elements[post_id];
                c.postName = elements[post_id]->name;
                c.conntype = CHEM;
                c.weight = chemWeight;
                elements[pre_id]->out_conns.push_back(c);
            }
        }
        //printf("i = %d\n",i);
        ++start_at;
    }
    //kinput.seekg(0);
}


void parse_chems(std::vector<ElementInfoModule*>& elements){
 
    std::vector<std::vector<std::string>> chem_dat;
    read_csv(FileShit::chemical_syns_csv, chem_dat);
    int count = (int)chem_dat.size(); // total row count
    int len = (int)chem_dat[0].size(); // total col count
    int start_at = offset; // it's a square matrix, with the same information on each side of the diagonal (but flipped)... only need one side
    int rows = (int)chem_dat.size() - 1; // last row is same as row 1 (column names)
    int cols = (int)chem_dat[0].size() - 1; // last col is same as col 1 (row names)
    int num_elements = (int)elements.size();
    int k;
    std::string temp_name;
    for (int i = offset; i < rows; i++){
       
        for (k = 0; k < num_elements; k++){
            temp_name = chem_dat[i][1];
            FileShit::remove_leading_zero_from_anywhere(temp_name);
            if (temp_name == elements[k]->name){
                break; // so we'll have k as our pre_id
            }
        }
        int pre_id = k;
        int post_id = -1;
        for (int j = offset; j < cols; j++){
            //trimmed = StrUtils::trim(chem_dat[i][j]);
            if (chem_dat[i][j] == ""){
                continue;// no connection between i and j
            }
            temp_name = chem_dat[1][j];
            FileShit::remove_leading_zero_from_anywhere(temp_name);
             for (k = 0; k < num_elements; k++){
                if (temp_name == elements[k]->name){
                    break; // same idea with k
                }
            }           
            post_id = k;
            
            // we don't want to create a connection if either the pre or post neuron/muscle has been 'ablated'... we don't want a reference to an object we'll delete later, because that can cause issues
            if (elements[pre_id]->ablated || elements[post_id]->ablated){
                continue;
            }
            Connection c;
            c.pre = elements[pre_id];
            c.post = elements[post_id];
            c.conntype = CHEM;
            c.weight = stof(chem_dat[i][j],NULL);
            //printf("pre, post -> (%d) %s, (%d) %s)\n", pre_id, elements[pre_id]->name.c_str(), post_id, elements[post_id]->name.c_str());
            elements[pre_id]->out_conns.push_back(c);
        }
        //printf("i = %d\n",i);
        ++start_at;
    }
}

void get_conns(std::vector<ElementInfoModule*>& elements){
    std::vector<ElementInfoModule*> tempElements; // after we get the gaps and chems, we need to fill the real elements vector with only the elements we want (as of now, all neurons and body wall muscles)
    createElements(tempElements);
    //ortus//parse_chems(tempElements);
    // now we need to go through the tempElements and only add the non-ablated ones to the 'real' elements vector.
    int numTemps = tempElements.size();
    int realIndex = 0; // this is the index for the 'real' elements (the ones that we care about, that aren't ablated)
    int round;
    int maxRounds = 2;
    for (round = 0; round < maxRounds; ++round){ // we want to go through the elements array once for neurons, then muscles, so that in the kernel we can apply a different rule if it's a muscle (e.g., if row > 302, do something specific for muscles).
        for( int i = 0; i < numTemps; ++i){
            if (!tempElements[i]->ablated){
                // change the element_id such that the elements we actually use are indexed consecutively. this allows us to use the element_id as the index when working with arrays/vectors (open cl)
                if (round == 0 && tempElements[i]->getEType() != MUSCLE){ // neuron
                    tempElements[i]->element_id = realIndex;
                    elements.push_back(tempElements[i]);
                    realIndex++;
                }
                else if (round == 1 && tempElements[i]->getEType() == MUSCLE){ // neuron
                    tempElements[i]->element_id = realIndex;
                    elements.push_back(tempElements[i]);
                    realIndex++;
                }
            }
            else if (tempElements[i]->ablated && round == 1){ // it's safe to delete once we know we won't access the element again (i.e., we're on our second time through)
                delete tempElements[i];
                tempElements[i] = NULL;
            }
        }
    }
    // now we must check to ensure that all neurons are in order of id, as are all muscles
    // this is both a sanity check and a check to ensure that the order of neurons in the elements array is the same as the order in the nim array, and same for mim array and elements array (though the muscles will be offset by the number of neurons in the elements arrray)
    bool muscleFound = false;
    int numNeurons = 0;
    int numMuscles = 0;
    for (int i = 0; i < realIndex; ++i){
        if (elements[i]->getEType() != MUSCLE && muscleFound){ // if we've seen at least one muscle, and then a neuron, that's not good.
            printf("The order of the elements is not such that all neurons come before all muscles.\n");
            exit(5);
        }
        if (elements[i]->getEType() == MUSCLE && !muscleFound){ // if we see a muscle, acknowledge this.
            muscleFound = true;
        }
        if (!muscleFound){// check element_id against neuron_id
            if (((NeuronInfoModule*)elements[i])->neuron_id != elements[i]->element_id){
                printf("We have a problem. neuron_id != element_id, during sanity check.\n");
                exit(6);
            }
            numNeurons++;
        }
        if (muscleFound){ // check element_id against numNeurons+muscle_id (due to offset)
            if ((numNeurons + ((MuscleInfoModule*)elements[i])->muscle_id) != elements[i]->element_id){
                printf("We have a problem. (numNeurons + muscle_id) != element_id, during sanity check.\n");
                exit(7);
            }
            numMuscles++;
        }
        //printf("EIM: %s, %d\n",elements[i]->name.c_str(),elements[i]->element_type);
    }
    printf("number of neurons in model: %d, number of muscles in model: %d\n",numNeurons, numMuscles);
}


int fmain(int argc, char** argv){
    //std::vector<GapJunction> gaps;
    //std::vector<ChemSynapse> chems;
    //get_conns(gaps, chems);
 
//    int num_gaps = gaps.size();
    /*
    for (int i = 0; i < num_gaps; i++){
        Element pre = *gaps[i].pre;
        Element post = *gaps[i].post;
        printf("GJ (i = %d): %s (%d) -> %s (%d) <%f>\n",i, pre.name.c_str(), pre.etype, post.name.c_str(), post.etype, gaps[i].weight);
    }
    */
    return 0;
}
