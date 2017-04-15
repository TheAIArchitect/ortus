//
//  main.cpp
// ortus
//
// Andrew W.E. McDonald 
//  Sean Grimes
//
// Copyright 2017
//

#include "Steward.hpp"
//#include "TSQueue.hpp"
//#include "GraphicsGovernor.hpp"
#include "DataVisualizer.hpp"
#include "CLHelper.hpp"
#include "OrtusStd.hpp"

// START DEFAULT VALUES (default values are set below class declaration)
// These get set in OrtUtil, if values are specified in the .ort file
int Ort::NUM_ELEMENTS = 10;
int Ort::MAX_ELEMENTS = 100;
int Ort::ACTIVATION_HISTORY_SIZE = 8; // 7 usable, and the 8th is the 'staging' area -- filled by the current one (but can't be read from because there's no [good] way to ensure other threads have updated theirs). in the future it might be good to create 2 kernel args -- history read only, and r/w for the staging area.

// this is the number of computations that can be stored per 'core',
// e.g., that number of XCorr computations, or Slope comptuations,
// w.r.t. historical values.
int Ort::SCRATCHPAD_COMPUTATION_SLOTS = 4;
int Ort::XCORR_COMPUTATIONS = Ort::SCRATCHPAD_COMPUTATION_SLOTS;
int Ort::XCORR_SIZE = 3;
int Ort::SLOPE_COMPUTATIONS = Ort::SCRATCHPAD_COMPUTATION_SLOTS;
int Ort::SLOPE_SIZE = 2;
int Ort::WEIGHT_HISTORY_SIZE = 4; // not based on anything, really -- seems better than 2, and not sure if 4 is needed (last index is staging area (it might be faster to have the history be read only, and create a different kernel arg for writing the new values in the future).
// END DEFAULT VALUES

int Ort::NUM_ITERATIONS = 800; // 300, really 600



int main(int argc, char** argv){
    
    /* Ortus */
    Steward theStewie;
    theStewie.initialize();
    theStewie.run();
    
    // this runs the 3d model.
    //gitErDone(&argc, argv);
    
    return 0;
}
