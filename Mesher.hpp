//
//  Mesher.hpp
//  LearningOpenGL
//
//  Created by onyx on 1/20/16.
//  Copyright © 2016 AweM. All rights reserved.
//

#ifndef Mesher_hpp
#define Mesher_hpp

#include <stdio.h>
#include <iostream>
#include "ShapeCreator.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

class Mesher{


    
public:
    void makeSphereMesh();
    void loadMesh(std::string inputFile, std::vector<tinyobj::shape_t>& theShapes, std::vector<tinyobj::material_t>& theMaterials);
    
    float* sphere_verts;
    int sphere_verts_size;
    int num_sphere_verts;
    int num_sphere_lines;
    
    
    std::vector<tinyobj::shape_t> shroomShapes;
    std::vector<tinyobj::material_t> shroomMaterials;
    
    std::vector<tinyobj::shape_t> centerCylinderShapes;
    std::vector<tinyobj::material_t> centerCylinderMaterials;
    
    std::vector<tinyobj::shape_t> muscleShapes;
    std::vector<tinyobj::material_t> muscleMaterials;
    
    std::vector<tinyobj::shape_t> petriDishShapes;
    std::vector<tinyobj::material_t> petriDishMaterials;
};
#endif /* Mesher_hpp */
