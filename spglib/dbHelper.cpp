//
//  dbHelper.cpp
//
//  @author Sean Grimes
//  Copyright © 2015, Sean Grimes. All rights reserved.
//

#include "include/dbHelper.hpp"

dbHelper::dbHelper(std::string file_path)
    : path_to_db_{file_path}
    , rc_{}
    , table_name_{""}
    {}

dbHelper::dbHelper(std::string file_path, std::string table_name)
    : path_to_db_{file_path}
    , table_name_{table_name}
    , rc_{}
    {}


dbHelper::~dbHelper(){}

void dbHelper::connect(){
    rc_ = sqlite3_open(path_to_db_.c_str(), &db_);
    if(rc_){

#ifdef LOG_DEBUG
        log_.fatal("Can't open database: " + std::string(sqlite3_errmsg(db_)), FUNC);
#endif
        throw std::runtime_error("Can't open database: " + std::string(sqlite3_errmsg(db_)));
    }
}

void dbHelper::close(){
    sqlite3_close(db_);
}

int dbHelper::execute_generic_statement(std::string sql_statement){
    connect();
    
    if(sql_statement[sql_statement.length() - 1] != ';'){
        // Append semi-colon if necessary
        sql_statement += ";";
    }

    // Callback can be null for some operations
    rc_ = sqlite3_exec(db_, sql_statement.c_str(), NULL, 0, &err_msg_);
    if(rc_ != SQLITE_OK){
        close();

#ifdef LOG_DEBUG
        log_.fatal("SQL error: " + std::string(err_msg_), FUNC);
#endif
        throw std::runtime_error("SQL error: " + std::string(err_msg_));
    }
    else{
        close();
        return 0;
    }
}

void dbHelper::create_table(std::string sql_statement){
    int rc = execute_generic_statement(sql_statement);
    
#ifdef LOG_DEBUG
    if(rc == 0) log_.info("dbHelper::create_table: " + sql_statement, FUNC);
#endif
}

void dbHelper::drop_table(std::string sql_statement){
    int rc = execute_generic_statement(sql_statement);

#ifdef LOG_DEBUG
    if(rc == 0) log_.info("dbHelper::drop_table: " + sql_statement, FUNC);
#endif
}

void dbHelper::insert(std::string sql_statement){
    int rc = execute_generic_statement(sql_statement);

#ifdef LOG_DEBUG
    if(rc == 0) log_.info("dbHelper::insert: " + sql_statement, FUNC);
#endif
}

// Note: Not using sqlite3_exec and callbacks, running through steps manually
//vector<string> dbHelper::select(string sql_statement, int num_elements){
std::vector<std::vector<std::string>> dbHelper::select(std::string sql_statement){
    connect();
    
    std::vector<std::vector<std::string>> rows;
    sqlite3_stmt *stmt;
    auto sql_statement_len = sql_statement.length();
    
    // Prepare the sql statement
    rc_ = sqlite3_prepare_v2(db_, sql_statement.c_str(), (unsigned int)sql_statement_len, &stmt, NULL);
    if(rc_ != SQLITE_OK){
        
#ifdef LOG_DEBUG
        log_.error("dbHelper::select: Failed to prepare database", FUNC);
#endif
        
        fprintf(stderr, "Failed to prepare database %s\n", sqlite3_errstr(rc_));
        std::string failed = "Failed to prepare database: ";
        failed += sqlite3_errstr(rc_);
    }
   
    // NOTE: This returns the number of columns in the result set returned by the
    //       prepared statement
    auto num_columns = sqlite3_column_count(stmt);
    
    // sqlite3 --> we're in 'c' land, so no c++ strings from DB
    char *result_p = new char[1024];
    if(result_p == nullptr){
        
#ifdef LOG_DEBUG
        log_.fatal("dbHelper::select: Couldn't allocate result_p", FUNC);
#endif
        
        throw std::runtime_error("dbHelper::select: Couldn't allocate result_p");
    }
    do{
        rc_ = sqlite3_step(stmt);
        if(rc_ == SQLITE_ROW){
            std::vector<std::string> row;
            for(int i = 0; i < num_columns; ++i){
                // Copy data from 1 column of 1 row from DB return
                strcpy(result_p, (char *)sqlite3_column_text(stmt, i));
                // Use new "emplace_back" to construct string in-place
                row.emplace_back(result_p);
            }
            // Place the columns from a single row into our rows vector
            rows.push_back(row);
        }
    }while(rc_ == SQLITE_ROW);
    
    delete[] result_p;
    result_p = nullptr;
    close();
    return rows;
}