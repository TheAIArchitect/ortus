//
//  Architect.cpp
//  ortus
//
//  Created by andrew on 4/7/17.
//  Copyright © 2017 Andrew W.E. McDonald. All rights reserved.
//

#include "Architect.hpp"

/** This is TheArchitect. */

/* this will also be called to update the connectome,
 * e.g., upon the introduction of a new combination of stimuli.
 *
 * at least, i think so...
 */



Architect::Architect(){};


void Architect::setConnectome(Connectome* cp){
    connectomep = cp;
}

void Architect::setDataSteward(DataSteward* ds){
    dataStewardp = ds;
}

// 'attribs' (second arg) is an OUTPUT...
ElementInfoModule* Architect::createSEI(ElementInfoModule* pre, std::unordered_map<RelationAttribute, cl_float>& attribs){
    std::string interName = "i"+pre->name;
    // sensory extension interneuron
    ElementInfoModule* seip = dataStewardp->addElement(interName);
    // put into vector that will link SEIs together.
    // this will need to be modified to keep different types separate (e.g. VISUAL, AUIDO)
    // but for now, we'll just connect them all together.
    connectomep->seiElements.push_back(seip);
    seip->isSEI = true;
    pre->marked = true; // mark this sensory element so we know not to create a second SEI for it, when we create SEIs for any sensory elements that don't have causal relations... this is a temporary fix.. really should do this before, and 'transfer' things like the causal relationship to the SEI. might need to rework reading the ORT file in, or storage, or something.
    seip->setType(ElementType::INTER);
    seip->setAffect(pre->getEAffect());
    std::unordered_map<RelationAttribute, cl_float> newRelAttribs;
    newRelAttribs[RelationAttribute::Polarity] = 1.f;
    ElementRelationType ert = ElementRelationType::CAUSES;
    newRelAttribs[RelationAttribute::Type] = static_cast<float>(ert);
    // age, mutability, thresh, decay??
    ElementRelation* newRelp = dataStewardp->addRelation(newRelAttribs, pre, seip, ert);
    newRelp->setCSWeight(1.f);
    attribs = newRelAttribs;
    return seip;
}


/**
 * maybe need to change .ort file s.t. causes and correlated are the two primary categories,
 * and each element has a 'precendence' -- not A "dominates" B.
 *
 * ALL sensory neurons have at least very weak GJs with all(?) emotional states, 
 * with some haveing strong ones... the whole system revolves around breathing,
 * so, breathing should be hardwired to pleasure, so that pleasure is inherently good.
 *
 * sensory consolidation (e.g., if 4 visual sensors, once the image being seen has been processed,
 * maybe then that gets tied to emotion?
 * 
 * initially, not enough resolution for it to matter.
 */
