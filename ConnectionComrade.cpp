//
//  ConnectionComrade.cpp
//  delegans
//
//  Created by onyx on 10/12/16.
//  Copyright © 2016 Andrew W.E. McDonald. All rights reserved.
//

#include "ConnectionComrade.hpp"


bool ConnectionComrade::doGaps = true;
bool ConnectionComrade::doChems = true;
bool ConnectionComrade::ANY_MUSCLE_WEIGHT = true;


ConnectionComrade::ConnectionComrade(std::vector<ElementInfoModule*> elements, std::string* startingNeurons,int numStartingNeurons){
    this->elements = elements;
    this->numStartingNeurons = numStartingNeurons;
    this->startingNeurons = startingNeurons;
    num_total_gaps = 0;
    num_gap_indices = 0;
    num_total_chems = 0;
    num_chem_indices = 0;
    num_all_indices = 0;
    gap_conns = new float[500000];
    chem_conns = new float[500000];
}

template int ConnectionComrade::addConn(float* connArray, int starting_index, NeuronInfoModule* premod, NeuronInfoModule* postmod, float preOffset, float postOffset);
template int ConnectionComrade::addConn(float* connArray, int starting_index, MuscleInfoModule* premod, NeuronInfoModule* postmod, float preOffset, float postOffset);
template int ConnectionComrade::addConn(float* connArray, int starting_index, NeuronInfoModule* premod, MuscleInfoModule* postmod, float preOffset, float postOffset);
template int ConnectionComrade::addConn(float* connArray, int starting_index, MuscleInfoModule* premod, MuscleInfoModule* postmod, float preOffset, float postOffset);

bool ConnectionComrade::resetConns(){
    delete gap_conns;
    delete chem_conns;
    delete all_conns;
    num_total_gaps = 0;
    num_gap_indices = 0;
    num_total_chems = 0;
    num_chem_indices = 0;
    num_all_indices = 0;
    gap_conns = new float[500000];
    chem_conns = new float[500000];
    if (!currentlyUsedConns.empty()){
        currentlyUsedConns.erase(currentlyUsedConns.begin());
        currentlyUsedConns.clear();
    }
    if (currentlyUsedConns.size() != 0){
        printf("Fuck this, I'm out.\n");
        exit(0); // haha, sean's going to really love this one.
    }
    for (int i = 0; i < elements.size(); i++){
        elements[i]->marked = false;
        int numOuts = elements[i]->out_conns.size();
        for (int j = 0; j < numOuts; ++j){
            elements[i]->out_conns[j].marked = false;
        }
    }
    return true;
    
}


/* this is all going to have to be redone for the 1D matrix representation
 */
