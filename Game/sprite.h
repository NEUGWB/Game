#ifndef SPRITE_H
#define SPRITE_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "texture.h"
#include "sprite_renderer.h"

class Sprite
{
public:
    glm::vec2   Position, Size;
    glm::vec3   Color;
    float       Rotation;

    Texture2D *Tex;
    glm::vec2 TexSize, TexPos;
    Sprite(std::string tex, glm::vec2 texPos = glm::vec2(0.0f), glm::vec2 texSize = glm::vec2(1.0f));
    //Sprite(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f));

    virtual void Draw(SpriteRenderer &renderer);
    virtual void RegistDraw(SpriteBatchRenderer &renderer);
};

#endif