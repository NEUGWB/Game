/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "texture.h"
#include "shader.h"
#include <vector>
#include <list>
#include <map>
#include "robin_hood.h"
#include "plf_list.h"


struct RenderParam
{
    glm::vec2 position;
    glm::vec2 size;
    float rotate = 0.0f;
    glm::vec3 color = glm::vec3(1.0f);

    glm::vec2 texPos = { 0.0f, 0.0f };
    glm::vec2 texSize = { 1.0f, 1.0f };
};

class SpriteRenderer
{
public:
    static void Init();
    static SpriteRenderer *GetInstance();
    static void Release();
    SpriteRenderer(Shader *shader);
    ~SpriteRenderer();
    void DrawSprite(const Texture2D &texture, glm::vec2 position, glm::vec2 size = glm::vec2(1.0f, 1.0f), float rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f),
        glm::vec2 texPos = glm::vec2(0), glm::vec2 texSize = glm::vec2(1.0f));
    void BatchDrawSprite(unsigned int textureID, plf::list<RenderParam> &vrp);
    friend class SpriteBatchRenderer;
private:
    Shader       *shader; 
    unsigned int quadVAO;
    void initRenderData();
    void innerDrawSprite(RenderParam &rp);

    DECLARE_GET_SHADER_UNIFORM_LOCATION(model, shader)
    DECLARE_GET_SHADER_UNIFORM_LOCATION(spriteColor, shader)
    DECLARE_GET_SHADER_UNIFORM_LOCATION(texMat, shader)

    static SpriteRenderer *inst;
};

#endif