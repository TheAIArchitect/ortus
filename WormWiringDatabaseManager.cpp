//
//  WormWiringDatabaseManager.cpp
//  delegans
//
//  @author Andrew W.E. McDonald
//  @author Sean Grimes
//  Copyright © 2015 delegans group. All rights reserved.
//

#include "WormWiringDatabaseManager.hpp"

WormWiringDatabaseManager::WormWiringDatabaseManager():_db{"db/wormwiring.db"}{}

std::vector<std::vector<std::string>> WormWiringDatabaseManager::select_records(std::string pre_name){
    std::string query = "select * from neurons";
    return _db.select(query);
}

std::vector<std::vector<std::string>> WormWiringDatabaseManager::select_neuron_names(){
    std::string query = "select distinct name from neurons";
    fprintf(stderr, "\n\n\n\nselect_neuron_names() not implemented\n\n\n\n");
    return std::vector<std::vector<std::string>>();
}
