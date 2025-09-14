//----------------------------------------------------------------------------------------
/**
 * \file       Shader.h
 * \author     Ilia Timofeev
 * \date       2025/05/20
 * \brief      Encapsulates an OpenGL shader program and uniform setup.
 *
 *  This file declares the Shader class, which wraps creation of a GLSL
 *  program from a ShaderSource, caches attribute and uniform locations
 *  in Utils and UtilsWater structs, and provides static methods to bind
 *  the program and set uniform values of various types.
 *
 */
//----------------------------------------------------------------------------------------

#pragma once
#include "GL/glew.h"
#include "ShaderSource.h"

// Features
#define IMPL_SHADER

/**
 * @struct Utils
 * @brief Holds common attribute and uniform locations for generic shader programs.
 */
struct Utils
{
    // Position
    int aPosition = -1;
    int ViewPosition = -1;
    int aNormal = -1;

    // Matrices
    int ModelM = -1;
    int ViewM = -1;
    int ProjectionM = -1;

    // Textures
    int useCubeMap = -1;
    int useTexture = -1;
    int diffMap = -1;
    int specMap = -1;
    int aTexCoords = -1;
    int cubeMap = -1;

    // Lights
    int lightCount = -1;

    // Fog
    int useFog = -1;
    int fogColor = -1;
    int fogStart = -1;
    int fogEnd = -1;

    // Fire
    int useFire = -1;
    int fireMap = -1;
    int frame = -1;

    // Alpha
    int useAlpha = -1;
    int alpha = -1;

    // Light flags
    int useFlashLight = -1;
    int useFireLight = -1;

    // Sphere flags
    int useToSphere = -1;
    int alphaToSphere = -1;
};

/**
 * @struct UtilsWater
 * @brief Holds uniform locations specific to water shader programs.
 */
struct UtilsWater
{
    int WaterTexture = -1;
    int Time = -1;
    int ScrollSpeed = -1;
};

/**
 * @class Shader
 * @brief Encapsulates an OpenGL shader program, including compilation, binding, and uniform management.
 *
 * Shader wraps creation of a GLSL program from vertex and fragment sources,
 * provides safe lookup of attribute and uniform locations, and static helpers
 * to set uniform values. Common locations are cached in the Utils and UtilsWater structs.
 */
class Shader
{
public:
    Utils _utils;
    UtilsWater _water;

private:
    unsigned int _id;

public:
    Shader() = default;
    explicit Shader(const ShaderSource &shaderSource);
    ~Shader();

    // Disable moving and copying for simplicity
    Shader(const Shader &other) = delete;
    Shader(Shader &&other) = delete;
    Shader &operator=(const Shader &other) = delete;

    // Implement move assignment operator for live reload
    Shader &operator=(Shader &&other) noexcept;
    bool operator==(const Shader & shader) const;

    // Static
    static void Bind(const Shader &shader);
    static void Delete(const Shader &shader);
    static void SetInt(int location, int value);
    static void SetFloat(int location, float value);
    static void SetVec2(int location, const glm::vec2 &value);
    static void SetVec3(int location, const glm::vec3 &value);
    static void SetVec4(int location, const glm::vec4 &value);
    static void SetMat3(int location, const glm::mat3 &value);
    static void SetMat4(int location, const glm::mat4 &value);

    // Getters
    static int GetAttribLocationSafe(const unsigned int &shaderID, const std::string &name);
    [[nodiscard]] int GetAttribLocationSafe(const std::string &name) const;

    static int GetUniformLocationSafe(const unsigned int &shaderID, const std::string &name);
    [[nodiscard]] int GetUniformLocationSafe(const std::string &name) const;

    [[nodiscard]] int GetUniformLocation(const std::string &name) const;

    [[nodiscard]] unsigned int &GetID();

