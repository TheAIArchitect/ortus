//
//  ParallelTest.cpp
//  cpplib
//
//  Created by Sean Grimes on 5/13/16.
//  Copyright © 2016 Sean Grimes. All rights reserved.
//

#include <iostream>
#include <vector>
#include <stack>
#include <queue>
#include "include/Parallel.hpp"
#include "include/FileUtils.hpp"
#include "include/StrUtils.hpp"
#include "include/NumUtils.hpp"
#include "include/ExeTimer.hpp"
#include "include/Logger.hpp"
#include <cassert>
#include <limits.h>
#include "include/GeneralUtils.hpp"

using namespace std;

void test_parallelTrimStrVec();
int testTask(int i);
void utils();

#ifdef LOG_DEBUG
#define BLAH 1
#else
#define BLAH 0
#endif

int main(){
    
    if(BLAH)
        cout << "BLAH DEFINED\n";
    else
        cout << "NOT DEFINED\n";
    
    cout << "FUNC: " << FUNC << endl;
    
    Logger log;
    log.warn("This is a warning message");
    log.info("This is an info message", FUNC);
    log.error("This is an error message", FUNC, false);
    log.fatal("This is a fatal message");
    log.fatal("FLUSHED");
    log.fatal("NO TIMESTAMP", false);
    log.fatal("BLAH with func", FUNC);
    
    //test_parallelTrimStrVec();
    utils();
    
    
    return 0;
}


void test_parallelTrimStrVec(){
    string small_file = "test_input.txt";
    string large_file = "test_input.txt.large";
    ExeTimer p_small_default_threads;
    ExeTimer p_large_default_threads;
    ExeTimer s_small;
    ExeTimer s_large;
    ExeTimer pool_startup;
    ExeTimer pool_resize;
    ExeTimer small_16_threads;
    ExeTimer large_16_threads;
    
    cout << "Running timing and testing code...\n\n";
    
    auto small = FileUtils::readLineByLine(small_file);
    auto large = FileUtils::readLineByLine(large_file);
    
    // Start the pool with the default number of threads
    pool_startup.start_timer();
    Parallel::startPool();
    pool_startup.stop_timer();
    
    p_small_default_threads.start_timer();
    auto small_trimmed = Parallel::trimStrVec(small);
    p_small_default_threads.stop_timer();
    
    p_large_default_threads.start_timer();
    auto large_trimmed = Parallel::trimStrVec(large);
    p_large_default_threads.stop_timer();
    
    s_small.start_timer();
    auto ser_small = StrUtils::trimStringVector(small);
    s_small.stop_timer();
    
    s_large.start_timer();
    auto ser_large = StrUtils::trimStringVector(large);
    s_large.stop_timer();
    
    pool_resize.start_timer();
    Parallel::setNumberOfThreadsInPool(16);
    pool_resize.stop_timer();
    
    small_16_threads.start_timer();
    auto small_trimmed_2 = Parallel::trimStrVec(small);
    small_16_threads.stop_timer();
    
    large_16_threads.start_timer();
    auto large_trimmer_2 = Parallel::trimStrVec(large);
    large_16_threads.stop_timer();
 
    ExeTimer new_thread;
    new_thread.start_timer();
    future<int> new_thread_fut = async(launch::async, testTask, 5);
    int new_thread_res = new_thread_fut.get();
    new_thread.stop_timer();
    
    // Make sure prior tasks have been completed and timing is done on an empty and ready pool
    Parallel::destroyPool();
    Parallel::startPool();
    ExeTimer pool_timer;
    pool_timer.start_timer();
    future<int> pool_fut = Parallel::runTask(testTask, 5);
    int pool_res = pool_fut.get();
    pool_timer.stop_timer();
    
    cout << "Timing complete, checking results...\n\n";
    
    bool failed = false;
    
    for(auto i = 0; i < small_trimmed.size(); ++i){
        if(small_trimmed[i] != ser_small[i]){
            cout << "\n\n";
            cout << "small_trimmed != ser_small at: " << i << endl;
            cout << "smalled_trimmed: " << small_trimmed[i] << '\n';
            cout << "ser_small: " << ser_small[i] << '\n';
            cout << "\n\n";
            failed = true;
            break;
        }
    }
    
    for(auto i = 0; i < large_trimmed.size(); ++i){
        if(large_trimmed[i] != ser_large[i]){
            cout << "large_trimmed != ser_large at: " << i << endl;
            failed = true;
            break;
        }
    }
    
    for(auto i = 0; i < large_trimmer_2.size(); ++i){
        if(large_trimmer_2[i] != large_trimmed[i]){
            cout << "larger_trimmed_2 != large_trimmed at: " << i << endl;
            cout << "Parallel code failed with large number of threads\n";
            failed = true;
            break;
        }
    }
    
    if(failed)
        cout << "*** Correctness testing complete, TEST FAILURES ***\n";
    else
        cout << "Correctness testing complete, all tests passed!\n";
    
    cout << "Timing results below.\n\n";
    
    cout << "Parallel::startPool(): " << pool_startup.get_exe_time_in_micro() << " us.\n";
    cout << "P::trimStrVec(small): " << p_small_default_threads.get_exe_time_in_micro() << " us.\n";
    cout << "P::trimStrVec(large): " << p_large_default_threads.get_exe_time_in_ms() << " ms.\n";
    cout << "S::trimStringVector(small): " << s_small.get_exe_time_in_micro() << " us.\n";
    cout << "S::trimStringVector(large): " << s_large.get_exe_time_in_ms() << " ms.\n";
    cout << "P::setNumberOfThreadsInPool(16): " << pool_resize.get_exe_time_in_micro() << " us.\n";
    cout << "P::trimStrVec(small) [16 Threads]: " << small_16_threads.get_exe_time_in_micro() << " us.\n";
    cout << "P::trimStrVec(large) [16 Threads]: " << large_16_threads.get_exe_time_in_ms() << " ms.\n";
    cout << "New Thread Time: " << new_thread.get_exe_time_in_micro() << " us. Result: " << new_thread_res << endl;
    cout << "Pool task time: " << pool_timer.get_exe_time_in_micro() << " us. Result: " << pool_res << endl;

}

int testTask(int i){
    return i * i;
}

void utils(){
    string neg_i = "-50";
    string neg_d = "-50.5542";
    string pos_i = "50";
    string pos_d = "50.5542";
    string zero_i = "0";
    string zero_d = "0.0";
    double max = DBL_MAX;
    
    cout << endl << endl;
    cout << NumUtils::strToInt(neg_i) << " | str: " << neg_i << endl;
    cout << NumUtils::strToDouble(neg_d) << " | str: " << neg_d << endl;
    cout << NumUtils::strToInt(pos_i) << " | str: " << pos_i << endl;
    cout << NumUtils::strToDouble(pos_d) << " | str: " << pos_d << endl;
    cout << NumUtils::strToInt(zero_i) << " | str: " << zero_i << endl;
    cout << NumUtils::strToDouble(zero_d) << " | str: " << zero_d << endl;
    cout << endl << endl;
    
    assert(NumUtils::strToDouble(neg_d) == -50.5542);
    assert(NumUtils::strToInt(neg_i) == -50);
    assert(NumUtils::strToDouble(pos_d) == 50.5542);
    assert(NumUtils::strToInt(pos_i) == 50);
    assert(NumUtils::strToDouble(zero_d) == 0.0);
    assert(NumUtils::strToInt(zero_i) == 0);
    assert((NumUtils::strToDouble(StrUtils::numToString(max))) == (atof(StrUtils::numToString(max).c_str())));
}




