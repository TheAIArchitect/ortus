//
//  Artist.cpp
//  LearningOpenGL
//
//  Created by onyx on 11/23/15.
//  Copyright © 2016 Andrew W.E. McDonald. All rights reserved.
//

#include "Artist.hpp"




float Artist::lblue[] = {0.0f, 0.5f, 1.0f, 1.0f};
float Artist::red[] = {1.0f, 0.0f, 0.0f, 1.0f};
float Artist::green[] = {0.0f, 1.0f, 0.0f, 1.0f};
float Artist::lpurple[4] = {1.0f, 0.0f, 1.0f, 1.0f};
float Artist::dgrey[4] = {0.3f, 0.3f, 0.3f, 1.0f};
float Artist::lgrey[4] = {0.6f, 0.6f, 0.6f, 1.0f};
float Artist::white[4] = {1.0f, 1.0f, 1.0f, 1.0f};
float Artist::orange[4] = {1.0f, 0.4f, 0.0f, 1.0f};
float Artist::pink[4] = {1.0f, 0.078f, 0.576f, 1.0f};
float Artist::dgreen[4] = {0.0f, 0.5f, 0.0f, 1.0f};



Artist::Artist(DataSteward* fakeStewie){
    this->fakeStewie = fakeStewie;
    //textMaker = *new TextMaker();
    textMaker.prep_text();
}

Artist::~Artist(){
}



void Artist::draw_muscles(Shader muscleShader, glm::mat4* projection_ptr, glm::mat4* view_ptr,  glm::mat4* model_ptr, Camera camera){
    muscleShader.Use();
    glBindVertexArray(muscles_vao);
    GLint myViewLoc = glGetUniformLocation(muscleShader.Program, "view");
    GLint myProjLoc = glGetUniformLocation(muscleShader.Program, "projection");
    //GLint objectColorLoc = glGetUniformLocation(myShader.Program, "objectColor");
    GLint lightColorLoc  = glGetUniformLocation(muscleShader.Program, "lightColor");
    GLint lightPosLoc    = glGetUniformLocation(muscleShader.Program, "lightPos");
    GLint viewPosLoc     = glGetUniformLocation(muscleShader.Program, "viewPos");
    GLint colorLoc     = glGetUniformLocation(muscleShader.Program, "color");
    glUniform3f(lightColorLoc,  1.0f, 1.0f, 1.0f);
    glm::vec3 cameraEye = camera.getEye();
    glUniform3f(lightPosLoc,     cameraEye.x, cameraEye.y+1.0f, cameraEye.z-1.f);
    glUniform3f(viewPosLoc,     cameraEye.x, cameraEye.y, cameraEye.z);
    glUniformMatrix4fv(myViewLoc, 1, GL_FALSE, glm::value_ptr(*view_ptr));
    glUniformMatrix4fv(myProjLoc, 1, GL_FALSE, glm::value_ptr(*projection_ptr));
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //glDrawArrays(GL_TRIANGLE_FAN, 0, 12);
    
    GLint myModelLoc = glGetUniformLocation(muscleShader.Program, "model");
    // seems to work with 10, 10, and 22, 22 --> but not with 100,100 and 204, 204
    float verts_per_strip = 2.0f*(mesher.num_sphere_lines+2);
    //printf("num_lines, num_vecs, verts_per_strip: %d, %d, %f\n",num_lines, num_vecs, verts_per_strip);
    int color_step = 0;
    glm::mat4 pmodel(1.0f);
    ///////////////////////////////// NOTE //////////////////////////////
    //////// model_ptr IS NOT BEING USED!!!!
    //////////////////////////////////////////////////////////////////////
    //    for (int k = 0; k < neuron_positions_len; k+=5){
    float xshift = .01f;
    float zshift = .1f;
    for (int k = 0; k < 95; k+=1){
        /** this should be 'pushed' -- MatrixStack !!!! */
        glm::mat4 model(1.0f);
        //glm::mat4 model = glm::rotate(pmodel,(float) (-M_PI/2.0f),glm::vec3(0.0f,1.0f,0.0f));
        //model = glm::translate(model,glm::vec3(1.0f,0.0f,-1.0f));
        //model = glm::rotate(model,(float) (M_PI/2.0f),glm::vec3(1.0f,0.0f,0.0f));
        /* end push */
        int start = 0;
        //glm::vec3 cur = fakeStewie->mim[k]->centerMassPoint->center;
        //model = glm::translate(model,cur);
        
        
        
        if (!OptionForewoman::WormOpts[SHOW_MUSCLES] && !fakeStewie->mim[k]->marked){
            // if we don't want to show muscles, and it's not marked, then we don't want to show it
            continue;
        }
        int eid = fakeStewie->mim[k]->id();// we need to access the voltage array by the element_id, **NOT** by k.
        
        model = glm::scale(model, MuscleInfoModule::MUSCLE_SIZE_SCALE*glm::vec3(1.f, 1.f, MuscleInfoModule::length_z_scale));
        model = glm::translate(model,(MuscleInfoModule::MUSCLE_TRANS_SCALE*fakeStewie->mim[k]->centerMassPoint->center)*glm::vec3(1.f, 1.f,MuscleInfoModule::trans_z_scale));// 2.5 is half of the night of the muscle model (i think)
        model = glm::translate(model,glm::vec3(0.f, -2.5f, .5f));
       
        float currentActivation = (*voltages)[Camera::voltageFrame][eid];
        glm::vec4 currentColor = colorByActivation(25.f*(*voltages)[Camera::voltageFrame][eid]);
        glUniform4f(colorLoc,currentColor[0],currentColor[1],currentColor[2],currentColor[3]);
        
       
        glUniformMatrix4fv(myModelLoc, 1, GL_FALSE, glm::value_ptr(model));
        
        glDrawElements(GL_TRIANGLES, num_muscles_indices, GL_UNSIGNED_INT, NULL);
    }
    
    //glDrawArrays(GL_TRIANGLE_STRIP,1, 202);
    //glDrawArrays(GL_TRIANGLE_STRIP,203, 202);
    //glDrawArrays(GL_TRIANGLE_STRIP,409, 204);
    
    glBindVertexArray(0);
}