std::queue<Connection*> ConnectionComrade::enqueueDesiredConns(int arg){
    
    std::queue<Connection*> cq;
    if (arg & STARTING_SET){
         //std::string start[2] = {"ASEL","ASER"};
        std::vector<int> startingIndices;
        //ElementType end_type = MOTOR;
        int pathLenCeiling = OptionForewoman::WormOpts[PATH_LENGTH_TO_SHOW];
        int curPathLen = 0;
        int minWeight = OptionForewoman::WormOpts[PATH_WEIGHT_TO_SHOW];
        
        for (int i = 0; i < elements.size(); i++){
            for (int j = 0; j < numStartingNeurons; j++){ // NOTE: we can probably get rid of this by using the name to index map in DataSteward
                if (elements[i]->name() == startingNeurons[j]){
                    startingIndices.push_back(i);
                }
            }
        }
        //#SanityCheck - if startingIndices.size() != numStartingNeurons that means we didn't find all our starting neurons -- this is bad.
        if (startingIndices.size() != numStartingNeurons){
            printf("We didn't find all our starting neurons to draw connections! This is bad.\n");
            exit(4);// im sprinking exits all over the place. this is also bad.
        }
        
            // set the current layer of starting neurons to currentLayerIndices (initially the starting neurons),
        // and collect the indices for the following layer. then, once we've gone though the current layer,
        // set currentLayerIndices = nextLayerIndices, and do it again until we reach maxPathLen layers
        std::vector<int> nextLayerIndices;
        std::vector<int> currentLayerIndices = startingIndices;
        int numCurrentStarts = currentLayerIndices.size();
        bool moreLayers = true;
        while (moreLayers){
            curPathLen++;
            for (int i = 0; i < numCurrentStarts; i++){
                std::vector<Connection>* connVec = &(elements[currentLayerIndices[i]])->out_conns;
                int cvSize = (*connVec).size();
                for (int j = 0; j < cvSize; j++){
                    //MUSCLE CHECK
                    if (!OptionForewoman::WormOpts[SHOW_MUSCLES] && (*connVec)[j].post->getEType() == MUSCLE){
                        continue;
                    }
                    ///////////////// END TEMP == MUSCLE CHECK
                    if (!(*connVec)[j].marked && (*connVec)[j].weight() >= minWeight){ // if we haven't already gone through it, and it's at least our minimum weight, go on
                        (*connVec)[j].path_len = curPathLen; // obviously relative to our set of starting neurons.
                        (*connVec)[j].marked = true;
                        // add it to the set of next layer start indices if we haven't reached our max path length yet
                        if (curPathLen < pathLenCeiling){
                            // add the post (pre is the one we are looping through) to the set of start indices, and increment num_starts_found
                            nextLayerIndices.push_back((*connVec)[j].post->id());
                        }
                        cq.push(&(*connVec)[j]);
                    }
                }
            }
            if (!nextLayerIndices.empty()){ // if it's not empty, set it to current, then create a new vector for the new nexts (if they come)
                currentLayerIndices = nextLayerIndices;
                numCurrentStarts = currentLayerIndices.size();
                nextLayerIndices = std::vector<int>();
            }
            else {
                moreLayers = false;
            }
            
        }
        if (arg & MUSCLES){
            for (int i = 0; i < elements.size(); i++){
                std::vector<Connection>* theseConns = &elements[i]->out_conns;
                int numConns = theseConns->size();
                for (int j = 0; j < numConns; j++){ // NOTE: we can probably get rid of this by using the name to index map in DataSteward
                    if ((*theseConns)[j].post->getEType() == MUSCLE && !(*theseConns)[j].marked && ((*theseConns)[j].weight() >= minWeight || ANY_MUSCLE_WEIGHT)){
                        (*theseConns)[j].marked = true;
                        cq.push(&(*theseConns)[j]);
                    }
                }
            }
        }
            
    }
    return cq;
}





void ConnectionComrade::generateConns(int arg){
    
    // either we generate connections from our starting set
    // or we generate connections from motor neurons
    // or both
    std::queue<Connection*> cq = enqueueDesiredConns(arg);
    populateConnArray(cq);

    
    
    
}

