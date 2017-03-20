#include "OdrParser.hpp"

OdrParser::OdrParser(){

}


std::string OdrParser::removeComment(std::string s){
    std::string newS;
    int commentPos = s.find("//");
    if (commentPos != std::string::npos && commentPos > 0){
        newS = s.substr(0, commentPos);
    }   
    else if ( commentPos == 0){
        return ""; // entire line is a comment
    }
    else {
        newS = s;
    }
    return newS;
}

std::vector<std::string> OdrParser::getOdr(std::string fname){
    std::vector<std::string> odrVec;
    std::ifstream odrFile(fname);
    std::string line;
    int lineNo = 0;
    std::string fixedLine;
    while (std::getline(odrFile,line)){
        fixedLine = removeComment(line);
        if (!fixedLine.empty()){
            odrVec.push_back(fixedLine); 
            printf("%s\n",odrVec[lineNo].c_str());
            lineNo++;
        }
    }
    return odrVec;
}

void OdrParser::getConnectome(std::vector<std::vector<float> >& CS){
    getOdr("simple.ort");
}
