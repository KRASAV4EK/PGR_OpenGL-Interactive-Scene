//----------------------------------------------------------------------------------------
/**
 * \file       CubeMap.h
 * \author     Ilia Timofeev
 * \date       2025/05/20
 * \brief      Environment cubemap loading and rendering.
 *
 *  This file declares the CubeMap class, responsible for loading six images into a
 *  cubemap texture and creating the vertex array and buffer objects to render a skybox.
 *  It sets texture parameters for seamless wrapping, and provides methods to generate
 *  and bind the appropriate VAO and VBO for cubemap rendering.
 *
 */
//----------------------------------------------------------------------------------------

#pragma once
#ifndef CUBEMAP_H
#define CUBEMAP_H

#include "src/Resources/Texture/Texture.h"
#include <iostream>
#include <vector>

class CubeMap
{
public:
    unsigned int VAO = 0, VBO = 0;
    unsigned int textureDiffID = 0;
    const bool useTexture = false;

    CubeMap() = default;

    const int vertexCount = 36;

    std::vector<std::string> faces = {
    "res/Models/Cubemap/skybox/right.jpg",    // +X
    "res/Models/Cubemap/skybox/left.jpg",     // -X
    "res/Models/Cubemap/skybox/top.jpg",      // +Y
    "res/Models/Cubemap/skybox/bottom.jpg",   // -Y
    "res/Models/Cubemap/skybox/front.jpg",    // -Z
    "res/Models/Cubemap/skybox/back.jpg"      // +Z
    };

    void LoadTextures()
    {
        glGenTextures(1, &textureDiffID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureDiffID);

        int width, height, nrChannels;
        for (unsigned int i = 0; i < faces.size(); i++)
        {
            unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
            if (data)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                           0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
                );
                stbi_image_free(data);
            }
            else
            {
                std::cout << "Cubemap texture loading failed: " << faces[i] << std::endl;
                stbi_image_free(data);
            }
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }
    void LoadCubeMap()
    {
        LoadTextures();

        // create VAO and VBO
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        // connect VAO and VBO
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        // insert buffer in VBO
        glBufferData(GL_ARRAY_BUFFER,
                    sizeof(vertices),
                    vertices,
                    GL_STATIC_DRAW);

        // set VBO pointers
        //   layout(location = 0) vec3 aPosition;
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
               0,                  // location in shader
               3, GL_FLOAT,    // vec3
               GL_FALSE,           // normalize
               3 * sizeof(float),  // stride (one vertex size)
               (void*)0);          // begin offset

        //   layout(location = 2) vec3 aNormal; (fake)
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(
              2,
              1, GL_FLOAT,
              GL_FALSE,
              0,
              (void*)0);

        //   layout(location = 3) vec2 aTexCoords; (fake)
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(
              3,
              1, GL_FLOAT,
              GL_FALSE,
              0,
              (void*)0);

        // disconnect VAO
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    float vertices[108] = {
      // positions
      -1.0f,  1.0f, -1.0f,
      -1.0f, -1.0f, -1.0f,
       1.0f, -1.0f, -1.0f,
       1.0f, -1.0f, -1.0f,
       1.0f,  1.0f, -1.0f,
      -1.0f,  1.0f, -1.0f,

      -1.0f, -1.0f,  1.0f,
      -1.0f, -1.0f, -1.0f,
      -1.0f,  1.0f, -1.0f,
      -1.0f,  1.0f, -1.0f,
      -1.0f,  1.0f,  1.0f,
      -1.0f, -1.0f,  1.0f,

       1.0f, -1.0f, -1.0f,
       1.0f, -1.0f,  1.0f,
       1.0f,  1.0f,  1.0f,
       1.0f,  1.0f,  1.0f,
       1.0f,  1.0f, -1.0f,
       1.0f, -1.0f, -1.0f,

      -1.0f, -1.0f,  1.0f,
      -1.0f,  1.0f,  1.0f,
       1.0f,  1.0f,  1.0f,
       1.0f,  1.0f,  1.0f,
       1.0f, -1.0f,  1.0f,
      -1.0f, -1.0f,  1.0f,

      -1.0f,  1.0f, -1.0f,
       1.0f,  1.0f, -1.0f,
       1.0f,  1.0f,  1.0f,
       1.0f,  1.0f,  1.0f,
      -1.0f,  1.0f,  1.0f,
      -1.0f,  1.0f, -1.0f,

      -1.0f, -1.0f, -1.0f,
      -1.0f, -1.0f,  1.0f,
       1.0f, -1.0f, -1.0f,
       1.0f, -1.0f, -1.0f,
      -1.0f, -1.0f,  1.0f,
       1.0f, -1.0f,  1.0f
    };
};

#endif //CUBEMAP_H
