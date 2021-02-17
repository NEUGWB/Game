#pragma once
#include "glm/glm.hpp"
#include "shader.h"
#include <vector>
class Primitive
{
public:
	static void Init();
	static Primitive *GetInstance();
	static void Release();

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

	void AddRectangle(const Rectangle &r);
	void BatchDraw();
	std::vector<Vertex> Vet;
	std::vector<int> Ind;

	unsigned int VBO, VAO, EBO;
	Shader *shader;

	static Primitive *inst;
};