//
//  TextMaker.hpp
//  LearningOpenGL
//
//  Created by onyx on 11/24/15.
//  Copyright © 2015 AweM. All rights reserved.
//

#ifndef TextMaker_hpp
#define TextMaker_hpp

#include <stdio.h>


#include "std_graphics_header.hpp"

#include <map>



#include "Shader.hpp"

class TextMaker{
    
public:
    
    struct Character {
        GLuint     TextureID;  // ID handle of the glyph texture
        glm::ivec2 Size;       // Size of glyph
        glm::ivec2 Bearing;    // Offset from baseline to left/top of glyph
        GLuint     Advance;    // Offset to advance to next glyph
    };
    
    std::map<GLchar, Character> Characters;
    
    FT_Library ft;
    FT_Face ft_face;
    
    GLuint textVAO, textVBOpos, textVBOtex;
    
public:
    TextMaker();
    ~TextMaker();
    void prep_text();
    void make_text(Shader& s, std::string text, glm::vec3 in_pos, GLfloat scale, glm::vec3 color);
};


#endif /* TextMaker_hpp */
