#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "StrUtils.hpp"

/**
 * Eventually, this should save the comments from the file it reads in.
 * It should also be able to write a new .ort file from the connectome
 */

class OrtUtil {
    public:
        OrtUtil();
        std::vector<std::string> getOdr(std::string fname);
        std::string removeLineComment(std::string s);
    
private:
        std::string removeCommented(std::string line, bool& startBlockComment);
        std::string getPreceedingOrProceeding(bool proceeding, unsigned long pos, std::string line);
};
