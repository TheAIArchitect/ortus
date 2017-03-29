#include "OrtUtil.hpp"

int OrtUtil::NUM_ELEMENTS = 0;
std::string OrtUtil::ORT_FILE = "";
std::vector<std::string> OrtUtil::ORT_DEFINITION_LEVELS = {"elements", "opposes", "causes", "dominates"};
/*
 the first intented line read determines the indentation style.
 the set of space characters at the beginning of that line are stored in INDENTATION_STRING,
 and every proceeding line is checked against that string,
 and the number of times it is found is counted -- this is the indentation level of that line.

 tl; dr; indentation basically works like python.
 */
bool OrtUtil::INDENTATION_DETERMINED = false;
std::string OrtUtil::INDENTATION_STRING = "";
int OrtUtil::INDENTATION_STRING_LENGTH = 0;


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

std::string OrtUtil::determineIndentationAndStripWhitespace(std::string line, int& indentationLevel){
    if (!INDENTATION_DETERMINED){
        unsigned long spaceCharCount = line.find_first_not_of("\t ");
        if (spaceCharCount > 0){ // then we found the first indented line
            INDENTATION_STRING = line.substr(0,spaceCharCount);
            INDENTATION_STRING_LENGTH = INDENTATION_STRING.size();
            INDENTATION_DETERMINED = true;
            indentationLevel = 1;
        }
        else {
            indentationLevel = 0;// should be the first line (comments should already have been stripped by the time this function is called)
        }
    }
    else {// we just check to see how many times we find INDENTATION_STRING at the start of line, which is our indentationLevel
        int localIndentationLevel = 0;
        unsigned long pos = std::string::npos;
        std::string tempLine = line;
        while ((pos = tempLine.find(INDENTATION_STRING)) != std::string::npos){
            localIndentationLevel++;
            tempLine = tempLine.substr(INDENTATION_STRING_LENGTH);// we want to grab from one past where the match ended, so we can try to match again -- the match should have started at 0, and ended at (length of indentation string - 1)
            if (localIndentationLevel > 25){// then we probably screwed up, becuase that's a huge indent...
                printf("Error: indentation level now at '%d' -- if this is not an error, find this line of code, and increase the number that allows the if block this statement is in to return true. But, it's probably an error...\n",localIndentationLevel);
                exit(60);
            }
        }
        indentationLevel = localIndentationLevel;
    }
    return StrUtils::trim(line);// strip leading and trailing whitespace
}

/**
 * converts the attribute keys from strings to enumerated values.
 *
 * this step leaves the corresponding attribute values as strings,
 * the Connectome class takes care of that, because it is more of a program execution concern,
 * rather than a parsing concern.
 */
ortus::attribute_unordered_map OrtUtil::getAttributeEnumsFromStrings(std::unordered_map<std::string, std::string> attributeMapStrings){
    ortus::attribute_unordered_map newMap;
    newMap.reserve(attributeMapStrings.size());
    int i = 0;
    for (auto entry : attributeMapStrings){
        for (i = 0; i <  ElementRelation::NUM_ATTRIBUTES; ++i){
            if (entry.first == ATTRIBUTE_STRINGS[i]){
                newMap[static_cast<Attribute>(i)] = entry.second;
            }
        }
    }
    return newMap;
}

/**
 * to parse lines of the form "[+-]<name>: { <key0>=<value0>, ..., <keyN>=<valueN> }"
 *
 * [+-] means either +, -, or neither.
 * the colon after name *must* be there
 * if there are no attributes, the braces are not necessary.
 * there may be any number of attributes.
 * there *must* not be any leading whitespace,
 * but whitespace within the line is irrelevant.
 *
 * NOTE: this doesn't convert the 'keys' to their enumerated values yet,
 * that happens later, (among other reasons, the name is in this map at the moment)
 */
std::unordered_map<std::string, std::string> OrtUtil::createAttributeMapStrings(std::string line){
    std::unordered_map<std::string, std::string> attributeMapStrings;
    unsigned long colonPos = line.find(":");
    if (colonPos == std::string::npos){
        printf("Error: no colon found in line '%s'\n", line.c_str());
        exit(54);
    }
    // deal with the element's name, and the sign prepended (e.g., +CO2)
    if (line[0] == '+'){
        attributeMapStrings["name"] = line.substr(1,colonPos-1); // -1 to account for sign
        attributeMapStrings["direction"] = "pos";
    }
    else if (line[0] == '-'){
        attributeMapStrings["name"] = line.substr(1,colonPos-1); // -1 to account for sign
        attributeMapStrings["direction"] = "neg";
    }
    else { // no sign
        attributeMapStrings["name"] = line.substr(0,colonPos); // no sign, no -1
    }
    // now see if there's a 'dictionary' -- { ... }
    unsigned long openBracketPos = line.find("{");
    if (openBracketPos != std::string::npos){
        unsigned long closeBracketPos = line.find("}");
        if (closeBracketPos == std::string::npos){
            printf("Error: no end to dictionary in line '%s'\n", line.c_str());
            exit(55);
        }
        // pull from after the start bracket
        unsigned long dictStartPos = openBracketPos+1;
        unsigned long dictLength = closeBracketPos - dictStartPos;
        if (dictLength != 0){
            std::string dictString = line.substr(dictStartPos, dictLength);
            std::vector<std::string> splitDict = StrUtils::parseOnCharDelim(dictString, ',');
            for (auto pair : splitDict){
                // now split each pair on the '=', and trim before putting into map
                std::vector<std::string> pairVec = StrUtils::parseOnCharDelim(pair, '=');
                attributeMapStrings[StrUtils::trim(pairVec[0])] = StrUtils::trim(pairVec[1]);
            }
        }
    }
    return attributeMapStrings;
}



/**
 * main reason for is the error checking
 */
