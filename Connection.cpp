//
//  Connection.cpp
//  delegans
//
//  Created by onyx on 10/13/16.
//  Copyright © 2016 Andrew W.E. McDonald. All rights reserved.
//

#include "Connection.hpp"

float Connection::ZEROF = 0.f;

Connection::Connection(){
    polarityp = &ZEROF; // if it's a gap junction, nothing is assigned on creation.
};

float Connection::weight(){
    return *weightp;
}

float Connection::polarity(){
    return *polarityp;
}

std::string Connection::toString(){
    int max = 512;
    char buffer[max];
    snprintf(buffer, max,"(%s->%s : %.2f : %.2f)",preName.c_str(),postName.c_str(),weight(),polarity());
    return std::string(buffer);
}
