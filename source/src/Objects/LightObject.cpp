#include "LightObject.h"

LightObject::LightObject() : _shader(nullptr), _transform(nullptr) {};

LightObject::LightObject(const Shader &shader, Transform *external, const Light &light)
    : _shader(&shader), _transform(external), _light(light) {}
LightObject::LightObject(const Shader& shader, Transform copied, const Light& light)
    : _shader(&shader), _ownTransform(copied), _light(light) {}

Light &LightObject::GetLight()
{
    return _light;
}
void LightObject::SetLight(const Light &light)
{
    _light = light;
}

void LightObject::SetData(const size_t idx)
{
    if (_transform != nullptr)
    {
        _transform->SetData(idx);
    } else
    {
        _ownTransform.SetData(idx);
    }
    _light.SetData(idx);
}
void LightObject::ApplyData()
{
    if (_transform != nullptr)
    {
        _transform->ApplyData(*_shader);
    } else
    {
        _ownTransform.ApplyData(*_shader);
    }
    _light.ApplyData(*_shader);
}