#include "GltfLoader.h"
#include <filesystem>
#include <tiny_gltf.h>

MeshSource GltfLoader::LoadMesh(const std::filesystem::path &path, bool binary)
{
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    bool ret;
    if (binary)
        ret = loader.LoadBinaryFromFile(&model, &err, &warn, ABSOLUTE_RESOURCE_PATH(path).string());
    else
        ret = loader.LoadASCIIFromFile(&model, &err, &warn, ABSOLUTE_RESOURCE_PATH(path).string());

    // Check loading
    if (!ret)
    {
        LOG_ERROR("{}", err);
        return MeshSource(0, 0);
    }
    if (!warn.empty())
        LOG_WARNING("Warning for '{}': {}", path.string(), warn);

    // Get mesh
    if (model.meshes.empty())
    {
        LOG_ERROR("No mesh in file.", path.string());
        return MeshSource(0, 0);
    }
    const tinygltf::Mesh &mesh = model.meshes[0];
    if (mesh.primitives.empty())
    {
        LOG_ERROR("No primitives in mesh '{}'.", mesh.name);
        return MeshSource(0, 0);
    }
    if (model.meshes.size() > 1)
        LOG_WARNING("Multiple meshes found, loading first mesh '{}'.", mesh.name);

    const tinygltf::Primitive &primitive = mesh.primitives[0];
    if (primitive.indices < 0)
    {
        LOG_ERROR("Primitives have no indices.");
        return MeshSource(0, 0);
    }

    // Load indices
    const tinygltf::Accessor &indexAccessor = model.accessors[primitive.indices];
    const tinygltf::BufferView &indexView = model.bufferViews[indexAccessor.bufferView];
    const tinygltf::Buffer &indexBuffer = model.buffers[indexView.buffer];
    const void *indexDataPtr = &indexBuffer.data[indexView.byteOffset + indexAccessor.byteOffset];
    std::vector<unsigned int> indices;
    switch (indexAccessor.componentType)
    {
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
            indices.assign(static_cast<const unsigned int *>(indexDataPtr), static_cast<const unsigned int *>(indexDataPtr) + indexAccessor.count);
            break;

        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
            for (size_t i = 0; i < indexAccessor.count; ++i)
                indices.push_back(static_cast<const uint16_t *>(indexDataPtr)[i]);
            break;

        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
            for (size_t i = 0; i < indexAccessor.count; ++i)
                indices.push_back(static_cast<const uint8_t *>(indexDataPtr)[i]);
            break;

        default:
            LOG_ERROR("Unsupported index component type.", path.string());
            return MeshSource(0, 0);
    }
    if (indices.size() % 3 != 0)
    {
        LOG_ERROR("Index count must be a multiple of 3.", path.string());
        return MeshSource(0, 0);
    }

    // Positions
    auto positions = LoadFloatAttribute("POSITION", 3, false, model, primitive);
    if (positions.empty())
        return MeshSource(0, 0);

    // Normals
    std::vector<float> normals = LoadFloatAttribute("NORMAL", 3, true, model, primitive);

    // Tangents
    std::vector<float> tangents4 = LoadFloatAttribute("TANGENT", 4, true, model, primitive);
#ifdef IMPL_MESH_ENABLE_TANGENT4
    std::vector<float> &tangents = tangents4;
#else
    std::vector<float> tangents;
    if (!tangents4.empty())
    {
        tangents.reserve(tangents4.size() / 4 * 3);
        for (size_t i = 0; i < tangents4.size() / 4; i++)
        {
            tangents[3 * i + 0] = tangents4[4 * i + 0];
            tangents[3 * i + 1] = tangents4[4 * i + 1];
            tangents[3 * i + 2] = tangents4[4 * i + 2];
        }
    }
#endif

    // Load UV channels
    std::vector<std::vector<float>> uvChannelsData;
    std::vector<int> uvChannels;
    for (int i = 0; i < 8; ++i)
    {
        std::vector<float> uvData = LoadFloatAttribute("TEXCOORD_" + std::to_string(i), 2, true, model, primitive);

        if (uvData.empty())
            break;

        uvChannelsData.push_back(std::move(uvData));
        uvChannels.push_back(i);
    }

    MeshSource meshSource(positions.size() / 3, indices.size() / 3);

    meshSource.SetPositions(positions.data());

    if (!normals.empty())
        meshSource.SetNormals(normals.data());

    if (!tangents.empty())
        meshSource.SetTangents(tangents.data());

    for (size_t i = 0; i < uvChannels.size(); ++i)
        meshSource.SetTexCoords(uvChannelsData[i].data(), uvChannels[i]);

    meshSource.SetIndices(indices.data());

    return meshSource;
}

std::vector<float> GltfLoader::LoadFloatAttribute(const std::string &attributeName, unsigned int components, bool optional, const tinygltf::Model &model,
                                                  const tinygltf::Primitive &primitive)
{
    std::vector<float> attribute;

    if (components == 0 || components > 4)
    {
        LOG_ERROR("Invalid component count {}.", components);
        return attribute;
    }

    // Get accessor
    if (!primitive.attributes.contains(attributeName))
    {
        if (optional)
            LOG_WARNING("Skipped missing mesh attribute '{}'.", attributeName);
        else
            LOG_ERROR("Mesh attribute '{}' is missing.", attributeName);

        return attribute;
    }
    const tinygltf::Accessor &attributeAccessor = model.accessors[primitive.attributes.at(attributeName)];
    if (attributeAccessor.type != components || attributeAccessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT)
    {
        LOG_ERROR("Unexpected format of mesh attribute '{}', expected {}.", attributeName, components == 1 ? "float" : "vec" + std::to_string(components));
        return attribute;
    }
    if (attributeAccessor.sparse.isSparse)
        LOG_WARNING("Sparse accessor is not supported, attribute '{}' may be incorrect.", attributeName);

    // Buffer
    if (attributeAccessor.bufferView < 0 || attributeAccessor.bufferView >= static_cast<int>(model.bufferViews.size()))
    {
        LOG_ERROR("Invalid buffer view index for attribute '{}'.", attributeName);
        return attribute;
    }
    const tinygltf::BufferView &attributeView = model.bufferViews[attributeAccessor.bufferView];
    if (attributeView.buffer < 0 || attributeView.buffer >= static_cast<int>(model.buffers.size()))
    {
        LOG_ERROR("Invalid buffer index for attribute '{}'.", attributeName);
        return attribute;
    }
    const tinygltf::Buffer &attributeBuffer = model.buffers[attributeView.buffer];

    // Stride and data offset
    size_t attributeStride = attributeView.byteStride;
    if (attributeStride == 0)
        attributeStride = components * sizeof(float);

    size_t byteOffset = attributeView.byteOffset + attributeAccessor.byteOffset;
    if (byteOffset + attributeAccessor.count * attributeStride > attributeBuffer.data.size())
    {
        LOG_ERROR("Buffer overflow on attribute '{}'.", attributeName);
        return attribute;
    }

    const unsigned char *attributeBufferStart = attributeBuffer.data.data() + byteOffset;

    attribute.resize(attributeAccessor.count * components);
    for (size_t i = 0; i < attributeAccessor.count; i++)
    {
        auto *sourceData = reinterpret_cast<const float *>(attributeBufferStart + i * attributeStride);

        for (size_t c = 0; c < components; c++)
            attribute[i * components + c] = sourceData[c];
    }

    return attribute;
}
