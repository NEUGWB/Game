#ifndef SPRITE_H
#define SPRITE_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "texture.h"
#include "sprite_renderer.h"

class Sprite
{
public:
    Texture2D *Tex;
    glm::vec2 TexSize, TexPos;
    Sprite(Texture2D *tex);
    Sprite(Texture2D *tex, glm::vec2 texPos, glm::vec2 texSize);

    virtual void Draw(SpriteRenderer &renderer, glm::vec2 pos, glm::vec2 size, float rotate, glm::vec3 color);
};

#endif