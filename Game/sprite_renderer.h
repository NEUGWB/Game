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
};

class SpriteRenderer
{
public:
    
    // Constructor (inits shaders/shapes)
    SpriteRenderer(Shader *shader);
    // Destructor
    ~SpriteRenderer();
    // Renders a defined quad textured with given sprite
    void DrawSprite(const Texture2D &texture, glm::vec2 position, glm::vec2 size = glm::vec2(1.0f, 1.0f), float rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f));
    void BatchDrawSprite(unsigned int textureID, plf::list<RenderParam> &vrp);
    friend class SpriteBatchRenderer;
private:
    // Render state
    Shader       *shader; 
    unsigned int quadVAO;
    // Initializes and configures the quad's buffer and vertex attributes
    void initRenderData();
    void innerDrawSprite(RenderParam &rp);

    DECLARE_GET_SHADER_UNIFORM_LOCATION(model, shader)
    DECLARE_GET_SHADER_UNIFORM_LOCATION(spriteColor, shader)
};

class SpriteBatchRenderer
{
public:
    SpriteBatchRenderer(SpriteRenderer *r) : Renderer(r) {}
    void ClearData();
    void AddSprit(const Texture2D &texture, glm::vec2 position, glm::vec2 size = glm::vec2(1.0f, 1.0f), float rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f));
    void Render();

    robin_hood::unordered_map<unsigned int, plf::list<RenderParam> > Sprits;
    SpriteRenderer *Renderer = nullptr;
};

#endif