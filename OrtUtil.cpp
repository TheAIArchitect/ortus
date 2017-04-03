#include "OrtUtil.hpp"

std::string OrtUtil::ORT_FILE = "";
// NOTE: the strings in this array must be in the same order that they are found in the .ort file
// (which is defined in the .ort specification)
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
 *
 * NOTE: the last argument, 'isElement', *must* be set to true if extracting an element,
 * and *must* be set to false if extracting an element relation (e.g. 'causes')
 *  -> different string arrays are used to match against the arguments for an element vs relation
 *
 * NOTE 2: Immediately after modifying this function to work with templates,
 * it came to my attention that this function is not actually used to parse elements.
 * Well... it's staying like this for the time being (until things work, then i might rip it out)
 */
template <class T>
ortus::enum_string_unordered_map<T> OrtUtil::getAttributeEnumsFromStrings(std::unordered_map<std::string, std::string> attributeMapStrings, bool isElement){
    ortus::enum_string_unordered_map<T> newMap;
    newMap.reserve(attributeMapStrings.size());
    int i = 0;
    /* begin screwy code:
     * need to pick between two array lengths, and two string arrays
     */
    int theRightNumberOfAttributesToCheck;
    const std::string* twoPossibleStringArrays[2] = {ELEMENT_ATTRIBUTE_STRINGS, RELATION_ATTRIBUTE_STRINGS};
    int theCorrectIndex = -1;
    if (isElement){
        theRightNumberOfAttributesToCheck = ortus::NUM_ELEMENT_ATTRIBUTES;
        theCorrectIndex = 0;
    }
    else {
        theRightNumberOfAttributesToCheck = ortus::NUM_RELATION_ATTRIBUTES;
        theCorrectIndex = 1;
    }
    const std::string* theRightAttributesToCheck = twoPossibleStringArrays[theCorrectIndex];
    /* end screwy code */
    for (auto entry : attributeMapStrings){
        for (i = 0; i <  theRightNumberOfAttributesToCheck; ++i){
            // NOTE: it is vital that the enums and the loop starts at 0.
            // it is also vital that the string arrays we check against,
            // have the same ordering as the enums.
            // **The assumtion, as you can see below, is that we can set 'i',
            // the loop var, to be the enumereated value when we find its name in the 'right' string array.
            if (entry.first == theRightAttributesToCheck[i]){
                newMap[static_cast<T>(i)] = entry.second;
            }
        }
    }
    return newMap;
}

