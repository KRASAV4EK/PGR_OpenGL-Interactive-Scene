//----------------------------------------------------------------------------------------
/**
 * \file       GlfwUtils.h
 * \author     Ilia Timofeev
 * \date       2025/05/20
 * \brief      Utility class for translating OpenGL debug enums to strings.
 *
 *  This file defines the GlfwUtils class, providing static helper methods
 *  DebugSourceToString and DebugTypeToString that convert OpenGL debug
 *  source and type enumeration values into human-readable C strings.
 *  These utilities facilitate logging and debugging of OpenGL messages.
 *
 */
//----------------------------------------------------------------------------------------

#pragma once
#include <GL/glew.h>

class GlfwUtils
{
public:
    static const char *DebugSourceToString(GLenum source)
    {
        switch (source)
        {
            case GL_DEBUG_SOURCE_API:
                return "API";
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
                return "Window System";
            case GL_DEBUG_SOURCE_SHADER_COMPILER:
                return "Shader Compiler";
            case GL_DEBUG_SOURCE_THIRD_PARTY:
                return "Third Party";
            case GL_DEBUG_SOURCE_APPLICATION:
                return "Application";
            case GL_DEBUG_SOURCE_OTHER:
                return "Other";
            default:
                return "Unknown";
        }
    }

    static const char *DebugTypeToString(GLenum type)
    {
        switch (type)
        {
            case GL_DEBUG_TYPE_ERROR:
                return "Error";
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
                return "Deprecated";
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
                return "Undefined";
            case GL_DEBUG_TYPE_PORTABILITY:
                return "Portability";
            case GL_DEBUG_TYPE_PERFORMANCE:
                return "Performance";
            case GL_DEBUG_TYPE_MARKER:
                return "Marker";
            case GL_DEBUG_TYPE_PUSH_GROUP:
                return "Push Group";
            case GL_DEBUG_TYPE_POP_GROUP:
                return "Pop Group";
            case GL_DEBUG_TYPE_OTHER:
                return "Other";
            default:
                return "Unknown";
        }
    }
};
