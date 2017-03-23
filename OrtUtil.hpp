#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "StrUtils.hpp"
#include <unordered_map>
#include "OrtusNamespace.hpp"
#include "ElementInfoModule.hpp"

/**
 * Eventually, this should save the comments from the file it reads in.
 * It should also be able to write a new .ort file from the connectome
 */

class OrtUtil {
    public:
    OrtUtil(std::string ortFile);
        std::vector<std::string> getOdrLines(std::string fname);
        void setElements(std::vector<std::string>& theLines, ortus::element_map& elementMap);
        std::string removeLineComment(std::string s);
        ortus::element_map* makeAndGetElements();
        std::string determineIndentationAndStripWhitespace(std::string line, int& indendationLevel);
        std::unordered_map<std::string, std::string> createAttributeMap(std::string line);
    
        void addElement(std::unordered_map<std::string, std::string> attributeMap, ortus::element_map& elementMap);
        void addOpposites(std::vector<std::unordered_map<std::string, std::string>> vecOfAttributeMaps, ortus::element_map& elementMap);
    
public:
        static int NUM_ELEMENTS;
        static std::string ORT_FILE;
        static std::vector<std::string> ORT_DEFINITION_LEVELS;
        static bool INDENTATION_DETERMINED;
        static std::string INDENTATION_STRING;
    
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