    /**
     * @brief Query and cache standard uniform/attribute locations for a general-purpose shader.
     */
    void Load()
    {
        // Positions
        _utils.aPosition = GetAttribLocationSafe("aPosition");
        _utils.ViewPosition = GetUniformLocationSafe("viewPos");
        _utils.aNormal = GetAttribLocationSafe("aNormal");

        // Matrices
        _utils.ModelM = GetUniformLocationSafe("ModelM");
        _utils.ViewM = GetUniformLocationSafe("ViewM");
        _utils.ProjectionM = GetUniformLocationSafe("ProjectionM");

        // Textures
        _utils.useTexture = GetUniformLocationSafe("material.useTexture");
        _utils.diffMap = GetUniformLocationSafe("material.diffuseMap");
        _utils.specMap = GetUniformLocationSafe("material.specularMap");
        _utils.aTexCoords = GetAttribLocationSafe("aTexCoords");

        _utils.useCubeMap = GetUniformLocationSafe("useCubeMap");
        _utils.cubeMap = GetUniformLocationSafe("cubeMap");

        // Lights
        _utils.lightCount = GetUniformLocationSafe("lightCount");

        // Fog
        _utils.useFog = GetUniformLocationSafe("useFog");
        _utils.fogColor = GetUniformLocationSafe("fogColor");
        _utils.fogStart = GetUniformLocationSafe("fogStart");
        _utils.fogEnd = GetUniformLocationSafe("fogEnd");

        // Fire
        _utils.useFire = GetUniformLocationSafe("useFire");
        _utils.fireMap = GetUniformLocationSafe("fireMap");
        _utils.frame = GetUniformLocationSafe("frame");

        // Alpha
        _utils.useAlpha = GetUniformLocationSafe("useAlpha");
        _utils.alpha = GetUniformLocationSafe("alpha");

        // FlashLight
        _utils.useFlashLight = GetUniformLocationSafe("useFlashLight");
        _utils.useFireLight = GetUniformLocationSafe("useFireLight");

        // Sphere
        _utils.useToSphere = GetUniformLocationSafe("useToSphere");
        _utils.alphaToSphere = GetUniformLocationSafe("alphaToSphere");
    }
    /**
     * @brief Query and cache uniform/attribute locations for the water shader variant.
     */
    void LoadWater()
    {
        // Positions
        _utils.aPosition = GetAttribLocationSafe("aPosition");
        _utils.ViewPosition = GetUniformLocationSafe("viewPos");
        _utils.aTexCoords = GetAttribLocationSafe("aTexCoords");

        // Matrices
        _utils.ModelM = GetUniformLocationSafe("ModelM");
        _utils.ViewM = GetUniformLocationSafe("ViewM");
        _utils.ProjectionM = GetUniformLocationSafe("ProjectionM");

        // Textures
        _water.WaterTexture = GetUniformLocationSafe("WaterTexture");
        _water.Time = GetUniformLocationSafe("Time");
        _water.ScrollSpeed = GetUniformLocationSafe("ScrollSpeed");
        _utils.alpha = GetUniformLocationSafe("Alpha");

        // Fog
        _utils.useFog = GetUniformLocationSafe("useFog");
        _utils.fogColor = GetUniformLocationSafe("fogColor");
        _utils.fogStart = GetUniformLocationSafe("fogStart");
        _utils.fogEnd = GetUniformLocationSafe("fogEnd");
    }
    /**
     * @brief Query and cache uniform/attribute locations for a simple (white) shader.
     */
    void LoadWhite()
    {
        // Positions
        _utils.aPosition = GetAttribLocationSafe("aPosition");
        // Matrices
        _utils.ModelM = GetUniformLocationSafe("ModelM");
        _utils.ViewM = GetUniformLocationSafe("ViewM");
        _utils.ProjectionM = GetUniformLocationSafe("ProjectionM");
    }

    /**
     * @brief Bind texture units to sampler uniforms for the standard shader.
     */
    void LinkTextures() const
    {
        glUseProgram(_id);
        Shader::SetInt(_utils.cubeMap, 0);
        Shader::SetInt(_utils.diffMap, 1);
        Shader::SetInt(_utils.specMap, 2);
        Shader::SetInt(_utils.fireMap, 3);
    }
    /**
     * @brief Bind texture unit to the water normal map sampler.
     */
    void LinkTexturesWater() const
    {
        glUseProgram(_id);
        Shader::SetInt(_water.WaterTexture, 0);
    }
};
