//
//  OptionForewoman.hpp
//  delegans
//
//  Created by onyx on 10/4/16.
//  Copyright © 2016 Sean Grimes. All rights reserved.
//

#ifndef OptionForewoman_hpp
#define OptionForewoman_hpp

#include <stdio.h>
#include <cstdlib>
#include <string>

// IF YOU CHANGE THESE, you MUST also change the options in keyboardOptionsGauntlet,
// AND "NUM_OPTS"
// .. to add an option, just add to all of these.
enum WormOptsEnum {PATH_LENGTH_TO_SHOW = 0, PATH_WEIGHT_TO_SHOW, SHOW_ALL_NEURONS, SHOW_MUSCLES, SHOW_ACTIVATION, SHOW_WEIGHTS}; // also, note 0 start.


class OptionForewoman {
    
public:
    
    const static int NUM_OPTS = 6;
    
    const static std::string WormOptsStrings[NUM_OPTS];
    static int WormOpts[NUM_OPTS];
    static bool WAITING_ON_OPTION_REQUEST;
    static int OPTION_REQUESTED;
    static std::string CHOICE;
    
    static std::string keyboardOptionGauntlet(int option, std::string sChoice);
    
    const static int MIN_PATH_LENGTH;
    const static int MAX_PATH_LENGTH; // just picked a big number
    const static int MIN_PATH_WEIGHT;
    const static int MAX_PATH_WEIGHT; // just picked a big number
    
    static bool REFRESH_CONNS;
    
private:
    OptionForewoman(){}; // no instantiations.
    
};


#endif /* OptionForewoman_hpp */
