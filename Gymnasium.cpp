//
//  Gymnasium.cpp
//  ortus
//
//  Created by onyx on 12/20/16.
//  Copyright © 2016 Andrew W.E. McDonald. All rights reserved.
//

#include "Gymnasium.hpp"

/* NOTES:
 *
 * In order to get ortus to learn that a stimulus is bad,
 * we must introduce it while causing something negative to happen.
 * 
 * need to have a 'good' and 'bad' part of the brain, which we kind of have (bad neurons)
 * but then when the 'good' or 'happy' part is stimulated,
 * that causes the weights of the things that caused it to happen to increase a little bit.
 * Conversely, if the 'bad' part is stimulated, synapses that tie that stimulus to the
 * bad state are triggered. 
 

 * for every new stimulus, perhaps we need to add a new neuron to the 'good' or 'bad' part, 
 * which will be responsible for creating 'meta' or 'different' connections to the synapses
 * that caused the 'good' or 'bad' feeling (that is, the thing that caused activation in the good or bad part)
 */


/* 
 Note 1: S, I, and M, as prefixes to a neuron A, B, C, etc., mean Sensory, Inter, and Motor, respectively.
 Note 2: GJ == gap junction, CS == chemical synapse
 Note 3: any neuron can be connected to any other neuron, theoretically.
 Note 4: Useful: http://www.human-memory.net/types.html
 
1) <<SEE (2)>> If two sensory neurons cause the same response, maybe increase the gap junction weight relative to the degree of similariy in response
    -> e.g. if SA and SB both cause IFEAR to increase, but SA causes IFEAR to increase with weight 5, and SB causes IFEAR to increase with weight 2, maybe the GJ between them goes up according to:
        => currentWeight - <<something to do with the difference between 5 and 2, or something...>>

 
2) Maybe (1) is stupid. Maybe we want to create GJs between neurons that are spacially close, and that are active at the same time, and in the same ways, but *not* in a cause->effect sort of way (which is what we use CS for)
    -> this would create the associations between different, unrelated stimuli!

3) Difference between short-term and long-term memory is learning rate, and decay rate
    -> maybe we want a floor that is movable (perhaps only up), that minimizes the decay
        => this allows for things that ortus simply won't forget, if it gets deeply ingrained enough
    -> maybe we need some sort of threshold to differentiate a long term vs short term connection
        -> or, maybe this is just the floor... at least, i think they're related.
 
4) Not here yet, but 'thinking' is simply triggering interneurons to trigger the same circuits that sensory neurons triggered (think, someone gives a lecture, then you think about it later), which sets off neurons, causes changes in the weights, which causes a slightly different thought next time around.
    -> looking at output neurons would be actions.
 
5) learning can occur via:
    a) emotional associations
        i) rate is influenced by activation of emotion (strong emotion causes stronger connection), 
        ii) as well as repitition
        *===> elevated sensory stimulus gets tied to elevated emotion
    b) non-emotional repititon -- sensory motor sort of thing.
        -> e.g. <<need to think of a simple thing for this>>
 
6) a 'reflective' period, the thinking period, should cause ortus to go and 'think' -- activate neurons tied to emotions (and maybe others), and create gap junctions between neurons that are at an elevated activation together... perhaps?
 
7) new connections are favored. That is, if there is a weight of 0 between the elements we want to connect, we use that. If not, we add an interneuron, and try to get the result we want. If we can't get it, we add 2 more neurons, and try to connect the initially desired one to the 2nd new one, and the 1st new one to the 3rd new one, via some fashion (need to figure this out...), and if that doesn't work, go out, then down, etc..
 
 8) working off of (7), we need a way to take specific sensory inputs, or even specific 'results' from different 'chains' of interneurons, and combine them to 'generalize' -- think, pixels -> lines -> borders idea. Basically, convolution.
        ->
 
 9) maybe this 'working memory' that can hold ~7 'items' is more of a place that allows one to quickly activate 7 distinct 'memories' (i.e., connection chains)
 
 10) thinking about something should increase the chances it is solidified, and aquires a floor high enough to not go away later on
    -> see (2), 
    -> if this floor isn't reached, then the weights go back to nearly zero -- maybe this floor means it'll never completely decay to nothing, though.
 
 
*/

Gymnasium::Gymnasium(){
    
}

/* simple, temporary -- want a modular approach -- 'Stimulator' superclass, for example */
void Gymnasium::stimulateSensors(Blade& voltageBlade, std::unordered_map<std::string,int>& nameToIndexMap){
    // SCO2
    int sco2Idx = nameToIndexMap["SCO2"];
    float CO2_addition = 2.5f;
    voltageBlade.add(sco2Idx, CO2_addition);
    // MINHALE
    //int minhaleIdx = nameToIndexMap["MINHALE"];
    // SO2
    //int so2Idx = nameToIndexMap["SO2"];
    
    return;
}
