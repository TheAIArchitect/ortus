//
//  StrUtils.cpp
//  cpplibs
//
//  Created by Sean Grimes on 11/26/15.
//  Copyright © 2015 Sean Grimes. All rights reserved.
//

#include "include/StrUtils.hpp"


void StrUtils::toUpper(std::string &s){
    transform(std::begin(s), std::end(s), std::begin(s), ::toupper);
}

void StrUtils::toLower(std::string &s){
    transform(std::begin(s), std::end(s), std::begin(s), ::tolower);
}

std::string StrUtils::trim(const std::string &s){
    if(s.find_first_not_of("\t\n\a\b\f\r\v ") == std::string::npos)
        return "";
    
    std::string trimmed = s;
    // Removing tab chars
    trimmed.erase(remove(std::begin(trimmed), std::end(trimmed), '\t'), std::end(trimmed));
    // Removing new line chars
    trimmed.erase(remove(std::begin(trimmed), std::end(trimmed), '\n'), std::end(trimmed));
    // Removing 'audible bell'
    trimmed.erase(remove(std::begin(trimmed), std::end(trimmed), '\a'), std::end(trimmed));
    // Removing backspace
    trimmed.erase(remove(std::begin(trimmed), std::end(trimmed), '\b'), std::end(trimmed));
    // Removing form-feed
    trimmed.erase(remove(std::begin(trimmed), std::end(trimmed), '\f'), std::end(trimmed));
    // Removing carriage return
    trimmed.erase(remove(std::begin(trimmed), std::end(trimmed), '\r'), std::end(trimmed));
    // Removing vertical tab
    trimmed.erase(remove(std::begin(trimmed), std::end(trimmed), '\v'), std::end(trimmed));
    
    auto first_char = trimmed.find_first_not_of(" ");
    auto last_char = trimmed.size() - first_char;
    std::string no_leading_ws = trimmed.substr(first_char, last_char);
    // Resetting first_char to 0;
    first_char = 0;
    last_char = no_leading_ws.find_last_not_of(" ");
    std::string all_trimmed = no_leading_ws.substr(first_char, last_char + 1);
    return all_trimmed;
}

std::vector<std::string> StrUtils::trimStringVector(const std::vector<std::string> &v){
    std::vector<std::string> r_vec(v.size());
    for(int i = 0; i < v.size(); ++i){
        r_vec[i] = trim(v[i]);
    }
    return r_vec;
}

std::vector<std::string> StrUtils::parseOnCharDelim(const std::string &line, const char delim){
    std::vector<std::string> vec;
    std::stringstream ss{line};
    std::string tmp;
    while(std::getline(ss, tmp, delim)){
        vec.push_back(tmp);
    }
    return vec;
}