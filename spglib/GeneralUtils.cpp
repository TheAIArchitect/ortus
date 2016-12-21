//
//  GeneralUtils.cpp
//  cpplibs
//
//  Created by Sean Grimes on 12/12/15.
//  Copyright © 2015 Sean Grimes. All rights reserved.
//

#include "include/GeneralUtils.hpp"

std::string GeneralUtils::getTimeStamp(){
    // http://stackoverflow.com/questions/997946/how-to-get-current-time-and-date-in-c
    // TrungTN, thanks!
    time_t now = time(0);
    struct tm tstruct;
    char buf[100];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
    std::string time_str(buf);
    return time_str;
}
