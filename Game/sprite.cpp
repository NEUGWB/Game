#include "sprite.h"
#include "resource_manager.h"

Sprite::Sprite(std::string tex, glm::vec2 texPos, glm::vec2 texSize)
	: Tex(ResourceManager::GetTexture(tex))
{
	TexSize.x = texSize.x / Tex->Width;
	TexSize.y = texSize.y / Tex->Height;
	TexPos.x = texPos.x / Tex->Width;
	TexPos.y = texPos.y / Tex->Height;
}

void Sprite::Draw(SpriteRenderer &renderer)
{
	renderer.DrawSprite(*this->Tex, this->Position, this->Size, this->Rotation, this->Color);
}

void Sprite::RegistDraw(SpriteBatchRenderer &renderer)
{
	renderer.AddSprit(*this->Tex, this->Position, this->Size, this->Rotation, this->Color);
}
