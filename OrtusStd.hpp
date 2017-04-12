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

// NOTE: 'Activation', while supremely important, is not an 'attribute', and gets its own Blade.

enum class WeightAttribute { CSWeight, GJWeight};

enum class ElementAttribute { Type, Affect, Thresh, NUM_ELEMENT_ATTRIBUTES};

/** NOTE NOTE NOTE: need to add a PreDirection and PostDirection, to replace Direction.
 * as it is now, the PreDirection gets overwritten with PostDirection, which is just Direction.
 *
 * though, polarity and direction might be the same thing...
 */
enum class RelationAttribute { Type, Polarity, Direction, Age, Thresh, Decay, Mutability, NUM_RELATION_ATTRIBUTES};

enum class GlobalAttribute { GapNormalizer, ChemNormalizer};

// these are all scalars -- NOTE: perhaps add MAX_ELEMENTS??
enum class MetadataAttribute { NumElements, MaxElements, KernelIterationNum, ActivationHistorySize, NumXCorrComputations, XCorrSize, NumSlopeComputations, SlopeSize};

// scratch pats for cross correlation (XCorr), and rate of change of activation (Slope)
enum class Scratchpad { XCorr, Slope};


// these are all lowercase in the .ort file, which is what they match against
// NOTE: 'rtype' is really just a placeholder. relation types are denoted via newlines and whitespace in .ort files.
const static std::string ELEMENT_ATTRIBUTE_STRINGS[static_cast<int>(ElementAttribute::NUM_ELEMENT_ATTRIBUTES)] = {"type", "affect", "thresh" };

const static std::string RELATION_ATTRIBUTE_STRINGS[static_cast<int>(RelationAttribute::NUM_RELATION_ATTRIBUTES)] = {"type", "polarity", "direction", "age", "thresh", "decay", "mutability"};


/* so that an enum can be used as an unordered_map key with c++11... thanks Sean  for the encouragment */
template <typename T>
struct EnumHash
{
    std::size_t operator()(const T& t) const
    {
        return static_cast<std::size_t>(t);
    }
};



class ElementRelation;
class ElementInfoModule;

class Ort {
public:
    const static int BLADE_METADATA_COUNT = 8;// note this is the number of metadata elements the kernel will get, not the number of elements in the .ort file
    
    
    // START DEFAULT VALUES 
    // These get set in OrtUtil, if values are specified in the .ort file
    // (default values are set in main)
    static int NUM_ELEMENTS;
    static int MAX_ELEMENTS;
    static int ACTIVATION_HISTORY_SIZE; // all but one usable, and the 0th is the 'staging' area -- filled by the current one (but can't be read from because there's no [good] way to ensure other threads have updated theirs)
    
    // this is the number of computations that can be stored per 'core',
    // e.g., that number of XCorr computations, or Slope comptuations,
    // w.r.t. historical values.
    static int SCRATCHPAD_COMPUTATION_SLOTS;
    static int XCORR_COMPUTATIONS;
    static int XCORR_SIZE;
    static int SLOPE_COMPUTATIONS;
    static int SLOPE_SIZE;
    static int WEIGHT_HISTORY_SIZE;
    // END DEFAULT VALUES
    static int NUM_ITERATIONS;
    
    
    // CS and GJ weights are *NOT* 'RelationAttribute's
    static const int NUM_ELEMENT_ATTRIBUTES = static_cast<int>(ElementAttribute::NUM_ELEMENT_ATTRIBUTES);
    static const int NUM_RELATION_ATTRIBUTES = static_cast<int>(RelationAttribute::NUM_RELATION_ATTRIBUTES);
    
    // if this is 'true', the relation data gets stored in a transposed fashion.
    // meaning, the rows and cols are swapped.
    // it *also* is returned after getting transposed again.
    // This happens in ElementRelation.
    static const bool STORE_RELATIONS_TRANSPOSED = true;
    
    
};




namespace ortus {
    using element_map = std::unordered_map<std::string, ElementInfoModule*>;
    using name_map = std::unordered_map<std::string, int>;
    using index_map = std::unordered_map<int, std::string>;
    
    using relation_map = std::unordered_map<int, std::vector<ElementRelation*>>;
    
    //template <class T>
    //using attribute_map = std::vector<T*>;
    using attribute_map = std::vector<cl_float*>;
    
    //using attribute_blade_map = std::vector<Blade<cl_float>*>;
    
    template <class T, class U>
    using enum_blade_unordered_map = std::unordered_map<T,Blade<U>*,EnumHash<T>>;
    
    template <class T>
    using enum_string_unordered_map = std::unordered_map<T,std::string, EnumHash<T>>;
    
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
    
    
};


#endif /* OrtusStd_h */
