//
//  Shader.hpp
//  LearningOpenGL
//
//  Created by onyx on 1/18/16.
//  Copyright © 2016 AweM. All rights reserved.
//

#ifndef Shader_hpp
#define Shader_hpp

#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>

class Shader{
    
public:
    GLuint Program;
    // Constructor generates the shader on the fly
    Shader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath = nullptr);
   
    // Uses the current shader
    void Use();
    
private:
    void checkCompileErrors(GLuint shader, std::string type);
    
};

#endif /* Shader_hpp */
