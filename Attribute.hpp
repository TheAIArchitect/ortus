//
//  Attribute.hpp
//  ortus
//
//  Created by andrew on 3/26/17.
//  Copyright © 2017 Andrew W.E. McDonald. All rights reserved.
//

#ifndef Attribute_hpp
#define Attribute_hpp

#include <string>

/** NOTE: if attributes are updated:
    # UPDATE 'Attribute_MAX' to the new last element
    # UPDATE "ATTRIBUTE_STRINGS" below... order *must* be parallel.
 */
// NOTE: shared names between different attributes have nothing to do with each other!!!

enum class ElementAttribute { Type, Affect, Activation, Thresh, NUM_ELEMENT_ATTRIBUTES};

enum class RelationAttribute { Type, Weight, Polarity, Direction, Age, Thresh, Decay, Mutability, NUM_RELATION_ATTRIBUTES};

// these are all scalars
enum class MetadataAttribute { NumElements, KernelIterationNum, ActivationHistorySize, NumXCorrComputations };

enum class ScalarAttribute { GapNormalizer, ChemNormalizer};



// these are all lowercase in the .ort file, which is what they match against
// NOTE: 'rtype' is really just a placeholder. relation types are denoted via newlines and whitespace in .ort files.
static std::string ELEMENT_ATTRIBUTE_STRINGS[static_cast<int>(ElementAttribute::NUM_ELEMENT_ATTRIBUTES)] = {"type", "affect", "activation", "thresh" };

static std::string RELATION_ATTRIBUTE_STRINGS[static_cast<int>(RelationAttribute::NUM_RELATION_ATTRIBUTES)] = {"type", "weight", "polarity", "direction", "age", "thresh", "decay", "mutability"};


/* so that Attribute can be used as an unordered_map key with c++11... thanks Sean  for the encouragment */
struct AttributeHash
{
    std::size_t operator()(const Attribute& a) const
    {
        return static_cast<std::size_t>(a);
    }
};


#endif /* Attribute_hpp */
