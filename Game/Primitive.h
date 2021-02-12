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
		glm::vec2 lu, rb;
		glm::vec4 color = glm::vec4(1.0f);
	};

	struct OutlinedRectangle
	{
		Rectangle out, in;
	};
	Primitive(Shader *);

	void DrawTrangle(const Triangle &t);
	void DrawRectangle(const Rectangle &r);
	void DrawOutlinedRectangle(const OutlinedRectangle &t);

	unsigned int VBO, VAO, EBO;
	Shader *shader;
};