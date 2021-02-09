/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <map>
#include "robin_hood.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "texture.h"
#include "shader.h"


/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    unsigned int TextureID = -1; // ID handle of the glyph texture
    glm::vec2    Pos;
    glm::ivec2   Size;      // size of glyph
    glm::ivec2   Bearing;   // offset from baseline to left/top of glyph
    unsigned int Advance;   // horizontal offset to advance to next glyph
};


// A renderer class for rendering text displayed by a font loaded using the 
// FreeType library. A single font is loaded, processed into a list of Character
// items for later rendering.
class TextRenderer
{
public:
    // holds a list of pre-compiled Characters
    robin_hood::unordered_map<wchar_t, Character> Characters; 
    // shader used for text rendering
    Shader *TextShader;
    // constructor
    TextRenderer(unsigned int width, unsigned int height);
    // pre-compiles a list of characters from the given font
    void PreLoad(std::string font, unsigned int fontSize, std::wstring text);
    // renders a string of text using the precompiled list of characters
    void RenderText(std::wstring text, float x, float y, float scale, glm::vec3 color = glm::vec3(1.0f));

    void LoadChar(wchar_t c, FT_Library Ft, FT_Face Face);
private:
    // render state
    unsigned int VAO, VBO;

    int FontTexX = 0, FontTexY = 0, FontTexMaxY = 0;
    unsigned TextureID;

    std::string Font;
    int FontSize;
};

#endif 