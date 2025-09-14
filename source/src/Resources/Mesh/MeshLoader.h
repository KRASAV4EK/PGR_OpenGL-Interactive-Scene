//----------------------------------------------------------------------------------------
/**
 * \file       MeshLoader.h
 * \author     Ilia Timofeev
 * \date       2025/05/20
 * \brief      Utilities for loading individual meshes and full scenes.
 *
 *  This file defines the SceneMesh struct and the MeshLoader class. SceneMesh
 *  represents a single mesh primitive with its geometry (MeshSource), node
 *  transform matrix, and material. MeshLoader provides static methods to load
 *  either a standalone mesh or an entire glTF/GLB scene into SceneMesh instances.
 *
 */
//----------------------------------------------------------------------------------------

#pragma once
#include <tiny_gltf.h>
#include "MeshSource.h"
#include "src/Resources/Material/MaterialPGR.h"

/**
 * @struct SceneMesh
 * @brief Represents a single mesh instance in a loaded scene, including its geometry, transform, and material.
 */
struct SceneMesh
{
    MeshSource   meshSource;
    glm::mat4    nodeMatrix;
    MaterialPGR     material;
};

/**
 * @class MeshLoader
 * @brief Utility class for loading mesh and scene data from external model files.
 *
 * MeshLoader provides static functions to load either a standalone mesh (exactly one primitive)
 * or an entire glTF/GLB scene (potentially multiple meshes, each with its own transform and material).
 */
class MeshLoader
{
public:
    /**
     * @brief Load a single mesh from a file into a MeshSource.
     *
     * This function reads the specified model file, which must contain exactly one mesh primitive,
     * and populates a MeshSource object with its vertex attributes and (optionally) index data.
     *
     * @param path        Filesystem path to the mesh file (e.g., .obj, .gltf, .glb) containing exactly one mesh.
     * @param useIndices  If true, the returned MeshSource will include the original index buffer;
     *                    if false, indices will be expanded into unique vertices.
     * @return            A MeshSource containing the loaded mesh data.
     */
    static MeshSource LoadMesh(const std::filesystem::path &path, bool useIndices);
    /**
     * @brief Load a full scene from a glTF or binary GLB file.
     *
     * Parses the given .gltf or .glb file and extracts all mesh primitives along with their
     * node transformation matrices and associated materials. Each primitive becomes one SceneMesh.
     *
     * @param gltfOrGlb   Filesystem path to the glTF (.gltf) or binary GLB (.glb) scene file.
     * @param shader      Reference to a Shader used to initialize MaterialPGR for each mesh.
     * @return            A std::vector of SceneMesh entries, one per mesh primitive in the scene.
     */
    static std::vector<SceneMesh> LoadScene(const std::filesystem::path& gltfOrGlb, const Shader& shader);
};