void Artist::prep_muscles(){
    mesher.muscleShapes;
    mesher.muscleMaterials;
    mesher.loadMesh(FileAssistant::muscleObjFile, mesher.muscleShapes, mesher.muscleMaterials);
    num_muscles_verts = mesher.muscleShapes[0].mesh.positions.size();
    num_muscles_indices = mesher.muscleShapes[0].mesh.indices.size();
    num_muscles_normals = mesher.muscleShapes[0].mesh.normals.size();
    muscles_verts = new float[num_muscles_verts*2];
    muscles_indices = new unsigned int[num_muscles_indices];
    muscles_normals = new float[num_muscles_normals];
    // USED FOR FINDING END POINTS OF MUSCLES
    /*
    glm::vec3 min(100.0f);
    glm::vec3 max(-100.0f);
    glm::vec3 temp(0.0f);
    for(int i = 0; i < num_muscles_verts/3; i++){
        float x = mesher.muscleShapes[0].mesh.positions[i*3];
        float y = mesher.muscleShapes[0].mesh.positions[i*3 + 1];
        float z = mesher.muscleShapes[0].mesh.positions[i*3 + 2];
        temp = glm::vec3(x, y, z);
        if (temp.z < min.z && temp.x == temp.y == 0){
            min = temp;
        }
        if (temp.z > max.z && temp.x == temp.y == 0){
            max = temp;
        }
    }
    printf("min, max -> %f, %f, %f to %f, %f, %f\n",min.x, min.y, min.z, max.x, max.y, max.z);
    exit(0);
    */
    for(int i = 0; i < num_muscles_verts; i++){
        muscles_verts[i] = mesher.muscleShapes[0].mesh.positions[i];
    
    }
    for(int i = 0; i < num_muscles_normals; i++){
        muscles_verts[num_muscles_verts+i] = mesher.muscleShapes[0].mesh.normals[i];
    }
    
    for(int i = 0; i < num_muscles_indices; i++){
        muscles_indices[i] = mesher.muscleShapes[0].mesh.indices[i];
    }
    
    
    glGenVertexArrays(1, &muscles_vao);
    glBindVertexArray(muscles_vao);
    
    // vertices
    glGenBuffers(1, &muscles_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, muscles_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, 2*num_muscles_verts*sizeof(GL_FLOAT), muscles_verts, GL_STATIC_DRAW);
    // normals
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3, GL_FLOAT, GL_FALSE,0,0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3, GL_FLOAT, GL_FALSE,0,(GLvoid*)(num_muscles_verts*sizeof(GL_FLOAT)));
    
    //indices
    glGenBuffers(1, &muscles_index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, muscles_index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_muscles_indices*sizeof(GL_FLOAT), muscles_indices, GL_STATIC_DRAW);
    
    glBindVertexArray(0);
    
}

/**
 * Neurons

void Artist::prep_neurons(){
    //glm::vec4* neuron_positions = new glm::vec4[302];
    //glm::vec4* neuron_positions =(glm::vec4<float>) malloc(302*sizeof(glm::vec4<float>));

    mesher.makeSphereMesh();
    int num_neuron_color_array_indices = 4*fakeStewie->num_neurons_in_json;
    fakeStewie->neuron_colors = new float[num_neuron_color_array_indices];
    int new_color_offset = 0;
    for (int i = 0; i < fakeStewie->num_neurons_in_json; i++){
        // 4, 9, 14, et....
        float_to_color(fakeStewie->nim[i]->getFType(), &(fakeStewie->neuron_colors[i*4]));
        //new_color_offset += 4;
    }
    
    
    
    // get neuron_colors
    glGenVertexArrays(1, &myVAO);
    glGenBuffers(1, &myVBO);
    glBindVertexArray(myVAO);
    glBindBuffer(GL_ARRAY_BUFFER, myVBO);
    glBufferData(GL_ARRAY_BUFFER, mesher.sphere_verts_size*sizeof(float), mesher.sphere_verts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), 0);
    //glEnableVertexAttribArray(1);
    //glVertexAttribPointer(1,4, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat),(GLvoid*) (6*sizeof(GLfloat)));
    // normals -- but sphere was built centered at origin... so, normals are just the normalized points, i think.
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (GLvoid*) (3*sizeof(GLfloat)));
    glBindVertexArray(0);
}
*/
/*
void Artist::draw_neurons(Shader neuronShader, glm::mat4* projection_ptr, glm::mat4* view_ptr,  glm::mat4* model_ptr, Camera camera){
    
    neuronShader.Use();
    
    GLint myViewLoc = glGetUniformLocation(neuronShader.Program, "view");
    GLint myProjLoc = glGetUniformLocation(neuronShader.Program, "projection");
    //GLint objectColorLoc = glGetUniformLocation(myShader.Program, "objectColor");
    GLint lightColorLoc  = glGetUniformLocation(neuronShader.Program, "lightColor");
    GLint lightPosLoc    = glGetUniformLocation(neuronShader.Program, "lightPos");
    GLint viewPosLoc     = glGetUniformLocation(neuronShader.Program, "viewPos");
    GLint colorLoc     = glGetUniformLocation(neuronShader.Program, "color");
    //glUniform3f(objectColorLoc, 1.0f, 0.5f, 0.31f);
    glUniform3f(lightColorLoc,  1.0f, 1.0f, 1.0f);
    //        glUniform3f(lightPosLoc,    lightPos.x, lightPos.y, lightPos.z);
    glm::vec3 cameraEye = camera.getEye();
    glUniform3f(lightPosLoc,     cameraEye.x, cameraEye.y+1.0f, cameraEye.z-1.f);
    glUniform3f(viewPosLoc,     cameraEye.x, cameraEye.y, cameraEye.z);
    glUniformMatrix4fv(myViewLoc, 1, GL_FALSE, glm::value_ptr(*view_ptr));
    glUniformMatrix4fv(myProjLoc, 1, GL_FALSE, glm::value_ptr(*projection_ptr));
    glBindVertexArray(myVAO);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //glDrawArrays(GL_TRIANGLE_FAN, 0, 12);
    
    
    GLint myModelLoc = glGetUniformLocation(neuronShader.Program, "model");
    // seems to work with 10, 10, and 22, 22 --> but not with 100,100 and 204, 204
    float verts_per_strip = 2.0f*(mesher.num_sphere_lines+2);
    int num_strips = mesher.num_sphere_verts/verts_per_strip;
    //printf("num_lines, num_vecs, verts_per_strip: %d, %d, %f\n",num_lines, num_vecs, verts_per_strip);
    int num_verts = (int) verts_per_strip;
    int color_step = 0;
    glm::mat4 pmodel(1.0f);
    ///////////////////////////////// NOTE //////////////////////////////
    //////// model_ptr IS NOT BEING USED!!!!
    //////////////////////////////////////////////////////////////////////
    for (int k = 0; k < fakeStewie->num_neuron_mass_points; k++){
        /////////// this should be 'pushed' -- MatrixStack !!!!
        //glm::mat4 model = glm::rotate(pmodel,(float) (-M_PI/2.0f),glm::vec3(0.0f,1.0f,0.0f));
        //model = glm::rotate(model,(float) (M_PI/2.0f),glm::vec3(1.0f,0.0f,0.0f));
        glm::mat4 model(1.0f);
        /////////// end push
        int start = 0;
        float scale1 = NeuronInfoModule::NEURON_SIZE_SCALE;
        float scale2 = NeuronInfoModule::NEURON_TRANS_SCALE;
        if (!OptionForewoman::WormOpts[SHOW_ALL_NEURONS] && !fakeStewie->nim[k]->marked){
            // if we don't want to show all neurons, and it's not marked, then we don't want to show it
            continue;
        }
        int eid = fakeStewie->nim[k]->id();// we need to access the voltage array by the element_id, **NOT** by k.
        float diam = fakeStewie->nim[k]->soma_diameter;
        model = glm::translate(model,(scale2*fakeStewie->nim[k]->massPoint->center));
        model = glm::scale(model, (scale1*glm::vec3(diam/2.0f, diam/2.0f, diam/2.0f)));
        //printf("color: %f, %f, %f, %f\n", neuron_colors[color_step],neuron_colors[color_step+1],neuron_colors[color_step+2],neuron_colors[color_step+3]);
        //glUniform4f(colorLoc,fakeStewie->neuron_colors[color_step],fakeStewie->neuron_colors[color_step+1],fakeStewie->neuron_colors[color_step+2],fakeStewie->neuron_colors[color_step+3]);
        //printf("frame: %d\n",Camera::voltageFrame);
        //glUniform4f(colorLoc,(*voltages)[Camera::voltageFrame][k],(*voltages)[Camera::voltageFrame][k],(*voltages)[Camera::voltageFrame][k],1.0f);
        float currentActivation = (*voltages)[Camera::voltageFrame][eid];

        glm::vec4 currentColor = colorByActivation((*voltages)[Camera::voltageFrame][eid]);
        glUniform4f(colorLoc,currentColor[0],currentColor[1],currentColor[2],currentColor[3]);
        color_step += 4;
        glUniformMatrix4fv(myModelLoc, 1, GL_FALSE, glm::value_ptr(model));
        for(int i = 0; i < num_strips; i++  ){
            
            glDrawArrays(GL_TRIANGLE_STRIP,start, num_verts);
            start += num_verts;
        }
 
    }
    
    //glDrawArrays(GL_TRIANGLE_STRIP,1, 202);
    //glDrawArrays(GL_TRIANGLE_STRIP,203, 202);
    //glDrawArrays(GL_TRIANGLE_STRIP,409, 204);
    
    glBindVertexArray(0);
}
*/
/**
 * Axes
 */

