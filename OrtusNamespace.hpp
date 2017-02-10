//
//  OrtusNamespace.hpp
//  ortus
//
//  Created by andrew on 1/26/17.
//  Copyright © 2017 Andrew W.E. McDonald. All rights reserved.
//

#ifndef OrtusNamespace_h
#define OrtusNamespace_h

#include <unordered_map>
#include <vector>
#include "StandardVTKHeader.hpp"

namespace ortus {
    
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

#endif /* OrtusNamespace_h */
