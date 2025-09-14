#include "MeshLoader.h"
#include <filesystem>
#include <glm/gtc/type_ptr.hpp>
#include <tiny_gltf.h>
#include "Loader/GltfLoader.h"
#include "src/Resources/Material/MaterialPGR.h"

MeshSource MeshLoader::LoadMesh(const std::filesystem::path &path, bool useIndices)
{
    auto extension = path.extension();
    MeshSource meshSource(0, 0);

    if (extension == ".glb")
        meshSource = GltfLoader::LoadMesh(path, true);
    else if (extension == ".gltf")
        meshSource = GltfLoader::LoadMesh(path, false);
    else
    {
        LOG_ERROR("Failed to load '{}', supported extensions are: glb, gltf.", path.string());
        return meshSource;
    }

    if (meshSource.VertexCount() == 0)
    {
        LOG_ERROR("Failed to load '{}'.", path.string());
        return meshSource;
    }

    // Expand when not indexed
    if (!useIndices)
    {
        auto indexCount = meshSource.FaceCount() * 3;
        MeshSource expandedMesh(indexCount, meshSource.FaceCount());

        std::vector<float> expandedPositions;
        expandedPositions.reserve(indexCount * 3);
        std::vector<float> expandedNormals;
        if (meshSource.Normals() != nullptr)
        {
            expandedNormals.reserve(indexCount * 3);
        }
        std::vector<float> expandedTangents;
        if (meshSource.Tangents() != nullptr)
        {
            expandedTangents.reserve(indexCount * 3);
        }

        std::vector<std::vector<float>> expandedTexCoords;
        for (unsigned int channel = 0; channel < meshSource.TexCoordCount(); channel++)
        {
            expandedTexCoords.emplace_back();
            expandedTexCoords[channel].reserve(indexCount * 2);
        }

        for (unsigned int i = 0; i < indexCount; i++)
        {
            auto baseIndex = meshSource.Indices()[i];

            expandedPositions.push_back(meshSource.Positions()[3 * baseIndex]);
            expandedPositions.push_back(meshSource.Positions()[3 * baseIndex + 1]);
            expandedPositions.push_back(meshSource.Positions()[3 * baseIndex + 2]);

            if (meshSource.Normals() != nullptr)
            {
                expandedNormals.push_back(meshSource.Normals()[3 * baseIndex]);
                expandedNormals.push_back(meshSource.Normals()[3 * baseIndex + 1]);
                expandedNormals.push_back(meshSource.Normals()[3 * baseIndex + 2]);
            }

            if (meshSource.Tangents() != nullptr)
            {
#ifdef IMPL_MESH_ENABLE_TANGENT4
                expandedTangents.push_back(meshSource.Tangents()[4 * baseIndex]);
                expandedTangents.push_back(meshSource.Tangents()[4 * baseIndex + 1]);
                expandedTangents.push_back(meshSource.Tangents()[4 * baseIndex + 2]);
                expandedTangents.push_back(meshSource.Tangents()[4 * baseIndex + 3]);
#else
                expandedTangents.push_back(meshSource.Tangents()[3 * baseIndex]);
                expandedTangents.push_back(meshSource.Tangents()[3 * baseIndex + 1]);
                expandedTangents.push_back(meshSource.Tangents()[3 * baseIndex + 2]);
#endif
            }

            for (unsigned int channel = 0; channel < meshSource.TexCoordCount(); channel++)
            {
                if (meshSource.TexCoords(channel) == nullptr)
                    break;

                expandedTexCoords[channel].push_back(meshSource.TexCoords(channel)[2 * baseIndex]);
                expandedTexCoords[channel].push_back(meshSource.TexCoords(channel)[2 * baseIndex + 1]);
            }
        }

        expandedMesh.SetPositions(expandedPositions.data());
        if (meshSource.Normals() != nullptr)
            expandedMesh.SetNormals(expandedNormals.data());
        if (meshSource.Tangents() != nullptr)
            expandedMesh.SetTangents(expandedTangents.data());
        for (unsigned int channel = 0; channel < meshSource.TexCoordCount(); channel++)
            if (!expandedTexCoords[channel].empty())
                expandedMesh.SetTexCoords(expandedTexCoords[channel].data(), channel);

        expandedMesh.UpdateCounts();
        return expandedMesh;
    }

    meshSource.UpdateCounts();
    return meshSource;
}

using namespace tinygltf;
namespace fs = std::filesystem;

static glm::mat4 ToGlm(const double* m) // glTF → glm
{
    return glm::transpose(glm::make_mat4(m)); // column-major
}

static void FillFloatAttr(const Model& mdl,
                          const Primitive& prim,
                          const std::string& name,
                          int comps,
                          std::vector<float>& dst)
{
    dst.clear();
    if (!prim.attributes.contains(name)) return;

    const Accessor&     acc = mdl.accessors[prim.attributes.at(name)];
    const BufferView& view  = mdl.bufferViews[acc.bufferView];
    const Buffer&      buf  = mdl.buffers[view.buffer];

    const size_t stride = view.byteStride ? view.byteStride : comps * sizeof(float);
    const uint8_t* base = buf.data.data() + view.byteOffset + acc.byteOffset;

    dst.resize(acc.count * comps);
    for (size_t i = 0; i < acc.count; ++i)
    {
        const float* src = reinterpret_cast<const float*>(base + i * stride);
        std::copy(src, src + comps, dst.begin() + i * comps);
    }
}

