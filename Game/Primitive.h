#pragma once
#include "glm/glm.hpp"
#include "shader.h"
class Primitive
{
public:
	struct Vertex
	{
		glm::vec2 pos;
		glm::vec4 color = glm::vec4(1.0f);
	};

	struct Triangle
	{
		Vertex vet[3];
	};

	struct Rectangle
	{
		Vertex vet[4];
	};

	struct HollowRectangle
	{
		Rectangle in, out;
	};

	void DrawTrangle(const Triangle &t);
};