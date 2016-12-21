//
//  FileUtils.hpp
//  cpplibs
//
//  Created by Sean Grimes on 12/7/15.
//  Copyright © 2015 Sean Grimes. All rights reserved.
//

#pragma once

#include <string>
#include <fstream>
#include <vector>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/file.h>
#include <fcntl.h>
#include <dirent.h>
#include <cstring>
#include <unistd.h>
#include <mutex>
#include <fstream>
#include "Logger.hpp"

class FileUtils{
public:
    static std::string readFullFile(const std::string &fileName);
    static std::vector<std::string> readLineByLine(const std::string &fileName);
    static size_t lineCount(const char *fname);
    static std::vector<std::string> getFilesInDir(const std::string &dirPath, std::string ext = "");
    static std::vector<std::string> getDirsInDir(const std::string &path);
    
    /* 
     * The next 3 functions *ONLY* tell you is something you know exists is a file, dir, has exec 
     * permissions. They should not be used to test if something exists
     */
    static bool isFile(const std::string &path);
    static bool isDir(const std::string &path);
    static bool isExc(const std::string& path);
    
    static bool ends_with_string(const std::string &str, const std::string &what);
    
    // Returns true if dir created successfully, false otherwise
    static bool mkdirWrapper(const std::string &path);
    
    // This can safely be used to test if a file exists
    static bool fexists(const std::string &path);

    // NOTE: This function is really designed to be used by a logger, it locks within a processes
    // using a mutex, once the mutex is obtained it locks on a system-wide file lock so multiple
    // processes can safely log to the same file. Obviously the above assumes that said logger will
    // be using this function to write to the file (which Logger.hpp does).
    static bool TSWriteToFile(const std::string &fname, const std::string& output);
    static int tryFileLock(const std::string optional_lock_file = "");
    static void releaseFileLock(int fd, const std::string optional_lock_file = "");
    
    static std::vector<std::vector<std::string>> csvToMatrix(const std::string& file);

private:
    static std::mutex file_writer_lock_;
    static std::string lockFile_;
};
