//----------------------------------------------------------------------------------------
/**
 * \file       Water.h
 * \author     Ilia Timofeev
 * \date       2025/05/20
 * \brief      Rendering of an animated water surface.
 *
 *  This file defines the Water class, which loads a water texture and prepares a plane
 *  mesh for rendering water surfaces. It creates OpenGL vertex array, buffer, and
 *  element buffer objects, configures attribute pointers for positions and UVs, and
 *  provides a Render method that applies stencil and depth mask settings.
 *
 */
//----------------------------------------------------------------------------------------

#pragma once
#ifndef WATER_H
#define WATER_H

#include <GL/glew.h>
#include "src/Resources/Shader/Shader.h"
#include "src/Resources/Texture/Texture.h"

class Water
{
public:
    unsigned int VAO = 0, VBO = 0, EBO = 0;
    unsigned int textureID = 0;

    static constexpr int indexCount = 6;

    Water() = default;

    void LoadWater()
    {
        Texture::LoadTextures(textureID, "res/Models/Water/Water.png");

        // Generate buffers
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        // --- VAO ---
        glBindVertexArray(VAO);

        // --- VBO ---
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(vertices),
                     vertices,
                     GL_STATIC_DRAW);

        // --- EBO ---
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     sizeof(indices),
                     indices,
                     GL_STATIC_DRAW);

        // layout(location = 0) vec3 aPosition;
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
                0,
                3,
                GL_FLOAT,
                GL_FALSE,
                5 * sizeof(float),
                (void*)0);

        // layout(location = 1) vec2 aTexCoords;
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(
                1,
                2,
                GL_FLOAT,
                GL_FALSE,
                5 * sizeof(float),
                (void*)(3 * sizeof(float)));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void Render(const Shader &shader) const
    {
        Shader::Bind(shader);

        glDepthMask(GL_FALSE);
        glStencilMask(0x00);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);

        glDepthMask(GL_TRUE);
        glStencilMask(0xFF);

        glBindVertexArray(0);
    }

private:

    static constexpr float vertices[20] = {
         // positions      // uv
        -1.f,  0.f, -1.f,  0.f, 0.f,
         1.f,  0.f, -1.f,  1.f, 0.f,
         1.f,  0.f,  1.f,  1.f, 1.f,
        -1.f,  0.f,  1.f,  0.f, 1.f,
    };

    static constexpr unsigned int indices[indexCount] = {
        0, 2, 1,
        2, 0, 3
    };
};

#endif // WATER_H