void ConnectionComrade::populateConnArray(std::queue<Connection*>& cq){
    while (!cq.empty()){
        Connection* cur = cq.front();
        float preHalfSomaScaled;
        float postHalfSomaScaled;
        void* prevoid = cur->pre;
        void* postvoid = cur->post;
        bool preIsNeuron = true;
        bool postIsNeuron = true;
        if (((ElementInfoModule*)prevoid)->getEType() == MUSCLE){
            preHalfSomaScaled = 0;
            preIsNeuron = false;
        } else{
            preHalfSomaScaled = NeuronInfoModule::NEURON_SIZE_SCALE*((NeuronInfoModule*)prevoid)->soma_diameter/4.f;
        }
        if (((ElementInfoModule*)postvoid)->getEType() == MUSCLE){
            postHalfSomaScaled = 0;
            postIsNeuron = false;
        } else{
            postHalfSomaScaled = NeuronInfoModule::NEURON_SIZE_SCALE*((NeuronInfoModule*)postvoid)->soma_diameter/4.f;
        }
        /*
         if (postmod->getEType() == SENSORY){
         cq.pop();
         continue;
         }
         */
        // we want to mark the neurons so that we can show/hide neurons without connections (this is done in Artist, under draw_neurons and draw_neuron_names)
        
        if (cur->type == GAP && doGaps){
            if (preIsNeuron && postIsNeuron){
                NeuronInfoModule* premod = (NeuronInfoModule*)prevoid;
                NeuronInfoModule* postmod = (NeuronInfoModule*)postvoid;
                printf("<<GAP>> conn: %s-> %s (%.2f)\n", premod->name().c_str(), postmod->name().c_str(), cur->weight());
                num_gap_indices += addConn(gap_conns, num_gap_indices, premod, postmod, preHalfSomaScaled, postHalfSomaScaled);
            }
            else if (!preIsNeuron && postIsNeuron){
                MuscleInfoModule* premod = (MuscleInfoModule*)prevoid;
                NeuronInfoModule* postmod = (NeuronInfoModule*)postvoid;
                printf("<<GAP>> conn: %s-> %s (%.2f)\n", premod->name().c_str(), postmod->name().c_str(), cur->weight());
                num_gap_indices += addConn(gap_conns, num_gap_indices, premod, postmod, preHalfSomaScaled, postHalfSomaScaled);
            }
            else if (preIsNeuron && !postIsNeuron){
                NeuronInfoModule* premod = (NeuronInfoModule*)prevoid;
                MuscleInfoModule* postmod = (MuscleInfoModule*)postvoid;
                printf("<<GAP>> conn: %s-> %s (%.2f)\n", premod->name().c_str(), postmod->name().c_str(), cur->weight());
                num_gap_indices += addConn(gap_conns, num_gap_indices, premod, postmod, preHalfSomaScaled, postHalfSomaScaled);
            }
            
            else if (!preIsNeuron && !postIsNeuron){
                MuscleInfoModule* premod = (MuscleInfoModule*)prevoid;
                MuscleInfoModule* postmod = (MuscleInfoModule*)postvoid;
                printf("<<GAP>> conn: %s-> %s (%.2f)\n", premod->name().c_str(), postmod->name().c_str(), cur->weight());
                num_gap_indices += addConn(gap_conns, num_gap_indices, premod, postmod, preHalfSomaScaled, postHalfSomaScaled);
            }
            num_total_gaps++;
        }
        else if (cur->type == CHEM && doChems){
            if (preIsNeuron && postIsNeuron){
                NeuronInfoModule* premod = (NeuronInfoModule*)prevoid;
                NeuronInfoModule* postmod = (NeuronInfoModule*)postvoid;
                printf("<<CHEM>> conn: %s-> %s (%.2f)\n", premod->name().c_str(), postmod->name().c_str(), cur->weight());
                num_chem_indices += addConn(chem_conns, num_chem_indices, premod, postmod, preHalfSomaScaled, -postHalfSomaScaled);
            }
            else if (!preIsNeuron && postIsNeuron){
                MuscleInfoModule* premod = (MuscleInfoModule*)prevoid;
                NeuronInfoModule* postmod = (NeuronInfoModule*)postvoid;
                printf("<<CHEM>> conn: %s-> %s (%.2f)\n", premod->name().c_str(), postmod->name().c_str(), cur->weight());
                num_chem_indices += addConn(chem_conns, num_chem_indices, premod, postmod, preHalfSomaScaled, -postHalfSomaScaled);
            }
            else if (preIsNeuron && !postIsNeuron){
                NeuronInfoModule* premod = (NeuronInfoModule*)prevoid;
                MuscleInfoModule* postmod = (MuscleInfoModule*)postvoid;
                printf("<<CHEM>> conn: %s-> %s (%.2f)\n", premod->name().c_str(), postmod->name().c_str(), cur->weight());
                num_chem_indices += addConn(chem_conns, num_chem_indices, premod, postmod, preHalfSomaScaled, -postHalfSomaScaled);
            }
            
            else if (!preIsNeuron && !postIsNeuron){
                MuscleInfoModule* premod = (MuscleInfoModule*)prevoid;
                MuscleInfoModule* postmod = (MuscleInfoModule*)postvoid;
                printf("<<CHEM>> conn: %s-> %s (%.2f)\n", premod->name().c_str(), postmod->name().c_str(), cur->weight());
                num_chem_indices += addConn(chem_conns, num_chem_indices, premod, postmod, preHalfSomaScaled, -postHalfSomaScaled);
            }
            num_total_chems++;
        }
        currentlyUsedConns.push_back(cur);
        cq.pop();
    }
    num_all_indices = num_gap_indices + num_chem_indices;
    all_conns = new float[num_all_indices];
    int i;
    for (i = 0; i < num_gap_indices; ++i){
        all_conns[i] = gap_conns[i];
    }
    for (int j = 0; j < num_chem_indices; ++j){
        all_conns[i] = chem_conns[j];
        ++i;
    }
    int connNum = 0;
    for (i = 0; i < num_all_indices; i += 6){
        currentlyUsedConns[connNum]->midpoint = (glm::vec3(all_conns[i], all_conns[i+1],all_conns[i+2])+glm::vec3(all_conns[i+3], all_conns[i+4],all_conns[i+5]))/2.f;
        char buf[10]; // why not. don't want it too small...
        snprintf(buf, sizeof(buf), "%.2f",currentlyUsedConns[connNum]->weight());
        currentlyUsedConns[connNum]->weightLabel = std::string(buf);
        connNum++;
    }
}