ElementInfoModule* OrtUtil::checkMapAndGetElementPointer(std::unordered_map<std::string, std::string>& attributeMap, ortus::element_map& elementMap){
    if (attributeMap.find("name") == attributeMap.end()){
        printf("Error: missing 'name' attribute in attribute map.\n");
        exit(56);
    }
    std::string name = attributeMap["name"];
    if (elementMap.find(name) == elementMap.end()){// something isn't right.
        // specifically, we're looking for an element that doesn't seem to exist.
        printf("Error: missing 'name' attribute in element map, '%s'.\n(This probably means a relation was defined without first defining all elements involved.)\n",name.c_str());
        exit(58);// i used 57 above. not sure about 58. this isn't a very good system.
    }
    else { // we have it!
        return elementMap[name];
    }
}


/**
 * main reason for is the error checking. Returns an empty string if value not present.
 */
std::string OrtUtil::checkMapAndGetValue(std::unordered_map<std::string, std::string>& attributeMap, std::string key){
    if (attributeMap.find(key) == attributeMap.end()){
        return "";
    }
    else {
        return attributeMap[key];
    }
}




/**
 * -----------------------------------------------------------------------------------------
 * NOTE: 'curLineNum' GETS UPDATED BY THIS FUNCTION,
 * SUCH THAT IT IS THE LAST INDEX USED OF theLines BY THIS FUNCTION!!!
 * ===> THAT MEANS THAT 'setElements' **MUST** INCREMENT IT PRIOR TO ACCESSING ANOTHER LINE!
 * -----------------------------------------------------------------------------------------
 *
 * gets all lines relevant to a 'pre' element, along with that element's line.
 * That is, all lines in 'theLines' that are one indentation level greater
 * than the current line (identified by curLineNum)
 *
 * e.g.:
 * 0.   causes:
 * 1.       +CO2:
 * 2.               +FEAR:
 * 3.               -SOMETHING:
 * 4.       +ANOTHERTHING:
 * 5.               +THING:
 *
 * In this case, if our starting line was 1, getRelevantLines() would return
 * a vector with maps created by calling 'createAttributeMap' (above)
 * from lines 1, 2, and 3. So, it would have 3 'attribute maps'.
 *
 * Note, a valid .ort file will have things above what is shown to be line 0 in the example above.
 */
std::vector<std::unordered_map<std::string, std::string>> OrtUtil::createVecOfAttributeMapsContainingRelevantLines(std::vector<std::string>& theLines, int& curLineNum){
    std::vector<std::string> relevantLines;
    int numLines = theLines.size();
    std::string tempLine;
    int tempLineNum = curLineNum;
    int startingLineIndent = -1;
    int curLineIndent = -1;
    int indentationDesired = -1;
    if (tempLineNum < numLines){
        tempLine = determineIndentationAndStripWhitespace(theLines[tempLineNum], startingLineIndent);
        indentationDesired = startingLineIndent + 1; // we want lines that are one out from the starting line
        relevantLines.push_back(tempLine);
    }
    tempLineNum++;// increment before using again...
    while (tempLineNum < numLines){
        tempLine = determineIndentationAndStripWhitespace(theLines[tempLineNum], curLineIndent);
        if (curLineIndent == indentationDesired){ // then we keep it!
            relevantLines.push_back(tempLine);
            tempLineNum++; // and increment
        }
        else { // we just grabbed a line that we don't want
            tempLineNum--; // need to *decrement* our tempLineNum, because we didn't actually use ths line
            break; // but then break, otherwise we're stuck here for a long time...
        }
    }
    // now that we have all the lines we care about, we loop through them and create 'attribute maps'
    std::vector<std::unordered_map<std::string, std::string>> vecOfAttributeMaps;
    for (auto relevantLine : relevantLines){
        vecOfAttributeMaps.push_back(createAttributeMapStrings(relevantLine));
    }
    // and finally, update our curLineNum ref
    curLineNum = tempLineNum;
    return vecOfAttributeMaps;
}





/**
 * Reads fname as an .ort file -- essentially, as of now, 
 * only strips comments and whitespace from the start and end of lines,
 * after removing comments that start with "//",
 * from the start location to the end of the line.
 */
std::vector<std::string> OrtUtil::getOdrLines(std::string fname){
    std::vector<std::string> odrVec;
    std::ifstream odrFile(fname);
    if (!odrFile.is_open()){
        printf("Error: Unable to locate the Ortus Development Rules (.ort) file. This is a BFD. Please rectify.\n");
        exit(52);
    }
    bool startBlockComment = false;
    std::string line;
    int lineNo = 0;
    std::string fixedLine;
    std::string trimTest;
    unsigned long noPos = std::string::npos;
    while (std::getline(odrFile,line)){
        
        line = removeCommented(line, startBlockComment); // this is for block comments
        if (noPos != line.find("/*") && noPos != line.find("*/")){
            printf("Error: Only one block comment may be 'opened' at a time in .ort files.\n");
            exit(53);
        }
        fixedLine = removeLineComment(line); // this is for single line comments
        // because we use whitespace to parse the file, we can't store the trimmed version yet, but we do want to make sure we're not including any empty lines
        trimTest = StrUtils::trim(fixedLine);
        if (!trimTest.empty()){
            odrVec.push_back(fixedLine); 
            //printf(">>%s<<\n",odrVec[lineNo].c_str());
            lineNo++;
        }
    }
    return odrVec; // note, there is still whitespace here, because we need the tabs/spaces at the beginning of lines... like python
}

void OrtUtil::getLines(std::string ortFile, std::vector<std::string>& theLines){
    ORT_FILE = ortFile;
    theLines = getOdrLines(ORT_FILE);
    //setElements(theLines, elementModules, elementRelations, elementMap);
}
