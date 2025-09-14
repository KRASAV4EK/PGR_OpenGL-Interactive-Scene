//----------------------------------------------------------------------------------------
/**
 * \file       CameraObject.h
 * \author     Ilia Timofeev
 * \date       2025/05/20
 * \brief      Camera controller with dynamic and preset transforms.
 *
 *  This file defines the CameraObject class, which manages a Camera and a
 *  collection of Transform presets for dynamic movement or fixed viewpoints.
 *  It allows linking a Transform to the Camera for view matrix updates and
 *  updating the camera position in six cardinal directions when in dynamic mode.
 *
 */
//----------------------------------------------------------------------------------------

#pragma once
#include "../Components/Camera.h"
#include "../Components/Transform.h"
#include "src/App.h"

/**
 * @enum Direction
 * @brief Enumerates possible movement directions for the camera.
 */
typedef enum
{
    FRONT = 0,
    LEFT = 1,
    BACK = 2,
    RIGHT = 3,
    UP = 4,
    DOWN = 5,
} Direction;

/**
 * @class CameraObject
 * @brief Wraps a Camera and its Transform, supporting dynamic and static view modes.
 *
 * Manages multiple preset transforms (dynamic and static) and allows switching
 * between them, as well as moving in six cardinal directions when in dynamic mode.
 */
class CameraObject
{
public:
    CameraObject()
    {
        _transforms.emplace_back(App::CameraDynamicPos, App::CameraDynamicDir);
        _transform = _transforms[0];
        LinkTransform();
        _transforms.emplace_back(App::CameraStatic1Pos, App::CameraStatic1Dir);
        _transforms.emplace_back(App::CameraStatic2Pos, App::CameraStatic2Dir);
        _transforms.emplace_back(App::CameraStaticCatPos, App::CameraStaticCatDir);
    }

    /**
     * @brief Link the current transform to the internal Camera instance.
     *
     * Must be called whenever _transform is replaced or modified directly.
     */
    void LinkTransform()
    {
        _camera.LinkTransform(_transform);
    }

    /**
     * @brief Move the dynamic camera transform in the specified direction.
     * @param dir Direction enum value indicating which way to move.
     *
     * Uses the camera's horizontal and vertical speeds to update position.
     */
    void UpdateTransform(Direction dir)
    {
        glm::vec3 position = _transform.GetPosition();
        float speedHor = _camera.GetSpeedHorizontal();
        float speedVer = _camera.GetSpeedVertical();
        switch (dir)
        {
            case FRONT:
                position += _transform.GetForward() * speedHor;
            break;
            case BACK:
                position -= _transform.GetForward() * speedHor;
            break;
            case LEFT:
                position -= _transform.GetRight() * speedHor;
            break;
            case RIGHT:
                position += _transform.GetRight() * speedHor;
            break;
            case UP:
                position.y += speedVer;
            break;
            case DOWN:
                position.y -= speedVer;
            break;
            default:
                break;
        }
        _transform.SetPosition(position);
    }

    Transform &GetTransform()
    {
        return _transform;
    }
    void SetTransform(Transform &transform)
    {
        _transform = transform;
        LinkTransform();
    }

    Camera &GetCamera()
    {
        return _camera;
    }

    std::vector<Transform> &GetTransforms()
    {
        return _transforms;
    }

    /**
     * @brief Attach the fourth (static cat view) transform as a child of another transform.
     * @param transform Parent Transform to which the cat camera should be attached.
     *
     * Enables following the cat object by parenting the camera's transform.
     */
    void SetStaticParent(Transform &transform)
    {
        _transforms[3].SetParent(&transform);
    }

private:
    Transform _transform;
    Camera _camera;

    std::vector<Transform> _transforms = {};
};
