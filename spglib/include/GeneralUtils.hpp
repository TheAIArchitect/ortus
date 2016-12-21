//
//  GeneralUtils.hpp
//  cpplibs
//
//  Created by Sean Grimes on 12/12/15.
//  Copyright © 2015 Sean Grimes. All rights reserved.
//

#pragma once

#include <vector>
#include <iostream>
#include <string>
#include <memory>
#include <algorithm>
#include <time.h>

class GeneralUtils{
public:
    
    static std::string getTimeStamp();
    
    template <typename T>
    static void removeEleFromVec(std::vector<T> &vec, T eleToRemove){
        vec.erase(std::remove(std::begin(vec), std::end(vec), eleToRemove), std::end(vec));
    }
    
    template<typename T>
    static size_t numOccuranceInVec(const std::vector<T> &vec, const T &objToCount){
        size_t total_objects = std::count(std::begin(vec), std::end(vec), objToCount);
        return total_objects;
    }
    
    // Look at an example of use in Parallel
    template <class T>
    static std::string type_name(){
        typedef typename std::remove_reference<T>::type TR;
        std::unique_ptr<char, void(*)(void*)> own(nullptr, std::free);
        std::string r = own != nullptr ? own.get() : typeid(TR).name();
        if (std::is_const<TR>::value)
            r += " const";
        if (std::is_volatile<TR>::value)
            r += " volatile";
        if (std::is_lvalue_reference<T>::value)
            r += "&";
        else if (std::is_rvalue_reference<T>::value)
            r += "&&";
        return r;
    }
    
};