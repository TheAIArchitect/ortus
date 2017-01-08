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
#include "TSQueue.hpp"
#include "GraphicsGovernor.hpp"
#include "DataVisualizer.hpp"

using std::cout;
using std::endl;


int main(int argc, char** argv){
    
  

    Steward theStewie;
    theStewie.initialize();
    theStewie.run();
    
    
    // this runs the 3d model.
    //gitErDone(&argc, argv);
    
    
    
    return 0;
}
