//
//  main.cpp
//  delegans
//
//  Created by Sean Grimes on 2/2/16.
//  Copyright © 2016 delegans group. All rights reserved.
//

//#include "Core.hpp"
#include "DataSteward.hpp"
#include "TSQueue.hpp"
#include "TheRealDeal.hpp"
#include "GraphicsGovernor.hpp"
#include "DataVisualizer.hpp"
using std::cout;
using std::endl;


int main(int argc, char** argv){
    
    //Core core;
    // Initialization...details in Core class
    //core.init(Core::numKernelLoops);
    //TSQueue<vector<float>> queue;
    size_t global_size = 512;
    size_t local_size = 64;
    DataSteward stewie;
    stewie.init();
    vector<vector<float>> kernel_voltages;
    
    
    //core.stewie.saveCurrentConnectome();
    //exit(0);
    
    for(int i = 0; i < DataSteward::numMainLoops; ++i){
        // Runs the kernel
        //core.run(global_size, local_size);
        stewie.run(global_size, local_size);
        
        // Get a vector<vector<float>> of voltages, each vector<float> is a
        // single iteration
        //kernel_voltages = core.get_voltage_vector();
        
        // NOTE: This will be done by a background thread in core in the future
        kernel_voltages.push_back(stewie.getOutputVoltageVector()); // NOTE: this can be just the first index because we return after each iteration
        
    }
    // Cleans up CPU related memory, GPU memory handled automatically
    //core.clean_up();
    
    // Just a basic runtime report and small sample of elements
    stewie.printReport(DataSteward::numKernelLoops);
    
    //int aselIdx = core.stewie.n2idxMap["ASEL"];
    int numKVs = kernel_voltages.size();
    printf("size of 'kernel_voltages': %d\n", numKVs);
    for (int i = 0; i < numKVs; ++i){
        for (int j = 0; j < kernel_voltages[i].size(); ++j){
            printf("%.2f, ",kernel_voltages[i][j]);
        }
        printf("\n");
    }
    //printf("info: %d, %d\n", aselIdx, core.stewie.bioElements[aselIdx]->element_id);
    stewie.kernelVoltages = kernel_voltages;
    DataVisualizer vizzer = DataVisualizer(&stewie);
    // NOTE: must create set
    const int plotSetSize = 6;
    std::string plotSet[plotSetSize] = {"SO2","SCO2","IPO2", "INO2", "MINHALE", "MEXHALE"};
    //vizzer.plotAll();
    vizzer.plotSet(plotSet, plotSetSize);
    //initGraphics(&core);
    exit(89);

    
    
        // this runs the 3d model.
    //gitErDone(&argc, argv);
    
    
    
    return 0;
}
