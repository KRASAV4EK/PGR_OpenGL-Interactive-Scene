//----------------------------------------------------------------------------------------
/**
 * \file       Mesh.h
 * \author     Ilia Timofeev
 * \date       2025/05/20
 * \brief      OpenGL buffer management for 3D mesh data.
 *
 *  This file defines the Mesh class, which encapsulates creation, initialization,
 *  and destruction of OpenGL VAO, VBO, and optional EBO for rendering mesh data
 *  supplied by a MeshSource. It supports both indexed and non-indexed drawing
 *  and provides accessors for buffer handles and counts.
 *
 */
//----------------------------------------------------------------------------------------

#pragma once
#include "GL/glew.h"
#include "MeshSource.h"

// Features
// #define IMPL_MESH
// #define IMPL_MESH_ENABLE_TANGENT4

/**
 * @class Mesh
 * @brief Encapsulates OpenGL buffer management for a 3D mesh.
 *
 * The Mesh class handles creation and destruction of the VAO, VBO, and optional
 * EBO needed to render mesh data provided by a MeshSource. It supports both
 * indexed and non-indexed drawing.
 */
class Mesh
{
public:
    Mesh()  = default;
    ~Mesh() { DestroyGLBuffers(); }

    /**
     * @brief Allocate and initialize OpenGL buffers from source data.
     * @param src The MeshSource containing vertex attributes and optional indices.
     *
     * Creates a VAO, VBO (and EBO if indexed), uploads the data to the GPU,
     * and configures vertex attribute pointers.
     */
    void CreateGLBuffers(const MeshSource& src);
    /**
     * @brief Release all OpenGL buffers owned by this mesh.
     *
     * Deletes the VAO, VBO, and EBO if they have been created, and resets
     * internal state to reflect that no buffers are allocated.
     */
    void DestroyGLBuffers();

    [[nodiscard]] GLuint   VAO()        const { return _vao; }
    [[nodiscard]] bool     IsIndexed()  const { return _indexed; }
    [[nodiscard]] uint32_t VertexCount()const { return _vertexCount; }
    [[nodiscard]] uint32_t IndexCount() const { return _indexCount; }

private:
    GLuint   _vao   = 0;
    GLuint   _vbo   = 0;
    GLuint   _ebo   = 0;

    bool     _indexed      = false;
    uint32_t _vertexCount  = 0;
    uint32_t _indexCount   = 0;
};
