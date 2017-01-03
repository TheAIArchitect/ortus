//
//  main.cpp
//  delegans
//
//  Created by Sean Grimes on 2/2/16.
//  Copyright © 2016 delegans group. All rights reserved.
//

#include "Core.hpp"
#include "TSQueue.hpp"
#include "TheRealDeal.hpp"
#include "GraphicsGovernor.hpp"
#include "DataVisualizer.hpp"
using std::cout;
using std::endl;


int main(int argc, char** argv){
    
    Core core;
    // Initialization...details in Core class
    core.init(Core::numKernelLoops);
    TSQueue<vector<float>> queue;
    auto global_size = 512;
    auto local_size = 64;
    vector<vector<float>> kernel_voltages;
    
    
    core.stewie.saveCurrentConnectome();
    exit(0);
    
    for(auto i = 0; i < Core::numMainLoops; ++i){
        // Runs the kernel
        core.run(global_size, local_size);
        
        // Get a vector<vector<float>> of voltages, each vector<float> is a
        // single iteration
        //kernel_voltages = core.get_voltage_vector();
        kernel_voltages.push_back(core.get_voltage_vector()[0]); // NOTE: this can be just the first index because we return after each iteration
        
        /*
        // Add the voltages to the queue
        // NOTE: This will be done by a background thread in core in the future
        for(auto j = 0; j < kernel_voltages.size(); ++j){
            //queue.push(kernel_voltages[j]);
            // now we create a new queue of maps from muscle names to muscle activations, one per entry in the voltages queue (well, what would be in the voltages queue, but instead of waiting, we're just pulling them out of the kernel_voltages -- see commented line above this comment).
            vector<float> theseVoltages = kernel_voltages[j];
            unordered_map<string, vector<MuscleWeight>>::iterator iter;
            iter = core.stewie.n2mGjMap.begin();
            unordered_map<string,float> tempActivationsGJ;
            while (iter != core.stewie.n2mGjMap.end()){
                string name = iter->first;
                    float voltageToAll = theseVoltages[core.stewie.n2idxMap[name]]; // all muscles in mwVec use this voltage (along with their individual weight)
                vector<MuscleWeight> mwVec = iter->second;
                for (int k = 0; k < mwVec.size(); ++k){
                    if (tempActivationsGJ.find(mwVec[k].name) == tempActivationsGJ.end()){
                        tempActivationsGJ[mwVec[k].name] = 0;
                    }
                    tempActivationsGJ[mwVec[k].name] += mwVec[k].weight * voltageToAll;
                }
                iter++;
            }
            DataSteward::muscleActivationsGJ.push_back(tempActivationsGJ);
            // CS
            unordered_map<string, vector<MuscleWeight>>::iterator iterTwo;
            iterTwo = core.stewie.n2mCsMap.begin();
            unordered_map<string,float> tempActivationsCS;
            while (iter != core.stewie.n2mCsMap.end()){
                string name = iter->first;
                float voltageToAll = theseVoltages[core.stewie.n2idxMap[name]]; // all muscles in mwVec use this voltage (along with their individual weight)
                vector<MuscleWeight> mwVec = iter->second;
                for (int k = 0; k < mwVec.size(); ++k){
                    if (tempActivationsCS.find(mwVec[k].name) == tempActivationsCS.end()){
                        tempActivationsCS[mwVec[k].name] = 0;
                    }
                    tempActivationsCS[mwVec[k].name] += mwVec[k].weight * voltageToAll;
                }
                iter++;
            }
            DataSteward::muscleActivationsCS.push_back(tempActivationsCS);
        }
    */
    }
    // Cleans up CPU related memory, GPU memory handled automatically
    //core.clean_up();
    
    // Just a basic runtime report and small sample of elements
    core.print_report(Core::numKernelLoops);
    
    //int aselIdx = core.stewie.n2idxMap["ASEL"];
    int numKVs = kernel_voltages.size();
    printf("size of 'kernel_voltages': %d\n", numKVs);
    for (int i = 0; i < numKVs; ++i){
        //printf("%.4f, %.4f, %.4f, %.4f\n", kernel_voltages[i][0],kernel_voltages[i][1],kernel_voltages[i][2],kernel_voltages[i][3]);
        //printf("ASEL (%d): %.4f\n",i, kernel_voltages[i][aselIdx]);
    }
    //printf("info: %d, %d\n", aselIdx, core.stewie.bioElements[aselIdx]->element_id);
    core.stewie.kernelVoltages = kernel_voltages;
    DataVisualizer vizzer = DataVisualizer(&core.stewie);
    // NOTE: must create set
    const int plotSetSize = 6;
    std::string plotSet[plotSetSize] = {"SO2","SCO2","IPO2", "INO2", "MINHALE", "MEXHALE"};
    //vizzer.plotAll();
    vizzer.plotSet(plotSet, plotSetSize);
    initGraphics(&core);
    exit(89);

    //printf("q = %d... kv = %d\n", queue.get_copy_of_queue().size(), kernel_voltages.size());
    
    //core.stewie.n2mGjMap; // unordered_map<string, vector<MuscleWeight>>
    //core.stewie.n2mCsMap; // unordered_map<string, vector<MuscleWeight>>
    //core.stewie.n2idxMap; // unordered_map<string, int>
    //core.stewie.idx2nMap; // unordered_map<int,string>
    //core.stewie.muscles; // vector<string>
    //int numGJ_activations = DataSteward::muscleActivationsGJ.size();
    //int numCS_activations = DataSteward::muscleActivationsCS.size();
    //printf("num GJ activations: %d\n", numGJ_activations);
    //printf("num CS activations: %d\n", numCS_activations);
    
//    string curSample = "vBWMR14";
//    string curSample = "AVBL";
//    int GJ_ident_ct = 0;
//    int CS_ident_ct = 0;
//    float lastGJ = DataSteward::muscleActivationsGJ[0][curSample];
//    float lastCS = DataSteward::muscleActivationsCS[0][curSample];
//    for(int i = 1; i < numGJ_activations; ++i){
//        if (lastGJ == DataSteward::muscleActivationsGJ[i][curSample]){
//            GJ_ident_ct++;
//        }
//        if (lastCS == DataSteward::muscleActivationsCS[i][curSample]){
//            CS_ident_ct++;
//        }
//        lastGJ = DataSteward::muscleActivationsGJ[i][curSample];
//        lastCS = DataSteward::muscleActivationsCS[i][curSample];
//    }
//    printf("idents -- GJ: %d, CS: %d\n",GJ_ident_ct, CS_ident_ct);
//    exit(0);
    
        // this runs the 3d model.
    //gitErDone(&argc, argv);
    
    
   
    
    
    auto q2 = queue.get_copy_of_queue();
    cout << "Queue size: " << q2.size() << endl;
    
    return 0;
}