void Artist::prep_axes(){
    float axes_verts[18];
    for(int i = 0; i < 18; i += 6){
        //origin
        axes_verts[i] = 0.0f;
        axes_verts[i+1] = 0.0f;
        axes_verts[i+2] = 0.0f;
        if (i == 0) {// x -- to the right, red
            axes_verts[i+3] = 10.0f;
            axes_verts[i+4] = 0.0f;
            axes_verts[i+5] = 0.0f;
        }
        else if (i == 6){// y -- up, green
            axes_verts[i+3] = 0.0f;
            axes_verts[i+4] = 10.0f;
            axes_verts[i+5] = 0.0f;
        }
        else if (i == 12){// z, blue
            axes_verts[i+3] = 0.0f;
            axes_verts[i+4] = 0.0f;
            axes_verts[i+5] = 10.0f;
        }
    }
    glGenVertexArrays(1, &axes_vao);
    glGenBuffers(1, &axes_vbo);
    glBindVertexArray(axes_vao);
    glBindBuffer(GL_ARRAY_BUFFER, axes_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axes_verts), axes_verts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3, GL_FLOAT, GL_FALSE,3*sizeof(GLfloat),0);
    glBindVertexArray(0);
    
}

void Artist::draw_axes(Shader axesShader,  glm::mat4* projection_ptr, glm::mat4* view_ptr){
    
    axesShader.Use();
    
    glBindVertexArray(axes_vao);
    glLineWidth(10.0f);
    GLint axes_color_loc = glGetUniformLocation(axesShader.Program,"color");
    GLint axes_view_loc = glGetUniformLocation(axesShader.Program,"view");
    GLint axes_proj_loc = glGetUniformLocation(axesShader.Program,"projection");
        GLint axes_model_loc = glGetUniformLocation(axesShader.Program,"model");
    //GLint axes_model_loc = glGetUniformLocation(axesShader.Program,"model");
    glUniformMatrix4fv(axes_view_loc, 1, GL_FALSE, glm::value_ptr(*view_ptr));
    glUniformMatrix4fv(axes_proj_loc, 1, GL_FALSE, glm::value_ptr(*projection_ptr));
    glm::mat4 model(1.0f);
    glUniformMatrix4fv(axes_model_loc, 1, GL_FALSE, glm::value_ptr(model));
    //glUniformMatrix4fv(axes_model_loc, 1, GL_FALSE, glm::value_ptr(model));
    glUniform4f(axes_color_loc,Artist::red[0], Artist::red[1], Artist::red[2], Artist::red[3]);
    glDrawArrays(GL_LINES, 0,2);
    glUniform4f(axes_color_loc,Artist::green[0], Artist::green[1], Artist::green[2], Artist::green[3]);
    glDrawArrays(GL_LINES, 2,2);
    glUniform4f(axes_color_loc,Artist::lblue[0], Artist::lblue[1], Artist::lblue[2], Artist::lblue[3]);
    glDrawArrays(GL_LINES, 4,2);
    glBindVertexArray(0);
}

int num_motors = 0;
void Artist::float_to_color(float color_index, float* color){
    int color_index_int = (int) color_index;
    printf("float, int: %f, %d\n", color_index, color_index_int);
    
    //memcpy(color, test1, 4*sizeof(float));
    //return;
    switch(color_index_int){
        case(0): // inter
            memcpy(color, Artist::lblue, 4*sizeof(float));
            break;
        case(1): // sensory
            memcpy(color, Artist::green, 4*sizeof(float));
            break;
        case(2): // motor
            num_motors++;
            printf("NUM MOTORS: %d\n",num_motors);
            memcpy(color, Artist::red, 4*sizeof(float));
            break;
        case(3): // poly
            memcpy(color, Artist::lpurple, 4*sizeof(float));
            break;
        case(4): // wfk
            memcpy(color, Artist::lgrey, 4*sizeof(float));
            break;
        default:
            memcpy(color, Artist::white, 4*sizeof(float));
            break;
            
    }
    
}

