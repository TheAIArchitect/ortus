//
//  WormWiringDatabaseManager.hpp
//  pelegans
//
//  @author Andrew W.E. McDonald
//  @author Sean Grimes
//  Copyright © 2015 delegans group. All rights reserved.
//

#pragma once

#include <stdio.h>
#include <string>
#include <vector>
#include "dbHelper.hpp"

class WormWiringDatabaseManager{
public:
    WormWiringDatabaseManager();
    std::vector<std::vector<std::string>> select_records(std::string pre_name = "not_currently_used");
    std::vector<std::vector<std::string>> select_neuron_names();
    
private:
    dbHelper _db;
};
