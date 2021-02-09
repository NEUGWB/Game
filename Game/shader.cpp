#include "shader.h"

#include <iostream>
#include "robin_hood.h"
#include <map>

Shader &Shader::Use()
{
    glUseProgram(this->ID);
    return *this;
}

bool Shader::Compile(const char* vertexSource, const char* fragmentSource, const char* geometrySource)
{
    bool ret = true;
    unsigned int sVertex, sFragment, gShader;
    // vertex Shader
    sVertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(sVertex, 1, &vertexSource, NULL);
    glCompileShader(sVertex);
    ret = ret && checkCompileErrors(sVertex, "VERTEX");
    // fragment Shader
    sFragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(sFragment, 1, &fragmentSource, NULL);
    glCompileShader(sFragment);
    ret = ret && checkCompileErrors(sFragment, "PROGRAM");
    // if geometry shader source code is given, also compile geometry shader
    if (geometrySource != nullptr)
    {
        gShader = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(gShader, 1, &geometrySource, NULL);
        glCompileShader(gShader);
        ret = ret && checkCompileErrors(gShader, "GEOMETRY");
    }
    // shader program
    this->ID = glCreateProgram();
    glAttachShader(this->ID, sVertex);
    glAttachShader(this->ID, sFragment);
    if (geometrySource != nullptr)
        glAttachShader(this->ID, gShader);
    glLinkProgram(this->ID);
    ret = ret && checkCompileErrors(this->ID, "PROGRAM");
    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(sVertex);
    glDeleteShader(sFragment);
    if (geometrySource != nullptr)
        glDeleteShader(gShader);
    return ret;
}

void Shader::SetFloat(const char *name, float value, bool useShader)
{
    if (useShader)
        this->Use();
    glUniform1f(getUniformLocation(name), value);
}
void Shader::SetInteger(const char *name, int value, bool useShader)
{
    if (useShader)
        this->Use();
    glUniform1i(getUniformLocation(name), value);
}
void Shader::SetVector2f(const char *name, float x, float y, bool useShader)
{
    if (useShader)
        this->Use();
    glUniform2f(getUniformLocation(name), x, y);
}
void Shader::SetVector2f(const char *name, const glm::vec2 &value, bool useShader)
{
    if (useShader)
        this->Use();
    glUniform2f(getUniformLocation(name), value.x, value.y);
}
void Shader::SetVector3f(const char *name, float x, float y, float z, bool useShader)
{
    if (useShader)
        this->Use();
    glUniform3f(getUniformLocation(name), x, y, z);
}
void Shader::SetVector3f(const char *name, const glm::vec3 &value, bool useShader)
{
    if (useShader)
        this->Use();
    glUniform3f(getUniformLocation(name), value.x, value.y, value.z);
}
void Shader::SetVector4f(const char *name, float x, float y, float z, float w, bool useShader)
{
    if (useShader)
        this->Use();
    glUniform4f(getUniformLocation(name), x, y, z, w);
}
void Shader::SetVector4f(const char *name, const glm::vec4 &value, bool useShader)
{
    if (useShader)
        this->Use();
    glUniform4f(getUniformLocation(name), value.x, value.y, value.z, value.w);
}
void Shader::SetMatrix4(const char *name, const glm::mat4 &matrix, bool useShader)
{
    if (useShader)
        this->Use();
    glUniformMatrix4fv(getUniformLocation(name), 1, false, glm::value_ptr(matrix));
}

void Shader::SetFloat(GLint uniform, float value)
{
    glUniform1f(uniform, value);
}

void Shader::SetInteger(GLint uniform, int value)
{
    glUniform1i(uniform, value);
}

void Shader::SetVector2f(GLint uniform, float x, float y)
{
    glUniform2f(uniform, x, y);
}

void Shader::SetVector2f(GLint uniform, const glm::vec2 &value)
{
    glUniform2f(uniform, value.x, value.y);
}


void Shader::SetVector3f(GLint uniform, float x, float y, float z)
{
    glUniform3f(uniform, x, y, z);
}

void Shader::SetVector3f(GLint uniform, const glm::vec3 &value)
{
    glUniform3f(uniform, value.x, value.y, value.z);
}

void Shader::SetVector4f(GLint uniform, float x, float y, float z, float w)
{
    glUniform4f(uniform, x, y, z, w);
}

void Shader::SetVector4f(GLint uniform, const glm::vec4 &value)
{
    glUniform4f(uniform, value.x, value.y, value.z, value.w);
}

void Shader::SetMatrix4(GLint uniform, const glm::mat4 &matrix)
{
    glUniformMatrix4fv(uniform, 1, false, glm::value_ptr(matrix));
}

bool Shader::checkCompileErrors(unsigned int object, std::string type)
{
    int success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(object, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(object, 1024, NULL, infoLog);
            std::cout << "| ERROR::SHADER: Compile-time error: Type: " << type << "\n"
                << infoLog << "\n -- --------------------------------------------------- -- "
                << std::endl;
        }
    }
    else
    {
        glGetProgramiv(object, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(object, 1024, NULL, infoLog);
            std::cout << "| ERROR::Shader: Link-time error: Type: " << type << "\n"
                << infoLog << "\n -- --------------------------------------------------- -- "
                << std::endl;
        }
    }
    return success;
}

GLint Shader::getUniformLocation(const char *name)
{
    //GLint location = glGetUniformLocation(this->ID, name);
    //printf("%u %s %d\n", this->ID, name, location);
    //return location;
    GLint location = -1;
    auto &uniloc = this->UniformLocate;
    auto it = uniloc.find(name);
    if (it == uniloc.end())
    {
        //printf("not get ");
        location = glGetUniformLocation(this->ID, name);
        uniloc[name] = location;
    }
    else
    {
        location = it->second;
    }
    //printf("%u %s %d\n", this->ID, name, location);
    return location;
}
