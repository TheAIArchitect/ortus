#ifndef OrtUtil_hpp
#define OrtUtil_hpp

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "StrUtils.hpp"
#include <unordered_map>
#include "OrtusStd.hpp"
#include "ElementInfoModule.hpp"
#include "ElementRelation.hpp"

/**
 * Eventually, this should save the comments from the file it reads in.
 * It should also be able to write a new .ort file from the connectome
 */

class OrtUtil {
private:
    std::vector<std::string> getOdrLines(std::string fname);
    
    
    public:
        OrtUtil();
    
        std::vector<std::string> getLines(std::string ortFile);
    
        void setElements(std::vector<std::string>& theLines, std::vector<ElementInfoModule*>& elementModules, std::vector<ElementRelation*>& elementRelations, ortus::element_map& elementMap);
    
        std::string removeLineComment(std::string s);
    
    
        std::string determineIndentationAndStripWhitespace(std::string line, int& indendationLevel);
    
        std::vector<std::unordered_map<std::string, std::string>> createVecOfAttributeMapsContainingRelevantLines(std::vector<std::string>& theLines, int& curLineNum);
    
    
        template <class T>
        ortus::enum_string_unordered_map<T> getAttributeEnumsFromStrings(std::unordered_map<std::string, std::string> attributeMapStrings, bool isElement);
    
    
        std::unordered_map<std::string, std::string> createAttributeMapStrings(std::string line, bool isPre);
    
        void addElement(std::unordered_map<std::string, std::string> attributeMap, std::vector<ElementInfoModule*>& elementModules, ortus::element_map& elementMap);
    
        ElementInfoModule* checkMapAndGetElementPointer(std::unordered_map<std::string, std::string>& attributeMap, ortus::element_map& elementMap);
    
        static std::string checkMapAndGetValue(std::unordered_map<std::string, std::string>& attributeMap, std::string key);
    
        void addRelation(std::vector<std::unordered_map<std::string, std::string>>& vecOfAttributeMaps, std::vector<ElementRelation*>& elementRelations, ortus::element_map& elementMap, ElementRelationType ert);
    
        void countAndSetNumElements(std::vector<std::string>& theLines);
    
    
public:
        static int NUM_ELEMENTS;
        static std::string ORT_FILE;
        static std::vector<std::string> ORT_DEFINITION_LEVELS;
        static bool INDENTATION_DETERMINED;
        static std::string INDENTATION_STRING;
        static int INDENTATION_STRING_LENGTH;
    
    void readAndStripOrtFileMetadata(std::vector<std::string>& theLines);
    
private:
        std::string removeCommented(std::string line, bool& startBlockComment);
        std::string getPreceedingOrProceeding(bool proceeding, unsigned long pos, std::string line);
};


/**
 * makeAndGetElements() is called, which new's an ortus::element_map.
 * Then, getOrdLines takes a filename and reads the .ort file with the Ortus Development Rules (ODRs). This function strips comments, and returns a vector of strings.
 * setElements() then runs through the vector of strings returned by getOrdLines,
 * and calls createAttributeMap for *each line*, to allow easy parsing of any attributes that may be added on any line
 * This map *always* has an attribute (key) called 'name'. Everything is mapped as string->string,
 * therefore, later parsing of individual values must do any string->numeric conversions required.
 * Calls to:
 * addElement()
 *
 * addOpposites()
 * addCause()
 * addDominator()
 *
 * create ElementInfoModules, and set attributes.
 *
 * the vector of maps passed to the 3rd three functions is for things like:
 * 
 * causes:
 *      +O2: { ... }
 *          +FEAR: { ... }
 *
 * and O2 and it's dictionary would be the first map,
 * with proceeding maps being the things that +O2 causes (along with their dictionaries).
 */

#endif
