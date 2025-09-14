//----------------------------------------------------------------------------------------
/**
 * \file       Box.h
 * \author     Ilia Timofeev
 * \date       2025/05/20
 * \brief      Declaration and setup of a textured cube primitive.
 *
 *  This file defines the Box class, which represents a textured cube primitive in the scene.
 *  It provides methods to load diffuse and specular textures, create OpenGL vertex array
 *  and buffer objects, and configure vertex attribute pointers for position, normals, and
 *  texture coordinates. The Box class supports multiple cube types and animation flags.
 *
 */
//----------------------------------------------------------------------------------------

#pragma once
#ifndef BOX_H
#define BOX_H

#include "src/Resources/Texture/Texture.h"

enum class TypeBox
{
    None,
    BoxBigT,
    BoxMidT,
    BoxSmlT,
    BoxBigA,
    BoxMidA,
    BoxSmlA
};

class Box
{
public:
    TypeBox _type = TypeBox::None;
    bool animFlag = false;
    static unsigned int VAO, VBO;
    static unsigned int textureDiffID, textureSpecID;
    static constexpr bool useTexture = true;

    Box() = default;
    Box(TypeBox type) : _type(type) {};

    static constexpr int vertexCount = 36;

    static void LoadBox()
    {
     // set textures
     Texture::LoadTextures(textureDiffID, "res/Models/Box/Diffuse.png");
     Texture::LoadTextures(textureSpecID, "res/Models/Box/Specular.png");

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
             8 * sizeof(float),  // stride (one vertex size)
             (void*)0);          // begin offset

     //   layout(location = 2) vec3 aNormal;
     glEnableVertexAttribArray(2);
     glVertexAttribPointer(
             2,
             3, GL_FLOAT,
             GL_FALSE,
             8 * sizeof(float),
             (void*)(3 * sizeof(float)));

     //   layout(location = 3) vec2 aTexCoords;
     glEnableVertexAttribArray(3);
     glVertexAttribPointer(
             3,
             2, GL_FLOAT,
             GL_FALSE,
             8 * sizeof(float),
             (void*)(6 * sizeof(float)));

     // disconnect VAO
     glBindBuffer(GL_ARRAY_BUFFER, 0);
     glBindVertexArray(0);
    }

    static const float vertices[288];
};

#endif //BOX_H
