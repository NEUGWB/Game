/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#ifndef POST_PROCESSOR_H
#define POST_PROCESSOR_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "texture.h"
#include "sprite_renderer.h"
#include "shader.h"


// PostProcessor hosts all PostProcessing effects for the Breakout
// Game. It renders the game on a textured quad after which one can
// enable specific effects by enabling either the Confuse, Chaos or 
// Shake boolean. 
// It is required to call BeginRender() before rendering the game
// and EndRender() after rendering the game for the class to work.
class PostProcessor
{
public:
    Shader *PostProcessingShader;
    Texture2D Texture;
    unsigned int Width, Height;
    
    PostProcessor(Shader *shader, unsigned int width, unsigned int height);
    void BeginRender();
    void EndRender();
    virtual void Render(float time);
protected:
    unsigned int MSFBO, FBO;
    unsigned int RBO;
    unsigned int VAO;
    void initRenderData();
};

#endif