void Architect::designConnectome(){
    
    int i,j;
    int causalIndex = 0;
    int numCausal = connectomep->causesRelations.size();
    std::vector<ElementRelation*> elRelVec;
    int elRelVecSize;
    ElementRelation* elRelp;
    //for (i = 0; i < numCausal; ++i){
    while (causalIndex < connectomep->causesRelations.size()){
        // all relations in this vector should have the same pre
        elRelVec = connectomep->causesRelations[causalIndex];
        elRelVecSize = elRelVec.size();
        for (j = 0; j < elRelVecSize; ++j){
            elRelp = elRelVec[j];
            if (elRelp->preId != i){
                printf("PreId != i -> %d != %d\n",elRelp->preId, causalIndex);
            }
            // so, now we need to create a causal relationship between the pre and post elements.
           
            // make an inter for the sensory pre's, connect them with a CS,
            // and connect the inter to the post of the current elRelp
            // further, anything that is caused by the sense, should actually come from the inter.
            if (elRelp->pre->getEType() == ElementType::SENSE){
                std::unordered_map<RelationAttribute, cl_float> newRelAttribs;
                ElementInfoModule* seip = createSEI(elRelp->pre, newRelAttribs);
                newRelAttribs;
                printf("don't forget to check newRelAttribs!!!\n");
                exit(0);
                // now we connect the inter to the 'post' specified by elRelp
                // for the time being, we can re-use the attribute map
                ElementRelationType ert = ElementRelationType::CAUSES;
                ElementRelation* newRelp = dataStewardp->addRelation(newRelAttribs, seip, elRelp->post, ert);
                newRelp->setCSWeight(1.f);
                //
                // since we added a neuron in the middle, let's check to see if there is an opposing relationship
                // also, this is (probably) a bad way to do this. opposes should be stored in the relation above... searching for it separately seems silly (at the moment...)
                if (connectomep->opposesRelations.find(elRelp->preId) != connectomep->opposesRelations.end()){
                    std::vector<ElementRelation*> tempRelVec = connectomep->opposesRelations[elRelp->preId];
                    int numOpposes = tempRelVec.size();
                    for (int k  = 0; k < numOpposes; ++k){
                        // create an inhibitory 'causes' (CS) from the new interneuron, to any 'post' of the element our current 'pre' opposes.
                        std::vector<ElementRelation*> stupidVec = connectomep->causesRelations[tempRelVec[k]->postId];
                        int stupidLen = stupidVec.size();
                        for ( int z = 0; z < stupidLen; ++z){
                            newRelAttribs[RelationAttribute::Polarity] = -1.f;
                            newRelp = dataStewardp->addRelation(newRelAttribs, seip, stupidVec[z]->post, ert);
                            newRelp->setCSWeight(1.f);    
                        }
                        
                    }
                }
                //// Must now delete old relation, or mark for deletion, or something...
                    //// ALSO probably want to do something about the 'opposes'
            }
            else if (elRelp->pre->getEType() == ElementType::MOTOR){
                elRelp->setCSWeight(1.f);
                elRelp->setAttribute(RelationAttribute::Polarity, elRelp->getAttribute(RelationAttribute::PostDirection));
            }
            else if (elRelp->pre->getEType() == ElementType::MUSCLE){
                elRelp->setCSWeight(1.f);
                elRelp->setAttribute(RelationAttribute::Polarity, elRelp->getAttribute(RelationAttribute::PostDirection));
            }
            
        }
        causalIndex++;
    }
    // this should eventually include sensors in an array, i think.
    int numPrimarySenoryElements = connectomep->primarySensoryElements.size();
    for (i = 0; i < numPrimarySenoryElements; ++i){
        if (!connectomep->primarySensoryElements[i]->marked){ // then we create an SEI for it.
            std::unordered_map<RelationAttribute, cl_float> newRelAttribs;
            ElementInfoModule* seip = createSEI(connectomep->primarySensoryElements[i], newRelAttribs);
            connectomep->seiElements.push_back(seip);
        }
        connectomep->primarySensoryElements[i]->marked = false; // don't need this anymore, so 'reset' all values
    }
    
    /** NOTE:
     * will need to implement sensor array creation. e.g.:
     * visual system, array size 4. so, create 4 visual neurons,
     * and then within those 4, create inters that connect to give a resolution of 2.
     * same idea for other array sensors.
     *
     * then, put the inters that should be consolidated with other types of sensory inters 
     * in a vector or something (but only ones that have a consolidation level of 3 sensors or more)
     *
     * then all of the consolidated sensory inters go through the emotion mapping section below
    /**
     NOTE: need a way to differentiate different sensory groups... maybe? .. yes. all sensors that
     * originate from a certain 'type' e.g., VISUAL, should have an int that is mapped to from a map in 
     * connectome, so that sensorType = theMap["VISUAL"], and then sensorArrayId = its position (e.g, 0, 1, 2, 3)
     *
     * as there is no physical body, it will be up to the stimulus and analyzer to determine the actual topography.
     */
    // emotion mapping.
    
    // below rule may not be sustainable, but should work for now...
    // now we want to connect the inters from the sciElementsToLink vector (also, need to connect to:
    //      #
    //      # a *new* inter for each emotion, and each of those new inters should have GJ connections with the primary neuron for each emotion (0th index in each emotion's element pointer vector)
    //      # then take that new inter, and give it a causal to the sensory consolitatory inter that causes it to activate... NOTE: the weight there *must* be far smaller than the weight causing the emotion inter to activate... other wise, there will be a bad feedback loop.
    
    // goal is to link all Sensory Extension Interneurons (SEIs) together
    int numToLink = connectomep->seiElements.size();
    //int numToLink = connectomep->sciElementsToLink.size();
    
    // currently, we are going to do all possible combinations... that is:
    // all groups of 1, all groups of 2, all groups of 3, etc...
    // order doesn't matter, so it's simply nCr (n choose r)
    int numLinks = 0;
    // start at 2, because SEI elements don't need to be linked to themselves.
    // go all the way to numToLink, because we need to link them all together, though some of these can probably be pruned away
    //for (i = 2; i <= numToLink; ++i){
    //    numLinks += nCr(numToLink, i);
    //}
    
    
    std::vector<std::vector<int>> allLinkGroups;
    for (i = 2; i < numToLink; ++i){
        int groupSize = i;
        for (j = 2; j < numToLink; ++j){
    
    
    std::vector<ElementInfoModule*> stagingElements; //
    for (i = 0; i < numToLink; ++i){
        
        std::string interName = "i"+pre->name;
        // sensory extension interneuron
        ElementInfoModule* seip = dataStewardp->addElement(interName);
        // put into vector that will link SEIs together.
        // this will need to be modified to keep different types separate (e.g. VISUAL, AUIDO)
        // but for now, we'll just connect them all together.
        connectomep->seiElements.push_back(seip);
        seip->isSEI = true;
        pre->marked = true; // mark this sensory element so we know not to create a second SEI for it, when we create SEIs for any sensory elements that don't have causal relations... this is a temporary fix.. really should do this before, and 'transfer' things like the causal relationship to the SEI. might need to rework reading the ORT file in, or storage, or something.
        seip->setType(ElementType::INTER);
        seip->setAffect(pre->getEAffect());
        std::unordered_map<RelationAttribute, cl_float> newRelAttribs;
        newRelAttribs[RelationAttribute::Polarity] = 1.f;
        ElementRelationType ert = ElementRelationType::CAUSES;
        newRelAttribs[RelationAttribute::Type] = static_cast<float>(ert);
        // age, mutability, thresh, decay??
        ElementRelation* newRelp = dataStewardp->addRelation(newRelAttribs, pre, seip, ert);
        newRelp->setCSWeight(1.f);
    }
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    printf("format: (cs weight/polarity)\n");
    for (i = 0; i < Ort::NUM_ELEMENTS; ++i){
        printf("\t\t\t\t|%s",connectomep->indexMap[i].c_str());
    }
    printf("\n");
    for (i = 0; i < Ort::NUM_ELEMENTS; ++i){
        printf("%s(%d|%d)\t\t|",connectomep->indexMap[i].c_str(), i, connectomep->elementModules[i]->id);
        for (j = 0; j < Ort::NUM_ELEMENTS; ++j){
            // need to flip incides if accessing blades directly, because 2D+ stuff is stored transposed
            float polarity = dataStewardp->relationAttributeBladeMap[RelationAttribute::Polarity]->getv(j,i);
            float weight = dataStewardp->weightBladeMap[WeightAttribute::CSWeight]->getv(j,i,0);
            printf("\t\t(%.2f, %.2f)",weight, polarity);
        }
        printf("\n");
    }
    printf("--------------------------\n");
    connectomep->cat();
    exit(2);
    
    
    
    ///////////////////////
    /*maybe need to add a 'dominated by':
        - question is: how to deal with a relation weight between two elements, if the *pre* element is dominated by some other element?
    
     */
     
   /* mappings:
    # relation type -> use enum values
    # element type -> use enum values
    # "causes":
        - if direction -1 (check to make sure that's how it's getting set... -1 or 1):
            > set polarity to -1, csWeight to 1
    
    
    
    
    */
    
    
    
    
    /*
     so, we have a vector of
     - elements
     - element relations
     
     => correlated means GJ (electrical, bidirectional)
     => causes means CS (chemical, unidirectional)
     => if A dominates B, inhibitory CS  from A to B
     => if A opposes B:
        -> create two new interneurons (Ai, Bi), one each for A and B, with the same affect as their 'parent'
            -> A excites Ai
            -> B excites Bi
        -> Ai inhibits whatever Bi excites
        -> Bi inhibits whatever Ai excites
        -> if A also dominates B, Bi must inhibit whatever Ai excites with a weight less than the weight between Ai and what Bi excites.
     => every sensory element should have at least 1 interneuron
     => if two stimuli (A and E) happen together, build association by creating a new interneuron,
     that gets input from Ai and Ei
     
     (later)
    => vision system:
     
    => plain/pleasure: a 'central' pain/pleasure center, that innervates all sensory neurons (in some way)
     
     
     
     
    => not relevant for architect, but "LIFE", so, every timestep, should 'use' O2, and 'produce' CO2.
     
     
     */
     
     
}



long Architect::nCr(int n, int r){
    long nFact = fact(n);
    long rFact = fact(r);
    long n_minus_rFact = fact(n-r);
    return (nFact/((double)(rFact*n_minus_rFact)));
}

long Architect::fact(int f){
    long result = 1;
    for (int i = (int)f; i > 0; --i){
        result *= i;
    }
    return result;
}


















/**
 *
 * 1) S == sense, E == emotion, M == motor
 * 2) for each sense S:
 *      create 2 interneurons???, one positive (P<sense>), and one negative (N<sense>)
 *      if S is desirable (good):
 *          S excites P<sense> and inhibits N<sense>
 *      if S is undesirable (bad):
 *          S inhibits P<sense> and excites N<sense>
 */

/** NOTES FROM old DataSteward */
/* we need another blade:
 
 =========> 'active state', cut down on this.
 -----------> 'sleeping state', or 'reflective state', maybe go back and do something?
 
 
 
 
 so, just as the connectome is a square matrix, and each entry is a weight,
 in this blade, each entry will be a crossCorrelation
 
 maybe -- pass in two, three, etc.. historic voltage arrays, and compute the xcorr in the kernel,
 and then alter the weight based upon that:
 -> if postiviely correlated at 0 offset (signals are on top of eachother):
 -> if positive change in correlation (so, more positive):
 -> increase strength of excititory synapse
 -> if negative change in correlation (so, less positive):
 -> decrease strength of excitatory synapse
 -> if inversely correlated at 0 offset (signals are on top of eachother)::
 -> if positive change in correlation (so, more negative):
 -> increase strength of inhibitory synapse
 -> if negative change in correlation (so, less negative):
 -> decrease strength of inhibitory synapse
 -> if postiviely correlated at offset > 0, where presynaptic neuron's signal is offset:
 -> if positive change in correlation (so, more positive):
 -> increase strength of excititory synapse
 -> if negative change in correlation (so, less positive):
 -> decrease strength of excitatory synapse
 -> if inversely correlated at offset > 0, where presynaptic neuron's signal is offset:
 -> if positive change in correlation (so, more negative):
 -> increase strength of inhibitory synapse
 -> if negative change in correlation (so, less negative):
 -> decrease strength of inhibitory synapse
 
 
 => note: 'change' refers to xcorr value from previous time-step (iteration)
 
 So, how do gap junctions factor into this?
 ->
 * if neuron A, at time n (A[n]) has an activation, x, and
 * if neuron B, at time n (B[n]) has an activation, y, and
 * if A[n+1] has an activation x+dX, then:
 * if B[n+1] has an activation y+dY
 => we have a correlation
 * if B[n+1] has an activation,  y, and if B[n+2] has an activation y+dY
 => we have a causal relationship
 
 
 
 
 */
/** END NOTES from old DataSteward */

/*
 void DataSteward::createConnections(){
 printf("connectome_rows, connectome_cols: %d. %d\n",CONNECTOME_ROWS, CONNECTOME_COLS);
 gaps = new Blade<float>(clHelperp, CL_MEM_READ_WRITE, CONNECTOME_ROWS, CONNECTOME_COLS, MAX_ELEMENTS, MAX_ELEMENTS);
 chems = new Blade<float>(clHelperp, CL_MEM_READ_WRITE, CONNECTOME_ROWS, CONNECTOME_COLS, MAX_ELEMENTS, MAX_ELEMENTS);
 int start_at = CSV_OFFSET; // it's a square matrix, with the same information on each side of the diagonal (but flipped)... could do one 'side' and add same connection pointer to each pre, but let's hold off... would need to switch the out_conns vector to hold Connection object pointers.
 for (int i = CSV_OFFSET; i < CSV_ROWS; i++){
 
 int pre_id = i - CSV_OFFSET;// there's no offset in our Element vector
 int post_id = -1;
 
 for (int j = CSV_OFFSET; j < CSV_COLS; j++){
 //printf("(%d, %d) >>%s<<\n",i,j,csvDat[i][j].c_str());
 
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
 printf("%s\n",c.toString().c_str());
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
 printf("%s\n",c.toString().c_str());
 elements[pre_id]->out_conns.push_back(c);
 }
 }
 //printf("i = %d\n",i);
 ++start_at;
 }
 }
 */
