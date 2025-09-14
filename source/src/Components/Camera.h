//----------------------------------------------------------------------------------------
/**
 * \file       Camera.h
 * \author     Ilia Timofeev
 * \date       2025/05/20
 * \brief      Manages view and projection matrices for a virtual camera.
 *
 *  This file defines the Camera class, which links to a Transform to derive its
 *  position and orientation, computes view and projection matrices, and supports
 *  configuring perspective parameters (FOV, aspect ratio, near/far planes) as well
 *  as adjustable horizontal and vertical movement speeds.
 *
 */
//----------------------------------------------------------------------------------------

#pragma once
#define HORIZONTAL_TO_VERTICAL_FOV(horizontalFov, aspectRatio) glm::degrees(2 * atan(tan(glm::radians((float)horizontalFov) / 2.0) / (aspectRatio)))
#include "Transform.h"


class Camera
{
public:
    static constexpr float DEFAULT_NEAR = 0.1f;
    static constexpr float DEFAULT_FAR = 1000.0f;

    static constexpr float DEFAULT_SPEED_H = 0.05f;
    static constexpr float DEFAULT_SPEED_V = 0.02f;

    enum Type
    {
        Perspective,
        Orthographic
    };
    const static inline std::string TypeNames[] = {"Perspective", "Orthographic"};

private:
    Type _type;

    float _near = DEFAULT_NEAR;
    float _far = DEFAULT_FAR;

    float _speedHor = DEFAULT_SPEED_H;
    float _speedVer = DEFAULT_SPEED_V;

    glm::mat4 _projection = glm::mat4(1.0f);
    Transform* _transform = nullptr;

public:
    Camera(Type type, float near, float far);
    Camera();

    /// Link the Camera to a Transform which will define its view transformation.
    /// @param transform The Transform to link to.
    void LinkTransform(Transform &transform);

    /// Sets the camera's projection matrix based on its type.
    /// @param aspectRatio The ratio of width to height for the projection.
    /// @param verticalFovOrScale The vertical field of view in degrees for Perspective, or
    /// the vertical world-space radius for orthographic projection (visible range of [-scale, scale] on the vertical axis).
    void SetProjection(float aspectRatio, float verticalFovOrScale);

    /// Sets the camera's general projection matrix based on its type.
    /// @param left viewport left
    /// @param right viewport right
    /// @param bottom viewport bottom
    /// @param top viewport top
    void SetGeneralProjection(float left, float right, float bottom, float top);

    // Speed
    [[nodiscard]] float GetSpeedHorizontal() const;
    void SetSpeedHorizontal(float speed);
    [[nodiscard]] float GetSpeedVertical() const;
    void SetSpeedVertical(float speed);

    [[nodiscard]] Type GetType() const;

    // Matrices
    [[nodiscard]] glm::mat4 GetViewMatrix() const;
    [[nodiscard]] glm::mat4 GetProjectionMatrix() const;
};
