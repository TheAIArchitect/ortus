#include "OrtUtil.hpp"

OrtUtil::OrtUtil(){

}


std::string OrtUtil::removeLineComment(std::string s){
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


/**
 * returns the portion of line before pos if proceeding == false,
 * and returns the portion of line after pos if proceeding == true.
 */
std::string OrtUtil::getPreceedingOrProceeding(bool proceeding, unsigned long pos, std::string line){
    std::string toReturn = "";
    if (proceeding){
        unsigned long theRestPos = pos+2;
        if (theRestPos < line.length()){ // there's stuff we want in the line
            toReturn = line.substr(theRestPos);
        }
    }
    else {
        if (pos > 0){
            toReturn = line.substr(0,pos);
        }
    }
    return toReturn;
}

/**
 * Will remove any portion of the input that is commented due to block comments,
 * assuming 'startBlockComment' is accurate.
 *
 * if the line is part of a block comment, and doesn't end it,
 * or ends it but has nothing to return after ending the comment,
 * or starts a block comment and doesn't finish it, (without having anything not commented before starting the block comment),
 * an empty string will be returned.
 *
 * NOTE: NO nested block comments, nor any situation with a second opening
 * occurrs before the first opening is closed!
 */
std::string OrtUtil::removeCommented(std::string line, bool& startBlockComment){
    unsigned long pos = std::string::npos;
    std::string noComment = "";
    bool proceeding = true;
    if (startBlockComment){ // we're looking for the end
        pos = line.find("*/");
        if (pos != std::string::npos){ // comment block is finished
            startBlockComment = false;
            noComment = getPreceedingOrProceeding(proceeding, pos, line);
        }
    }
    else {
        std::string tempLine = line;
        while (((pos = tempLine.find("/*")) != std::string::npos)){ // comment block is starting somewhere
            proceeding = false; // we want preceeding
            noComment += getPreceedingOrProceeding(proceeding, pos, tempLine);
            startBlockComment = true;
            std::string commented = tempLine.substr(pos+2);
            noComment += removeCommented(commented, startBlockComment);
            tempLine = noComment;
            noComment = "";
        }
        noComment = tempLine; // these will be the same at the end if comments were removed, and if nothing changed, then tempLine == line
    }
    return noComment;
}


/**
 * Reads fname as an .ort file -- essentially, as of now, 
 * only strips comments and whitespace from the start and end of lines,
 * after removing comments that start with "//",
 * from the start location to the end of the line.
 */
std::vector<std::string> OrtUtil::getOdr(std::string fname){
    std::vector<std::string> odrVec;
    std::ifstream odrFile(fname);
    if (!odrFile.is_open()){
        printf("Unable to locate the Ortus Development Rules (.ort) file. This is a BFD. Please rectify.\n");
        exit(52);
    }
    bool startBlockComment = false;
    std::string line;
    //int lineNo = 0;
    std::string fixedLine;
    unsigned long noPos = std::string::npos;
    while (std::getline(odrFile,line)){
        
        line = removeCommented(line, startBlockComment);
        if (noPos != line.find("/*") && noPos != line.find("*/")){
            printf("Only one block comment may be 'opened' at a time in .ort files.\n");
            exit(53);
        }
                
        fixedLine = removeLineComment(line);
        if (!fixedLine.empty()){
            odrVec.push_back(fixedLine); 
            //printf("%s\n",odrVec[lineNo].c_str());
            //lineNo++;
        }
    }
    odrVec = StrUtils::trimStrVec(odrVec); // gets rid of whitespace
    return odrVec;
}
