#include "Mesh.h"

void Mesh::DestroyGLBuffers()
{
    if (_ebo) glDeleteBuffers(1, &_ebo);
    if (_vbo) glDeleteBuffers(1, &_vbo);
    if (_vao) glDeleteVertexArrays(1, &_vao);

    _vao = _vbo = _ebo = 0;
}

void Mesh::CreateGLBuffers(const MeshSource& src)
{
    DestroyGLBuffers();

    // save statistics
    _vertexCount = src.VertexCount();
    _indexCount  = static_cast<uint32_t>(src._indices.size());
    _indexed     = !_indexCount ? false : true;

    // set bools
    const bool hasN = !src._normals  .empty();
    const bool hasT = !src._tangents .empty();
    const bool hasUV= !src._uvs      .empty();

    const size_t stride =
          3                       /* pos */
        + (hasN  ? 3 : 0)
        + (hasUV ? 2 : 0)
        + (hasT  ? 3 : 0);

    std::vector<float> interleaved;
    interleaved.reserve(_vertexCount * stride);

    for (uint32_t v = 0; v < _vertexCount; ++v)
    {
        // pos
        interleaved.push_back(src._positions[v*3+0]);
        interleaved.push_back(src._positions[v*3+1]);
        interleaved.push_back(src._positions[v*3+2]);

        // normal (if exists)
        if (hasN)
        {
            interleaved.push_back(src._normals[v*3+0]);
            interleaved.push_back(src._normals[v*3+1]);
            interleaved.push_back(src._normals[v*3+2]);
        }

        // uv (if exists)
        if (hasUV)
        {
            interleaved.push_back(src._uvs[v*2+0]);
            interleaved.push_back(src._uvs[v*2+1]);
        }

        // tangent (if exists)
        if (hasT)
        {
            interleaved.push_back(src._tangents[v*3+0]);
            interleaved.push_back(src._tangents[v*3+1]);
            interleaved.push_back(src._tangents[v*3+2]);
        }
    }

    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    glGenBuffers(1, &_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 interleaved.size() * sizeof(float),
                 interleaved.data(),
                 GL_STATIC_DRAW);

    GLuint offset = 0;
    const GLuint strideBytes = stride * sizeof(float);

    // location 0 ─ position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          strideBytes, (void*)(offset));
    offset += 3 * sizeof(float);

    // location 1 ─ normal
    if (hasN)
    {
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                              strideBytes, (void*)(offset));
        offset += 3 * sizeof(float);
    }

    // location 2 ─ uv
    if (hasUV)
    {
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
                              strideBytes, (void*)(offset));
        offset += 2 * sizeof(float);
    }

    // location 3 ─ tangent
    if (hasT)
    {
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE,
                              strideBytes, (void*)(offset));
        offset += 3 * sizeof(float);
    }

    // create EBO
    if (_indexed)
    {
        glGenBuffers(1, &_ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     src._indices.size() * sizeof(unsigned int),
                     src._indices.data(),
                     GL_STATIC_DRAW);
    }

    // disconnect
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
