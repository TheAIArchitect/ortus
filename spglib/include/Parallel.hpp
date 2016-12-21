//
//  Parallel.hpp
//  cpplib
//
//  Created by Sean Grimes on 5/13/16.
//  Copyright © 2016 Sean Grimes. All rights reserved.
//

/*
 * Implements some of the algorithms in StrUtils, FileUtils, NumUtils, GeneralUtils in parallel.
 *
 * The pool is static, and all function calls will use the same pool. The pool will be started at 
 * the first call to startPool, subsequent calls will do nothing.
 * 
 * Calls to setNumberOfThreadsInPool will unfortunately (as of now) kill the current pool and start
 * a new pool once tasks in the current pool have finished. 
 * 
 * Calls to serSerialCrossoverPoint will only impact tasks that have not yet been submitted, any 
 * task that is currently running will use the old value.
 *
 * Defaults:
 *  - NUM_THREADS, defaults to std::thread::hardware_concurrency();
 *  - JUMP_TO_SERIAL, defaults to 15 tasks per thread
 *      - e.g., trimStrVec where NUM_THREADS = 4 and the vector has 50 strings to be trimmed:
 *          The parallel call will not split the vector up between the threads, instead it will 
 *          call the serial version of that function
 *      - e.g., trimStrVec where NUM_THREADS = 4 and the vector has 100 strings, it will
 *          split the vector into NUM_THREADS component vectors and concurrently pass them through
 *          the serial version of trimStrVec, after the threads have returned the vectors will be
 *          reassembled into a single vector and returned. 
 *      - My testing on my machine tells me that 15 items per thread is the right number, below that
 *        it does not make sense to split and reassemble the data. Your results may vary. 
 *  
 *  NOTE:   Calculating number of items per thread obviously incurs some overhead, if you are fairly
 *          certain that your data does not need to be parallelized this overhead can add up. Each
 *          call to the parallel version of a function incurs about 80 microseconds of overhead
 *          compared with the serial version
 *          
 *          The remaining overhead (hence the 15 tasks per thread) comes from splitting the data
 *          add then putting it back together after the threads complete. 
 *
 *          My testing has shown the parallel functions to produce the same output as their serial
 *          versions (testing in ParallelTest.cpp), however more stringent testing is required to be
 *          considered 'production ready' (though that's how I'm using it...). This class will not 
 *          be merged into the stable (master) branch until said testing is complete.
 *
 *  startPool() is called at the beginning of all algorithms and runTask with the currently defined
 *  values for NUM_THREADS and JUMP_TO_SERIAL. If a pool is running nothing will happen, if a pool 
 *  has not already been started a new one will be started.
 */

#pragma once

#include <stdio.h>
#include <string>
#include <algorithm>
#include <vector>
#include <fstream>
#include <future>
#include <iostream>
#include <cmath>
#include "ThreadPool.hpp"
#include "Logger.hpp"
#include "StrUtils.hpp"
#include "GeneralUtils.hpp"
#include "FileUtils.hpp"
#include "ExeTimer.hpp"

class Parallel{
public:
    
    // Will create a new pool after killing the old one (if exists) with num_threads threads
    static void setNumberOfThreadsInPool(size_t num_threads);
    
    // Will not create new pool, however current tasks will use the old serial value
    static void setSerialCrossoverPoint(size_t num_tasks_per_thread);
    
    // Instantiate pool if none exists, can be called to start threads at arbitraty point in code to
    // save thread startup time at a more critical place in the code
    static void startPool(size_t num_threads, size_t num_tasks_per_thread);
    static void startPool();
    
    // This will wait until tasks in the pool are finished, currently no way to immediately cancel
    static void destroyPool();
    
    // Runs StrUtils::trimStringVector in parallel on ThreadPool
    static std::vector<std::string> trimStrVec(const std::vector<std::string> &v);
    
    // Counts objToCount in vec in parallel, using GeneralUtils
    template<class T>
    static size_t numOccuranceInVec(const std::vector<T> &vec, const T &objToCount){
        return 1;
    }
    
    // Removes eleToRemove from vec in parallel, using GeneralUtils
    template<class T>
    static void removeEleFromVec(std::vector<T> &vec, const T &eleToRemove){
        return;
    }
    
    // Runs a single task on the ThreadPool, delegating task creation and management to ThreadPool
    template<class F, class... Args>
    static auto runTask(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>{
        fprintf(stderr, "Parallel has mistakenly been migrated to master from dev, runTask does not currently work!\n");
#ifndef USE_DEV_CLASSES
        throw std::logic_error("Parallel runTask is currently broken and shouldn't be in master, use ThreadPool instead");
#endif
        startPool();
        fprintf(stderr, "***Parallel::runTask is currently broken***\n");
        fprintf(stderr, "Use ThreadPool::push as a temporary replacement\n");
#ifdef LOG_DEBUG
        Logger log;
        log.error("Currently broken, ThreadPool::push is a temporary replacement", FUNC);
#endif
        return mPool->push(f, std::forward<Args>(args)...);
    }
    
    
private:
    
    // Determine if serial version of algorithm should be run
    template<class T>
    static bool checkSize(const std::vector<T> &vec){
        auto tasks_per_thread = vec.size() / NUM_THREADS;
#ifdef LOG_DEBUG
        Logger log;
        log.info("tasks_per_thread: " + std::to_string(tasks_per_thread), FUNC);
        log.info("JUMP_TO_SERIAL: " + std::to_string(JUMP_TO_SERIAL), FUNC);
#endif
        if(tasks_per_thread < JUMP_TO_SERIAL)
            return false;
        return true;
    }
    
    // Splits input data up for parallel operation, could use some optimizations
    template<class T>
    static std::vector<std::vector<T>> splitData(const std::vector<T> &v){
        auto size = v.size();
        auto tasks_per_thread = size / NUM_THREADS;
        
        std::vector<std::vector<std::string>> thread_vecs;
        for(auto i = 0; i < NUM_THREADS; ++i){
            std::vector<std::string> vec;
            thread_vecs.push_back(vec);
        }
        
        // FIXME: Speedup this process with iterators???
        // Split the data up into NUM_THREADS vectors
        auto num_loops = tasks_per_thread * NUM_THREADS;
        auto vec_num = 0;
        for(auto i = 0; i < num_loops; ++i){
            thread_vecs[vec_num].push_back(v[i]);
            if(i % tasks_per_thread == 0 && i != 0)
                ++vec_num;
        }
        
        // Make sure to get the remaining data, add to first thread since it starts running first
        
        for(auto i = num_loops; i < size; ++i)
            thread_vecs[vec_num].push_back(v[i]);
     
        return thread_vecs;
    }

private:
    // Uses ThreadPool.hpp
    static ThreadPool *mPool;
    
    // Number of threads in the pool, defaults to std::thread::hardware_concurrency()
    static size_t NUM_THREADS;
    
    // Number of 'tasks' required per thread before it uses a serial version
    // If it's a short task this number could be increased, if it's a long task, vice versa
    // By default this will be set to 15, which is the crossover point for the trimStrVec function
    // in StrUtils operating on an average size code file lines of text.
    static size_t JUMP_TO_SERIAL;
    
};