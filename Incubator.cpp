//
//  Incubator.cpp
//  ortus
//
//  Created by andrew on 3/20/17.
//  Copyright © 2017 Andrew W.E. McDonald. All rights reserved.
//

#include "Incubator.hpp"

Incubator::Incubator(){
    
}

Incubator::~Incubator(){
    if(ctomeNewed){
        delete ctome;
    }
    ctomeNewed = false;
}

void Incubator::conceive(std::string ortFileName){
    ctome = new Connectome(ortFileName);
    ctomeNewed = true;
}




/**
 * 
 * 1) S == sense, E == emotion, M == motor
 * 2) for each sense S:
 *      create 2 interneurons, one positive (P<sense>), and one negative (N<sense>)
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
