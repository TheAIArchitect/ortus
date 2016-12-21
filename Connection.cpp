//
//  Connection.cpp
//  delegans
//
//  Created by onyx on 10/13/16.
//  Copyright © 2016 Andrew W.E. McDonald. All rights reserved.
//

#include "Connection.hpp"

Connection::Connection(){
    polarity = 0;// initialize poliarity to 0. if it's not a gap junction, we'll change it to -1 or 1.
};


std::string Connection::toString(){
    int max = 512;
    char buffer[max];
    snprintf(buffer, max,"(%s->%s : %.2f : %d)",preName.c_str(),postName.c_str(),weight,polarity);
    return std::string(buffer);
}
