#include "Primitive.h"
#include "resource_manager.h"

Primitive *Primitive::inst;
void Primitive::Init()
{
    inst = new Primitive(ResourceManager::GetShader("primitive"));
}

Primitive *Primitive::GetInstance()
{
    return inst;
}

void Primitive::Release()
{
    delete inst;
}

Primitive::Primitive(Shader *s) : shader(s)
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 1000 * sizeof(Vertex), NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 1000 * sizeof(unsigned int), NULL, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Primitive::DrawTrangle(const Triangle &t)
{
    unsigned int indices[] = {
        0, 1, 2,
    };
    this->shader->Use();
    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(t), &t);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(indices), indices);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Primitive::DrawRectangle(const Rectangle &r)
{
    Vertex vet[4] =
    {
        {glm::vec2{r.lu.x, r.lu.y}, r.color},
        {glm::vec2{r.rb.x, r.lu.y}, r.color},
        {glm::vec2{r.rb.x, r.rb.y}, r.color},
        {glm::vec2{r.lu.x, r.rb.y}, r.color},
    };
    unsigned int indices[] = {
        0, 1, 2,
        0, 3, 2
    };
    this->shader->Use();
    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vet), &vet);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(indices), indices);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Primitive::DrawOutlinedRectangle(const OutlinedRectangle &r)
{
    Vertex vet[8] =
    {
        {glm::vec2{r.out.lu.x, r.out.lu.y}, r.out.color},
        {glm::vec2{r.out.rb.x, r.out.lu.y}, r.out.color},
        {glm::vec2{r.out.rb.x, r.out.rb.y}, r.out.color},
        {glm::vec2{r.out.lu.x, r.out.rb.y}, r.out.color},

        {glm::vec2{r.in.lu.x, r.in.lu.y}, r.in.color},
        {glm::vec2{r.in.rb.x, r.in.lu.y}, r.in.color},
        {glm::vec2{r.in.rb.x, r.in.rb.y}, r.in.color},
        {glm::vec2{r.in.lu.x, r.in.rb.y}, r.in.color},
    };
    unsigned int indices[] = {
        0,1,5,2,6,3,7,0,4,5
    };
    this->shader->Use();
    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vet), &vet);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(indices), indices);
    glDrawElements(GL_TRIANGLE_STRIP, 10, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Primitive::AddRectangle(const Rectangle &r)
{
    Vertex vet[4] =
    {
        {glm::vec2{r.lu.x, r.lu.y}, r.color},
        {glm::vec2{r.rb.x, r.lu.y}, r.color},
        {glm::vec2{r.rb.x, r.rb.y}, r.color},
        {glm::vec2{r.lu.x, r.rb.y}, r.color},
    };
    unsigned int indices[] = {
        0, 1, 2,
        0, 3, 2
    };
    for (auto &i : indices)
    {
        i += this->Vet.size();
    }
    for (int i = 0; i < 4; ++i)
    {
        this->Vet.push_back(vet[i]);
    }
    for (int i = 0; i < 6; ++i)
    {
        this->Ind.push_back(indices[i]);
    }
}

void Primitive::BatchDraw()
{
    this->shader->Use();
    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * this->Vet.size(), this->Vet.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(int) * this->Ind.size(), this->Ind.data());
    glDrawElements(GL_TRIANGLES, this->Ind.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    this->Vet.clear();
    this->Ind.clear();
}
