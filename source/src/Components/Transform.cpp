#include "Transform.h"

// Constructors
Transform::Transform() : _position(0.0f), _rotation(glm::quat()), _parent(nullptr) {}
Transform::Transform(float point) : _position(point), _rotation(glm::quat()), _parent(nullptr) {}
Transform::Transform(glm::vec3 position, glm::vec3 direction)
  : _position(position), _parent(nullptr)
{
    _rotation = glm::quatLookAt(normalize(direction), glm::vec3(0.0f, 1.0f, 0.0f));

    yaw   = glm::degrees(atan2(direction.z, direction.x));
    pitch = glm::degrees(asin(direction.y));
}
Transform::Transform(glm::vec3 position, float scale) : _position(position), _rotation(glm::quat()), _localScale(scale), _parent(nullptr) {}
Transform::Transform(const glm::mat4& m) : _position(0.0f), _parent(nullptr)
{
    glm::vec3   scale;          // sx, sy, sz
    glm::quat   rotation;
    glm::vec3   translation;
    glm::vec3   skew;  glm::vec4 persp;

    bool ok = decompose(m, scale, rotation, translation, skew, persp);

    // error check
    if (!ok) {
        _position   = glm::vec3(0);
        _rotation   = glm::quat(1,0,0,0);
        _localScale = 1.f;
        return;
    }

    _position   = translation;
    _rotation   = rotation;
    _localScale = (scale.x + scale.y + scale.z) / 3.f;   // mean
}

// Position
glm::vec3 Transform::GetStartPosition() const { return _startPosition; }
void Transform::SetStartPosition(glm::vec3 pos) { _startPosition = pos; }

glm::vec3 Transform::GetPosition() const { return _position; }
void Transform::SetPosition(glm::vec3 pos) { _position = pos; }

glm::vec3 Transform::GetWorldPosition() const {
    return glm::vec3( GetMatrix() * glm::vec4(0,0,0,1) );
}
glm::vec3 Transform::GetWorldForward() const {
    return glm::normalize(glm::vec3( GetMatrix() * glm::vec4(0,0,-1,0) ));
}
glm::vec3 Transform::GetWorldUp() const {
    return glm::normalize(glm::vec3( GetMatrix() * glm::vec4(0,1,0,0) ));
}

// Rotation
glm::quat Transform::GetRotation() const { return _rotation; }
void Transform::SetRotation(glm::quat q) { _rotation = q; }

// Scale
float Transform::GetLocalScale() const { return _localScale; }
void Transform::SetLocalScale(float scale) { _localScale = scale; }

// Matrices
glm::mat4 Transform::GetMatrix() const {
    glm::mat4 trans = glm::translate(glm::mat4(1.0f), _position);
    glm::mat4 rot = glm::toMat4(_rotation);
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(_localScale));
    glm::mat4 localM = trans * rot * scale;

    if (_parent)
    {
        return _parent->GetMatrix() * localM;
    }
    return localM;
}
glm::mat4 Transform::GetInverseMatrix() const {
    return glm::inverse(GetMatrix());
}

// Directions
glm::vec3 Transform::GetForward() const {
    return glm::normalize(_rotation * glm::vec3(0.0f, 0.0f, -1.0f));
}
void Transform::SetForward(const glm::vec3& direction) {
    _rotation = glm::normalize(glm::quatLookAt(direction, this->GetUp()));
}
glm::vec3 Transform::GetUp() const {
    return glm::normalize(_rotation * glm::vec3(0.0f, 1.0f, 0.0f));
}
glm::vec3 Transform::GetRight() const {
    return glm::normalize(_rotation * glm::vec3(1.0f, 0.0f, 0.0f));
}

// Rotations
void Transform::RotateToLookAt(const glm::vec3& lookPosition, const glm::vec3& up) {
    glm::vec3 direction = glm::normalize(lookPosition - _position);
    _rotation = glm::quatLookAt(direction, up);
}
void Transform::Rotate(const glm::vec3& axis, float angle) {
    _rotation = glm::rotate(_rotation, angle, axis);
}
void Transform::UpdateRotationFromEuler()
{
    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    glm::vec3 forward = glm::normalize(direction);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 right = glm::normalize(glm::cross(up, forward));
    up = glm::normalize(glm::cross(forward, right));

    _rotation = glm::quatLookAt(forward, up);
}

// Parent
#ifdef IMPL_TRANSFORM_PARENTING
Transform* Transform::GetParent() const { return _parent; }
void Transform::SetParent(Transform* p) { _parent = p; }

glm::vec3 Transform::GetLocalPosition() const {
    if (!_parent) return _position;
    return glm::vec3(glm::inverse(_parent->GetMatrix()) * glm::vec4(_position, 1.0f));
}
void Transform::SetLocalPosition(glm::vec3 localPosition) {
    if (!_parent) _position = localPosition;
    else _position = glm::vec3(_parent->GetMatrix() * glm::vec4(localPosition, 1.0f));
}

glm::quat Transform::GetLocalRotation() const {
    if (!_parent) return _rotation;
    return glm::inverse(_parent->GetRotation()) * _rotation;
}
void Transform::SetLocalRotation(glm::quat q) {
    if (!_parent) _rotation = q;
    else _rotation = _parent->GetRotation() * q;
}

void Transform::RotateLocal(const glm::vec3& axis, float angle) {
    glm::quat localQuat = glm::normalize(glm::angleAxis(angle, axis));
    _rotation = glm::normalize(_rotation * localQuat);
}
#endif

void Transform::SetData(const size_t idx)
{
    _idx = idx;
}
void Transform::ApplyData(const Shader &shader) const
{
    std::string i = std::to_string(_idx);
    int location = shader.GetUniformLocationSafe("lights[" + i + "].position");
    Shader::SetVec3(location, _position);
    location = shader.GetUniformLocationSafe("lights[" + i + "].direction");
    Shader::SetVec3(location, this->GetForward());
}
