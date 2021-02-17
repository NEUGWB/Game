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

struct Character {
    unsigned int TextureID = -1;
    glm::vec2    Pos;
    glm::ivec2   Size;
    glm::ivec2   Bearing;
    unsigned int Advance;
};

class TextRenderer
{
public:

    static void Init(unsigned int width, unsigned int height);
    static TextRenderer *GetInstance();
    static void Release();
    robin_hood::unordered_map<wchar_t, Character> Characters; 
    Shader *TextShader;
    TextRenderer(unsigned int width, unsigned int height);
    void PreLoad(std::string font, unsigned int fontSize, std::wstring text);
    void RenderText(std::wstring text, float x, float y, float scale, glm::vec3 color = glm::vec3(1.0f));

    void LoadChar(wchar_t c, FT_Library Ft, FT_Face Face);
private:
    unsigned int VAO, VBO;

    int FontTexX = 0, FontTexY = 0, FontTexMaxY = 0;
    unsigned TextureID;

    std::string Font;
    int FontSize;

    static TextRenderer *inst;
};

#endif 