//----------------------------------------------------------------------------------------
/**
 * \file       MeshRenderer.h
 * \author     Ilia Timofeev
 * \date       2025/05/20
 * \brief      Renders a 3D mesh with an optional material using a shader.
 *
 *  This file defines the MeshRenderer class, which encapsulates a Mesh and Shader,
 *  and optionally a MaterialPGR. It manages binding the shader, applying material
 *  values when the linked shader matches, and issues OpenGL draw calls for the mesh.
 *
 */
//----------------------------------------------------------------------------------------

#pragma once
#include "../Resources/Material/MaterialPGR.h"
#include "../Resources/Mesh/Mesh.h"
#include "../Resources/Shader/Shader.h"

/**
 * @class MeshRenderer
 * @brief Wraps a Mesh, Shader and optional MaterialPGR to handle drawing.
 *
 * This class binds the provided Shader and, if available,
 * applies the associated Material before rendering the Mesh.
 */
class MeshRenderer
{
public:
    /**
     * @brief Construct a MeshRenderer with no material.
     * @param mesh Reference to the Mesh to render.
     * @param shader Reference to the Shader to use.
     */
    MeshRenderer(Mesh& mesh, Shader& shader)
        : _mesh(&mesh), _shader(&shader) {}
    /**
     * @brief Construct a MeshRenderer with an associated material.
     * @param mesh Reference to the Mesh to render.
     * @param shader Reference to the Shader to use.
     * @param material Reference to the MaterialPGR to apply before rendering.
     */
    MeshRenderer(Mesh& mesh, Shader& shader, MaterialPGR& material)
        : _mesh(&mesh), _shader(&shader), _material(&material) {}

    MeshRenderer(const MeshRenderer&)            = delete;
    MeshRenderer& operator=(const MeshRenderer&) = delete;
    MeshRenderer(MeshRenderer&&)                = default;
    MeshRenderer& operator=(MeshRenderer&&)     = default;

    /**
     * @brief Bind the shader and apply the material if it matches.
     * @param shader The Shader instance to bind.
     *
     * If a material was provided at construction and its shader
     * matches the one being bound, the material's values are applied.
     */
    void Bind(const Shader &shader) const
    {
        Shader::Bind(shader);

        if (_material && *_shader == shader)
            _material->ApplyValues();
    }

    [[nodiscard]] Mesh&     GetMesh()     const    { return *_mesh; }
    [[nodiscard]] Shader&   GetShader()   const    { return *_shader; }
    [[nodiscard]] MaterialPGR* GetMaterial() const { return _material; }

private:
    Mesh*        _mesh     = nullptr;
    Shader*      _shader   = nullptr;
    MaterialPGR* _material = nullptr;
};
