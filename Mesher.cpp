//
//  Mesher.cpp
//  LearningOpenGL
//
//  Created by onyx on 1/20/16.
//  Copyright © 2016 AweM. All rights reserved.
//

#include "Mesher.hpp"



void Mesher::makeSphereMesh(){
    num_sphere_lines = 10;
    glm::vec3* sphere_verts_vec = ShapeCreator::genSphere(1.0f, num_sphere_lines, num_sphere_lines, &num_sphere_verts);
    sphere_verts_size = 6*num_sphere_verts;
    sphere_verts = new float[sphere_verts_size]; // [ x, y, z, norm_x, norm_y, norm_z]
    int count = 0;
    for (int i = 0; i < sphere_verts_size; i+=6){
        glm::vec3 temp_vec = sphere_verts_vec[count];
        count++;
        sphere_verts[i] = temp_vec.x;
        sphere_verts[i+1] = temp_vec.y;
        sphere_verts[i+2] = temp_vec.z;
        glm::vec3 norm_temp = glm::normalize(temp_vec);
        //sphere_verts[i+3] = ((i*i)%100)/100.0;
        //sphere_verts[i+4] = ((i*3*i*i)%100)/100.0;
        //sphere_verts[i+5] = ((i*i*7*i*i)%100)/100.0;
        
        sphere_verts[i+3] = norm_temp.x;
        sphere_verts[i+4] = norm_temp.y;
        sphere_verts[i+5] = norm_temp.z;//i%100)/100.0;
        
        /*
         sphere_verts[i+6] = 1.0f;
         sphere_verts[i+7] = 0.0f;
         sphere_verts[i+8] = 0.0f;//i%100)/100.0;
         sphere_verts[i+9] = 1.0f;
         */
        
    }
}


void Mesher::loadMesh(std::string inputFile, std::vector<tinyobj::shape_t>& theShapes, std::vector<tinyobj::material_t>& theMaterials){
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    
    std::string err;
    bool ret = tinyobj::LoadObj(shapes, materials, err, inputFile.c_str());
    
    if (!err.empty()) { // `err` may contain warning message.
        std::cerr << err << std::endl;
    }
    
    if (!ret) {
        exit(1);
    }
    
    std::cout << "# of shapes    : " << shapes.size() << std::endl;
    std::cout << "# of materials : " << materials.size() << std::endl;
    
    for (size_t i = 0; i < shapes.size(); i++) {
        printf("shape[%ld].name = %s\n", i, shapes[i].name.c_str());
        printf("Size of shape[%ld].indices: %ld\n", i, shapes[i].mesh.indices.size());
        printf("Size of shape[%ld].material_ids: %ld\n", i, shapes[i].mesh.material_ids.size());
        assert((shapes[i].mesh.indices.size() % 3) == 0);
        for (size_t f = 0; f < shapes[i].mesh.indices.size() / 3; f++) {
            printf("  idx[%ld] = %d, %d, %d. mat_id = %d\n", f, shapes[i].mesh.indices[3*f+0], shapes[i].mesh.indices[3*f+1], shapes[i].mesh.indices[3*f+2], shapes[i].mesh.material_ids[f]);
        }
        
        printf("shape[%ld].vertices: %ld\n", i, shapes[i].mesh.positions.size());
        assert((shapes[i].mesh.positions.size() % 3) == 0);
        for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
            printf("  v[%ld] = (%f, %f, %f)\n", v,
                   shapes[i].mesh.positions[3*v+0],
                   shapes[i].mesh.positions[3*v+1],
                   shapes[i].mesh.positions[3*v+2]);
        }
    }
    for (size_t i = 0; i < materials.size(); i++) {
        printf("material[%ld].name = %s\n", i, materials[i].name.c_str());
        printf("  material.Ka = (%f, %f ,%f)\n", materials[i].ambient[0], materials[i].ambient[1], materials[i].ambient[2]);
        printf("  material.Kd = (%f, %f ,%f)\n", materials[i].diffuse[0], materials[i].diffuse[1], materials[i].diffuse[2]);
        printf("  material.Ks = (%f, %f ,%f)\n", materials[i].specular[0], materials[i].specular[1], materials[i].specular[2]);
        printf("  material.Tr = (%f, %f ,%f)\n", materials[i].transmittance[0], materials[i].transmittance[1], materials[i].transmittance[2]);
        printf("  material.Ke = (%f, %f ,%f)\n", materials[i].emission[0], materials[i].emission[1], materials[i].emission[2]);
        printf("  material.Ns = %f\n", materials[i].shininess);
        printf("  material.Ni = %f\n", materials[i].ior);
        printf("  material.dissolve = %f\n", materials[i].dissolve);
        printf("  material.illum = %d\n", materials[i].illum);
        printf("  material.map_Ka = %s\n", materials[i].ambient_texname.c_str());
        printf("  material.map_Kd = %s\n", materials[i].diffuse_texname.c_str());
        printf("  material.map_Ks = %s\n", materials[i].specular_texname.c_str());
        printf("  material.map_Ns = %s\n", materials[i].specular_highlight_texname.c_str());
        std::map<std::string, std::string>::const_iterator it(materials[i].unknown_parameter.begin());
        std::map<std::string, std::string>::const_iterator itEnd(materials[i].unknown_parameter.end());
        for (; it != itEnd; it++) {
            printf("  material.%s = %s\n", it->first.c_str(), it->second.c_str());
        }
        printf("\n");
    }
    
    int num_cc_verts = shapes[0].mesh.positions.size();
    printf("%d\n", num_cc_verts);
    printf("oh boy...\n");
    theShapes = shapes;
    theMaterials = materials;
    
}