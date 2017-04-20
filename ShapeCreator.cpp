//
//  SphereCreator.cpp
//  LearningOpenGL
//
//  Created by onyx on 11/3/15.
//  Copyright © 2015 AweM. All rights reserved.
//

#include "ShapeCreator.hpp"


float to_radians(float deg){
    return ((deg/360.0f)* 2*M_PI);
}

glm::vec3* ShapeCreator::genSphere(float radius, int latitudes, int longitudes, int* num_vecs) {
    
    float latitude_increment = 360.0f / latitudes;
    float longitude_increment = 180.0f / longitudes;
    
    // if this causes an error, consider changing the size to [(latitude + 1)*(longitudes + 1)], but this should work.
    glm::vec3* vertices = new glm::vec3[2*((2+latitudes)*(2+longitudes))];
    std::vector<glm::vec3> all_verts;
    glm::vec3 saved1;
    glm::vec3 saved2;
    int counter = 0;
    float rad = radius;
            float x = (float) (rad * sin(to_radians(0)) * sin(to_radians(0)));
            float y = (float) (rad * sin(to_radians(0)) * cos(to_radians(0)));
            float z = (float) (rad * cos(to_radians(0)));
            
            vertices[0] = glm::vec3(x, y, z);
    counter++;
    //bool first = true;
    //bool second = true;
    
    for (float t = 0.0f; t <= (180.0f + longitude_increment); t += longitude_increment) {
        for (float u = 0.0f; u <= (360.0f + latitude_increment); u += latitude_increment) {
            float rad = radius;
            float x = (float) (rad * sin(to_radians(t)) * sin(to_radians(u)));
            float y = (float) (rad * sin(to_radians(t)) * cos(to_radians(u)));
            float z = (float) (rad * cos(to_radians(t)));
            
            vertices[counter++] = glm::vec3(x, y, z);
           
            x = (float) (rad * sin(to_radians(t+longitude_increment)) * sin(to_radians(u+latitude_increment)));
            y = (float) (rad * sin(to_radians(t+longitude_increment)) * cos(to_radians(u+latitude_increment)));
            z = (float) (rad * cos(to_radians(t+longitude_increment)));
            
            vertices[counter++] = glm::vec3(x, y, z);
 
        }
       
    }
    *num_vecs =  counter;
  
    return vertices;
    
}

glm::vec3* ShapeCreator::genMuscle(float rad_cap, float rad_mid, int latitudes, int longitudes, int* num_verts){
    
    float rad = 1.0f;//rad_mid;
    float height = 5.0f;
    float latitude_increment = 360.0f / latitudes;
    float longitude_increment = 180.0f / longitudes;
    float height_inc = height/latitudes;
    float h= height_inc;
    
    // if this causes an error, consider changing the size to [(latitude + 1)*(longitudes + 1)], but this should work.
    glm::vec3* vertices = new glm::vec3[6*((2+latitudes)*(2+longitudes))];
    std::vector<glm::vec3> all_verts;
    glm::vec3 saved1;
    glm::vec3 saved2;
    int counter = 0;
    //    float rad = radius;
    float x = (float) ((height - 0)/height)*rad*cos(0);
    float y = (float) ((height - 0)/height)*rad*sin(0);
    float z = 0.0f;
    
    vertices[0] = glm::vec3(x, y, z);
    counter++;
    //bool first = true;
    //bool second = true;
    int num_cyl = 5;
    int nc_count = num_cyl;
    float x_y_adjust = 0.0f;
    float t;
    for (t = 0.0f; t < num_cyl*height_inc; t += height_inc) {
        for (float u = 0.0f; u <= (360.0f + latitude_increment); u += latitude_increment) {
            
            
            float x = (float) rad*cos(to_radians(u));
            float y = (float) rad*sin(to_radians(u));
            float z = t;
            
            
            vertices[counter++] = glm::vec3(x, y, z);
            
            if (x_y_adjust > 0);
            x_y_adjust += height_inc;
            x = (float) rad*cos(to_radians(u));
            y = (float) rad*sin(to_radians(u));
            z = (t+height_inc);
            
            vertices[counter++] = glm::vec3(x, y, z);
            
        }
    }
    float height_adjust = t;
    for (t = 0; t < height; t += height_inc) {
        
        for (float u = 0.0f; u <= (360.0f + latitude_increment); u += latitude_increment) {
            
            
            float x = (float) ((height - t)/height)*rad*cos(to_radians(u));
            float y = (float) ((height - t)/height)*rad*sin(to_radians(u));
            float z = t + height_adjust;
            
            
            vertices[counter++] = glm::vec3(x, y, z);
            x = (float) ((height - (t+height_inc))/height)*rad*cos(to_radians(u));
            y = (float) ((height - (t+height_inc))/height)*rad*sin(to_radians(u));
            z = (t+height_inc+height_adjust);
            
            vertices[counter++] = glm::vec3(x, y, z);
            
        }
        
        
    }
    for (int i = 0; i < counter; i++) {
        vertices[counter+i] = glm::vec3(vertices[i].x, vertices[i].y,-vertices[i].z);
    }
    
    *num_verts =  2*counter;
    
    return vertices;
}