/**
 * sine
 */

void Artist::prep_sine(){
    num_sine_verts = 1000; //
    float sine_verts[num_sine_verts*3];
    float div = 5.0f/1000.0f;
    float cur_x = -2.0f;
    float coeff = .2;
    for(int i = 0; i < 3*num_sine_verts; i+=3){
        //origin
        sine_verts[i] = cur_x;
        sine_verts[i+1] = coeff*sin(.3*cur_x*2*M_PI);
        sine_verts[i+2] = 0.0f;
        cur_x += div;
        coeff+=.001;
    }
    glGenVertexArrays(1, &sine_vao);
    glGenBuffers(1, &sine_vbo);
    glBindVertexArray(sine_vao);
    glBindBuffer(GL_ARRAY_BUFFER, sine_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sine_verts), sine_verts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3, GL_FLOAT, GL_FALSE,3*sizeof(GLfloat),0);
    glBindVertexArray(0);
    
}

void Artist::draw_sine(Shader axesShader,  glm::mat4* projection_ptr, glm::mat4* view_ptr){
    
    axesShader.Use();
    
    glBindVertexArray(sine_vao);
    glLineWidth(10.0f);
    GLint axes_color_loc = glGetUniformLocation(axesShader.Program,"color");
    GLint axes_view_loc = glGetUniformLocation(axesShader.Program,"view");
    GLint axes_proj_loc = glGetUniformLocation(axesShader.Program,"projection");
    GLint axes_model_loc = glGetUniformLocation(axesShader.Program,"model");
    //GLint axes_model_loc = glGetUniformLocation(axesShader.Program,"model");
    glUniformMatrix4fv(axes_view_loc, 1, GL_FALSE, glm::value_ptr(*view_ptr));
    glUniformMatrix4fv(axes_proj_loc, 1, GL_FALSE, glm::value_ptr(*projection_ptr));
    glm::mat4 model(1.0f);
    glUniformMatrix4fv(axes_model_loc, 1, GL_FALSE, glm::value_ptr(model));
    //glUniformMatrix4fv(axes_model_loc, 1, GL_FALSE, glm::value_ptr(model));
    glUniform4f(axes_color_loc,Artist::red[0], Artist::red[1], Artist::red[2], Artist::red[3]);
    glDrawArrays(GL_LINE_STRIP, 0,num_sine_verts);
    glBindVertexArray(0);
}

