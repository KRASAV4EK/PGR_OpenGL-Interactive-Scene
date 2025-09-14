//----------------------------------------------------------------------------------------
/**
 * \file       ShaderSource.h
 * \author     Ilia Timofeev
 * \date       2025/05/20
 * \brief      Holds vertex and fragment GLSL source code.
 *
 *  This file declares the ShaderSource class, a simple container for
 *  vertex and fragment shader strings. It provides getters for both
 *  source strings to be used in shader compilation.
 *
 */
//----------------------------------------------------------------------------------------

#pragma once


/// Class containing all necessary data to construct a Shader
class ShaderSource
{
public:
    ShaderSource(std::string vertexSource, std::string fragmentSource);

    const std::string &GetVertexSource() const;
    const std::string &GetFragmentSource() const;

private:
    std::string _vertexSource;
    std::string _fragmentSource;
};
