/**
 * Author:  Sean Grimes
 * Date:    06/08/15
 * Edited:  06/08/15
 *
 * NOTE: This singleton class should be thread safe, not needing the memory 
 *       barriers specified in:
 *       http://www.aristeia.com/Papers/DDJ_Jul_Aug_2004_revised.pdf
 *       due to c++11 standard, object initialization will be handled by one
 *       thread, with other threads waiting until it's complete
 */

#include "include/Logger.hpp"

Logger::Logger(bool use_single_thread)
    : logFile_{"log.txt"}
    , pool_(1)
    {
        use_pool_ = !use_single_thread;
        use_pool_ = false;
        debug("Logger TP disabled while searching for an OSX bug");
    }

Logger::Logger(std::string& log_file, bool use_single_thread)
    : logFile_{log_file}
    , pool_{1}
    {
        use_pool_ = !use_single_thread;
        use_pool_ = false;
        debug("Logger TP disabled while searching for an OSX bug");
    }


Logger::~Logger(){
    flushBuf();
}

void Logger::setLog(const std::string& file){
    if(file.length() < 1)
        throw std::invalid_argument("file name must contain at least 1 character");
    logFile_ = file;
}

void Logger::disable(){
    is_disabled_ = true;
}

void Logger::enable(){
    is_disabled_ = false;
}

void Logger::disable_function_name(){
    include_function_name_ = false;
}

void Logger::enable_function_name(){
    include_function_name_ = true;
}

void Logger::setBufferSize(size_t new_size){
    buf_size_ = new_size;
}

void Logger::write(const std::string msg){
    std::lock_guard<std::mutex> lock(file_writer_);
    std::ofstream out(logFile_, std::ios::out | std::ios::app);
    out << msg << std::flush;
}

void Logger::flushBuf(){
// If pool ignore return code from TSWriteToFile and assume okay, msg will appear in stderr if bad
// If pool, make sure to pass copy of string so buf_ can be reset immediately
    if(use_pool_)
        pool_.push(&FileUtils::TSWriteToFile, logFile_, std::string(buf_));
    else
        FileUtils::TSWriteToFile(logFile_, buf_);
    buf_ = "";
}