void Artist::draw_neuron_names(Shader textShader, Camera camera, glm::mat4* projection_ptr, glm::mat4* view_ptr){
    
    glUseProgram(textShader.Program);
    for (int i = 0; i < fakeStewie->NUM_ELEMS; ++i){
        glm::mat4 billboard_mat(1.0f);
        glm::vec3 pos;
        if (fakeStewie->elements[i]->getEType() == MUSCLE){
            if (!OptionForewoman::WormOpts[SHOW_MUSCLES] && !fakeStewie->elements[i]->marked){
                continue;
            }
            MuscleInfoModule* tmim = (MuscleInfoModule*)fakeStewie->elements[i];
            pos = tmim->centerMassPoint->center;
            
            billboard_mat = glm::scale(billboard_mat, MuscleInfoModule::MUSCLE_SIZE_SCALE*glm::vec3(1.f, 1.f, MuscleInfoModule::length_z_scale));
            billboard_mat = glm::translate(billboard_mat,(MuscleInfoModule::MUSCLE_TRANS_SCALE*pos)*glm::vec3(1.f, 1.f,MuscleInfoModule::trans_z_scale));// 2.5 is half of the night of the muscle model (i think)
            billboard_mat = glm::translate(billboard_mat,glm::vec3(0.f, -2.5f, .5f));
            
            //billboard_mat = glm::translate(billboard_mat,(pos*MuscleInfoModule::MUSCLE_TRANS_SCALE));
            
            //billboard_mat = glm::translate(billboard_mat, pos); // translate and then offset from center of neuron
            //billboard_mat = glm::translate(billboard_mat, pos+(MuscleInfoModule::MUSCLE_SIZE_SCALE*2.f/2.f)); // translate and then offset from center of neuron
        }
        else {
            if (!OptionForewoman::WormOpts[SHOW_ALL_NEURONS] && !fakeStewie->elements[i]->marked){
                continue;
            }
            NeuronInfoModule* tnim = (NeuronInfoModule*)fakeStewie->elements[i];
            glm::vec3 pos = tnim->massPoint->center;// - (nim[i].soma_diameter/2.0f);
            billboard_mat = glm::translate(billboard_mat, ((NeuronInfoModule::NEURON_TRANS_SCALE*pos) + (NeuronInfoModule::NEURON_SIZE_SCALE*tnim->soma_diameter/2.f))); // translate and then offset from center of neuron
        }
        billboard_mat = glm::rotate(billboard_mat, camera.m_theta, glm::vec3(0,1,0));
        billboard_mat = glm::rotate(billboard_mat, camera.m_phi - ((float)M_PI/2.f), glm::vec3(1,0,0));
        //billboard_mat = glm::scale(billboard_mat, (.1f/NEURON_SIZE_SCALE*glm::vec3(1.f, 1.f, 1.f)));
        glUniformMatrix4fv(glGetUniformLocation(textShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(*projection_ptr));
        glUniformMatrix4fv(glGetUniformLocation(textShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(*view_ptr));
        glUniformMatrix4fv(glGetUniformLocation(textShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(billboard_mat));
        //printf("creating text: %s\n",fakeStewie->nim[i]->name.c_str());
        std::string label = "";
        if (OptionForewoman::WormOpts[SHOW_ACTIVATION]){
            // get activation
            int eid = fakeStewie->elements[i]->id();// we need to access the voltage array by the element_id, **NOT** by k.
            float currentActivation = (*voltages)[Camera::voltageFrame][eid];
            label =fakeStewie->elements[i]->graphicalName + ": "+std::to_string(currentActivation);
        }
        else {
            label = fakeStewie->elements[i]->graphicalName;
        }
        if (fakeStewie->elements[i]->getEType() == MUSCLE){ // maybe we want a different scale or something for muscle labels
            textMaker.make_text(textShader, label, pos, .01f/MuscleInfoModule::MUSCLE_TRANS_SCALE, glm::vec3(1.0f, 1.0f, 1.0f));
        }
        else{
            textMaker.make_text(textShader, label, pos, .5f/NeuronInfoModule::NEURON_TRANS_SCALE, glm::vec3(1.0f, 1.0f, 1.0f));
        }
        //textMaker.make_text(textShader, label, pos, 1.f, glm::vec3(1.0f, 1.0f, 1.0f));
    }
}

void Artist::prep_center_cylinder(){
    mesher.centerCylinderShapes;
    mesher.centerCylinderMaterials;
    mesher.loadMesh(FileAssistant::centerCylinderObjFile, mesher.centerCylinderShapes, mesher.centerCylinderMaterials);
    num_cc_verts = mesher.centerCylinderShapes[0].mesh.positions.size();
    num_cc_indices = mesher.centerCylinderShapes[0].mesh.indices.size();
    num_cc_normals = mesher.centerCylinderShapes[0].mesh.normals.size();
    cc_verts = new float[num_cc_verts*2];
    cc_indices = new unsigned int[num_cc_indices];
    cc_normals = new float[num_cc_normals];
    /*
    glm::vec3 min(100.0f);
    glm::vec3 max(-100.0f);
    glm::vec3 temp(0.0f);
    for(int i = 0; i < num_cc_verts/3; i++){
        float x = mesher.centerCylinderShapes[0].mesh.positions[i*3];
        float y = mesher.centerCylinderShapes[0].mesh.positions[i*3 + 1];
        float z = mesher.centerCylinderShapes[0].mesh.positions[i*3 + 2];
        temp = glm::vec3(x, y, z);
        //if (temp.z < min.z){
        //    min = temp;
        //}
        //if (temp.z > max.z){
        //    max = temp;
        //}
        
        
    }
    printf("min, max -> %f, %f, %f to %f, %f, %f\n",min.x, min.y, min.z, max.x, max.y, max.z);
    */
    
    
    for(int i = 0; i < num_cc_verts; i++){
        cc_verts[i] = mesher.centerCylinderShapes[0].mesh.positions[i];
    }
    for(int i = 0; i < num_cc_normals; i++){
        cc_verts[num_cc_verts+i] = mesher.centerCylinderShapes[0].mesh.normals[i];
    }
    
    for(int i = 0; i < num_cc_indices; i++){
        cc_indices[i] = mesher.centerCylinderShapes[0].mesh.indices[i];
    }
    
    
    glGenVertexArrays(1, &cc_vao);
    glBindVertexArray(cc_vao);
    
    // vertices
    glGenBuffers(1, &cc_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, cc_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, 2*num_cc_verts*sizeof(GL_FLOAT), cc_verts, GL_STATIC_DRAW);
    // normals
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3, GL_FLOAT, GL_FALSE,0,0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3, GL_FLOAT, GL_FALSE,0,(GLvoid*)(num_cc_verts*sizeof(GL_FLOAT)));
    
    //indices
    glGenBuffers(1, &cc_index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cc_index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_cc_indices*sizeof(GL_FLOAT), cc_indices, GL_STATIC_DRAW);
    
    glBindVertexArray(0);
    
}

/*
void Artist::draw_center_cylinder(Shader cc_shader,glm::mat4* projection_ptr, glm::mat4* view_ptr, Camera camera){
    cc_shader.Use();
    glBindVertexArray(cc_vao);
    GLint myViewLoc = glGetUniformLocation(cc_shader.Program, "view");
    GLint myProjLoc = glGetUniformLocation(cc_shader.Program, "projection");
    GLint lightColorLoc  = glGetUniformLocation(cc_shader.Program, "lightColor");
    GLint lightPosLoc    = glGetUniformLocation(cc_shader.Program, "lightPos");
    GLint viewPosLoc     = glGetUniformLocation(cc_shader.Program, "viewPos");
    GLint colorLoc     = glGetUniformLocation(cc_shader.Program, "color");
    GLint myModelLoc = glGetUniformLocation(cc_shader.Program, "model");
    glUniform3f(lightColorLoc,  1.0f, 1.0f, 1.0f);
    glm::vec3 cameraEye = camera.getEye();
    glUniform3f(lightPosLoc,     cameraEye.x+5.0f, cameraEye.y+5.0f, cameraEye.z+5.0f);
    glUniform3f(viewPosLoc,     cameraEye.x, cameraEye.y, cameraEye.z);
    glUniformMatrix4fv(myViewLoc, 1, GL_FALSE, glm::value_ptr(*view_ptr));
    glUniformMatrix4fv(myProjLoc, 1, GL_FALSE, glm::value_ptr(*projection_ptr));
    glUniform4f(colorLoc,1.0f, 0.0f,0.0f,1.0f);
    for (int i = 0; i < fakeStewie->NUM_CC; i++){
        glm::mat4 model(1.0f);
        //model = glm::scale(model, glm::vec3(.1, .1,.1));
        model = glm::translate(model, fakeStewie->centerCylinderMassPoints[3*i+1]->center);
        //model = glm::scale(model, glm::vec3(10,10,10));
        glUniformMatrix4fv(myModelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawElements(GL_TRIANGLES, num_cc_indices, GL_UNSIGNED_INT, NULL);
    }
}
*/

void Artist::prep_petriDish(){
    mesher.loadMesh(FileAssistant::petriDishObjFile, mesher.petriDishShapes, mesher.petriDishMaterials);
    num_petriDish_verts = mesher.petriDishShapes[0].mesh.positions.size();
    num_petriDish_indices = mesher.petriDishShapes[0].mesh.indices.size();
    num_petriDish_normals = mesher.petriDishShapes[0].mesh.normals.size();
    petriDish_verts = new float[num_petriDish_verts*2];
    petriDish_indices = new unsigned int[num_petriDish_indices];
    petriDish_normals = new float[num_petriDish_normals];
    for(int i = 0; i < num_petriDish_verts; i++){
        petriDish_verts[i] = mesher.petriDishShapes[0].mesh.positions[i];
    }
    for(int i = 0; i < num_petriDish_normals; i++){
        petriDish_verts[num_petriDish_verts+i] = mesher.petriDishShapes[0].mesh.normals[i];
    }
    
    for(int i = 0; i < num_petriDish_indices; i++){
        petriDish_indices[i] = mesher.petriDishShapes[0].mesh.indices[i];
    }
    
    
    glGenVertexArrays(1, &petriDish_vao);
    glBindVertexArray(petriDish_vao);
    
    // vertices
    glGenBuffers(1, &petriDish_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, petriDish_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, 2*num_petriDish_verts*sizeof(GL_FLOAT), petriDish_verts, GL_STATIC_DRAW);
    // normals
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3, GL_FLOAT, GL_FALSE,0,0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3, GL_FLOAT, GL_FALSE,0,(GLvoid*)(num_petriDish_verts*sizeof(GL_FLOAT)));
    
    //indices
    glGenBuffers(1, &petriDish_index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, petriDish_index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_petriDish_indices*sizeof(GL_FLOAT), petriDish_indices, GL_STATIC_DRAW);
    
    glBindVertexArray(0);
    
}

void Artist::draw_petriDish(Shader petriDish_shader,glm::mat4* projection_ptr, glm::mat4* view_ptr, Camera camera){
    petriDish_shader.Use();
    glBindVertexArray(petriDish_vao);
    GLint myViewLoc = glGetUniformLocation(petriDish_shader.Program, "view");
    GLint myProjLoc = glGetUniformLocation(petriDish_shader.Program, "projection");
    GLint lightColorLoc  = glGetUniformLocation(petriDish_shader.Program, "lightColor");
    GLint lightPosLoc    = glGetUniformLocation(petriDish_shader.Program, "lightPos");
    GLint viewPosLoc     = glGetUniformLocation(petriDish_shader.Program, "viewPos");
    GLint colorLoc     = glGetUniformLocation(petriDish_shader.Program, "color");
    GLint myModelLoc = glGetUniformLocation(petriDish_shader.Program, "model");
    glUniform3f(lightColorLoc,  1.0f, 1.0f, 1.0f);
    
    glm::vec3 cameraEye = camera.getEye();
    glUniform3f(lightPosLoc,     cameraEye.x+5.0f, cameraEye.y+5.0f, cameraEye.z+5.0f);
    glUniform3f(viewPosLoc,     cameraEye.x, cameraEye.y, cameraEye.z);
    glUniformMatrix4fv(myViewLoc, 1, GL_FALSE, glm::value_ptr(*view_ptr));
    glUniformMatrix4fv(myProjLoc, 1, GL_FALSE, glm::value_ptr(*projection_ptr));
    glUniform4f(colorLoc,0.5294f, 0.8078f,0.9803f,.7f);// 135, 206, 250
    glm::mat4 model(1.0f);
    model = glm::scale(model, 5.0f*glm::vec3(1.0f));
    model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
    glUniformMatrix4fv(myModelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glDrawElements(GL_TRIANGLES, num_petriDish_indices, GL_UNSIGNED_INT, NULL);
}



void Artist::prep_shroom(){
    mesher.loadMesh(FileAssistant::mushroomObjFile, mesher.shroomShapes, mesher.shroomMaterials);
    num_shroom_verts = mesher.shroomShapes[0].mesh.positions.size();
    num_shroom_indices = mesher.shroomShapes[0].mesh.indices.size();
    num_shroom_normals = mesher.shroomShapes[0].mesh.normals.size();
    shroom_verts = new float[num_shroom_verts*2];
    shroom_indices = new unsigned int[num_shroom_indices];
    shroom_normals = new float[num_shroom_normals];
    //num_shroom_verts = 3;
    //num_shroom_indices = 3;
    for(int i = 0; i < num_shroom_verts; i++){
        shroom_verts[i] = mesher.shroomShapes[0].mesh.positions[i];
    }
    for(int i = 0; i < num_shroom_normals; i++){
        shroom_verts[num_shroom_verts+i] = mesher.shroomShapes[0].mesh.normals[i];
    }
    
    for(int i = 0; i < num_shroom_indices; i++){
        shroom_indices[i] = mesher.shroomShapes[0].mesh.indices[i];
    }
    
    
    glGenVertexArrays(1, &shroom_vao);
    glBindVertexArray(shroom_vao);
    
    // vertices
    glGenBuffers(1, &shroom_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, shroom_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, 2*num_shroom_verts*sizeof(GL_FLOAT), shroom_verts, GL_STATIC_DRAW);
    // normals
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3, GL_FLOAT, GL_FALSE,0,0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3, GL_FLOAT, GL_FALSE,0,(GLvoid*)(num_shroom_verts*sizeof(GL_FLOAT)));
    
    //indices
    glGenBuffers(1, &shroom_index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shroom_index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_shroom_indices*sizeof(GL_FLOAT), shroom_indices, GL_STATIC_DRAW);
    
    glBindVertexArray(0);
    
}





void Artist::draw_shroom(Shader shroom_shader,glm::mat4* projection_ptr, glm::mat4* view_ptr, Camera camera){
    shroom_shader.Use();
    glBindVertexArray(shroom_vao);
    GLint myViewLoc = glGetUniformLocation(shroom_shader.Program, "view");
    GLint myProjLoc = glGetUniformLocation(shroom_shader.Program, "projection");
    GLint lightColorLoc  = glGetUniformLocation(shroom_shader.Program, "lightColor");
    GLint lightPosLoc    = glGetUniformLocation(shroom_shader.Program, "lightPos");
    GLint viewPosLoc     = glGetUniformLocation(shroom_shader.Program, "viewPos");
    GLint colorLoc     = glGetUniformLocation(shroom_shader.Program, "color");
    GLint myModelLoc = glGetUniformLocation(shroom_shader.Program, "model");
    glUniform3f(lightColorLoc,  1.0f, 1.0f, 1.0f);
    glm::vec3 cameraEye = camera.getEye();
    glUniform3f(lightPosLoc,     cameraEye.x+5.0f, cameraEye.y+5.0f, cameraEye.z+5.0f);
    glUniform3f(viewPosLoc,     cameraEye.x, cameraEye.y, cameraEye.z);
    glUniformMatrix4fv(myViewLoc, 1, GL_FALSE, glm::value_ptr(*view_ptr));
    glUniformMatrix4fv(myProjLoc, 1, GL_FALSE, glm::value_ptr(*projection_ptr));
    glUniform4f(colorLoc,1.0f, 0.0f,0.0f,1.0f);
    glm::mat4 model(1.0f);
    glUniformMatrix4fv(myModelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glDrawElements(GL_TRIANGLES, num_shroom_indices, GL_UNSIGNED_INT, NULL);
    //glDrawArrays(GL_TRIANGLES, 0, 3);
}

void Artist::prep_bez(){
    
    // get neuron_colors
    glGenVertexArrays(1, &bezVao);
    glGenBuffers(1, &bezVbo);
    glBindVertexArray(bezVao);
    glBindBuffer(GL_ARRAY_BUFFER, bezVbo);
    glBufferData(GL_ARRAY_BUFFER, mesher.sphere_verts_size*sizeof(float), mesher.sphere_verts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), 0);
    //glEnableVertexAttribArray(1);
    //glVertexAttribPointer(1,4, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat),(GLvoid*) (6*sizeof(GLfloat)));
    // normals -- but sphere was built centered at origin... so, normals are just the normalized points, i think.
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (GLvoid*) (3*sizeof(GLfloat)));
    glBindVertexArray(0);
}

