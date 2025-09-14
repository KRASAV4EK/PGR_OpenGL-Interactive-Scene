//----------------------------------------------------------------------------------------
/**
 * \file       RenderObject.h
 * \author     Ilia Timofeev
 * \date       2025/05/20
 * \brief      Polymorphic encapsulation of renderable scene elements.
 *
 *  This file defines the RenderObject class, which encapsulates a Transform
 *  and one of several renderable types (Mesh, Box, Icosphere, CubeMap, Water,
 *  Fire, Cat). It provides a unified Render() method that dispatches to the
 *  appropriate type-specific rendering routine, handling shader binding,
 *  VAO setup, and material or texture parameters.
 *
 */
//----------------------------------------------------------------------------------------

#pragma once
#include <res/Models/Box/Box.h>
#include <res/Models/Cubemap/Cubemap.h>
#include <res/Models/Icosphere/Icosphere.h>
#include <res/Models/Water/Water.h>
#include <res/Models/Fire/Fire.h>
#include <res/Models/Cat/Cat.h>

#include <utility>

#include "../Components/MeshRenderer.h"
#include "../Components/Transform.h"

/**
 * @class RenderObject
 * @brief A polymorphic wrapper for various renderable types (meshes and primitives).
 *
 * RenderObject holds a transform and one of several renderable resources:
 * mesh, box, sphere, cubemap, water, fire, or cat. It exposes a unified
 * Render() method to draw the underlying resource with a given Shader.
 */
class RenderObject
{
public:
    /**
     * @enum Type
     * @brief Identifier for the kind of object stored.
     */
    enum class Type {
        Mesh,
        Box,
        Sphere,
        CubeMap,
        Water,
        Fire,
        CatType
    };

    RenderObject() = default;

    RenderObject(Transform const &transform, MeshRenderer &renderer) : _transform(transform), _renderer(&renderer), _type(Type::Mesh) {}
    RenderObject(Transform const &transform, const Box box) : _transform(transform), _box(box), _type(Type::Box) {}
    RenderObject(Transform const &transform, const Icosphere &icoSphere) : _transform(transform), _sphere(icoSphere), _type(Type::Sphere) {}
    RenderObject(Transform const &transform, CubeMap cubemap) : _transform(transform), _cubemap(std::move(cubemap)), _type(Type::CubeMap) {}
    RenderObject(Transform const &transform, const Water water) : _transform(transform), _water(water), _type(Type::Water) {}
    RenderObject(Transform const &transform, const Fire &fire) : _transform(transform), _fire(fire), _type(Type::Fire) {}
    RenderObject(Transform const &transform, const Cat cat) : _transform(transform), _cat(cat), _type(Type::CatType) {}

    [[nodiscard]] Transform &GetTransform()
    {
        return _transform;
    }
    [[nodiscard]] MeshRenderer &GetMeshRenderer() const
    {
        return *_renderer;
    }

    Box &GetBox()
    {
        return _box;
    }
    Icosphere &GetSphere()
    {
        return _sphere;
    }
    CubeMap &GetCubemap()
    {
        return _cubemap;
    }
    Water &GetWater()
    {
        return _water;
    }
    Fire &GetFire()
    {
        return _fire;
    }
    Cat &GetCat()
    {
        return _cat;
    }
    Type GetType() const
    {
        return _type;
    }

    /**
     * @brief Render the object with the provided shader.
     * @param shader Shader to bind and use for drawing.
     *
     * Delegates to the specific Render method based on Type.
     */
    void Render(const Shader &shader)
    {
        switch (_type)
        {
            case Type::Mesh:
                RenderMesh(shader);
                break;
            case Type::Box:
                RenderBox(shader);
                break;
            case Type::Sphere:
                RenderSphere(shader);
                break;
            case Type::CubeMap:
                RenderCubeMap(shader);
                break;
            case Type::Water:
                RenderWater(shader);
                break;
            case Type::Fire:
                RenderFire(shader);
                break;
            case Type::CatType:
                RenderCat(shader);
                break;
            default:
                break;
        }
    }

