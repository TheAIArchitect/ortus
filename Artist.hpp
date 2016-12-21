//
//  Artist.hpp
//  LearningOpenGL
//
//  Created by onyx on 11/23/15.
//  Copyright © 2015 AweM. All rights reserved.
//

#ifndef Artist_hpp
#define Artist_hpp

#include <stdio.h>



#include "Shader.hpp"
#include "Camera.hpp"
#include "MuscleInfoModule.hpp"
#include "TextMaker.hpp"
#include "Mesher.hpp"
#include "DataSteward.hpp"
#include "OptionForewoman.hpp"
#include "ConnectionComrade.hpp"


class Artist{
    
public:
    static float lblue[4];
    static float red[4];
    static float green[4];
    static float lpurple[4];
    static float lgrey[4];
    static float dgrey[4];
    static float white[4];
    static float orange[4];
    static float pink[4];
    static float dgreen[4];
    std::vector<std::string> neuron_names;
    
public:
    GLuint axes_vao;
    GLuint axes_vbo;
    GLuint myVAO;
    GLuint myVBO;
    GLuint sine_vao;
    GLuint sine_vbo;
    float axes_verts[18];
    float sine_verts[3000];
    int num_sine_verts;
    
    GLuint shroom_vao;
    GLuint shroom_index_buffer;
    GLuint shroom_vertex_buffer;
    GLuint shroom_normals_buffer;
    float* shroom_verts;
    int num_shroom_verts;
    unsigned int* shroom_indices;
    int num_shroom_indices;
    float* shroom_normals;
    int num_shroom_normals;
    
    // center cylinder (cc)
    GLuint cc_vao;
    GLuint cc_index_buffer;
    GLuint cc_vertex_buffer;
    GLuint cc_normals_buffer;
    float* cc_verts;
    int num_cc_verts;
    unsigned int* cc_indices;
    int num_cc_indices;
    float* cc_normals;
    int num_cc_normals;
    
    // muscles
    GLuint muscles_vao;
    GLuint muscles_index_buffer;
    GLuint muscles_vertex_buffer;
    GLuint muscles_normals_buffer;
    float* muscles_verts;
    int num_muscles_verts;
    unsigned int* muscles_indices;
    int num_muscles_indices;
    float* muscles_normals;
    int num_muscles_normals;
    
    
    // petri dish
    GLuint petriDish_vao;
    GLuint petriDish_index_buffer;
    GLuint petriDish_vertex_buffer;
    GLuint petriDish_normals_buffer;
    float* petriDish_verts;
    int num_petriDish_verts;
    unsigned int* petriDish_indices;
    int num_petriDish_indices;
    float* petriDish_normals;
    int num_petriDish_normals;
    
    TextMaker textMaker;
    Mesher mesher;
    
    GLuint bezVao;
    GLuint bezVbo;
    
    // connections
    GLuint connVao;
    GLuint connVbo;
    void prep_conns(int num_all_indices, int num_chem_indices, int num_gap_indices, float* all_conns);
    void draw_conns(Shader axesShader, Camera camera, glm::mat4* projection_ptr, glm::mat4* view_ptr);
    void draw_conn_weights(Shader textShader, ConnectionComrade* commie, Camera camera, glm::mat4* projection_ptr, glm::mat4* view_ptr);
    
    int numAllIndices;
    int numChemIndices;
    int numGapIndices;
    float* allConns;
    
    
    vector<vector<float>>* voltages;
    
public:
    Artist(DataSteward* fakeStewie);
    DataSteward* fakeStewie;
    ~Artist();

    void draw_sine(Shader axesShader, glm::mat4* projection_ptr, glm::mat4* view_ptr);
    void prep_sine();
    void prep_neurons();
    void draw_neurons(Shader neuronShader, glm::mat4* projection_ptr, glm::mat4* view_ptr, glm::mat4* model_ptr, Camera camera);
    void prep_axes();
    void draw_axes(Shader axesShader, glm::mat4* projection_ptr, glm::mat4* view_ptr);
    void float_to_color(float color_index, float* color);
    
    void draw_neuron_names(Shader textShader, Camera camera, glm::mat4* projection_ptr, glm::mat4* view_ptr);
    
    
    void prep_shroom();
    void draw_shroom(Shader shroom_shader,glm::mat4* projection_ptr, glm::mat4* view_ptr, Camera camera);
   
    void prep_petriDish();
    void draw_petriDish(Shader petriDish_shader,glm::mat4* projection_ptr, glm::mat4* view_ptr, Camera camera);
    
    
    void prep_center_cylinder();
    void draw_center_cylinder(Shader cc_shader,glm::mat4* projection_ptr, glm::mat4* view_ptr, Camera camera);
    
    
    void draw_muscles(Shader muscleShader, glm::mat4* projection_ptr, glm::mat4* view_ptr,  glm::mat4* model_ptr, Camera camera);
    void prep_muscles();
    
    
    void prep_bez();
    void draw_bez(Shader bez_shader,glm::mat4* projection_ptr, glm::mat4* view_ptr, Camera camera, MassPoint** bezDat, int numBezPts);
    
    glm::vec4 colorByActivation(float activation);
    
    
    
    
private:
    //int num_lines, num_vecs, neuron_positions_len;
};

#endif /* Artist_hpp */
