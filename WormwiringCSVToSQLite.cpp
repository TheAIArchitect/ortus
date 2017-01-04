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


/* 
 * NOTE: this assumes the connectome has NOT been transposed!!!
 *
 */
bool write_connectome(std::string csv_name, float** gaps, float** chems, std::vector<std::string> elements){
    
    // the first row is complicated, it starts with "gap/chem" (as a 'key' of sorts),
    // then has an empty column, and then 'SENSORY', because we start with sensory neurons.
    std::string firstRow = "gap/chem,,";
    // before we continue, we need to create a map that holds the indices that the SENSORY, INTER, MOTOR, etc..
    // elements start at, so that we know what columns to put the SENSORY/INTER/MOTOR headings in on the firstRow
    std::unordered_map<std::string,int> columnHeadingMap;
    // start at 2, (see 'firstRow' to see why)
    int colOffset = 2;
    int currentColIndex = colOffset;
    int numElements = elements.size();
    std::string headings[4] = {"SENSORY", "INTER", "MOTOR","BUFFER"}; // "BUFFER" allows us to keep accessing the array after we've used all headings (after 'MOTOR', we have to add all motor neurons, but headingNum has been incremented, and we still check the array)
    int headingNum = 0;
    //columnHeadingMap[headings[headingNum]] = currentColIndex;
    //headingNum++;
    // now we run through the list of element names (elements) until the first letter stops being "S",
    // and becomes "I", (and then "M", for motor). At the time of this, motors and muscles are used as the same thing.
    std::string nextKey = headings[headingNum];
    headingNum++;
    char firstLetter = nextKey[0];
    // we can also build the second row, which is the offset, and then all the element names
    std::string secondRow = ",,";
    for (int i = 0; i < numElements; ++i){
        // if the element's name starts with firstLetter and nextKey isn't in the map
        if (elements[i][0] == firstLetter && !columnHeadingMap.count(nextKey)){
            columnHeadingMap[nextKey] = i; // don't add the colOffset to this, because we'll use it to check against the elements' actual indices later on
            firstRow += nextKey + ",";
            nextKey = headings[headingNum];
            headingNum++;
            firstLetter = nextKey[0];
        }
        else {
            firstRow += ",";
        }
        secondRow += elements[i] + ",";
        
    }
    firstRow += "placeholder\n";
    secondRow += "placeholder\n";
    // AND, we can do the last row, which is the same as the secondRow
    std::string lastRow = secondRow;
    // now we have something that looks somewhat like:
    // (for firstRow): "gap/chem,,SENSORY,,,,,,INTER,,,,,,MOTOR,,,,,,placeholder"
    // (for secondRow): ",,e1,e2,e3,...,eN,placeholder"
    
    // next, we create the 'main' rows, that are the actual connectome.
    // formula: <space>,<name>,<either connection or blank for all elements>,<name>,<newline>
    // NOTE: this assumes the connectome has NOT been transposed!!!
    std::string actualConnectome = "";
    // also, we need to set headingNum to zero, because we need to check if a heading is supposed to be there
    headingNum = 0;
    for (int i = 0; i < numElements; ++i){
        // check to see if we're at a point that we need to add a 'type' heading
        if (columnHeadingMap[headings[headingNum]] == i){
            actualConnectome += headings[headingNum] + ",";
            headingNum++;
        }
        else {
            actualConnectome += ",";
        }
        actualConnectome += elements[i] + ",";
        for (int j = 0; j < numElements; ++j){
            if (gaps[i][j] == 0 && chems[i][j] == 0){
               actualConnectome += ","; // there's no connection
            }
            else {
                actualConnectome += std::to_string((int)gaps[i][j]) + "/" + std::to_string((int)chems[i][j]) + ",";
            }
        }
        actualConnectome += elements[i] + "\n";
    }
    std::ofstream newConnectome = FileShit::wopen(FileShit::ortus_basic_connectome_test);
    newConnectome.write(firstRow.c_str(), firstRow.size());
    newConnectome.write(secondRow.c_str(), secondRow.size());
    newConnectome.write(actualConnectome.c_str(), actualConnectome.size());
    newConnectome.write(secondRow.c_str(), secondRow.size());
    newConnectome.close();
    return true;
}

void get_conns(std::vector<ElementInfoModule*>& elements){
    std::vector<ElementInfoModule*> tempElements; // after we get the gaps and chems, we need to fill the real elements vector with only the elements we want (as of now, all neurons and body wall muscles)
    //createElements(tempElements);
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
/////////////// NEED OFFICIAL IDS                    tempElements[i]->idp = realIndex;
                    elements.push_back(tempElements[i]);
                    realIndex++;
                }
                else if (round == 1 && tempElements[i]->getEType() == MUSCLE){ // neuron
/////////////// NEED OFFICIAL IDS                    tempElements[i]->idp = realIndex;
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
            if (((NeuronInfoModule*)elements[i])->neuron_id != elements[i]->id()){
                printf("We have a problem. neuron_id != element_id, during sanity check.\n");
                exit(6);
            }
            numNeurons++;
        }
        if (muscleFound){ // check element_id against numNeurons+muscle_id (due to offset)
            if ((numNeurons + ((MuscleInfoModule*)elements[i])->muscle_id) != elements[i]->id()){
                printf("We have a problem. (numNeurons + muscle_id) != element_id, during sanity check.\n");
                exit(7);
            }
            numMuscles++;
        }
        //printf("EIM: %s, %d\n",elements[i]->name.c_str(),elements[i]->element_type);
    }
    printf("number of neurons in model: %d, number of muscles in model: %d\n",numNeurons, numMuscles);
}

