#include "MeshSource.h"

MeshSource::MeshSource(uint32_t vertexCount, uint32_t faceCount) :
    _vertexCount(vertexCount), _faceCount(faceCount), _texCoordChannelCount(0)
{}

void MeshSource::SetPositions(const float *positions)
{
    _positions.assign(positions, positions + _vertexCount * 3);
}

void MeshSource::SetNormals(const float *normals)
{
    _normals.assign(normals, normals + _vertexCount * 3);
}

void MeshSource::SetTangents(const float *tangents)
{
#ifdef IMPL_MESH_ENABLE_TANGENT4
    _tangents.assign(tangents, tangents + _vertexCount * 4);
#else
    _tangents.assign(tangents, tangents + _vertexCount * 3);
#endif
}

void MeshSource::SetTexCoords(const float *texCoords, int channel)
{
    if (channel < 0 || channel > 3)
    {
        LOG_ERROR("Channel {} out of range [0, {}].", channel, _texCoordChannels.size());
        return;
    }

    if (channel > _texCoordChannelCount)
        _texCoordChannelCount = channel;

    _texCoordChannels[channel].assign(texCoords, texCoords + _vertexCount * 2);
}

uint32_t MeshSource::VertexCount() const
{
    return _vertexCount;
}

uint32_t MeshSource::FaceCount() const
{
    return _faceCount;
}

const float *MeshSource::Positions() const
{
    return _positions.data();
}

const float *MeshSource::Normals() const
{
    return _normals.data();
}

const float *MeshSource::Tangents() const
{
    return _tangents.data();
}

unsigned int MeshSource::TexCoordCount() const
{
    return _texCoordChannelCount;
}

const float *MeshSource::TexCoords(int channel) const
{
    return _texCoordChannels[channel].data();
}

bool MeshSource::IsIndexed() const
{
    return !_indices.empty();
}

void MeshSource::SetIndices(const unsigned int *indices)
{
    _indices.assign(indices, indices + _faceCount * 3);
}

const unsigned int *MeshSource::Indices() const
{
    return _indices.data();
}
