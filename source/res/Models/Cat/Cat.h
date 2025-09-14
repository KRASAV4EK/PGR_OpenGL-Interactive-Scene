//----------------------------------------------------------------------------------------
/**
 * \file       Cat.h
 * \author     Ilia Timofeev
 * \date       2025/05/20
 * \brief      Loading and rendering of a 3D cat mesh.
 *
 *  This file defines the Cat class which loads a 3D cat model from a glTF file into
 *  OpenGL buffers. It sets up vertex array and buffer objects for positions and normals,
 *  configures vertex attribute pointers based on the provided Shader, and manages the
 *  vertex count for rendering the cat mesh. The class supports toggling texture usage.
 *
 */
//----------------------------------------------------------------------------------------

#pragma once
#ifndef CAT_H
#define CAT_H

#include "src/Resources/Mesh/MeshLoader.h"
#include "src/Resources/Shader/Shader.h"

class Cat
{
public:
    static unsigned int VAO, VBOPos, VBONorm;
    static bool isMoving;
    static constexpr bool useTexture = false;

    Cat() = default;

    static unsigned int vertexCount;

    static void LoadCat(const Shader &shader)
    {
        MeshSource catMesh = MeshLoader::LoadMesh("Models/Cat/cat.glb", false);

        vertexCount = catMesh.VertexCount();

        // Create VAO
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        // Create VBO for positions
        glGenBuffers(1, &VBOPos);
        glBindBuffer(GL_ARRAY_BUFFER, VBOPos);
        glBufferData(GL_ARRAY_BUFFER, vertexCount * 3 * sizeof(float), catMesh.Positions(), GL_STATIC_DRAW);
        glVertexAttribPointer(shader._utils.aPosition, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(shader._utils.aPosition);

        // Create VBO for normals
        if (catMesh.Normals() && shader._utils.aNormal != -1)
        {
            glGenBuffers(1, &VBONorm);
            glBindBuffer(GL_ARRAY_BUFFER, VBONorm);
            glBufferData(GL_ARRAY_BUFFER, vertexCount * 3 * sizeof(float), catMesh.Normals(), GL_STATIC_DRAW);
            glVertexAttribPointer(shader._utils.aNormal, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
            glEnableVertexAttribArray(shader._utils.aNormal);
        }

        //   layout(location = 3) vec2 aTexCoords; (fake)
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 0, (void *)0);

        // Disconnect VAO
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
};

#endif //CAT_H
