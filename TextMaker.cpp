//
//  TextMaker.cpp
//  LearningOpenGL
//
//  Created by onyx on 11/24/15.
//  Copyright © 2015 AweM. All rights reserved.
// code basically taken from: http://learnopengl.com/#!In-Practice/Text-Rendering

#include "TextMaker.hpp"
#include <iostream>


TextMaker::TextMaker(){
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glGenVertexArrays(1, &textVAO);
    glGenBuffers(1, &textVBOpos);
    glGenBuffers(1, &textVBOtex);
    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBOpos);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 3, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, textVBOtex);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 2, NULL, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
    GLfloat tex[6][2] = {
        {0.0, 0.0},
        {0.0, 1.0},
        {1.0, 1.0},
        {0.0, 0.0},
        {1.0, 1.0},
        {1.0, 0.0}};
    glBindBuffer(GL_ARRAY_BUFFER, textVBOtex);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(tex), tex);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

TextMaker::~TextMaker(){}

/**
 * Text
 */

void TextMaker::prep_text(){
    if (FT_Init_FreeType(&ft))
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    
    if (FT_New_Face(ft, FileAssistant::open_sans_ttf.c_str(), 0, &ft_face))
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
    
    FT_Set_Pixel_Sizes(ft_face, 0, 48);
    
    if (FT_Load_Char(ft_face, 'X', FT_LOAD_RENDER))
        std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction
    
    for (GLubyte c = 0; c < 128; c++)
    {
        // Load character glyph
        if (FT_Load_Char(ft_face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // Generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
                     GL_TEXTURE_2D,
                     0,
                     GL_RED,
                     ft_face->glyph->bitmap.width,
                     ft_face->glyph->bitmap.rows,
                     0,
                     GL_RED,
                     GL_UNSIGNED_BYTE,
                     ft_face->glyph->bitmap.buffer
                     );
        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Now store character for later use
        Character character = {
            texture,
            glm::ivec2(ft_face->glyph->bitmap.width, ft_face->glyph->bitmap.rows),
            glm::ivec2(ft_face->glyph->bitmap_left, ft_face->glyph->bitmap_top),
            (GLuint)ft_face->glyph->advance.x
        };
        Characters.insert(std::pair<GLchar, Character>(c, character));
    }
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    FT_Done_Face(ft_face);
    FT_Done_FreeType(ft);
    
}

void TextMaker::make_text(Shader& s, std::string text, glm::vec3 in_pos, GLfloat scale, glm::vec3 color){
        // Activate corresponding render state
        glUniform3f(glGetUniformLocation(s.Program, "textColor"), color.x, color.y, color.z);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(textVAO);
        
        // Iterate through all characters
        std::string::const_iterator c;
        for (c = text.begin(); c != text.end(); c++)
        {
            Character ch = Characters[*c];
            
            GLfloat xpos = in_pos.x + ch.Bearing.x * scale;
            GLfloat ypos = in_pos.y - (ch.Size.y - ch.Bearing.y) * scale;
            
            GLfloat w = ch.Size.x * scale;
            GLfloat h = ch.Size.y * scale;
            // Update VBO for each character
            GLfloat pos[6][3] = {
                { xpos,     ypos + h,  in_pos.z},
                { xpos,     ypos,      in_pos.z},
                { xpos + w, ypos,      in_pos.z},
                
                { xpos,     ypos + h,  in_pos.z},
                { xpos + w, ypos,      in_pos.z},
                { xpos + w, ypos + h,  in_pos.z}
            };
       
            // Render glyph texture over quad
            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            // Update content of VBO memory
            glBindBuffer(GL_ARRAY_BUFFER, textVBOpos);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(pos), pos);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            // Render quad
            glDrawArrays(GL_TRIANGLES, 0, 6);
            // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            in_pos.x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
        }
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
}
