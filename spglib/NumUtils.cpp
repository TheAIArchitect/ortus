//
//  NumUtils.cpp
//  cpplibs
//
//  Created by Sean Grimes on 11/26/15.
//  Copyright © 2015 Sean Grimes. All rights reserved.
//

#include "include/NumUtils.hpp"

int NumUtils::strToInt(const std::string &s){
    char *end;
    long l;
    char *c_str = new char[s.length() + 1];
    strcpy(c_str, s.c_str());
    std::string err = "strToInt Error: ";
    errno = 0;
    l = strtol(c_str, &end, 10);
    if((errno == ERANGE && l == LONG_MAX) || l > INT_MAX)
        throw std::runtime_error(err + "Integer overflow on conversion.");
    if((errno == ERANGE && l == -LONG_MAX) || l < -INT_MAX)
        throw std::runtime_error(err + "Integer underflow on conversion.");
    if(*c_str == '\0' || *end != '\0')
        throw std::runtime_error(err + "Cannot convert " + s + " to an integer.");

    delete[] c_str;
    c_str = nullptr;
   
    return (int) l;
}

double NumUtils::strToDouble(const std::string &s){
    char *end;
    double d;
    char *c_str = new char[s.length() + 1];
    strcpy(c_str, s.c_str());
    std::string err = "strToDouble Error: ";
    errno = 0;
    d = strtod(c_str, &end);
    if((errno == ERANGE && d == DBL_MAX) || d > DBL_MAX)
        throw std::runtime_error(err + "Double overflow on conversion.");
    if((errno == ERANGE && d == -DBL_MAX) || d < -DBL_MAX)
        throw std::runtime_error(err + "Double underflow on conversion.");
    if(*c_str == '\0' || *end != '\0')
        throw std::runtime_error(err + "Cannot convert " + s + " to a double.");

    delete[] c_str;
    c_str = nullptr;
    
    return (double) d;
}

int NumUtils::sumOfDigits(int x){
    if(x < 0)
        return -sumOfDigits(-x);
    if(x < 10)
        return x;
    return (x % 10 + sumOfDigits(x / 10));
}

int NumUtils::fibonacci(int x){
    if(x <= 1)
        return x;
    return (fibonacci(x - 1) + fibonacci(x - 2));
}

int NumUtils::getRandomInt(int start, int end){
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist(start, end);
    return dist(mt);
}

double NumUtils::getRandomDouble(double start, double end){
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(start, end);
    return dist(mt);
}