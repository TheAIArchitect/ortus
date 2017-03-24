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
        if (spaceCharCount != std::string::npos){ // then we found the first indented line
            INDENTATION_STRING = line.substr(0,spaceCharCount);
            INDENTATION_DETERMINED = true;
            indentationLevel = 1;
        }
        else {
            indentationLevel = 0;// should be the first line (comments should already have been stripped by the time this function is called)
        }
    }
    else {// we just check to see how many times we find INDENTATION_STRING at the start of line, which is our indentationLevel
        int indentation_level = 0;
        unsigned long pos = std::string::npos;
        std::string tempLine = line;
        while ((pos = tempLine.find(INDENTATION_STRING)) != std::string::npos){
            indentation_level++;
            tempLine = tempLine.substr(0,pos);
        }
        indentationLevel = indentation_level;
    }
    return StrUtils::trim(line);// strip leading and trailing whitespace
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
 */
std::unordered_map<std::string, std::string> OrtUtil::createAttributeMap(std::string line){
    std::unordered_map<std::string, std::string> attributeMap;
    unsigned long colonPos = line.find(":");
    if (colonPos == std::string::npos){
        printf("Error: no colon found in line '%s'\n", line.c_str());
        exit(54);
    }
    // deal with the element's name, and the sign prepended (e.g., +CO2)
    if (line[0] == '+'){
        attributeMap["name"] = line.substr(1,colonPos);
        attributeMap["direction"] = "pos";
    }
    else if (line[0] == '-'){
        attributeMap["name"] = line.substr(1,colonPos);
        attributeMap["direction"] = "neg";
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
                attributeMap[StrUtils::trim(pairVec[0])] = StrUtils::trim(pairVec[1]);
            }
        }
    }
    return attributeMap;
}

/**
 * new's an ElementInfoModule, and fills it according to the data in the attributeMap
 * 
 * NOTE: This is sets the id for each element.
 *
 * list of recognized attributes (all others are ignored):
 *  # type -- either sense, emotion, motor, or muscle
 *  # affect -- either pos or neg
 *  # opposite -- the name of an element that has an opposing funciton (e.g, CO2 and O2 sensors)
 *
 * NOTE: to add more attributes, add a block, similar to the blocks for currently recognized attributes,
 * and update the above list.
 */
