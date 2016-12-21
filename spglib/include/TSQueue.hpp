//
//  TSQueue.hpp
//  pelegans
//
//  Created by Sean Grimes on 2/2/16.
//  Copyright © 2016. All rights reserved.
//

#pragma once

#include <queue>
#include <vector>
#include <mutex>
#include <condition_variable>

template <typename T>
class TSQueue{
public:
    void push(const T &element){
        std::unique_lock<std::mutex> mlock(mMutex);
        mQueue.push(element);
        mlock.unlock();
        mCondVar.notify_one();
    }
    
    T pop(){
        // Lock released when unique_lock goes out of scope
        std::unique_lock<std::mutex> mlock(mMutex);
        
        // Prevent spurious wakes from ruining everything
        // then release lock and wait
        while(mQueue.empty())
            mCondVar.wait(mlock);
        
        // Same queue operations as normal
        auto element = mQueue.front();
        mQueue.pop();
        return element;
    }
    
    std::queue<T> get_copy_of_queue(){
        std::unique_lock<std::mutex> mlock(mMutex);
        return mQueue;
    }
    
    bool empty(){
        std::unique_lock<std::mutex> mlock(mMutex);
        return mQueue.empty();
    }
    
private:
    std::queue<T> mQueue{};
    std::mutex mMutex;
    std::condition_variable mCondVar;
};