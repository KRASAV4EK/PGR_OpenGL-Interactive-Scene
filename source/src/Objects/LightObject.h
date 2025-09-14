//----------------------------------------------------------------------------------------
/**
 * \file       LightObject.h
 * \author     Ilia Timofeev
 * \date       2025/05/20
 * \brief      Wraps a Light with spatial transform for scene placement.
 *
 *  This file defines the LightObject class, which associates a Light instance
 *  with a Transform to position and orient it within the scene. It provides
 *  methods to configure light parameters and upload indexed light data to a
 *  Shader for real-time rendering.
 *
 */
//----------------------------------------------------------------------------------------
#pragma once
#include "../Components/Light.h"
#include "../Components/Transform.h"


class LightObject
{
private:
    const Shader *_shader;
    Transform  _ownTransform;
    Transform *_transform = nullptr;
    Light _light;

public:
    LightObject();
    LightObject(const Shader& shader, Transform* external, const Light& light);
    LightObject(const Shader& shader, const Transform copied, const Light& light);

    Transform       &GetTransform()       { return _ownTransform; }
    const Transform &GetTransform() const { return _ownTransform; }

    Light &GetLight();
    void SetLight(const Light &light);

    void SetData(const size_t idx);
    void ApplyData();
};
