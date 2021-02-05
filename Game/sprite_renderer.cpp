/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include "sprite_renderer.h"


SpriteRenderer::SpriteRenderer(Shader *shader)
{
    this->shader = shader;
    this->initRenderData();
}

SpriteRenderer::~SpriteRenderer()
{
    glDeleteVertexArrays(1, &this->quadVAO);
}

void SpriteRenderer::DrawSprite(const Texture2D &texture, glm::vec2 position, glm::vec2 size, float rotate, glm::vec3 color)
{
    // prepare transformations
    this->shader->Use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));  // first translate (transformations are: scale happens first, then rotation, and then final translation happens; reversed order)

    if (rotate != 0.0f)
    {
        printf("rotate %f\n", rotate);
        model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f)); // move origin of rotation to center of quad
        model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f)); // then rotate
        model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f)); // move origin back
    }
    
    model = glm::scale(model, glm::vec3(size, 1.0f)); // last scale

    this->shader->SetMatrix4(getShaderLocation_model(), model);

    // render textured quad
    this->shader->SetVector3f(getShaderLocation_spriteColor(), color);

    glActiveTexture(GL_TEXTURE0);
    texture.Bind();

    glBindVertexArray(this->quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void SpriteRenderer::BatchDrawSprite(unsigned int textureID, plf::list<RenderParam> &vrp)
{
    //texture.Bind();
    glBindTexture(GL_TEXTURE_2D, textureID);
    glActiveTexture(GL_TEXTURE0);
    
    for (auto &rp : vrp)
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(rp.position, 0.0f));  // first translate (transformations are: scale happens first, then rotation, and then final translation happens; reversed order)

        if (rp.rotate != 0.0f)
        {
            printf("rotate %f\n", rp.rotate);
            model = glm::translate(model, glm::vec3(0.5f * rp.size.x, 0.5f * rp.size.y, 0.0f)); // move origin of rotation to center of quad
            model = glm::rotate(model, glm::radians(rp.rotate), glm::vec3(0.0f, 0.0f, 1.0f)); // then rotate
            model = glm::translate(model, glm::vec3(-0.5f * rp.size.x, -0.5f * rp.size.y, 0.0f)); // move origin back
        }

        model = glm::scale(model, glm::vec3(rp.size, 1.0f)); // last scale

        this->shader->SetMatrix4(getShaderLocation_model(), model);

        // render textured quad
        this->shader->SetVector3f(getShaderLocation_spriteColor(), rp.color);

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}

void SpriteRenderer::initRenderData()
{
    // configure VAO/VBO
    unsigned int VBO;
    float vertices[] = { 
        // pos      // tex
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    glGenVertexArrays(1, &this->quadVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(this->quadVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void SpriteBatchRenderer::ClearData()
{
    for (auto &it : this->Sprits)
    {
        it.second.clear();
    }
    //this->Sprits.clear();
}

void SpriteBatchRenderer::AddSprit(const Texture2D &texture, glm::vec2 position, glm::vec2 size, float rotate, glm::vec3 color)
{
    //this->VecRenderParam.emplace_back(RenderParam{ position, size, rotate, color });
    auto &vrp = this->Sprits[texture.ID];
    vrp.emplace_back(RenderParam{ position, size, rotate, color });
}

void SpriteBatchRenderer::Render()
{
    //this->Renderer->BatchDrawSprite(this->Texture, this->VecRenderParam);
    this->Renderer->shader->Use();
    glBindVertexArray(this->Renderer->quadVAO);
    for (auto &it : this->Sprits)
    {
        this->Renderer->BatchDrawSprite(it.first, it.second);
    }
    glBindVertexArray(0);
}
