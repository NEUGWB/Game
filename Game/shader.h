#ifndef SHADER_H
#define SHADER_H

#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>
#include "robin_hood.h"

// General purpsoe shader object. Compiles from file, generates
// compile/link-time error messages and hosts several utility 
// functions for easy management.
class ShaderUniform
{
public:
    char name[32];
    ShaderUniform(const char *u)
    {
        strcpy_s(name, u);
    }
    
    struct hash_fun
    {
        size_t operator()(const ShaderUniform &su) const
        {
            return std::hash<const char*>()(su.name);
        }
    };
    bool operator == (const ShaderUniform &other) const
    {
        return strcmp(this->name, other.name) == 0;
    }
    bool operator < (const ShaderUniform &other) const
    {
        return strcmp(this->name, other.name) < 0;
    }
};
//inline bool operator < (const ShaderUniform &left, const ShaderUniform &other)
//{
//    return strcmp(left.name, other.name) < 0;
//}
class Shader
{
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
public:
    // state
    unsigned int ID; 
    // constructor
    Shader() { }
    // sets the current shader as active
    Shader  &Use();
    // compiles the shader from given source code
    bool    Compile(const char *vertexSource, const char *fragmentSource, const char *geometrySource = nullptr); // note: geometry source code is optional 
                                                                                                                 // utility functions
    void    SetFloat    (const char *name, float value, bool useShader = false);
    void    SetInteger  (const char *name, int value, bool useShader = false);
    void    SetVector2f (const char *name, float x, float y, bool useShader = false);
    void    SetVector2f (const char *name, const glm::vec2 &value, bool useShader = false);
    void    SetVector3f (const char *name, float x, float y, float z, bool useShader = false);
    void    SetVector3f (const char *name, const glm::vec3 &value, bool useShader = false);
    void    SetVector4f (const char *name, float x, float y, float z, float w, bool useShader = false);
    void    SetVector4f (const char *name, const glm::vec4 &value, bool useShader = false);
    void    SetMatrix4  (const char *name, const glm::mat4 &matrix, bool useShader = false);

    void    SetFloat    (GLint uniform, float value);
    void    SetInteger  (GLint uniform, int value);
    void    SetVector2f (GLint uniform, float x, float y);
    void    SetVector2f (GLint uniform, const glm::vec2 &value);
    void    SetVector3f (GLint uniform, float x, float y, float z);
    void    SetVector3f (GLint uniform, const glm::vec3 &value);
    void    SetVector4f (GLint uniform, float x, float y, float z, float w);
    void    SetVector4f (GLint uniform, const glm::vec4 &value);
    void    SetMatrix4  (GLint uniform, const glm::mat4 &matrix);

    GLint   getUniformLocation(const char *name);
private:
    // checks if compilation or linking failed and if so, print the error logs
    bool    checkCompileErrors(unsigned int object, std::string type); 

    //std::map<ShaderUniform, GLint> UniformLocate;
    robin_hood::unordered_map<ShaderUniform, GLint, ShaderUniform::hash_fun> UniformLocate;
    //std::unordered_map<std::string, GLint> UniformLocate;
};

#define DECLARE_GET_SHADER_UNIFORM_LOCATION(uniform_name, shader_name) GLint shader##uniform_name = -1;\
GLint getShaderLocation_##uniform_name(){\
    if (shader##uniform_name == -1){shader##uniform_name = shader_name->getUniformLocation(#uniform_name);}\
    return shader##uniform_name;\
}

#endif