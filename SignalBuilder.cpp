//
//  SignalBuilder.cpp
//  App_ExampleBrowser
//
//  Created by onyx on 3/3/16.
//
//

#include "SignalBuilder.hpp"

// DON'T EDIT
float leftRightOffsetDecimal;
float inverseLeftRightOffsetDecimal;
float muscleToMuscleOffsetDecimal;
int leftRightOffset;
int muscleToMuscleOffset;
int overlap;
int overhang;
int rightStartingOffset;
int SignalBuilder::numWindows = -1;


SignalBuilder::SignalBuilder(){
   
}

SignalBuilder::SignalBuilder(BodyParameterSet* paramSet){
    init(paramSet);
}

void SignalBuilder::init(BodyParameterSet* paramSet){
    this->paramSet = paramSet;
    signal_index = 0;
    first = true;
    /* START DON'T EDIT */
    leftRightOffsetDecimal = paramSet->leftRightOffsetPercent/100.f;
    inverseLeftRightOffsetDecimal = 1.f - leftRightOffsetDecimal;
    muscleToMuscleOffsetDecimal = paramSet->muscleToMuscleOffsetPercent/100.f;
    muscleToMuscleOffset = ceil(muscleToMuscleOffsetDecimal*paramSet->numStates);
    float lengthDueToStatesPerSide = muscleToMuscleOffset * paramSet->num_sections + (paramSet->numStates - muscleToMuscleOffset);
    leftRightOffset = ceil(leftRightOffsetDecimal*lengthDueToStatesPerSide); // in sections
    
    //overlap = ceil(inverseLeftRightOffsetDecimal*paramSet->num_sections*muscleToMuscleOffset);
    //overhang = ceil(paramSet->numStates* (1-muscleToMuscleOffsetDecimal)); // this 'overhang' happens at the last window
    numWindows = lengthDueToStatesPerSide + leftRightOffset;//(muscleToMuscleOffset*paramSet->num_sections + overhang)*2 - overlap;
    rightStartingOffset = leftRightOffset;// ceil((muscleToMuscleOffset*paramSet->num_sections+overhang) -  overlap);
    /* END DON't EDIT */
    //buildSignal(); 
}


std::vector<float> SignalBuilder::buildSingleMuscleContraction(){
    float t = 0.f;
    float offset_unit = (M_PI/((float)paramSet->wave_freq))/paramSet->numStates; // not 2pi beacuse we only care about the positive half of the cycle
    std::vector<float> muscleActivations;
    float amp = paramSet->wave_amp;
    for (int i = 0; i < paramSet->numStates; i++){
        muscleActivations.push_back(amp*sin(t));
        t += offset_unit;
    }
    return muscleActivations;
}
/*
 \frac{\ln \left(-x+25\right)}{3.219} -> ln(-x + 25)/3.129
 */

float SignalBuilder::getForceMultiplier(int section){
    return log(-section+paramSet->forceMultiplierOffset)/log(paramSet->forceMultiplierOffset);
}


void SignalBuilder::buildSignal(){
    initSignal();
    // add shit
    std::vector<float> muscleActivations = buildSingleMuscleContraction();
    
    printf("numStates: %d\n",paramSet->numStates);
    for (int section = 0; section < paramSet->num_sections; section++){
//        int leftSection = (paramSet->num_sections - 1) - section;
        int leftSection = section;
        float forceMultiplier = getForceMultiplier(section);// (.4*(halfSections - fabs(halfSections - leftSection))/((float)halfSections)) + .75;
        for (int j = 0; j < paramSet->numStates; j++){
            signal[(leftSection*muscleToMuscleOffset)+j][leftSection][0] = forceMultiplier * muscleActivations[j];
        }
    }
    int rightOffset = rightStartingOffset;
    for (int section = 0; section < paramSet->num_sections; section++){
//        int rightSection = (paramSet->num_sections - 1) - section;
        int rightSection = section;
        float forceMultiplier = getForceMultiplier(section);//(.4*(halfSections - fabs(halfSections - rightSection))/((float)halfSections)) + .75;
        for (int j = 0; j < paramSet->numStates; j++){
            signal[rightOffset+(rightSection*muscleToMuscleOffset)+j][rightSection][1] = forceMultiplier * muscleActivations[j];
        }
    }
    for (int i = 0; i < numWindows; i++){
        for (int j = 0; j < paramSet->num_sections; j++){
            if (signal[i][j].size() != 2){
                printf("umm... why?\n");
            }
        }
    }
    
    /*
     for (int j = 0; j < num_sections; j++){
     float* temp_floats = (*(*signal)[i])[j];
     t = (((float)j)/num_sections) + total_offset;
     left_sin = amp*sin(2*M_PI*wave_freq*t+wave_offset_left);
     right_sin = amp*sin(2*M_PI*wave_freq*t+wave_offset_right);
     if (left_sin < 0)
     left_sin = 0.f;
     if (right_sin < 0)
     right_sin = 0.f;
     temp_floats[0] = left_sin; // left
     temp_floats[1] = right_sin; // right
     }
     total_offset += offset_unit;
     }
     */
    signal_index = 0;
}

void SignalBuilder::initSignal(){
    signal = std::vector<std::vector<std::vector<float>>>();
    for (int i = 0; i < numWindows; i++){
        std::vector<std::vector<float>> temp_window;
        for (int j = 0; j < paramSet->num_sections; j++){
            std::vector<float> temp_floats = {0,0};
            temp_window.push_back(temp_floats);
        }
        signal.push_back(temp_window);
    }
}

void SignalBuilder::wreckSignal(){
     for (int i = 0; i < numWindows; i++){
        for (int j = 0; j < paramSet->num_sections; j++){
            signal[i][j].clear();
        }
        signal[i].clear();
     }
    signal.clear();
}


std::vector<std::vector<float>>* SignalBuilder::getNextWindow(){
    if (signal_index >= signal.size()){
        signal_index = 0;
        printf("boom\n");
        //return nosignal;
    }
    //printf("len (%d): %d\n",signal_index, signal[signal_index].size());
    signal_index++;
    return &signal[signal_index-1];
}
