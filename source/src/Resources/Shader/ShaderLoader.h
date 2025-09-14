//----------------------------------------------------------------------------------------
/**
 * \file       ShaderLoader.h
 * \author     Ilia Timofeev
 * \date       2025/05/20
 * \brief      Loads and preprocesses GLSL shader source files.
 *
 *  This file defines the ShaderLoader class, which reads vertex and
 *  fragment shader files, processes custom #include directives recursively,
 *  and combines code into a ShaderSource object ready for compilation.
 *
 */
//----------------------------------------------------------------------------------------

#pragma once
#include "ShaderSource.h"
#define INCLUDE_DIRECTORY "res/Shaders"


class ShaderLoader
{
public:
    /// Load and process separate shader files into a ShaderSource.
    /// @param vertexPath Path to vertex shader file
    /// @param fragmentPath Path to fragment shader file
    /// @return ShaderSource with processed source code
    static ShaderSource LoadShaderSeparate(const std::filesystem::path &vertexPath,
                                           const std::filesystem::path &fragmentPath);

private:
    enum ShaderSourceType
    {
        Vertex,
        Fragment
    };

    /// Prepare a shader code source of a specific type for compilation.
    /// @param sourceStream Source code for the shader. Expected ordered layout:
    /// 1. includes
    /// 2. uniforms
    /// 3. functions
    /// 4. shader specific code (attributes, output, main, ...)
    /// @param type Type of the shader source
    /// @return ShaderSource for creating a Shader
    static std::string ProcessShaderSource(std::stringstream &sourceStream, ShaderSourceType type);

    /// Parse <code>#include</code> directives recrusively to generate a string with their source code,
    /// intended to replace the directives in order to make source file compilable.
    /// @param[in,out] shaderSource Shader source with include directives, will modify reading head to be after last
    /// include directive
    /// @return Source code of included files in required order
    static std::string GenerateIncludes(std::stringstream &shaderSource);
};