void OrtUtil::addElement(std::unordered_map<std::string, std::string> attributeMap, std::vector<ElementInfoModule*>& elementModules, ortus::element_map& elementMap){
    
    if (attributeMap.find("name") == attributeMap.end()){
        printf("Error: missing 'name' attribute in attribute map.\n");
        exit(56);
    }
    std::string name = attributeMap["name"];
    if (elementMap.find(name) == elementMap.end()){// that's good, and we'll add it.
        elementMap[name] = new ElementInfoModule();
        elementMap[name]->id = elementModules.size();// new index will always be 1 more than current largest index, which will always be equal to the size of the vector.
        elementModules.push_back(elementMap[name]);
    }
    else {
        printf("Error: attempting to add duplicate element '%s' to elementMap.\n");
        exit(57);
    }
    ElementInfoModule* eim = elementMap[name];
    
    if (attributeMap.find("type") != attributeMap.end()){
        eim->setType(attributeMap["type"]);
    }
    if (attributeMap.find("affect") != attributeMap.end()){
        eim->setAffect(attributeMap["affect"]);
    }
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
        printf("Error: missing 'name' attribute in attribute map.\n");
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
 *
 */
void OrtUtil::addRelation(std::vector<std::unordered_map<std::string, std::string>>& vecOfAttributeMaps, std::vector<ElementRelation*>& elementRelations, ortus::element_map& elementMap, ElementRelationType ert){
    int numMaps = -1;
    if ((numMaps = vecOfAttributeMaps.size()) < 2){
        printf("Error: must have at least one opposing element, only found '%d' attribute maps, one of which should be the 'pre' element.\n",numMaps);
        exit(59);
    }
    // the first one is always the 'pre' (we'll call it major here, just for fun)
    std::unordered_map<std::string, std::string> majorMap = vecOfAttributeMaps[0];
    std::unordered_map<std::string, std::string> minorMap;
    ElementInfoModule* majorE = checkMapAndGetElementPointer(majorMap, elementMap);
    ElementInfoModule* minorE;
    std::string tempAttrib;
    // start at 1; we already took care of 0
    for (int i = 1; i < numMaps; ++i){
        tempAttrib = "";
        minorMap = vecOfAttributeMaps[i];
        minorE = checkMapAndGetElementPointer(minorMap, elementMap);
        ElementRelation* elrel = new ElementRelation();
        elrel->pre = majorE;// NOTE: if 'major' has attributes to set, they must be re-set for each ElementRelation
        elrel->post = minorE;
        // NOTE: as this grows, it might make more sense to have a function take an array of attributes to check for for each case.
        switch(ert){ 
            case CORRELATED:
                // none yet...
                break;
            case CAUSES:
                tempAttrib = checkMapAndGetValue(minorMap, "thresh");
                elrel->thresh = tempAttrib.empty() ? elrel->thresh : tempAttrib;
                break;
            case DOMINATES:
                // none yet...
                break;
            case OPPOSES:
                // none yet...
                break;
            default:
                printf("Error: unknown ElementRelationType, '%d'\n",ert);
                break;
        }
        elementRelations.push_back(elrel);
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
        vecOfAttributeMaps.push_back(createAttributeMap(relevantLine));
    }
    // and finally, update our curLineNum ref
    curLineNum = tempLineNum;
    return vecOfAttributeMaps;
}


void OrtUtil::setElements(std::vector<std::string>& theLines, std::vector<ElementInfoModule*>& elementModules, std::vector<ElementRelation*>& elementRelations, ortus::element_map& elementMap){
    //    fixedLine = StrUtils::trim(fixedLine); // remove any remaining whitespace
    /*
     definitionLevel = 0: elements => neurons/areas/muscles, with attributes (type, +affect, etc.)
     definitionLevel = 1: causes => how increases or decreases in neurons/areas impact other things
     definitionLevel = 2: dominates => defines hierarchy of neurons/areas
    */
    int definitionLevel = -1;
    std::string trimmedLine = "";
    int indentationLevel = 0;
    int prevIdentationLevel = -1;
    // NOTE: at end of level 0, update NUM_ELEMENTS!!!
    int numLines = theLines.size();
    int lineNum = 0;
    std::vector<std::unordered_map<std::string, std::string>> vecOfAttributeMaps;
    std::string line;
    ElementRelationType ert;
    while (lineNum < numLines){
        vecOfAttributeMaps.clear();
        line = theLines[lineNum];
        trimmedLine = determineIndentationAndStripWhitespace(line, indentationLevel);
        if (indentationLevel == 0 && prevIdentationLevel != 0){
            definitionLevel++; // move to new definition level if we drop back to no indentation
            prevIdentationLevel = 0;
            lineNum++;
            continue; // nothing else on this line
        }
        if ("elements" == ORT_DEFINITION_LEVELS[definitionLevel]){ // just one line
            std::unordered_map<std::string, std::string> attributeMap = createAttributeMap(line);
            addElement(attributeMap, elementModules, elementMap);
        }
        else {
            vecOfAttributeMaps = createVecOfAttributeMapsContainingRelevantLines(theLines, lineNum);
            ert = NONE;
            if ("opposes" == ORT_DEFINITION_LEVELS[definitionLevel]){ // at least 2 lines
                ert = OPPOSES;
                
            }
            else if ("causes" == ORT_DEFINITION_LEVELS[definitionLevel]){ // at least 2 lines
                ert = CAUSES;
                
            }
            else if ("dominates" == ORT_DEFINITION_LEVELS[definitionLevel]){ // at least 2 lines
                ert = DOMINATES;
            }
            addRelation(vecOfAttributeMaps, elementRelations, elementMap, ert);
        }
        lineNum++;
    }
    printf("cool.\n");
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
        printf("Unable to locate the Ortus Development Rules (.ort) file. This is a BFD. Please rectify.\n");
        exit(52);
    }
    bool startBlockComment = false;
    std::string line;
    int lineNo = 0;
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
            printf(">>%s<<\n",odrVec[lineNo].c_str());
            lineNo++;
        }
    }
    return odrVec; // note, there is still whitespace here, because we need the tabs/spaces at the beginning of lines... like python
}

void OrtUtil::makeAndSetElements(std::string ortFile, std::vector<ElementInfoModule*>& elementModules, std::vector<ElementRelation*>& elementRelations, ortus::element_map& elementMap){
    ORT_FILE = ortFile;
    std::vector<std::string> theLines = getOdrLines(ORT_FILE);
    setElements(theLines, elementModules, elementRelations, elementMap);
}
