#include "Camera.h"

Camera::Camera(Type type, float near, float far)
    : _type(type), _near(near), _far(far) {}

Camera::Camera() : Camera(Type::Perspective, DEFAULT_NEAR, DEFAULT_FAR) {}

void Camera::LinkTransform(Transform &transform) {
    _transform = &transform;
}

void Camera::SetProjection(float aspectRatio, float verticalFovOrScale) {
    if (_type == Perspective) {
        _projection = glm::perspective(glm::radians(verticalFovOrScale), aspectRatio, _near, _far);
    } else if (_type == Orthographic) {
        float top = verticalFovOrScale;
        float bottom = -verticalFovOrScale;
        float right = verticalFovOrScale * aspectRatio;
        float left = -right;
        _projection = glm::ortho(left, right, bottom, top, _near, _far);
    }
}

void Camera::SetGeneralProjection(float left, float right, float bottom, float top) {
    if (_type == Perspective) {
        // Optional: not typically used for perspective this way
    } else if (_type == Orthographic) {
        _projection = glm::ortho(left, right, bottom, top, _near, _far);
    }
}

Camera::Type Camera::GetType() const {
    return _type;
}

float Camera::GetSpeedHorizontal() const { return _speedHor; }
void Camera::SetSpeedHorizontal(float speed) { _speedHor = speed; }

float Camera::GetSpeedVertical() const { return _speedVer; }
void Camera::SetSpeedVertical(float speed) { _speedVer = speed; }

glm::mat4 Camera::GetViewMatrix() const {
    if (!_transform) return glm::mat4(1.0f);
    glm::vec3 pos     = _transform->GetWorldPosition();
    glm::vec3 forward = _transform->GetWorldForward();
    glm::vec3 up      = _transform->GetWorldUp();
    return glm::lookAt(pos, pos + forward, up);
}


glm::mat4 Camera::GetProjectionMatrix() const {
    return _projection;
}
