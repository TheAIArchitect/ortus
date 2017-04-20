//
//  OptionForewoman.cpp
//  delegans
//
//  Created by onyx on 10/4/16.
//  Copyright © 2016 Sean Grimes. All rights reserved.
//

#include "OptionForewoman.hpp"

// option names
const std::string OptionForewoman::WormOptsStrings[NUM_OPTS] = {"PATH_LENGTH_TO_SHOW", "PATH_WEIGHT_TO_SHOW", "SHOW_ALL_NEURONS", "SHOW_MUSCLES", "SHOW_ACTIVATION", "SHOW_WEIGHTS"};
// initial values
int OptionForewoman::WormOpts[NUM_OPTS] = { 3, 10, 0, 0, 0, 0}; // indices 2 - 4 are T/F -> 1/0

const int OptionForewoman::MIN_PATH_LENGTH = 0;
const int OptionForewoman::MAX_PATH_LENGTH = 100; // just picked a big number
const int OptionForewoman::MIN_PATH_WEIGHT = 0;
const int OptionForewoman::MAX_PATH_WEIGHT = 100; // just picked a big number


// these are used to allow the user to input an option, followed by a string of characters,
// which then get sent to the keyboardOptionGauntlet. If a 1/2 (for example) is hit,
// a loop for all remaining input is entered until ENTER is hit.
// SEE BOTTOM OF CHIEF FOR DETAILS
bool OptionForewoman::WAITING_ON_OPTION_REQUEST = false; // turns true to enter the loop
int OptionForewoman::OPTION_REQUESTED = -1; // remembers the option key that caused the loop entry.
std::string OptionForewoman::CHOICE = ""; // this collects the input characters while in the waiting state

// if path_length_to_show or path_weight_to_show are changed,
// this must be set to true to tell GraphicsGovernor to
bool OptionForewoman::REFRESH_CONNS = false;

std::string OptionForewoman::keyboardOptionGauntlet(int option, std::string sChoice){
    // first, clear the old input variables
    bool success = false;
    // for now, all valid choices can be converted to an int...
    int choice = atoi(sChoice.c_str()); 
    // predict success and create happy string...
    std::string toReturn = "Option '"+std::to_string(option)+"' ("+WormOptsStrings[option]+") set to '"+std::to_string(choice)+"'";
    switch(option){
        case PATH_LENGTH_TO_SHOW://PATH_LENGTH_TO_SHOW:
            if (choice >= MIN_PATH_LENGTH && choice <= MAX_PATH_LENGTH){
                WormOpts[PATH_LENGTH_TO_SHOW] = choice;
                REFRESH_CONNS = true;
                success = true;
            }
            break;
        case PATH_WEIGHT_TO_SHOW://MIN_PATH_WEIGHT:
            if (choice >= MIN_PATH_WEIGHT && choice <= MAX_PATH_WEIGHT){
                WormOpts[PATH_WEIGHT_TO_SHOW] = choice;
                REFRESH_CONNS = true;
                success = true;
            }
            break;
        case SHOW_ALL_NEURONS://SHOW_ALL_NEURONS:
            if (choice == 1 || choice == 0){
                WormOpts[SHOW_ALL_NEURONS] = choice;
                success = true;
            }
            break;
        case SHOW_MUSCLES://SHOW_MUSCLES:
            if (choice == 1 || choice == 0){
                WormOpts[SHOW_MUSCLES] = choice;
                REFRESH_CONNS = true;
                success = true;
            }
            break;
        case SHOW_ACTIVATION:// SHOW_ACTIVATION
            if (choice == 1 || choice == 0){
                WormOpts[SHOW_ACTIVATION] = choice;
                success = true;
            }
            break;
        case SHOW_WEIGHTS: // SHOW_WEIGHTS
            if (choice == 1 || choice == 0){
                WormOpts[SHOW_WEIGHTS] = choice;
                success = true;
            }
            break;
        default:
            // and if no success, put sad string in its place :(
            // (actually, this only covers the option not being found... below we cover the situation where the choice isn't valid)
            toReturn += "Option '"+std::to_string(option)+" not recognized. Choice '"+std::to_string(choice)+"' not applied";
            return toReturn;
    }
    if (!success){ // then we had a valid option, but not a valid choice.
        toReturn = "Option '"+std::to_string(option)+"' ("+WormOptsStrings[option]+") *cannot* be set to '"+std::to_string(choice)+"'!";
    }
    return toReturn;
}