template ortus::enum_string_unordered_map<RelationAttribute> OrtUtil::getAttributeEnumsFromStrings(std::unordered_map<std::string, std::string> attributeMapStrings, bool isElement);

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
        attributeMapStrings["direction"] = "1";
    }
    else if (line[0] == '-'){
        attributeMapStrings["name"] = line.substr(1,colonPos-1); // -1 to account for sign
        attributeMapStrings["direction"] = "-1";
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
 * Reads fname as an .ort file
 * strips comments  (block and line), and blank lines
 * (including lines that end up blank after stripping any comments in them)
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



std::vector<std::string> OrtUtil::getLines(std::string ortFile){
    ORT_FILE = ortFile;
    return getOdrLines(ORT_FILE);
}


/**
 * THIS FUNCTION SETS 'ortus::NUM_ELEMENTS'
 *
 * BUT, IT MUST BE CALLED AFTER 'readAndStripOrtFileMetadata'
 *
 * This counts the number of elements, and sets ortus::NUM_ELEMENTS
 */
void OrtUtil::countAndSetNumElements(std::vector<std::string>& theLines){
    std::string trimmed = "";
    int elementCount = 0;
    int indentationLevel = 0;
    int lineNo = 0;
    int lineCount = theLines.size();
    // now we go through the lines, and:
    trimmed = determineIndentationAndStripWhitespace(theLines[lineNo], indentationLevel);
    // we need to check trimmed from start to end-1 because of the ":" at the end of 'elements'
    if (trimmed.substr(0,trimmed.size()-1) == OrtUtil::ORT_DEFINITION_LEVELS[0]){ // index 0 is "elements"
        // once we find the 'elements' line, we enter a new loop that counts all the elements
        lineNo++;// first, we increment lineNo though
        determineIndentationAndStripWhitespace(theLines[lineNo], indentationLevel); // do first one outside of loop
        while (indentationLevel == 1 && lineNo < lineCount){// we are still running over elements (each element occupies 1 line)
            elementCount++; // we know first one exists becuase we entered the loop
            lineNo++;
            // we need to save the trimmed output so we can match against it once the loop exits.
            trimmed = determineIndentationAndStripWhitespace(theLines[lineNo], indentationLevel);
        }
        // now that we are out of the loop, it means the *current* line has a different indentation level... don't skip it!
    }
    else {
        printf("(OrtUtil) Error: .ort file format incorrect. 'elements:' must be the first line after any metadata. First line found was '%s'\n", theLines[lineNo].c_str());
        exit(17);
    }
    ortus::NUM_ELEMENTS = elementCount;
}




/**
 * Metadata may be placed in (and in some cases, may be required, see blelow)
 * the .ort file, at the top, before the first definition level ('elements'),
 * with one per line.
 *
 * required metadata:
 *      * MAX_ELEMENTS
 */
void OrtUtil::readAndStripOrtFileMetadata(std::vector<std::string>& theLines){
    bool maxElementsSpecified = false; // this *must* be in the .ort file.
    int indentationLevel = -1;// won't use this.
    std::string trimmed = determineIndentationAndStripWhitespace(theLines[0], indentationLevel);
    // we need to check trimmed from start to end-1 because of the ":" at the end of 'elements'
    while (trimmed.substr(0,trimmed.size()-1) != ORT_DEFINITION_LEVELS[0]) {// when 'elements' starts, we're done looking for metadata
        std::vector<std::string> tempSplit = StrUtils::parseOnCharDelim(theLines[0],'=');
        if (StrUtils::trim(tempSplit[0]) == "MAX_ELEMENTS"){
            ortus::MAX_ELEMENTS = std::stoi(StrUtils::trim(tempSplit[1]));// so now, we know what our MAX_ELEMENTS will be
            if (ortus::MAX_ELEMENTS < 1){
                printf("MAX_ELEMENTS, as specified at the top of the .ort file, must be greater than 0.\n");
                exit(15);
            }
            maxElementsSpecified = true;
        }
        else if (StrUtils::trim(tempSplit[0]) == "XCORR_COMPUTATIONS"){
            ortus::XCORR_COMPUTATIONS = std::stoi(StrUtils::trim(tempSplit[1]));
            if (ortus::XCORR_COMPUTATIONS < 3){ // less than 3 doesn't seem to make much sense for xcorr...
                printf("XCORR_COMPUTATIONS must be at least 3.\n");
                exit(15);
            }
        }
        else if (StrUtils::trim(tempSplit[0]) == "SLOPE_COMPUTATIONS"){
            ortus::SLOPE_COMPUTATIONS = std::stoi(StrUtils::trim(tempSplit[1]));
            if (ortus::SLOPE_COMPUTATIONS < 3){ // xcorr and slope use the same kernel arg (as of now)
                printf("SLOPE_COMPUTATIONS must be at least 3.\n");
                exit(15);
            }
        }
        else if (StrUtils::trim(tempSplit[0]) == "ACTIVATION_HISTORY_SIZE"){
            ortus::ACTIVATION_HISTORY_SIZE = std::stoi(StrUtils::trim(tempSplit[1]));;
            if (ortus::ACTIVATION_HISTORY_SIZE < 2){ // one index is for current, need at least one for history
                printf("ACTIVATION_HISTORY_SIZE must be at least 2. Using default.\n");
                exit(15);
            }
        }
        else if (StrUtils::trim(tempSplit[0]) == "WEIGHT_HISTORY_SIZE"){
            ortus::WEIGHT_HISTORY_SIZE = std::stoi(StrUtils::trim(tempSplit[1]));;
            if (ortus::WEIGHT_HISTORY_SIZE < 2){ // one index is for current, need at least one for history
                printf("WEIGHT_HISTORY_SIZE must be at least 2. Using default.\n");
                exit(15);
            }
        }
        //finally, delete that line, and re-read line number 0, because now the line that was 1 is in index 0.
        theLines.erase(theLines.begin());
        trimmed = determineIndentationAndStripWhitespace(theLines[0], indentationLevel);
    }
    if (ortus::XCORR_COMPUTATIONS != ortus::SLOPE_COMPUTATIONS){
        printf("XCORR_COMPUTATIONS and SLOPE_COMPUTATIONS must be equivalent!\n");
        exit(15);
    }
    if (!maxElementsSpecified){
        printf("(Connectome) Error: .ort file missing metadata 'MAX_ELEMENTS = <int>', where <int> is an integer specifiying the maximum permissible number of elements.");
        exit(15);
    }
}