    void RenderMesh(const Shader &shader) const
    {
        auto &R = *_renderer;
        R.Bind(shader);
        Shader::SetMat4(shader._utils.ModelM,
                        _transform.GetMatrix());
        auto &M = R.GetMesh();
        glBindVertexArray(M.VAO());
        if (M.IsIndexed())
            glDrawElements(GL_TRIANGLES, M.IndexCount(),
                           GL_UNSIGNED_INT, nullptr);
        else
            glDrawArrays(GL_TRIANGLES, 0, M.VertexCount());
    }
    void RenderBox(const Shader &shader)
    {
        Shader::Bind(shader);
        // Connect box VAO
        glBindVertexArray(Box::VAO);

        // Set textures
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, Box::textureDiffID);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, Box::textureSpecID);

        Shader::SetInt(shader._utils.useTexture, Box::useTexture);
        Shader::SetMat4(shader._utils.ModelM, _transform.GetMatrix());

        glDrawArrays(GL_TRIANGLES, 0, Box::vertexCount);

        Shader::SetInt(shader._utils.useAlpha, false);
        Shader::SetFloat(shader._utils.alpha, 1.0f);

        // Reset texture using
        Shader::SetInt(shader._utils.useTexture, !Box::useTexture);
        glBindVertexArray(0);
    }
    void RenderSphere(const Shader &shader)
    {
        Shader::Bind(shader);

        // Connect sphere VAO
        glBindVertexArray(_sphere.VAO);

        // Set uniforms
        Shader::SetMat4(shader._utils.ModelM, _transform.GetMatrix());
        Shader::SetInt(shader._utils.useTexture, _sphere.useTexture);
        Shader::SetInt(shader._utils.useToSphere, true);
        if (Icosphere::useToSphere)
        {
            Icosphere::lastDynamicScale += 0.01;
            Shader::SetFloat(shader._utils.alphaToSphere, glm::sin(Icosphere::lastDynamicScale));
        }

        // Set textures
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, _sphere.textureDiffID);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, _sphere.textureSpecID);

        // Draw sphere
        glDrawArrays(GL_TRIANGLES, 0, _sphere.vertexCount);

        Shader::SetInt(shader._utils.useTexture, false);
        Shader::SetInt(shader._utils.useToSphere, false);
        glBindVertexArray(0);
    }
    void RenderCubeMap(const Shader &shader)
    {
        Shader::Bind(shader);

        // Connect cube map VAO
        glBindVertexArray(_cubemap.VAO);

        // Set uniforms
        Shader::SetInt(shader._utils.useCubeMap, true);
        Shader::SetInt(shader._utils.useTexture, _cubemap.useTexture);
        Shader::SetMat4(shader._utils.ModelM, _transform.GetMatrix());
        glDepthMask(GL_FALSE);
        glStencilMask(0x00);

        // Set textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, _cubemap.textureDiffID);
        glDrawArrays(GL_TRIANGLES, 0, _cubemap.vertexCount);

        // Reset uniforms
        Shader::SetInt(shader._utils.useCubeMap, false);
        glDepthMask(GL_TRUE);
        glStencilMask(0xFF);

        glBindVertexArray(0);
    }
    void RenderWater(const Shader &shader)
    {
        Shader::Bind(shader);

        const double t = glfwGetTime();
        Shader::SetFloat(shader._water.Time, static_cast<float>(t));

        _water.Render(shader);
    }
    void RenderFire(const Shader &shader)
    {
        Shader::SetMat4(shader._utils.ModelM, _transform.GetMatrix());
        _fire.Render(shader);
    }
    void RenderCat(const Shader &shader)
    {
        Shader::Bind(shader);

        // Connect cat VAO
        glBindVertexArray(Cat::VAO);

        // Set uniforms
        if (Cat::isMoving) UpdateCirclePosition(_transform, 0.5f, 1.0f);
        Shader::SetMat4(shader._utils.ModelM, _transform.GetMatrix());
        Shader::SetInt(shader._utils.useTexture, Cat::useTexture);
        // material.ApplyValues(); // Bronze

        // Draw cat
        glDrawArrays(GL_TRIANGLES, 0, Cat::vertexCount);

        glBindVertexArray(0);
    }

    /**
     * @brief Update an object's transform to move along a circle.
     * @param transform Transform to update (position and rotation).
     * @param radiusX Radius along the X axis.
     * @param radiusY Radius along the Z axis.
     *
     * This static helper animates a circular trajectory and orients
     * the object to face its direction of motion.
     */
    static void UpdateCirclePosition(Transform& transform, const float radiusX = 0.5f, const float radiusY = 0.5f)
    {
        transform.lastCircleAngle -= 0.01;

        glm::vec3 updPosition = glm::vec3(
            radiusX * std::cos(transform.lastCircleAngle),
            0.0f,
            radiusY * std::sin(transform.lastCircleAngle)
        );
        glm::vec3 direction = glm::normalize(glm::vec3(
            -radiusX * std::sin(transform.lastCircleAngle),
            0.0f,
            radiusY * std::cos(transform.lastCircleAngle)
        ));
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::quat rotation = glm::quatLookAt(direction, up);

        transform.SetPosition(transform.GetStartPosition() + updPosition);
        transform.SetRotation(rotation);
    }
private:
    Transform _transform;
    MeshRenderer*  _renderer = nullptr;

    Box _box;
    Icosphere _sphere;
    CubeMap _cubemap;
    Water _water;
    Fire _fire;
    Cat _cat;

    Type _type;
};
