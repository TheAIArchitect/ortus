#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>


class OdrParser {
    public:
        OdrParser();
        void getConnectome(std::vector<std::vector<float> >& CS);        
        std::vector<std::string> getOdr(std::string fname);
        std::string removeComment(std::string s);
};
