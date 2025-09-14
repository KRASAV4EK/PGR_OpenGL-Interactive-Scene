//----------------------------------------------------------------------------------------
/**
 * \file       ShaderUtils.h
 * \author     Ilia Timofeev
 * \date       2025/05/20
 * \brief      Helper functions for shader compilation and linking.
 *
 *  This file defines the ShaderUtils class with static methods to compile
 *  GLSL code (vertex or fragment), link shader programs, and validate
 *  compilation/linking steps, reporting errors and warnings from the GPU.
 *
 */
//----------------------------------------------------------------------------------------

#pragma once
#include <GL/glew.h>

enum ShaderCodeType
{
    Vertex = GL_VERTEX_SHADER,
    Fragment = GL_FRAGMENT_SHADER,
};

class ShaderUtils
{
public:
    static unsigned int CompileShaderCode(ShaderCodeType type, const std::string &code)
    {
        auto id = glCreateShader(type);
        auto source = code.c_str();
        glShaderSource(id, 1, &source, nullptr);
        glCompileShader(id);

        if (!CheckShaderCompilation(id))
        {
            glDeleteShader(id);
            return 0;
        }

        return id;
    }

    static unsigned int LinkShader(unsigned int vertexShader, unsigned int fragmentShader)
    {
        auto id = glCreateProgram();
        glAttachShader(id, vertexShader);
        glAttachShader(id, fragmentShader);
        glLinkProgram(id);

        if (!CheckShaderLinking(id))
        {
            glDeleteProgram(id);
            return 0;
        }

        return id;
    }

    static bool CheckShaderCompilation(unsigned int shaderId)
    {
        int success;
        glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
        int shaderType;
        glGetShaderiv(shaderId, GL_SHADER_TYPE, &shaderType);
        int length;
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &length);

        if (length > 0)
        {
            const char *typeStr;
            switch (shaderType)
            {
                case GL_VERTEX_SHADER:
                    typeStr = "Vertex";
                    break;
                case GL_FRAGMENT_SHADER:
                    typeStr = "Fragment";
                    break;
                case GL_GEOMETRY_SHADER:
                    typeStr = "Geometry";
                    break;
                default:
                    typeStr = "Unspecified";
                    break;
            }

            char message[1024];
            glGetShaderInfoLog(shaderId, sizeof(message), &length, message);

            if (success != GL_TRUE)
            {
                LOG_ERROR("{} shader compilation error: {}", typeStr, message);
                return false;
            }
            else
                LOG_WARNING("{} shader compilation warning: {}", typeStr, message);
        }

        return true;
    }

    static bool CheckShaderLinking(unsigned int shaderProgramId)
    {
        glValidateProgram(shaderProgramId);
        int validationStatus = 0;
        glGetProgramiv(shaderProgramId, GL_VALIDATE_STATUS, &validationStatus);
        if (validationStatus != GL_TRUE)
        {
            int length;
            glGetProgramiv(shaderProgramId, GL_INFO_LOG_LENGTH, &length);
            char message[1024];
            glGetProgramInfoLog(shaderProgramId, sizeof(message), &length, message);
            LOG_ERROR("Shader linking error: {}", message);
            return false;
        }
        return true;
    }
};