void Artist::draw_bez(Shader bez_shader,glm::mat4* projection_ptr, glm::mat4* view_ptr, Camera camera, MassPoint** bezDat, int numBezPts){
    bez_shader.Use();
    glBindVertexArray(bezVao);
    GLint myViewLoc = glGetUniformLocation(bez_shader.Program, "view");
    GLint myProjLoc = glGetUniformLocation(bez_shader.Program, "projection");
    GLint lightColorLoc  = glGetUniformLocation(bez_shader.Program, "lightColor");
    GLint lightPosLoc    = glGetUniformLocation(bez_shader.Program, "lightPos");
    GLint viewPosLoc     = glGetUniformLocation(bez_shader.Program, "viewPos");
    GLint colorLoc     = glGetUniformLocation(bez_shader.Program, "color");
    GLint myModelLoc = glGetUniformLocation(bez_shader.Program, "model");
    glUniform3f(lightColorLoc,  1.0f, 1.0f, 1.0f);
    glm::vec3 cameraEye = camera.getEye();
    glUniform3f(lightPosLoc,     cameraEye.x+5.0f, cameraEye.y+5.0f, cameraEye.z+5.0f);
    glUniform3f(viewPosLoc,     cameraEye.x, cameraEye.y, cameraEye.z);
    glUniformMatrix4fv(myViewLoc, 1, GL_FALSE, glm::value_ptr(*view_ptr));
    glUniformMatrix4fv(myProjLoc, 1, GL_FALSE, glm::value_ptr(*projection_ptr));
    glUniform4f(colorLoc,1.0f, 0.0f,0.0f,1.0f);
    glm::mat4 model(1.0f);
    glUniformMatrix4fv(myModelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glDrawElements(GL_TRIANGLES, num_shroom_indices, GL_UNSIGNED_INT, NULL);
    //glDrawArrays(GL_TRIANGLES, 0, 3);
    float verts_per_strip = 2.0f*(mesher.num_sphere_lines+2);
    int num_strips = mesher.num_sphere_verts/verts_per_strip;
    //printf("num_lines, num_vecs, verts_per_strip: %d, %d, %f\n",num_lines, num_vecs, verts_per_strip);
    int num_verts = (int) verts_per_strip;
    int color_step = 0;
    glm::mat4 pmodel(1.0f);
    ///////////////////////////////// NOTE //////////////////////////////
    //////// model_ptr IS NOT BEING USED!!!!
    //////////////////////////////////////////////////////////////////////
    for (int k = 0; k < numBezPts; k++){
        /** this should be 'pushed' -- MatrixStack !!!! */
        //glm::mat4 model = glm::rotate(pmodel,(float) (-M_PI/2.0f),glm::vec3(0.0f,1.0f,0.0f));
        //model = glm::rotate(model,(float) (M_PI/2.0f),glm::vec3(1.0f,0.0f,0.0f));
        glm::mat4 model(1.0f);
        /* end push */
        int start = 0;
        float scale1 = NeuronInfoModule::NEURON_SIZE_SCALE*NeuronInfoModule::AVG_NEURON_DIAM;//.0001f;
        float scale2 = NeuronInfoModule::NEURON_TRANS_SCALE;
        model = glm::scale(model, (scale1*glm::vec3(1.0f, 1.0f, 1.0f)));
        model = glm::translate(model,(scale2*bezDat[k]->center));
        //printf("color: %f, %f, %f, %f\n", neuron_colors[color_step],neuron_colors[color_step+1],neuron_colors[color_step+2],neuron_colors[color_step+3]);
        glUniform4f(colorLoc,1.0f, 0.0f,0.0f,1.0f);
        color_step += 4;
        glUniformMatrix4fv(myModelLoc, 1, GL_FALSE, glm::value_ptr(model));
        for(int i = 0; i < num_strips; i++  ){
            
            glDrawArrays(GL_TRIANGLE_STRIP,start, num_verts);
            start += num_verts;
        }
        /*
         if (k > 10){
         break;
         }
         */
    }
    
}

void Artist::prep_conns(int num_all_indices, int num_chem_indices, int num_gap_indices, float* all_conns){
    numAllIndices = num_all_indices;
    numChemIndices = num_chem_indices;
    numGapIndices = num_gap_indices;
    allConns = all_conns;
    glGenVertexArrays(1, &connVao);
    glGenBuffers(1, &connVbo);
    glBindVertexArray(connVao);
    glBindBuffer(GL_ARRAY_BUFFER, connVbo);
    printf("#incides :%d\n",numAllIndices);
    glBufferData(GL_ARRAY_BUFFER, numAllIndices*sizeof(GLfloat), all_conns, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3, GL_FLOAT, GL_FALSE,3*sizeof(GLfloat),0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
}


void Artist::draw_conns(Shader axesShader, Camera camera, glm::mat4* projection_ptr, glm::mat4* view_ptr){
    glBindVertexArray(connVao);
    GLint axes_color_loc = glGetUniformLocation(axesShader.Program,"color");
    GLint axes_view_loc = glGetUniformLocation(axesShader.Program,"view");
    GLint axes_proj_loc = glGetUniformLocation(axesShader.Program,"projection");
    GLint axes_model_loc = glGetUniformLocation(axesShader.Program,"model");
    //GLint axes_model_loc = glGetUniformLocation(axesShader.Program,"model");
    glUniformMatrix4fv(axes_view_loc, 1, GL_FALSE, glm::value_ptr(*view_ptr));
    glUniformMatrix4fv(axes_proj_loc, 1, GL_FALSE, glm::value_ptr(*projection_ptr));
    glm::mat4 model_mat(1.0f);
    
    //billboard_mat = glm::translate(billboard_mat, 5000.0f*glm::vec3(1.0f)); // translate and then offset from center of neuron
    glUniformMatrix4fv(axes_model_loc, 1, GL_FALSE, glm::value_ptr(model_mat));
    //glUniformMatrix4fv(axes_model_loc, 1, GL_FALSE, glm::value_ptr(model));
    
    int num_verts = (int) numAllIndices/3.0f;
    int num_chem_verts = (int) numChemIndices/3.0f;
    int num_gap_verts = (int) numGapIndices/3.0f;
    
    glUniform4f(axes_color_loc,Artist::orange[0], Artist::orange[1], Artist::orange[2], Artist::orange[3]);
    //glLineWidth(2.0f);// at least with mac, line width doesn't work.
    glDrawArrays(GL_LINES, 0, num_gap_verts);
    glUniform4f(axes_color_loc,Artist::pink[0], Artist::pink[1], Artist::pink[2], Artist::pink[3]);
    glDrawArrays(GL_LINES, num_gap_verts, num_chem_verts);
    glBindVertexArray(0);
    
}

void Artist::draw_conn_weights(Shader textShader, ConnectionComrade* commie, Camera camera, glm::mat4* projection_ptr, glm::mat4* view_ptr){
    glUseProgram(textShader.Program);
    int numConns = commie->currentlyUsedConns.size();
    for (int i = 0; i < numConns; ++i){
        /*
        if ((!OptionForewoman::WormOpts[SHOW_ALL_NEURONS] && !fakeStewie->elements[i]->marked) || (!OptionForewoman::WormOpts[SHOW_MUSCLES] && !fakeStewie->elements[i]->marked)){
            // if we don't want to show all neurons, and it's not marked, then we don't want to show it
            continue;
        }
         */
        glm::mat4 billboard_mat(1.0f);
        glm::vec3 pos;
       // if (fakeStewie->elements[i]->getEType() == MUSCLE){
       //     MuscleInfoModule* tmim = (MuscleInfoModule*)fakeStewie->elements[i];
        //    pos = tmim->centerMassPoint->center;
            
         //   billboard_mat = glm::scale(billboard_mat, MuscleInfoModule::MUSCLE_SIZE_SCALE*glm::vec3(1.f, 1.f, MuscleInfoModule::length_z_scale));
          //  billboard_mat = glm::translate(billboard_mat,(MuscleInfoModule::MUSCLE_TRANS_SCALE*pos)*glm::vec3(1.f, 1.f,MuscleInfoModule::trans_z_scale));// 2.5 is half of the night of the muscle model (i think)
            billboard_mat = glm::translate(billboard_mat,commie->currentlyUsedConns[i]->midpoint);
            
            //billboard_mat = glm::translate(billboard_mat,(pos*MuscleInfoModule::MUSCLE_TRANS_SCALE));
            
            //billboard_mat = glm::translate(billboard_mat, pos); // translate and then offset from center of neuron
            //billboard_mat = glm::translate(billboard_mat, pos+(MuscleInfoModule::MUSCLE_SIZE_SCALE*2.f/2.f)); // translate and then offset from center of neuron
        //}
        //else {
        /*
            NeuronInfoModule* tnim = (NeuronInfoModule*)fakeStewie->elements[i];
            glm::vec3 pos = tnim->massPoint->center;// - (nim[i].soma_diameter/2.0f);
            billboard_mat = glm::translate(billboard_mat, ((NEURON_TRANS_SCALE*pos) + (NEURON_SIZE_SCALE*tnim->soma_diameter/2.f))); // translate and then offset from center of neuron
        }
         */
        billboard_mat = glm::rotate(billboard_mat, camera.m_theta, glm::vec3(0,1,0));
         
        billboard_mat = glm::rotate(billboard_mat, camera.m_phi - ((float)M_PI/2.f), glm::vec3(1,0,0));
        //billboard_mat = glm::scale(billboard_mat, (.1f/NEURON_SIZE_SCALE*glm::vec3(1.f, 1.f, 1.f)));
        glUniformMatrix4fv(glGetUniformLocation(textShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(*projection_ptr));
        glUniformMatrix4fv(glGetUniformLocation(textShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(*view_ptr));
        glUniformMatrix4fv(glGetUniformLocation(textShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(billboard_mat));
        //printf("creating text: %s\n",fakeStewie->nim[i]->name.c_str());
        
        /*
        if (OptionForewoman::WormOpts[SHOW_ACTIVATION]){
            // get activation
            int eid = fakeStewie->elements[i]->element_id;// we need to access the voltage array by the element_id, **NOT** by k.
            float currentActivation = (*voltages)[Camera::voltageFrame][eid];
            label =fakeStewie->elements[i]->graphicalName + ": "+std::to_string(currentActivation);
        }
        else {
            label = fakeStewie->elements[i]->graphicalName;
        }
        */
        //if (fakeStewie->elements[i]->getEType() == MUSCLE){ // maybe we want a different scale or something for muscle labels
        textMaker.make_text(textShader, commie->currentlyUsedConns[i]->weightLabel, pos, 1.f/NeuronInfoModule::NEURON_TRANS_SCALE, glm::vec3(1.0f, 1.0f, 1.0f));
        //}
        //else{
            //textMaker.make_text(textShader, label, pos, .5f/NEURON_TRANS_SCALE, glm::vec3(1.0f, 1.0f, 1.0f));
        //}
        //textMaker.make_text(textShader, label, pos, 1.f, glm::vec3(1.0f, 1.0f, 1.0f));
    }
}

/* -100 - 100  ... 0 is grey */
glm::vec4 Artist::colorByActivation(float activation){
    float minActivation = -100;
    float maxActivation = 100;
    float r,g,b,a, fracAct;
    a = 1.f; // alpha
    if (activation == 0){
        r = 1.f;
        g = 1.f;
        b = 1.f;
    }
    else if (activation < 0){// start at white, tend toward bright green
        fracAct = (fabsf(activation)/100.f);
        r = 1.f - fracAct;
        g = 1.f;
        b = 1.f - fracAct;
    }
    else if (activation > 0){ // start at white, tend toward bright red
        fracAct = (fabsf(activation)/100.f);
        r = 1.f;
        g = 1.f - fracAct;
        b = 1.f - fracAct;
    }
    return glm::vec4(r, g, b, a);
}


/* NOTE: OLD/ORIGINAL (the yellow version)
 * might need to be changed...
 *
 * If activation is below the minActivation level, we color it a shade of blue/teal.
 * If activation is between the min and max (inclusive), we color it a shade of yellow
 * If activation is over the max, we color it a shade of orange
glm::vec4 Artist::colorByActivation(float activation){
    float adjActivation = activation + 10; // adjusting for -5 min -- move range from 0 to 10
    float minActivation = 0;
    float maxActivation = 20;
    float r,g,b,a;
    a = 1.f; // alpha
    if (adjActivation <= minActivation){// we want shades of blue/teal
        r = 0.f;
        g = b = .5;
    }
    else if (adjActivation >= maxActivation){ // we want shades of orange
        r = 1.f; // can't go above 1
        g = 0.f;
        b = 0.f;
    }
    else {
        r = g = adjActivation/maxActivation;
        b = 0.f;
    }
    return glm::vec4(r, g, b, a);
}
 */
