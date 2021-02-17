#include "game_object.h"


GameObject::GameObject() 
    : Position(0.0f, 0.0f), Size(1.0f, 1.0f), Velocity(0.0f), Color(1.0f), Rotation(0.0f), Spr(nullptr), IsSolid(false), Destroyed(false) { }

GameObject::GameObject(glm::vec2 pos, glm::vec2 size, Texture2D *tex, glm::vec3 color, glm::vec2 velocity, glm::vec2 texPos, glm::vec2 texSize) 
    : Position(pos), Size(size), Velocity(velocity), Color(color), Rotation(0.0f), IsSolid(false), Destroyed(false) 
{ 
    Spr = new Sprite(tex);
}

void GameObject::Draw(SpriteRenderer &renderer)
{
    if (this->Spr)
    {
        this->Spr->Draw(renderer, this->Position, this->Size, this->Rotation, this->Color);
    }
}