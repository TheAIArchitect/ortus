//
// Created by Sean Grimes on 6/7/15.
//

#pragma once

#include <string>
#include <sstream>
#include <fstream>
#include <mutex>
#include <iostream>
#include "StrUtils.hpp"
#include "FileUtils.hpp"
#include "GeneralUtils.hpp"
#include "ThreadPool.hpp"

#define FUNC __PRETTY_FUNCTION__
#ifndef NOBUF
    #define LOGBUF 1024
#else
    #define LOGBUF 1
#endif


/**
 * Note: messages logged as "fatal" or "error" are immediately flushed to file and therefore slower
 */
class Logger {
private:
    size_t buf_size_{LOGBUF};
    bool is_disabled_{false};
    bool use_pool_;
    bool include_function_name_{true};
    std::string buf_;
    std::string logFile_;
    std::mutex log_m_;
    std::mutex file_writer_;
    ThreadPool pool_;
    
    void write(const std::string msg);
    void flushBuf();
    
    template<class T>
    void log(const std::string& type, const T& msg, bool timestamp){
        // Build the timestamp before locking
        std::string time_str = "";
        if(timestamp) time_str = GeneralUtils::getTimeStamp();
        
        std::lock_guard<std::mutex> lock(log_m_);
        std::stringstream ss;
        ss << time_str << " " << type << " ";
        if(!(ss << msg)){
            return error("LOGGING ERROR: " + GeneralUtils::type_name<T>(), FUNC);
        }
        else{
            ss << '\n';
            buf_.append(ss.str());
        }
        if(buf_.length() > buf_size_)
            flushBuf();
        
    }
    
public:
    Logger(bool use_single_thread = false);
    Logger(std::string& log_file, bool use_single_thread = false);
    ~Logger();
    
    void setLog(const std::string& file);
    void disable();
    void enable();
    void disable_function_name();
    void enable_function_name();
    void setBufferSize(size_t new_size);
    
    template<class T>
    void info(const T& msg, const char* fname, bool timestamp){
        if(is_disabled_) return;
        std::string name(fname);
        std::string type_and_message;
        if(name != "")
            type_and_message = "INFO     " + name + ": ";
        else
            type_and_message = "INFO    ";
        log(type_and_message, msg, timestamp);
    }
    
    template<class T>
    void info(const T& msg, const char* fname = ""){
        info(msg, fname, true);
    }
    
    template<class T>
    void info(const T& msg, bool timestamp){
        info(msg, "", timestamp);
    }
    
    template<class T>
    void debug(const T& msg, const char* fname, bool timestamp){
        if(is_disabled_) return;
        std::string name(fname);
        std::string type_and_message;
        if(name != "")
            type_and_message = "DEBUG    " + name + ": ";
        else
            type_and_message = "DEBUG   ";
        log(type_and_message, msg, timestamp);
    }
    
    template<class T>
    void debug(const T& msg, const char* fname = ""){
        debug(msg, fname, true);
    }
    
    template<class T>
    void debug(const T& msg, bool timestamp){
        debug(msg, "", timestamp);
    }
    
    template<class T>
    void warn(const T& msg, const char* fname, bool timestamp){
        if(is_disabled_) return;
        std::string name(fname);
        std::string type_and_message;
        if(name != "")
            type_and_message = "WARNING  " + name + ": ";
        else
            type_and_message = "WARNING ";
        log(type_and_message, msg, timestamp);
    }
    
    template<class T>
    void warn(const T& msg, const char* fname = ""){
        warn(msg, fname, true);
    }
    
    template<class T>
    void warn(const T& msg, bool timestamp){
        warn(msg, "", timestamp);
    }
    
    template<class T>
    void error(const T& msg, const char* fname, bool timestamp){
        if(is_disabled_) return;
        std::string name(fname);
        std::string type_and_message;
        if(name != "")
            type_and_message = "ERROR    " + name + ": ";
        else
            type_and_message = "ERROR   ";
        log(type_and_message, msg, timestamp);
        flushBuf();
    }
    template<class T>
    void error(const T& msg, const char* fname = ""){
        error(msg, fname, true);
    }
    
    template<class T>
    void error(const T&msg, bool timestamp){
        error(msg, "", timestamp);
    }
    
    template<class T>
    void fatal(const T& msg, const char* fname, bool timestamp){
        if(is_disabled_) return;
        std::string name(fname);
        std::string type_and_message;
        if(name != "")
            type_and_message = "FATAL    " + name + ": ";
        else
            type_and_message = "FATAL   ";
        log(type_and_message, msg, timestamp);
        flushBuf();
    }
    
    template<class T>
    void fatal(const T& msg, const char* fname = ""){
        fatal(msg, fname, true);
    }
    
    template<class T>
    void fatal(const T& msg, bool timestamp){
        fatal(msg, "", timestamp);
    }
};