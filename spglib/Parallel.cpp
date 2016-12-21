//
//  Parallel.cpp
//  cpplib
//
//  Created by Sean Grimes on 5/13/16.
//  Copyright © 2016 Sean Grimes. All rights reserved.
//

#define serial_jump 15

#include "include/Parallel.hpp"

//ThreadPool* Parallel::mPool = nullptr;
size_t Parallel::NUM_THREADS = std::thread::hardware_concurrency();
size_t Parallel::JUMP_TO_SERIAL = serial_jump;
ThreadPool* Parallel::mPool = new ThreadPool(NUM_THREADS);
                                             
                                             
void Parallel::setNumberOfThreadsInPool(size_t num_threads){
    NUM_THREADS = num_threads;
    
    if(mPool != nullptr)
        delete mPool;
    
    mPool = new ThreadPool(NUM_THREADS);
}

void Parallel::setSerialCrossoverPoint(size_t num_tasks_per_thread){
    JUMP_TO_SERIAL = num_tasks_per_thread;
}

void Parallel::startPool(size_t num_threads, size_t num_tasks_per_thread){
    if(mPool != nullptr)
        return;
    
    NUM_THREADS = num_threads;
    JUMP_TO_SERIAL = num_tasks_per_thread;
    
    mPool = new ThreadPool(NUM_THREADS);
}

void Parallel::startPool(){
    if(mPool != nullptr)
        return;
    
    mPool = new ThreadPool(NUM_THREADS);
}

void Parallel::destroyPool(){
    if(mPool != nullptr)
        delete mPool;
    mPool = nullptr;
}

std::vector<std::string> Parallel::trimStrVec(const std::vector<std::string> &v){
    // Determine if we should run in parallel
    if(!checkSize(v))
        return StrUtils::trimStringVector(v);
    
    startPool();
    
    // Split up the data, returned is a vector of NUM_THREADS vectors of data
    auto thread_vecs = splitData(v);
    
    // Push the data and function onto the thread pool
    std::vector<std::future<std::vector<std::string>>> futures;
    for(auto i = 0; i < NUM_THREADS; ++i)
        futures.push_back(mPool->push(&StrUtils::trimStringVector, thread_vecs[i]));
    
    // Get the results
    std::vector<std::vector<std::string>> results;
    for(auto i = 0; i < NUM_THREADS; ++i)
        results.push_back(futures[i].get());
    
    // Put the data back together
    auto res_size = results.size();
    std::vector<std::string> trimmed;
    trimmed.reserve(res_size * results[res_size-1].size());
    for(auto &&vec : results){
        for(auto &&str : vec){
            trimmed.push_back(str);
        }
    }
    trimmed.shrink_to_fit();
    
    return trimmed;
}












