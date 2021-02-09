#include "sprite.h"
#include "resource_manager.h"

Sprite::Sprite(Texture2D *tex)
	: Tex(tex), TexPos(glm::vec2(0.0f)), TexSize(glm::vec2(1.0f))
{
}

Sprite::Sprite(Texture2D *tex, glm::vec2 texPos, glm::vec2 texSize)
	: Tex(tex)
{
	TexSize.x = texSize.x / Tex->Width;
	TexSize.y = texSize.y / Tex->Height;
	TexPos.x = texPos.x / Tex->Width;
	TexPos.y = texPos.y / Tex->Height;
}

void Sprite::Draw(SpriteRenderer &renderer, glm::vec2 pos, glm::vec2 size, float rotate, glm::vec3 color)
{
	renderer.DrawSprite(*this->Tex, pos, size, rotate, color, this->TexPos, this->TexSize);
}

void Sprite::RegistDraw(SpriteBatchRenderer &renderer)
{
	//renderer.AddSprit(*this->Tex, this->Position, this->Size, this->Rotation, this->Color);
}
