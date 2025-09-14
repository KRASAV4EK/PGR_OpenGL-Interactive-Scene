//----------------------------------------------------------------------------------------
/**
 * \file       Fire.h
 * \author     Ilia Timofeev
 * \date       2025/05/20
 * \brief      Animated fire effect using a texture atlas.
 *
 *  This file defines the Fire class, which loads a fire texture atlas and sets up a
 *  quad geometry with VBO, VAO, and EBO. It handles frame-based animation by computing
 *  the current frame based on elapsed time and passes it to the shader. The class
 *  configures blending for transparent fire rendering.
 *
 */
//----------------------------------------------------------------------------------------

#pragma once
#include "src/Resources/Shader/Shader.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include "src/Resources/Texture/Texture.h"

class Fire {
public:
    Fire() = default;
    Fire(int cols, int rows, float frameDuration)
     : cols(cols), rows(rows), frameDuration(frameDuration)
    {}

    int   cols = 0, rows = 0;
    float frameDuration = 0.0f;
    unsigned int textureID = 0;
    unsigned int VAO = 0, VBO = 0, EBO = 0;

    const int vertexCount = 6;

    static bool pointFlag;
    static constexpr int indexCount = 6;
    static constexpr float vertices[20] = {
    // position          // uv
    -0.5f, 0.0f, -0.5f,  0.0f, 0.0f,
     0.5f, 0.0f, -0.5f,  1.0f, 0.0f,
     0.5f, 0.0f,  0.5f,  1.0f, 1.0f,
    -0.5f, 0.0f,  0.5f,  0.0f, 1.0f,
    };
    static constexpr unsigned indices[indexCount] = {
    0, 2, 1,
    2, 0, 3
    };

    // Load texture and set VAO/VBO/EBO
    void LoadFire() {
        Texture::LoadTextures(textureID, "res/Models/Fire/Fire.png");

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // aPosition
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                              5*sizeof(float), (void*)0);

        //   aNormal (fake)
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE,
                        0, (void*)0);

        // aTexCoord
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE,
                              5*sizeof(float), (void*)(3*sizeof(float)));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void Render(const Shader& shader) const {
        Shader::Bind(shader);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        int frame = int(float(glfwGetTime()) / frameDuration);
        frame %= cols * rows;

        // Set uniforms
        Shader::SetInt(shader._utils.useFire, true);
        Shader::SetInt(shader._utils.frame, frame);

        // Set texture
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, textureID);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);

        Shader::SetInt(shader._utils.useFire, false);
        glBindVertexArray(0);
    }
};
