//
//  FileUtils.cpp
//  cpplibs
//
//  Created by Sean Grimes on 12/7/15.
//  Copyright © 2015 Sean Grimes. All rights reserved.
//

#include "include/FileUtils.hpp"

std::mutex FileUtils::file_writer_lock_;
std::string FileUtils::lockFile_ = "FileUtils.lock";

std::string FileUtils::readFullFile(const std::string &fileName){
    std::ifstream ifs(fileName, std::ios::in | std::ios::binary | std::ios::ate);
    if(!ifs.is_open()){
#ifdef LOG_DEBUG
        Logger log(true);
        log.fatal("Couldn't open " + fileName, FUNC);
#endif
        throw std::runtime_error("Couldn't open " + fileName);
    }
    std::ifstream::pos_type f_size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    
    std::vector<char> bytes(f_size);
    ifs.read(&bytes[0], f_size);

    return std::string(&bytes[0], f_size);
}

std::vector<std::string> FileUtils::readLineByLine(const std::string &fileName){
    std::string line;
    std::ifstream in(fileName);
    if(!in.is_open()){
#ifdef LOG_DEBUG
        Logger log(true);
        log.fatal("Couldn't open " + fileName, FUNC);
#endif
        throw std::runtime_error("Couldn't open " + fileName);
    }
    std::vector<std::string> vec;
    vec.reserve(lineCount(fileName.c_str()));
    while(std::getline(in, line)){
        vec.push_back(line);
    }
    vec.shrink_to_fit();
    return vec;
}

// NOTE: This is more or less taken directly from the "wc" command line utility
size_t FileUtils::lineCount(const char *fname){
    static const auto BUF_SIZE = 16*1024;
    int fd = open(fname, O_RDONLY);
    if(fd == -1){
#ifdef LOG_DEBUG
        Logger log(true);
        log.fatal("Couldn't open " + std::string(fname), FUNC);
#endif
        throw std::runtime_error("Couldn't open " + std::string(fname));
    }
    
    char buf[BUF_SIZE + 1];
    size_t lines = 0;
    
    while(size_t bytes_read = read(fd, buf, BUF_SIZE)){
        if(bytes_read == (size_t) -1){
#ifdef LOG_DEBUG
            Logger log(true);
            log.fatal("Read Failure " + std::string(fname), FUNC);
#endif
            throw std::runtime_error("Read Failure" + std::string(fname));
        }
        if(!bytes_read)
            break;
        
        for(char *p = buf; (p = (char *) memchr(p, '\n', (buf + bytes_read) - p)); ++p){
            ++lines;
        }
    }
    close(fd);
    return lines;
}

std::vector<std::string> FileUtils::getFilesInDir(const std::string &dirPath, std::string ext){
    std::vector<std::string> files;
    
    DIR *dir;
    struct dirent *ent;
    
    if ((dir = opendir(dirPath.c_str())) != NULL) {
        // print all the files and directories within directory
        while ((ent = readdir(dir)) != NULL) {
            if(isFile(std::string(dirPath + "/" + ent->d_name))){
                // Only add files with the requested file extension
                if(ext != ""){
                    // Note: This can easily be fooled
                    std::string f_name(ent->d_name);
                    if(ends_with_string(f_name, ext)){
                        files.push_back(f_name);
                    }
                }
                // Don't care what the file extension is
                else{
                    files.emplace_back(ent->d_name);
                }
            }
        }
        closedir(dir);
    }
    else {
        // could not open directory
#ifdef LOG_DEBUG
        Logger log(true);
        log.fatal("Couldn't open dir " + dirPath, FUNC);
#endif
        throw std::runtime_error("Could not open dir " + dirPath);
    }
    
    return files;
}

std::vector<std::string> FileUtils::getDirsInDir(const std::string &path){
    std::vector<std::string> dirs;
    DIR *dir;
    struct dirent *ent;
    if((dir = opendir(path.c_str())) != NULL){
        while((ent = readdir(dir)) != NULL){
            if(isDir(std::string(path + "/" + ent->d_name))){
                dirs.emplace_back(ent->d_name);
            }
        }
    }
    return dirs;
}

bool FileUtils::isFile(const std::string &path){
    struct stat buffer;
    stat(path.c_str(), &buffer);
    return S_ISREG(buffer.st_mode);
}

bool FileUtils::fexists(const std::string &path){
	struct stat buffer;
	return (stat(path.c_str(), &buffer) == 0);
}

bool FileUtils::isDir(const std::string &path){
    struct stat buffer;
    stat(path.c_str(), &buffer);
    return S_ISDIR(buffer.st_mode);
}

bool FileUtils::isExc(const std::string& path){
    struct stat buffer;
    if(stat(path.c_str(), &buffer) == 0 && buffer.st_mode & S_IXUSR)
        return true;
    return false;
}

bool FileUtils::ends_with_string(const std::string &str, const std::string &what) {
    return what.size() <= str.size() && str.find(what, str.size() - what.size()) != str.npos;
}

bool FileUtils::mkdirWrapper(const std::string &path){
    if(isDir(path))
        return true;
    mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
    return isDir(path);
}

// NOTE: unlike most of the code this won't throw on open failure
// NOTE: This is going to be crazy slow if used for anything but general logging
bool FileUtils::TSWriteToFile(const std::string& fname, const std::string& output){
    // Lock with a process wide mutex for MT processes
    //std::lock_guard<std::mutex> lock(FileUtils::file_writer_lock_);
    
    /**
     * NOTE: currently dealing with a bug (limited to osx) where the use of a static private mutex
     * is throwing a std::system_error mutex lock failed invalid argument error
     * The use of a local static mutex with manual locking and unlocking bypasses the error on OS X
     */
    //**//static std::mutex local_mx;
    //**//local_mx.lock();
    
    // Lock with a system-wide file lock for multiple processes logging to the same file
    //**//int fd = -1;
    //**//while(fd == -1)
        //**//fd = tryFileLock();
    
    std::ofstream out(fname, std::ios::out | std::ios::app);
    if(!out.is_open()){
        fprintf(stderr, "TSWriteToFile: Failed to open %s\n", fname.c_str());
        return false;
    }
    out << output;
    
    // Release the system-wide file lock, mutex will be released automatically at end-of-scope
    //**//releaseFileLock(fd);
    //**//local_mx.unlock();
    
    return true;
}

std::vector<std::vector<std::string>> FileUtils::csvToMatrix(const std::string& file){
    auto file_lines = readLineByLine(file);
    std::vector<std::vector<std::string>> matrix;
    for(auto &&line : file_lines){
        matrix.emplace_back(StrUtils::parseOnCharDelim(line, ','));
    }
    return matrix;
}
    
int FileUtils::tryFileLock(const std::string optional_lock_file){
    std::string lock_file;
    if(optional_lock_file == "")
        lock_file = lockFile_;
    else
        lock_file = optional_lock_file;
        
    mode_t mode = umask(0);
    auto fd = open(lock_file.c_str(), O_RDWR | O_CREAT, 0666);
    umask(mode);
    if(fd >= 0 && flock(fd, LOCK_EX | LOCK_NB) < 0){
        close(fd);
        fd = -1;
    }
    return fd;
}

void FileUtils::releaseFileLock(int fd, const std::string optional_lock_file){
    if(fd < 0)
        return;
    
    std::string lock_file;
    if(optional_lock_file == "")
        lock_file = lockFile_;
    else
        lock_file = optional_lock_file;
    
    remove(lock_file.c_str());
    close(fd);
}
