//----------------------------------------------------------------------------------------
/**
 * \file       MeshSource.h
 * \author     Ilia Timofeev
 * \date       2025/05/20
 * \brief      Container for raw mesh attribute and index data.
 *
 *  This file defines the MeshSource class, which stores raw vertex attribute
 *  arrays (positions, normals, tangents, texture coordinates) and optional index
 *  buffers. It offers setters and getters for each attribute, methods to update
 *  vertex/face counts, and queries for indexed vs. non-indexed geometry.
 *
 */
//----------------------------------------------------------------------------------------

#pragma once
#include <cstdint>

/// Class containing all necessary data to construct a mesh, possibly with indexing.
class MeshSource
{
public:
    MeshSource()
    : _vertexCount(0),
      _faceCount  (0),
      _texCoordChannelCount(0)
    {}
    explicit MeshSource(uint32_t vertexCount, uint32_t faceCount);

    void SetPositions(const float *positions);
    void SetNormals(const float *normals);
    void SetTangents(const float *tangents);
    void SetTexCoords(const float *texCoords, int channel);

    [[nodiscard]] uint32_t VertexCount() const;
    [[nodiscard]] uint32_t FaceCount() const;
    [[nodiscard]] const float *Positions() const;
    [[nodiscard]] const float *Normals() const;
    [[nodiscard]] const float *Tangents() const;
    [[nodiscard]] unsigned int TexCoordCount() const;
    [[nodiscard]] const float *TexCoords(int channel = 0) const;

    [[nodiscard]] bool IsIndexed() const;
    void SetIndices(const unsigned int *indices);
    [[nodiscard]] const unsigned int *Indices() const;

    void UpdateCounts()
    {
        _vertexCount = static_cast<uint32_t>(_positions.size() / 3);
        _faceCount   = IsIndexed() ? static_cast<uint32_t>(_indices.size() / 3)
                                   : _vertexCount / 3;
    }

    uint32_t _vertexCount;
    uint32_t _faceCount;

    std::vector<float> _positions;
    std::vector<float> _normals;
    std::vector<float> _uvs;
    std::vector<float> _tangents;
    std::array<std::vector<float>, 4> _texCoordChannels;
    unsigned int _texCoordChannelCount;

    std::vector<unsigned int> _indices;

    std::string diffusePath;
    std::string specularPath;
};
