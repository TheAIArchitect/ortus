//
//  NumUtils.hpp
//  cpplibs
//
//  Created by Sean Grimes on 11/26/15.
//  Copyright © 2015 Sean Grimes. All rights reserved.
//

#pragma once

#include <vector>
#include <algorithm>
#include <sstream>
#include <string>
#include <cstdlib>
#include <string.h>
#include <float.h>
#include <random>
#include <limits.h>
#include <cstdint>

class NumUtils{
public:
     /**
     * Gets median
     *
     * Author: Andrew W.E. McDonald
     * Email: awm32@cs.drexel.edu
     */
    template<typename T>
    static T getMedian(std::vector<T> &vec){
        size_t v_size = vec.size();
        // will automatically round down due to int division, subtract 1 due to 0 indexing
        size_t mid = (v_size/2) - 1;
        size_t midpo = mid + 1;
        if (v_size % 2 == 1){
            std::nth_element(vec.begin(), vec.begin()+(midpo), vec.end(),
                             [](T const a, T const b){
                                 return b > a;
                             });
            return vec[midpo];
        }
        else{
            std::nth_element(vec.begin(), vec.begin()+(mid), vec.end(),
                             [](T const a, T const b){
                                 return b > a;
                             });
            T m1 = vec[mid];
            std::nth_element(vec.begin(), vec.begin()+(midpo), vec.end(),
                             [](T const a, T const b){
                                 return b > a;
                             });
            T m2 = vec[midpo];
            return ((m1+m2)/2.f);
            
        }
    }
    
    /**
     * Returns the larger value, accepts ints, doubles, floats, chars
     * Returns a if the values are equal
     *
     * @param: a - The first value
     * @param: b - The second value
     * @return: T - The larger value
     */
    template<typename T>
    static T maxValue(const T &a, const T &b) __attribute__((deprecated("use std::max"))){
        if(b > a)
            return b;
        return a;
    }
    
    /**
     * Returns the smaller value, accepts ints, doubles, floats, chars
     * Returns a if the values are equal
     *
     * @param: a - The first value
     * @param: b - The second value
     * @return: T - The larger value
     */
    template<typename T>
    static T minValue(const T &a, const T &b) __attribute__((deprecated("use std::min"))){
        if(b < a)
            return b;
        return a;
    }
    
    /**
     * Returns the largest value in a vector of ints, doubles, floats, or chars
     *      Vector sequentially searched, requires n-1 comparisons
     *
     * @param: vec - the vector to be looked at
     * @return: T - the largest value
     * @return: -1 - If vector has 0 elements
     */
    template<typename T>
    static T maxValueVec(const std::vector<T> &vec){
        auto max = std::max_element(std::begin(vec), std::end(vec));
        return vec[std::distance(std::begin(vec), max)];
    }
    
    /**
     * Returns the smallest value in a vector of ints, doubles, floats, or chars
     *      Vector sequentially searched, requires n-1 comparisons
     *
     * @param: vec - the vector to be looked at
     * @return: T - the smallest value
     * @return: -1 - iIf vector has 0 elements
     */
    template<typename T>
    static T minValueVec(const std::vector<T> &vec){
        auto min = std::min_element(std::begin(vec), std::end(vec));
        return vec[std::distance(std::begin(vec), min)];
    }
    
    /**
     * Divide and conquere approach to finding the min and max element in a
     *      vector
     *
     * @param: vec - an int, double, or float std::vector
     * @return: A vector of type 'vec', holding 2 entries:
     *      M[0] - holds the min value of vec
     *      M[1] - holds the max value of vec
     */
    template<typename T>
    static std::vector<T> minMaxInVec(const std::vector<T> &vec, size_t begin, size_t end){
        std::vector<T> M(2);
        if(end == begin){
            M[0] = vec[begin]; M[1] = vec[end];
            return M;
        }
        if(end - begin == 1){
            if(vec[begin] <= vec[end]){
                M[0] = vec[begin]; M[1] = vec[end];
            }
            else{
                M[0] = vec[end]; M[1] = vec[begin];
            }
            return M;
        }
        auto m = (end + begin) / 2;
        std::vector<T> M1(2), M2(2);
        M1 = minMaxInVec(vec, begin, m);
        M2 = minMaxInVec(vec, m+1, end);
        M[0] = minValue(M1[0], M2[0]);
        M[1] = maxValue(M1[1], M2[1]);
        return M;
    }
   
    /**
     * Returns an int from a string, using stringstream
     *
     * @param: s - The string to be converted to an int
     * @return: int - The converted string
     */
    static int strToInt(const std::string &s);
   
    /**
     * Returns a double from a string, using stringstream
     *
     * WARNING: May cause a rounding error, it shouldn't, but it might.
     *      Check with cout << setprecision(WHATEVER_YOU_NEED) << var;
     *
     * @param: s - The string to be converted to a double
     * @return: double - The converted string
     */
    static double strToDouble(const std::string &s);
    
    /**
     * Recursively sum the digits of a number
     *
     * @param: x - int
     * @return: T - returns an int
     */
    static int sumOfDigits(int x);
    
    /**
     * Recursively get fibonacci numbers based on inputted int
     *
     * @param: x - int
     * @return: int - returns the fib #
     */
    static int fibonacci(int x);
    
    /**
     * Gets a random integer between the range given
     * Modeled from: http://stackoverflow.com/questions/19665818/generate-random-numbers-using-c11-random-library
     * Answer #1: Bill Lynch
     */
    static int getRandomInt(int start, int end);
    
    /**
     * Gets a random double between the given range
     */
    static double getRandomDouble(double start, double end);
};