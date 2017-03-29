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

enum class Attribute { Type, Affect, Activation, Weight, Polarity, Direction, Age, Thresh, Decay, Mutability, NUM_ATTRIBUTES};

// these are all lowercase in the .ort file, which is what they match against
static std::string ATTRIBUTE_STRINGS[static_cast<int>(Attribute::NUM_ATTRIBUTES)] =
{"type", "affect", "activation", "weight", "polarity", "direction", "age", "thresh", "decay", "mutability"};

/* so that Attribute can be used as an unordered_map key with c++11... thanks Sean  for the encouragment */
struct AttributeHash
{
    std::size_t operator()(const Attribute& a) const
    {
        return static_cast<std::size_t>(a);
    }
};


#endif /* Attribute_hpp */
