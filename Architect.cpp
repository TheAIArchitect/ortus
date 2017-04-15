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
    pre->setSEI(seip);
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
    
    float totalDesiredCSWeight = 1.f;
    float totalDesiredCSMotorToMuscleWeight = 1.f;
    float totalDesiredHardwiredEmotionCSWeight = 0.25f;// as it is now, i think we create a double connection... (because the SEIs connect during the combinatoric thing too..)
    float totalDesiredAssociationPotentialInitializationCSWeight = .25f;
    float totalDesiredGJWeight = 1.f;
    int i,j;
    int causalIndex = 0;
    int numCausal = connectomep->causesRelations.size();
    std::vector<ElementRelation*> elRelVec;
    int elRelVecSize;
    ElementRelation* elRelp;
    // causesRelations will grow, but we don't want to create new SEIs for the SEIs...
    // so, don't recompute size.
    //for (causalIndex = 0; causalIndex < numCausal; ++causalIndex){
    for (auto entry : connectomep->causesRelations){
        // all relations in this vector should have the same pre
        causalIndex = entry.first; // this is the element index
        elRelVec = entry.second;
        elRelVecSize = elRelVec.size();
        for (j = 0; j < elRelVecSize; ++j){
            elRelp = elRelVec[j];
            // so, now we need to create a causal relationship between the pre and post elements.
           
            // make an inter for the sensory pre's, connect them with a CS,
            // and connect the inter to the post of the current elRelp
            // further, anything that is caused by the sense, should actually come from the inter.
            if (elRelp->pre->getEType() == ElementType::SENSE){
                std::unordered_map<RelationAttribute, cl_float> newRelAttribs;
                ElementInfoModule* seip;
                // set the SEI pointer, seip, if it exists. if not, create it.
                if ((seip = elRelp->pre->getSEI()) == NULL){
                    seip = createSEI(elRelp->pre, newRelAttribs);
                }
                // now we connect the inter to the 'post' specified by elRelp
                // for the time being, we can re-use the attribute map
                ElementRelationType ert = ElementRelationType::CAUSES;
                ElementRelation* newRelp = dataStewardp->addRelation(newRelAttribs, seip, elRelp->post, ert);
                if (elRelp->post->eType == ElementType::EMOTION){// then we want to use our 'hardwiredEmotionCSWeight'
                    // doing this because otherwise bad things happen like normal breathing scares the hell out of ortus.
                    newRelp->setCSWeight(totalDesiredHardwiredEmotionCSWeight);
                    
                }
                else {
                    newRelp->setCSWeight(totalDesiredCSWeight);
                }
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
                            newRelp->setCSWeight(totalDesiredCSWeight);
                        }
                        
                    }
                }
                //// Must now delete old relation, or mark for deletion, or something...
                    //// ALSO probably want to do something about the 'opposes'
            }
            else if (elRelp->pre->getEType() == ElementType::MOTOR){
                elRelp->setCSWeight(totalDesiredCSMotorToMuscleWeight);
                elRelp->setAttribute(RelationAttribute::Polarity, elRelp->getAttribute(RelationAttribute::PostDirection));
            }
            /* should deal with muscle stuff in the sensory stimulator...
            else if (elRelp->pre->getEType() == ElementType::MUSCLE){
                elRelp->setCSWeight(totalDesiredCSWeight);
                elRelp->setAttribute(RelationAttribute::Polarity, elRelp->getAttribute(RelationAttribute::PostDirection));
            }
             */
            
        }
    }
    // this should eventually include sensors in an array, i think.
    int numPrimarySenoryElements = connectomep->primarySensoryElements.size();
    for (i = 0; i < numPrimarySenoryElements; ++i){
        if (!connectomep->primarySensoryElements[i]->marked){ // then we create an SEI for it.
            std::unordered_map<RelationAttribute, cl_float> newRelAttribs;
            ElementInfoModule* seip = createSEI(connectomep->primarySensoryElements[i], newRelAttribs);
        }
        connectomep->primarySensoryElements[i]->marked = false; // don't need this anymore, so 'reset' all values
    }
    /*  NOT SURE HOW PERMANENT THIS IS... BUT:
     * here we set the above 'structural' causal relationships to essentially be immutable,
     * so that learning can't screw them up.
     */
    for (auto entry : connectomep->causesRelations){
        std::vector<ElementRelation*> currVec = entry.second;
        for (auto relp : currVec){
            relp->setAttribute(RelationAttribute::Mutability, 0.01f);// just picked this to be small but not zero.
        }
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
    
    
    // each inner vector is a 'link group'
    std::vector<std::vector<int>> allLinkGroups;
    int resolution = 1;
    allLinkGroups = Statistician::getIndicesToLink(numToLink, resolution);
    
    int numLinkGroups = allLinkGroups.size();
    int curGroupSize = 0;
    int k;
    //std::vector<ElementInfoModule*> tempModps;
    for (i = 0; i < numLinkGroups; ++i){
        std::vector<int> curGroup = allLinkGroups[i];
        curGroupSize = curGroup.size();
        //tempModps.clear();
        //tempModps.resize(curGroupSize);
        std::string interName = "";
        for (j = 0; j < curGroupSize; ++j){
            if (curGroupSize == 1){
                // then we have a single SEI, but we still need an SCI for it,
                // because if we don't, the feedback from the EEI will pollute the network.
                // (the SEI feeds into other SCIs)
                interName += connectomep->seiElements[curGroup[j]]->name+"c";
            }
            else {
                interName += connectomep->seiElements[curGroup[j]]->name;
                if (j != curGroupSize-1){
                    interName += "-";
                }
            }
        }
        // sensory consolidatory interneuron
        ElementInfoModule* scip = dataStewardp->addElement(interName);
        connectomep->sciElements.push_back(scip);
        scip->isSCI = true;
        scip->setType(ElementType::INTER);
        // if combining multiple sensors, setting affect doesn't make sense... at least, not at the moment.
        //scip->setAffect(pre->getEAffect());
        std::unordered_map<RelationAttribute, cl_float> newRelAttribs;
        // all excitatory
        newRelAttribs[RelationAttribute::Polarity] = 1.f;
        // all causes
        ElementRelationType ert = ElementRelationType::CAUSES;
        newRelAttribs[RelationAttribute::Type] = static_cast<float>(ert);
        //// NEED TO SET LOW MUTABILITY!!!
        newRelAttribs[RelationAttribute::Mutability] = .01f; // these are sensory links... maybe in the future mutability for them should increase.
        // age, mutability, thresh, decay??
        // now we create as many relations as we have elements in our group to link, and set the weight to be
        // <total desired weight>/<group size>
        for (j = 0; j < curGroupSize; ++j){
            ElementRelation* newRelp = dataStewardp->addRelation(newRelAttribs, connectomep->seiElements[curGroup[j]], scip, ert);
            newRelp->setCSWeight(totalDesiredCSWeight/curGroupSize);
        }
        /**
         could do below in a different loop...
         */
        // now that we have created the connetions going into the SCI,
        // we want to create one of each emotion (right now, only using 2, so just manually write it out)
        std::string eFInterName = connectomep->fearElements[0]->name + std::to_string(connectomep->fearElements.size());
        std::string ePInterName = connectomep->pleasureElements[0]->name + std::to_string(connectomep->pleasureElements.size());
        ElementInfoModule* eFeip = dataStewardp->addElement(eFInterName);
        connectomep->fearElements.push_back(eFeip);
        ElementInfoModule* ePeip = dataStewardp->addElement(ePInterName);
        connectomep->pleasureElements.push_back(ePeip);
        eFeip->isEEI = true;
        eFeip->setType(ElementType::EMOTION);
        eFeip->setAffect(connectomep->fearElements[0]->getEAffect());
        ePeip->isEEI = true;
        ePeip->setType(ElementType::EMOTION);
        ePeip->setAffect(connectomep->pleasureElements[0]->getEAffect());
        //
        // now, first connect the SCI to each of our new EEIs
        //
        std::unordered_map<RelationAttribute, cl_float> sci_to_eei_attribs;
        sci_to_eei_attribs[RelationAttribute::Polarity] = 1.f;
        ert = ElementRelationType::CAUSES;
        sci_to_eei_attribs[RelationAttribute::Type] = static_cast<float>(ert);
        sci_to_eei_attribs[RelationAttribute::Mutability] = 1.f; // very high.
        ElementRelation* sciToFear = dataStewardp->addRelation(sci_to_eei_attribs, scip, eFeip, ert);
        ElementRelation* sciToPleasure = dataStewardp->addRelation(sci_to_eei_attribs, scip, ePeip, ert);
        // use a weight of totalDesiredCSWeight/2.f... just for fun? I suppose to decrease sensitivity? might not be needed.
        sciToFear->setCSWeight(totalDesiredAssociationPotentialInitializationCSWeight);
        sciToPleasure->setCSWeight(totalDesiredAssociationPotentialInitializationCSWeight);
        //
        // next, connect the EEIs' "backlink" to the SCI,
        // BUT IT MUST HAVE A MUCH LOWER WEIGHT!!! (not sure what it should be).. maybe totalDesiredCSWeight * .1?
        //
        std::unordered_map<RelationAttribute, cl_float> eei_to_sci_backlink_attribs;
        eei_to_sci_backlink_attribs[RelationAttribute::Polarity] = 1.f;
        ert = ElementRelationType::CAUSES;
        eei_to_sci_backlink_attribs[RelationAttribute::Type] = static_cast<float>(ert);
        eei_to_sci_backlink_attribs[RelationAttribute::Mutability] = .1;// no justification for this other than it should be somewhat mutable, but shouldn't overpower the 'forward' link...
        ElementRelation* fearToSCI = dataStewardp->addRelation(eei_to_sci_backlink_attribs, eFeip, scip, ert);
        ElementRelation* pleasureToSCI = dataStewardp->addRelation(eei_to_sci_backlink_attribs, ePeip, scip, ert);
        float backlinkMultiplier = .25f;
        fearToSCI->setCSWeight(totalDesiredAssociationPotentialInitializationCSWeight * backlinkMultiplier);
        pleasureToSCI->setCSWeight(totalDesiredAssociationPotentialInitializationCSWeight * backlinkMultiplier);
        //
        // finally, connect each new EEI to its 'primary' emotion (index 0 in each emotions' respective element vector)
        std::unordered_map<RelationAttribute, cl_float> eei_to_e_attribs;
        // all GJ
        ert = ElementRelationType::CORRELATED;
        eei_to_e_attribs[RelationAttribute::Type] = static_cast<float>(ert);
        eei_to_e_attribs[RelationAttribute::Mutability] = .01f; // these are pretty static, id think.
        // age, mutability, thresh, decay??
        ElementRelation* eFeiToE = dataStewardp->addRelation(eei_to_e_attribs, eFeip, connectomep->fearElements[0], ert);
        eFeiToE->setGJWeight(totalDesiredGJWeight);
        ElementRelation* ePeiToE = dataStewardp->addRelation(eei_to_e_attribs, ePeip, connectomep->pleasureElements[0], ert);
        ePeiToE->setGJWeight(totalDesiredGJWeight);
        // need to do the other way, too.
        ElementRelation* eToEfEI = dataStewardp->addRelation(eei_to_e_attribs, connectomep->fearElements[0], eFeip, ert);
        ElementRelation* eToEpEI = dataStewardp->addRelation(eei_to_e_attribs, connectomep->pleasureElements[0], ePeip, ert);
        eToEfEI->setGJWeight(totalDesiredGJWeight);
        eToEpEI->setGJWeight(totalDesiredGJWeight);
        
    }
    
    
    //connectomep->cat();
    
    
    
    ////////// OLD NOTES BELOW (but some may still be helpful) /////////////
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
