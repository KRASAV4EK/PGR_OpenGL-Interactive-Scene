//----------------------------------------------------------------------------------------
/**
 * \file       Transform.h
 * \author     Ilia Timofeev
 * \date       2025/05/20
 * \brief      Represents a 3D transform with position, rotation, scale, and parenting.
 *
 *  This file defines the Transform class, which encapsulates local and world-space
 *  transformations. It provides methods to set and retrieve position, rotation (quaternion
 *  or Euler), and scale, compute model and inverse matrices, handle yaw/pitch updates,
 *  and support hierarchical parent–child relationships for scene graph management.
 *
 */
//----------------------------------------------------------------------------------------

#pragma once

#include "src/Resources/Shader/Shader.h"
#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>

// Features
#define IMPL_TRANSFORM_PARENTING
#define  GLM_ENABLE_EXPERIMENTAL

/**
 * @class Transform
 * @brief Represents a 3D transform with position, rotation, scale, and optional parenting.
 *
 * Encapsulates local and world transforms, provides utilities for
 * building model matrices, handling Euler angles, and parent–child relationships.
 */
class Transform
{
public:
    float yaw = -90.0f;
    float pitch = 0.0f;

    double lastCircleAngle = 0.0f;

private:
    size_t _idx = 0;
    glm::vec3 _position;
    glm::vec3 _startPosition = _position;
    glm::quat _rotation;
    float _localScale = 1.0f;
    Transform* _parent;

public:
    Transform();
    Transform(Transform &other) = default;
    Transform(const Transform &other) = default;
    explicit Transform(float point);
    explicit Transform(glm::vec3 position, glm::vec3 direction = glm::vec3(0.0f, 0.0f, -1.0f));
    explicit Transform(glm::vec3 position, float scale);
    explicit Transform(const glm::mat4& m);

    // Position
    /**
     * @brief Get the initial “start” position (for animations).
     * @return Start position in local space.
     */
    [[nodiscard]] glm::vec3 GetStartPosition() const;
    /**
     * @brief Set the initial “start” position (for animations).
     * @param pos New start position in local space.
     */
    void SetStartPosition(glm::vec3 pos);

    [[nodiscard]] glm::vec3 GetPosition() const;
    void SetPosition(glm::vec3 pos);

    // Rotation
    [[nodiscard]] glm::quat GetRotation() const;
    void SetRotation(glm::quat q);

    // Scale
    [[nodiscard]] float GetLocalScale() const;
    void SetLocalScale(float localScale);

#ifdef IMPL_TRANSFORM_PARENTING
    [[nodiscard]] Transform *GetParent() const;
    void SetParent(Transform *parent);

    [[nodiscard]] glm::vec3 GetLocalPosition() const;
    void SetLocalPosition(glm::vec3 localPosition);

    [[nodiscard]] glm::quat GetLocalRotation() const;
    void SetLocalRotation(glm::quat q);

    void RotateLocal(const glm::vec3& axis, float angle);
#endif

    // Matrices
    /**
     * @brief Compute the 4×4 model matrix (including parent chain).
     * @return Homogeneous transformation matrix.
     */
    [[nodiscard]] glm::mat4 GetMatrix() const;
    /**
     * @brief Compute the inverse of the model matrix.
     * @return Inverse homogeneous transformation matrix.
     */
    [[nodiscard]] glm::mat4 GetInverseMatrix() const;

    // Directions
    [[nodiscard]] glm::vec3 GetForward() const;
    void SetForward(const glm::vec3& direction);
    [[nodiscard]] glm::vec3 GetUp() const;
    [[nodiscard]] glm::vec3 GetRight() const;

    /**
     * @brief Get world-space position (including parent transforms).
     * @return World-space position vector.
     */
    [[nodiscard]] glm::vec3 GetWorldPosition() const;
    /**
     * @brief Get world-space forward direction.
     * @return Normalized forward vector in world space.
     */
    [[nodiscard]] glm::vec3 GetWorldForward() const;
    /**
     * @brief Get world-space up direction.
     * @return Normalized up vector in world space.
     */
    [[nodiscard]] glm::vec3 GetWorldUp() const;

    // Movement
    void RotateToLookAt(const glm::vec3 &lookPosition, const glm::vec3 &up);
    void Rotate(const glm::vec3 &axis, float angle);
    void UpdateRotationFromEuler();

    /**
     * @brief Set the index for shader array uniforms.
     * @param idx Zero-based index used in uniform names.
     */
    void SetData(const size_t idx);
    /**
     * @brief Upload transform data to GPU via shader uniforms.
     * @param shader Shader instance to receive uniform updates.
     */
    void ApplyData(const Shader &shader) const;
};
