//----------------------------------------------------------------------------------------
/**
 * \file       MaterialPGR.h
 * \author     Ilia Timofeev
 * \date       2025/05/20
 * \brief      Implementation of a simple built-in material for the PGR renderer.
 *
 *  This file defines the MaterialPGR class, which encapsulates common material
 *  properties such as diffuse color, specular color, and shininess, and allows
 *  setting arbitrary uniform values by name. It provides methods to initialize
 *  default PBR-like values and apply all stored uniforms to an associated Shader
 *  for rendering.
 *
 */
//----------------------------------------------------------------------------------------

#pragma once
#include "../Shader/Shader.h"

#define USE_BUILTIN_MATERIAL

#if defined(USE_BUILTIN_MATERIAL) && defined(IMPL_SHADER)

/**
 * @class MaterialPGR
 * @brief Implements a simple built-in material for the PGR renderer.
 *
 * MaterialPGR encapsulates common material properties (diffuse, specular, shininess)
 * and allows setting arbitrary uniform values by name. Once configured, it can
 * upload all stored values to the associated Shader in one call.
 */
class MaterialPGR
{
public:

private:
    static constexpr glm::vec3 Diffuse = glm::vec3(1.0f, 0.5f, 0.31f);
    static constexpr glm::vec3 Specular = glm::vec3(0.5f, 0.5f, 0.5f);
    static constexpr float     Shininess = 32.0f;

    struct MaterialValue
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
    const Shader *_shader;
    std::unordered_map<std::string, MaterialValue> _values;

public:
    MaterialPGR() = default;
    explicit MaterialPGR(const Shader &shader);

    void SetInt(const std::string &name, int value);
    void SetFloat(const std::string &name, float value);
    void SetVec2(const std::string &name, const glm::vec2 &value);
    void SetVec3(const std::string &name, const glm::vec3 &value);
    void SetVec4(const std::string &name, const glm::vec4 &value);
    void SetMat3(const std::string &name, const glm::mat3 &value);
    void SetMat4(const std::string &name, const glm::mat4 &value);

    /**
     * @brief Initialize the material with default PBR-like values.
     *
     * Populates the internal uniform map with default
     * Diffuse, Specular, and Shininess settings.
     */
    void SetValues();
    /**
     * @brief Apply all stored uniform values to the Shader.
     *
     * Iterates over the internal map of names and values, binding
     * each to the associated Shader via the appropriate uniform calls.
     */
    void ApplyValues() const;

private:
};
#endif
