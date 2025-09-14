#include "MaterialPGR.h"

#define ENABLE_BUILTIN_MATERIAL

#if defined(ENABLE_BUILTIN_MATERIAL) && defined(IMPL_SHADER)
MaterialPGR::MaterialPGR(const Shader &shader) : _shader(&shader)
{}

void MaterialPGR::SetInt(const std::string &name, int value)
{
    _values[name] = {MaterialValue::INT, {.i = value}};
}
void MaterialPGR::SetFloat(const std::string &name, float value)
{
    _values[name] = {MaterialValue::FLOAT, {.f = value}};
}
void MaterialPGR::SetVec2(const std::string &name, const glm::vec2 &value)
{
    _values[name] = {MaterialValue::VEC2, {.v2 = value}};
}
void MaterialPGR::SetVec3(const std::string &name, const glm::vec3 &value)
{
    _values[name] = {MaterialValue::VEC3, {.v3 = value}};
}
void MaterialPGR::SetVec4(const std::string &name, const glm::vec4 &value)
{
    _values[name] = {MaterialValue::VEC4, {.v4 = value}};
}
void MaterialPGR::SetMat3(const std::string &name, const glm::mat3 &value)
{
    _values[name] = {MaterialValue::MAT3, {.m3 = value}};
}
void MaterialPGR::SetMat4(const std::string &name, const glm::mat4 &value)
{
    _values[name] = {MaterialValue::MAT4, {.m4 = value}};
}

void MaterialPGR::SetValues()
{
    // Bronze
    SetVec3("material.diffuse", Diffuse);
    SetVec3("material.specular", Specular);
    SetFloat("material.shininess", Shininess);
}
void MaterialPGR::ApplyValues() const
{
    for (const auto &[name, value] : _values)
    {
        const int location = _shader->GetUniformLocationSafe(name);

        switch (value.type)
        {
            case MaterialValue::INT:
                Shader::SetInt(location, value.i);
                break;

            case MaterialValue::FLOAT:
                Shader::SetFloat(location, value.f);
                break;

            case MaterialValue::VEC2:
                Shader::SetVec2(location, value.v2);
                break;

            case MaterialValue::VEC3:
                Shader::SetVec3(location, value.v3);
                break;

            case MaterialValue::VEC4:
                Shader::SetVec4(location, value.v4);
                break;

            case MaterialValue::MAT3:
                Shader::SetMat3(location, value.m3);
                break;

            case MaterialValue::MAT4:
                Shader::SetMat4(location, value.m4);
                break;
        }
    }
}
#endif
