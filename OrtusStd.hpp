//
//  OrtusStd.hpp
//  ortus
//
//  Created by andrew on 1/26/17.
//  Copyright © 2017 Andrew W.E. McDonald. All rights reserved.
//

#ifndef OrtusStd_h
#define OrtusStd_h

#include <unordered_map>
#include <vector>
#include "vtkFloatArray.h"
#include "Blade.hpp"

#include <string>

/** NOTE: if attributes are updated:
    # UPDATE 'Attribute_MAX' to the new last element
    # UPDATE "ATTRIBUTE_STRINGS" below... order *must* be parallel.
 */
// NOTE: shared names between different attributes have nothing to do with each other!!!

enum class WeightAttribute { CSWeight, GJWeight};

enum class ElementAttribute { Type, Affect, Activation, Thresh, NUM_ELEMENT_ATTRIBUTES};

enum class RelationAttribute { Type, Polarity, Direction, Age, Thresh, Decay, Mutability, NUM_RELATION_ATTRIBUTES};

enum class GlobalAttribute { GapNormalizer, ChemNormalizer};

// these are all scalars
enum class MetadataAttribute { NumElements, KernelIterationNum, ActivationHistorySize, NumXCorrComputations, NumSlopeComputations};

// scratch pats for cross correlation (XCorr), and rate of change of activation (Slope)
enum class Scratchpad { XCorr, Slope};


// these are all lowercase in the .ort file, which is what they match against
// NOTE: 'rtype' is really just a placeholder. relation types are denoted via newlines and whitespace in .ort files.
const static std::string ELEMENT_ATTRIBUTE_STRINGS[static_cast<int>(ElementAttribute::NUM_ELEMENT_ATTRIBUTES)] = {"type", "affect", "activation", "thresh" };

const static std::string RELATION_ATTRIBUTE_STRINGS[static_cast<int>(RelationAttribute::NUM_RELATION_ATTRIBUTES)] = {"type", "polarity", "direction", "age", "thresh", "decay", "mutability"};


class ElementRelation;
class ElementInfoModule;


namespace ortus {
    
    static int NUM_ELEMENTS = 10;
    static int MAX_ELEMENTS = 100;
    // CS and GJ weights are *NOT* 'RelationAttribute's
    static const int NUM_ELEMENT_ATTRIBUTES = static_cast<int>(ElementAttribute::NUM_ELEMENT_ATTRIBUTES);
    static const int NUM_RELATION_ATTRIBUTES = static_cast<int>(RelationAttribute::NUM_RELATION_ATTRIBUTES);
    
    using element_map = std::unordered_map<std::string, ElementInfoModule*>;
    using name_map = std::unordered_map<std::string, int>;
    using index_map = std::unordered_map<int, std::string>;
    
    using relation_map = std::unordered_map<int, std::vector<ElementRelation*>>;
    
    using attribute_map = std::vector<float*>;
    
    //using attribute_blade_map = std::vector<Blade<cl_float>*>;
    using attribute_blade_unordered_map = std::unordered_map<Attribute,Blade<cl_float>*,AttributeHash>;
    
    using attribute_unordered_map = std::unordered_map<Attribute,std::string, AttributeHash>;
    
    using vector = std::vector<float>;
    using vectrix = std::vector<vector>;
    using trivec = std::vector<vectrix>;
    using partialxcorr = std::unordered_map<int, vectrix>;
    
    /* map element index (Ek) to a 3D array of xcorr results:
     *
     * Ek -> [XCE0, XCE1, XCE2, ..., XCEk]
     *
     * where XCEn is a 3D array, containing the xcorr computation between element n,
     * and all other elements, at all other times that we cared about, 
     * such that (taking XC0 as the example):
     *
     * XCE0 -> [E0W0, E0W1, ..., E0Wn]
     * (^ for XCE1, it would be E1W[0..n])
     *
     * where E0Wn is the xcorr computation between element 0 and all other elements,
     * at time window n. So,
     * 
     * E0W0 -> [R0E0, R0E1, ..., R0Ek]
     * 
     * where R0Ek is a 1D vector of xcorr results between element 0 (in this example),
     * and element k at lags 0, 1, 2, and 3 (as of this writing).
     *
     * R0E0 -> [Exc0, Exc1, Exc2, Exc3]
     *
     */
    using fullxcorr = std::unordered_map<int, trivec>;
    
    ///////////////////////// TEMP
    using vtkVector = vtkFloatArray;
    using vtkVectrix = std::vector<vtkVector*>;
    using vtkTrivec = std::vector<vtkVectrix*>;
    using vtkFullxcorr = std::unordered_map<int, vtkTrivec*>;
    
    
}


/* so that Attribute can be used as an unordered_map key with c++11... thanks Sean  for the encouragment */
struct AttributeHash
{
    std::size_t operator()(const Attribute& a) const
    {
        return static_cast<std::size_t>(a);
    }
};




#endif /* OrtusStd_h */
