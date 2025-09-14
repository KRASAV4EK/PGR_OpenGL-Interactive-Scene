#include "Shader.h"
#include <GL/glew.h>
#include "ShaderUtils.h"

Shader::Shader(const ShaderSource &shaderSource)
{
    unsigned int vertexShader = ShaderUtils::CompileShaderCode(Vertex, shaderSource.GetVertexSource());
    if (vertexShader == 0)
    {
        _id = 0;
        LOG_ERROR("Failed to compile shader.");
        return;
    }

    unsigned int fragmentShader = ShaderUtils::CompileShaderCode(Fragment, shaderSource.GetFragmentSource());
    if (fragmentShader == 0)
    {
        _id = 0;
        LOG_ERROR("Failed to compile shader.");
        return;
    }

    _id = ShaderUtils::LinkShader(vertexShader, fragmentShader);
    if (_id == 0)
    {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(_id);
        LOG_ERROR("Failed to compile shader.");
        return;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}
bool Shader::operator==(const Shader &shader) const
{
    return _id == shader._id;
};

Shader::~Shader()
{
    glDeleteProgram(_id);
}

Shader &Shader::operator=(Shader &&other) noexcept
{
    if (this != &other)
        std::swap(_id, other._id);

    return *this;
}

void Shader::Bind(const Shader &shader)
{
    glUseProgram(shader._id);
}
void Shader::Delete(const Shader &shader)
{
    glDeleteProgram(shader._id);
}

void Shader::SetInt(int location, int value)
{
    glUniform1i(location, value);
}
void Shader::SetFloat(int location, float value)
{
    glUniform1f(location, value);
}
void Shader::SetVec2(int location, const glm::vec2 &value)
{
    glUniform2fv(location, 1, value_ptr(value));
}
void Shader::SetVec3(int location, const glm::vec3 &value)
{
    glUniform3fv(location, 1, value_ptr(value));
}
void Shader::SetVec4(int location, const glm::vec4 &value)
{
    glUniform4fv(location, 1, value_ptr(value));
}
void Shader::SetMat3(int location, const glm::mat3 &value)
{
    glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(value));
}
void Shader::SetMat4(int location, const glm::mat4 &value)
{
    glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(value));
}

int Shader::GetAttribLocationSafe(const unsigned int &shaderID, const std::string &name)
{
    const int ret = glGetAttribLocation(shaderID, name.c_str());
    if (ret == -1) { LOG_ERROR("Attribute \"{}\" was not assigned in shader", name); }
    return ret;
}
int Shader::GetAttribLocationSafe(const std::string &name) const
{
    const int ret = glGetAttribLocation(_id, name.c_str());
    if (ret == -1) { LOG_ERROR("Attribute \"{}\" was not assigned in shader", name); }
    return ret;
}

int Shader::GetUniformLocationSafe(const unsigned int &shaderID, const std::string &name)
{
    const int ret = glGetUniformLocation(shaderID, name.c_str());
    if (ret == -1) { LOG_ERROR("Uniform \"{}\" was not assigned in shader", name); }
    return ret;
}
int Shader::GetUniformLocationSafe(const std::string &name) const
{
    const int ret = glGetUniformLocation(_id, name.c_str());
    if (ret == -1) { LOG_ERROR("Uniform \"{}\" was not assigned in shader", name); }
    return ret;
}

int Shader::GetUniformLocation(const std::string &name) const
{
    return glGetUniformLocation(_id, name.c_str());
}

unsigned int &Shader::GetID()
{
    return _id;
}
