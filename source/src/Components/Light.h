//----------------------------------------------------------------------------------------
/**
 * \file       Light.h
 * \author     Ilia Timofeev
 * \date       2025/05/20
 * \brief      Encapsulates various types of light sources for scene illumination.
 *
 *  This file defines the Light class, representing ambient, directional, point,
 *  and spot lights. It provides methods to configure color, intensity, attenuation
 *  parameters, and spot cone angles, and uploads light data to a Shader for real-time
 *  lighting calculations.
 *
 */
//----------------------------------------------------------------------------------------

#pragma once
#include "../Resources/Shader/Shader.h"

// Features
// #define IMPL_LIGHT_USE_COLOR_FOR_SPECULAR
// #define IMPL_LIGHT_USE_DEDICATED_AMBIENT_LIGHT

class Light
{
public:
    enum Type { Ambient, Direct, Point, Spot };
    const static inline std::string TypeNames[] = {"Ambient", "Direct", "Point", "Spot"};

private:
    struct LightValue
    {
        enum
        {
            INT,
            FLOAT,
            VEC2,
            VEC3,
            VEC4,
            MAT3,
            MAT4
        } type;
        union
        {
            int i;
            float f;
            glm::vec2 v2;
            glm::vec3 v3;
            glm::vec4 v4;
            glm::mat3 m3;
            glm::mat4 m4;
        };
    };

    Type _type;
    int  _idx = -2;
    std::unordered_map<std::string, LightValue> _data;

    static constexpr glm::vec3 LightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    static constexpr glm::vec3 LightAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
    static constexpr glm::vec3 LightDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    static constexpr glm::vec3 LightSpecular = glm::vec3(1.0f, 1.0f, 1.0f);

    static constexpr float Constant = 1.0f;
    static constexpr float Linear = 0.09f;
    static constexpr float Quadratic = 0.032f;
    static constexpr float CutOff = glm::cos(glm::radians(12.5f));
    static constexpr float OuterCutOff = glm::cos(glm::radians(17.5f));

public:
    Light(const Light& other) = default;
    explicit Light(Type type = Point);

    // Colors
    void SetColor(const glm::vec3 &color);
#ifndef IMPL_LIGHT_USE_DEDICATED_AMBIENT_LIGHT
    void SetAmbientColor(const glm::vec3 &ambientColor);
#endif
#ifndef IMPL_LIGHT_USE_COLOR_FOR_SPECULAR
    void SetSpecularColor(const glm::vec3 &specularColor);
#endif
    void SetSpotAttenuation(glm::vec4 spotAttenuation);

    Type GetType();
    void SetType(Type type);

    void SetInt(const std::string &name, int value);
    void SetFloat(const std::string &name, float value);
    void SetVec2(const std::string &name, const glm::vec2 &value);
    void SetVec3(const std::string &name, const glm::vec3 &value);
    void SetVec4(const std::string &name, const glm::vec4 &value);
    void SetMat3(const std::string &name, const glm::mat3 &value);
    void SetMat4(const std::string &name, const glm::mat4 &value);

    /**
     * @brief Set the index for shader array uniforms.
     * @param idx Zero-based index used in uniform names.
     */
    void SetData(const size_t idx);
    /**
     * @brief Upload light data to GPU via shader uniforms.
     * @param shader Shader instance to receive uniform updates.
     */
    void ApplyData(const Shader &shader) const;
};
