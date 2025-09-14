//----------------------------------------------------------------------------------------
/**
 * \file       GltfLoader.h
 * \author     Ilia Timofeev
 * \date       2025/05/20
 * \brief      Helper class for loading mesh data from glTF/GLB files.
 *
 *  This file declares the GltfLoader class, which leverages tinygltf to parse
 *  glTF (.gltf) and binary GLB (.glb) model files. It extracts vertex attributes
 *  and optional index data into a MeshSource, handling missing attributes and
 *  component counts.
 *
 */
//----------------------------------------------------------------------------------------

#ifndef GLTFLOADER_H
#define GLTFLOADER_H
#include <tiny_gltf.h>
#include "src/Resources/Mesh/MeshSource.h"


class GltfLoader
{
public:
    static MeshSource LoadMesh(const std::filesystem::path &path, bool binary);

private:
    static std::vector<float> LoadFloatAttribute(const std::string &attributeName, unsigned int components, bool optional, const tinygltf::Model &model, const tinygltf::Primitive &primitive);
};



#endif
