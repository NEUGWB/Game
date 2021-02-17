/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include <iostream>

#include <glm/gtc/matrix_transform.hpp>

#include "text_renderer.h"
#include "resource_manager.h"
TextRenderer *TextRenderer::inst;
void TextRenderer::Init(unsigned int width, unsigned int height)
{
    inst = new TextRenderer(width, height);
}
TextRenderer *TextRenderer::GetInstance()
{
    return inst;
}

void TextRenderer::Release()
{
    delete inst;
}

TextRenderer::TextRenderer(unsigned int width, unsigned int height)
{
    // load and configure shader
    this->TextShader = ResourceManager::LoadShader("shaders/text.vs", "shaders/text.frag", nullptr, "text");
    this->TextShader->SetMatrix4("projection", glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f), true);
    this->TextShader->SetInteger("text", 0);
    // configure VAO/VBO for texture quads
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);
    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void TextRenderer::PreLoad(std::string font, unsigned int fontSize, std::wstring text)
{
    this->Font = font;
    this->FontSize = fontSize;
    this->Characters.clear();
    FT_Library ft;    
    if (FT_Init_FreeType(&ft))
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    FT_Face face;
    if (FT_New_Face(ft, font.c_str(), 0, &face))
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
    FT_Set_Pixel_Sizes(face, 0, fontSize);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RED,
        1024,
        1024,
        0,
        GL_RED,
        GL_UNSIGNED_BYTE,
        NULL
    );
    // set texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    this->TextureID = texture;

    for (wchar_t c : text)
    {
        LoadChar(c, ft, face);
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

void TextRenderer::RenderText(std::wstring text, float x, float y, float scale, glm::vec3 color)
{
    FT_Library ft = NULL;
    FT_Face face = NULL;
    // activate corresponding render state	
    this->TextShader->Use();
    this->TextShader->SetVector3f("textColor", color);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(this->VAO);

    // iterate through all characters
    std::wstring::const_iterator c;
    unsigned int prevTex = -1;
    for (c = text.begin(); c != text.end(); c++)
    {
        if (Characters.find(*c) == Characters.end())
        {
            if (!ft || !face)
            {
                if (FT_Init_FreeType(&ft)) // all functions return a value different than 0 whenever an error occurred
                    std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
                if (FT_New_Face(ft, this->Font.c_str(), 0, &face))
                    std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
                FT_Set_Pixel_Sizes(face, 0, this->FontSize);
            }
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 
            LoadChar(*c, ft, face);
        }
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y + (this->Characters['H'].Bearing.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float lTex = ch.Pos.x / 1024, tTex = ch.Pos.y / 1024;
        float rTex = (ch.Pos.x + ch.Size.x) / 1024, bTex = (ch.Pos.y + ch.Size.y) / 1024;
        float vertices[6][4] = {
            { xpos,     ypos + h,   lTex, bTex },
            { xpos + w, ypos,       rTex, tTex },
            { xpos,     ypos,       lTex, tTex },

            { xpos,     ypos + h,   lTex, bTex },
            { xpos + w, ypos + h,   rTex, bTex },
            { xpos + w, ypos,       rTex, tTex }
        };
        // render glyph texture over quad
        if (prevTex != ch.TextureID)
        {
            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            prevTex = ch.TextureID;
        }
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (1/64th times 2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    if (face)
        FT_Done_Face(face);
    if (ft)
        FT_Done_FreeType(ft);
}

void TextRenderer::LoadChar(wchar_t c, FT_Library ft, FT_Face face)
{
    glBindTexture(GL_TEXTURE_2D, TextureID);
    if (FT_Load_Char(face, c, FT_LOAD_RENDER))
    {
        std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
    }
    // generate texture
    glTexSubImage2D(GL_TEXTURE_2D, 0, this->FontTexX, FontTexY, face->glyph->bitmap.width, face->glyph->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

    Character character = {
        this->TextureID,
        glm::ivec2(this->FontTexX, FontTexY),
        glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
        glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
        face->glyph->advance.x
    };
    this->FontTexX += face->glyph->bitmap.width;
    if (this->FontTexMaxY < face->glyph->bitmap.rows)
        this->FontTexMaxY = face->glyph->bitmap.rows;
    if (this->FontTexX > 1010)
    {
        this->FontTexX = 0;
        this->FontTexY += this->FontTexMaxY + 1;
        this->FontTexMaxY = 0;
    }

    Characters.insert(robin_hood::pair<wchar_t, Character>(c, character));
    glBindTexture(GL_TEXTURE_2D, 0);
}
