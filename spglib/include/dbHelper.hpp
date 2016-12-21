//
//  dbHelper.hpp
//  pelegans
//
//  Created by Sean Grimes on 8/14/15.
//  Copyright © 2015. All rights reserved.
//

#pragma once

#include <stdio.h>
#include <string>
#include <sqlite3.h>
#include "Logger.hpp"
#include <vector>
#include <string.h>
#include <iostream>

class dbHelper{
public:
    // Constructor will automatically try connecting to the supplied db
    dbHelper(std::string file_path);
    dbHelper(std::string file_path, std::string table_name);
    ~dbHelper();
    
    // Creates a table in the database specified in the constructor
    // Does not check for previous existence of table
    void create_table(std::string sql_statement);
    
    // Drops the specified table it is exists
    void drop_table(std::string sql_statement);
    
    // ....
    void insert(std::string sql_statement);
    
    // returns a vector<string> of elements returned from select statement
    // Note: caller needs do the string to int/double etc...conversion
    // You need to provide the number of results you're looking to get from the
    // query
    // i.e... select id from.... looking for 1 result, num_elements = 1
    // i.e... select * from.... need to know how many elements '*' will return
    //vector<string> select(string sql_statement, int num_elements);
    std::vector<std::vector<std::string> > select(std::string sql_statement);
    
private:
    sqlite3* db_;
    char* err_msg_;
    int rc_;
    std::string path_to_db_;
    std::string table_name_;
    Logger log_;
    
private:
    int execute_generic_statement(std::string sql_statement);
    void connect();
    void close();
};