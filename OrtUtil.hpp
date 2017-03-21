#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "StrUtils.hpp"
#include <unordered_map>
#include "ElementInfoModule.hpp"

/**
 * Eventually, this should save the comments from the file it reads in.
 * It should also be able to write a new .ort file from the connectome
 */

class OrtUtil {
    public:
        OrtUtil();
        std::vector<std::string> getOdrLines(std::string fname);
        void setElements();
        std::string removeLineComment(std::string s);
        std::unordered_map<std::string,ElementInfoModule> elementMap;
    
private:
        std::string removeCommented(std::string line, bool& startBlockComment);
        std::string getPreceedingOrProceeding(bool proceeding, unsigned long pos, std::string line);
};