void FillPhongFromGltf(const tinygltf::Material& gMat,
                                       MaterialPGR& outMat)
{
    // albedo
    glm::vec3 albedo(1.0f);
    if (!gMat.pbrMetallicRoughness.baseColorFactor.empty())
        albedo = glm::make_vec3(gMat.pbrMetallicRoughness.baseColorFactor.data());

    // roughness & metallic
    float rough    = gMat.pbrMetallicRoughness.roughnessFactor;
    float metallic = gMat.pbrMetallicRoughness.metallicFactor;

    // diffuse / specular / shininess
    glm::vec3 diffuse  = albedo * (1.0f - metallic);

    // specular color
    glm::vec3 specular(1.0f);

    if (gMat.extensions.contains("KHR_materials_specular"))
    {
        const tinygltf::Value& ext =
            gMat.extensions.at("KHR_materials_specular");

        if (ext.IsObject() && ext.Has("specularColorFactor"))
        {
            const tinygltf::Value& arr = ext.Get("specularColorFactor");
            if (arr.IsArray() && arr.ArrayLen() >= 3)
            {
                specular = glm::vec3(
                    static_cast<float>(arr.Get(0).GetNumberAsDouble()),
                    static_cast<float>(arr.Get(1).GetNumberAsDouble()),
                    static_cast<float>(arr.Get(2).GetNumberAsDouble()));
            }
        }
    }
    else
    {
        specular = glm::mix(glm::vec3(1.0f), albedo, metallic);
    }

    float shininess = (1.0f - rough) * 256.0f;

    // save in material
    outMat.SetVec3 ("material.diffuse",   diffuse);
    outMat.SetVec3 ("material.specular",  specular);
    outMat.SetFloat("material.shininess", shininess);
}

static void ProcessNode(const Model& mdl,
                        const Shader& shader,
                        const Node&  node,
                        const glm::mat4& parentM,
                        std::vector<SceneMesh>& out,
                        const fs::path& dir)
{
    glm::mat4 local(1.f);

    if (node.matrix.size() == 16)
        local = ToGlm(node.matrix.data());
    else
    {
        glm::mat4 T(1.f), R(1.f), S(1.f);

        if (!node.translation.empty())
            T = glm::translate(glm::vec3(
                 node.translation[0], node.translation[1], node.translation[2]));

        if (!node.rotation.empty())
            R = glm::mat4_cast(glm::quat(
                 node.rotation[3], node.rotation[0],
                 node.rotation[1], node.rotation[2]));

        if (!node.scale.empty())
            S = glm::scale(glm::vec3(
                 node.scale[0], node.scale[1], node.scale[2]));

        local = T * R * S;
    }

    const glm::mat4 globalM = parentM * local;

    // if node contains mesh -> upload every primitive
    if (node.mesh >= 0)
    {
        const Mesh& mesh = mdl.meshes[node.mesh];

        for (const Primitive& prim : mesh.primitives)
        {
            if (prim.mode != TINYGLTF_MODE_TRIANGLES) continue;

            MeshSource ms;

            // attributes
            FillFloatAttr(mdl, prim, "POSITION",  3, ms._positions);
            FillFloatAttr(mdl, prim, "NORMAL",    3, ms._normals);
            FillFloatAttr(mdl, prim, "TEXCOORD_0",2, ms._uvs);
            FillFloatAttr(mdl, prim, "TANGENT",   4, ms._tangents);

            // indices
            if (prim.indices >= 0)
            {
                const Accessor& acc  = mdl.accessors[prim.indices];
                const BufferView& view = mdl.bufferViews[acc.bufferView];
                const Buffer& buf   = mdl.buffers[view.buffer];
                const uint8_t* base = buf.data.data() + view.byteOffset + acc.byteOffset;

                ms._indices.resize(acc.count);

                switch (acc.componentType)
                {
                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                        std::memcpy(ms._indices.data(), base, acc.count * sizeof(uint32_t));
                        break;
                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                        for (size_t i = 0; i < acc.count; ++i)
                            ms._indices[i] = reinterpret_cast<const uint16_t*>(base)[i];
                        break;
                    default:                                     // BYTE
                        for (size_t i = 0; i < acc.count; ++i)
                            ms._indices[i] = reinterpret_cast<const uint8_t*>(base)[i];
                        break;
                }
            }

            // material
            MaterialPGR mat(shader);

            if (prim.material >= 0)
            {
                const tinygltf::Material& gMat = mdl.materials[prim.material];
                FillPhongFromGltf(gMat, mat);
            }

            ms.UpdateCounts();

            // unload in sceneMesh
            SceneMesh sceneMesh;
            sceneMesh.meshSource = std::move(ms);
            sceneMesh.nodeMatrix = globalM;
            sceneMesh.material   = std::move(mat);

            out.push_back(std::move(sceneMesh));
        }
    }

    // children recursive
    for (int child : node.children)
        ProcessNode(mdl, shader, mdl.nodes[child], globalM, out, dir);
}


std::vector<SceneMesh> MeshLoader::LoadScene(const fs::path& file, const Shader& shader)
{
    TinyGLTF loader;
    Model    mdl;
    std::string err, warn;

    bool ok = file.extension() == ".glb"
              ? loader.LoadBinaryFromFile(&mdl, &err, &warn, file.string())
              : loader.LoadASCIIFromFile (&mdl, &err, &warn, file.string());

    if (!ok)  throw std::runtime_error("tinygltf: " + err);

    // select scene
    const Scene& scn = mdl.scenes.empty()     ? Scene{} :
                       mdl.defaultScene >= 0 ? mdl.scenes[mdl.defaultScene]
                                             : mdl.scenes[0];

    std::vector<SceneMesh> result;
    const fs::path dir = file.parent_path();

    for (int nodeIdx : scn.nodes)
        ProcessNode(mdl, shader, mdl.nodes[nodeIdx], glm::mat4(1.0f), result, dir);

    return result;
}
