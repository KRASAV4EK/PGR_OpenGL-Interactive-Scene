#include "Light.h"
#include <tiny_gltf.h>

Light::Light(Type type) : _type(type) {};

void Light::SetColor(const glm::vec3 &color)
{
    SetVec3("light.color", color);
}

#ifndef IMPL_LIGHT_USE_DEDICATED_AMBIENT_LIGHT
void Light::SetAmbientColor(const glm::vec3 &ambientColor)
{
    SetVec3("light.ambient", ambientColor);
}
#endif

#ifndef IMPL_LIGHT_USE_COLOR_FOR_SPECULAR
void Light::SetSpecularColor(const glm::vec3 &specularColor)
{
    SetVec3("light.specular", specularColor);
}
#endif

void Light::SetSpotAttenuation(glm::vec4 spotAttenuation)
{
    SetVec4("light.spotAttenuation", spotAttenuation);
}

Light::Type Light::GetType()
{
    return _type;
};
void Light::SetType(Light::Type type)
{
    _type = type;
}

void Light::SetInt(const std::string &name, int value)
{
    _data[name] = {LightValue::INT, {.i = value}};
}
void Light::SetFloat(const std::string &name, float value)
{
    _data[name] = {LightValue::FLOAT, {.f = value}};
}
void Light::SetVec2(const std::string &name, const glm::vec2 &value)
{
    _data[name] = {LightValue::VEC2, {.v2 = value}};
}
void Light::SetVec3(const std::string &name, const glm::vec3 &value)
{
    _data[name] = {LightValue::VEC3, {.v3 = value}};
}
void Light::SetVec4(const std::string &name, const glm::vec4 &value)
{
    _data[name] = {LightValue::VEC4, {.v4 = value}};
}
void Light::SetMat3(const std::string &name, const glm::mat3 &value)
{
    _data[name] = {LightValue::MAT3, {.m3 = value}};
}
void Light::SetMat4(const std::string &name, const glm::mat4 &value)
{
    _data[name] = {LightValue::MAT4, {.m4 = value}};
}

void Light::SetData(const size_t idx)
{
    _idx = idx;
    std::string i = std::to_string(_idx);

    SetInt("lights[" + i + "].type", _type);
    SetVec3("lights[" + i + "].color", LightColor);

    SetVec3("lights[" + i + "].ambient", LightAmbient);
    SetVec3("lights[" + i + "].diffuse", LightDiffuse);
    SetVec3("lights[" + i + "].specular", LightSpecular);

    if (_type == Point || _type == Spot)
    {
        SetFloat("lights[" + i + "].constant", Constant);
        SetFloat("lights[" + i + "].linear", Linear);
        SetFloat("lights[" + i + "].quadratic", Quadratic);

        SetFloat("lights[" + i + "].cutOff", CutOff);
        SetFloat("lights[" + i + "].outerCutOff", OuterCutOff);
    }
}
void Light::ApplyData(const Shader &shader) const
{
    for (const auto &[name, value] : _data)
    {
        const int location = shader.GetUniformLocationSafe(name);

        switch (value.type)
        {
            case LightValue::INT:
                Shader::SetInt(location, value.i);
            break;

            case LightValue::FLOAT:
                Shader::SetFloat(location, value.f);
            break;

            case LightValue::VEC2:
                Shader::SetVec2(location, value.v2);
            break;

            case LightValue::VEC3:
                Shader::SetVec3(location, value.v3);
            break;

            case LightValue::VEC4:
                Shader::SetVec4(location, value.v4);
            break;

            case LightValue::MAT3:
                Shader::SetMat3(location, value.m3);
            break;

            case LightValue::MAT4:
                Shader::SetMat4(location, value.m4);
            break;
        }
    }
}
