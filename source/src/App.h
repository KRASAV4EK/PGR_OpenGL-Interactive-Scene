//----------------------------------------------------------------------------------------
/**
 * \file       App.h
 * \author     Ilia Timofeev
 * \date       2025/05/20
 * \brief      Core application class managing the rendering loop and input.
 *
 *  This file defines the App class, which centralizes application-wide
 *  configuration constants (window parameters, scene object templates,
 *  animation flags), and provides static methods for initializing the
 *  GLFW window and OpenGL state, responding to window resize and input
 *  events, updating per-frame logic, rendering the scene, and cleaning up
 *  resources on exit. It also declares the Input struct to track keyboard
 *  and mouse state.
 *
 */
//----------------------------------------------------------------------------------------

#pragma once
#include "Components/Transform.h"
#include "Objects/RenderObject.h"

struct GLFWwindow;

class App
{
public:
    static constexpr float MouseSensitivity = 0.01f;

    // Resolution
    static float WindowWidth;
    static float WindowHeight;

    // FOV and culling
    static constexpr float WindowFOV = 35.0f;
    static constexpr float WindowZNear = 0.1f;
    static constexpr float WindowZFar = 100.0f;

    // Title and window pointer
    static constexpr std::string WindowTitle = "PGR timofili";
    static GLFWwindow *_window;

    // Scene bounds
    static constexpr glm::vec3 minBounds = glm::vec3(-15.0f, 0.1f, -15.0f);
    static constexpr glm::vec3 maxBounds = glm::vec3(15.0f, 15.0f, 15.0f);

    // Modes
    static bool flyMode;
    static bool dynamicMode;

    // Use flags
    static bool useFog;
    static bool useFlashLight;

    // Indexes
    static int stencilIdx;
    static int stencilIdxLast;
    static int cameraIdx;

    // Templates for boxes ------------------------------------------------------------------------
    static constexpr float BoxBigScale = 1.0f;
    static constexpr float BoxMidScale = 0.6f;
    static constexpr float BoxSmlScale = 0.3f;

    static constexpr float BoxBigAlpha = 0.2f;
    static constexpr float BoxMidAlpha = 0.5f;
    static constexpr float BoxSmlAlpha = 0.8f;

    static constexpr glm::vec3 BoxBigADir = glm::vec3(0.0f, -0.5f, 1.0f);

    // Position of dynamic boxes
    static constexpr glm::vec3 BoxBigTPos = glm::vec3(-7.0f, 3.2f, 1.5f);
    static constexpr glm::vec3 BoxMidTPos = glm::vec3(0.0f, 0.8f, 0.0f);
    static constexpr glm::vec3 BoxSmlTPos = glm::vec3(0.64f, 0.0f, 0.0f);

    // Position of alpha boxes
    static constexpr glm::vec3 BoxBigAPos = glm::vec3(-5.0f, 3.0f, 3.0f);
    static constexpr glm::vec3 BoxMidAPos = glm::vec3(0.0f, 0.0f, 0.0f);
    static constexpr glm::vec3 BoxSmlAPos = glm::vec3(0.0f, 0.0f, 0.0f);
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    // Template for sphere ------------------------------------------------------------------------
    static constexpr float SphereScale = 1.0f;
    static constexpr glm::vec3 SpherePos = glm::vec3(-5.0f, 3.5f, -1.0f);
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    // Template for cube map ----------------------------------------------------------------------
    static constexpr float CubeMapScale = 100.0f;
    static constexpr glm::vec3 CubeMapPos = glm::vec3(0.0f, 0.0f, 0.0f);
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    // Template for water -------------------------------------------------------------------------
    static constexpr float WaterScale = 1000.0f;
    static constexpr float WaterAlpha = 0.6f;
    static constexpr glm::vec3 WaterPos = glm::vec3(0.0f, 0.0f, 0.0f);
    static constexpr glm::vec2 WaterDir = glm::vec2(0.01f, 0.005f);
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    // Template for fire --------------------------------------------------------------------------
    static constexpr int FireCols = 14;
    static constexpr int FireRows = 1;
    static constexpr float FireFrameDuration = 0.2f;

    static constexpr glm::vec3 FirePos = glm::vec3(-3.0f, 4.5f, -1.0f);
    static constexpr glm::vec3 FireDir = glm::vec3(0.0f, 1.0f, 0.0f);
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    // Template for cat ---------------------------------------------------------------------------
    static constexpr glm::vec3 CatPos = glm::vec3(1.1f, 4.2f, 0.6f);
    static constexpr float CatScale = 0.01f;
    static constexpr float CatMoveRadiusX = 0.5f;
    static constexpr float CatMoveRadiusY = 1.0f;
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    // Template for camera ------------------------------------------------------------------------
    static constexpr glm::vec3 CameraDynamicPos = glm::vec3(-3.5f, 5.5f, 8.0f);
    static constexpr glm::vec3 CameraDynamicDir = glm::vec3(0.0f, 0.0f, -1.0f);

    static constexpr glm::vec3 CameraStatic1Pos = glm::vec3(-12.0f, 10.0f, -10.0f);
    static constexpr glm::vec3 CameraStatic1Dir = glm::vec3(1.0f, -0.5f, 1.0f);

    static constexpr glm::vec3 CameraStatic2Pos = glm::vec3(-12.0f, 10.0f, 10.0f);
    static constexpr glm::vec3 CameraStatic2Dir = glm::vec3(1.0f, -0.5f, -1.0f);

    static constexpr glm::vec3 CameraStaticCatPos = glm::vec3(0.0f, 7.0f, -3.0f);
    static constexpr glm::vec3 CameraStaticCatDir = glm::vec3(0.0f, 0.0f, 1.0f);
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    // Template for collision ---------------------------------------------------------------------
    static constexpr float CollisionDistance = 1.5f;
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    // Template for fog ---------------------------------------------------------------------------
    static float FogColor;
    static float FogColorStep;
    static constexpr float FogColorMin = 0.2f;
    static constexpr float FogColorMax = 0.6f;
    static constexpr float FogStart = 3.0f;
    static constexpr float FogEnd = 10.0f;
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

private:

public:
    /// Called once at the start of the application after glfw has been initialized.
    static void InitWindow(GLFWwindow *window);

    /// Called when the size of the application window changes.
    /// @param width New window width
    /// @param height New window height
    static void OnResize(float width, float height);

    /// Called when a physical keyboard key is pressed or released.
    /// @param key The GLFW code of the key, starting with GLFW_KEY_
    /// @param pressed True if the key was just pressed or false if it was released
    static void OnKeyChanged(int key, bool pressed);

    /// Called when a mouse button is pressed or released.
    /// @param button The GLFW code of the button, starting with GLFW_MOUSE_BUTTON_
    /// @param pressed True if the button was just pressed or false if it was released
    static void OnMouseButtonChanged(int button, bool pressed);

    /// Called once before terminating glfw and exiting the application.
    static void End();

    /// Called every frame after polling for input and before rendering.
    static void Update();
    static void UpdateMouse();
    static void UpdateKeyboard();

    /// Called every frame after update.
    static void Render();
};

struct Input
{
    // Mouse actions
    bool mouseLooking = false;
    double lastMouseX = 0, lastMouseY = 0;
    double modelRotationX = 0, modelRotationY = 0;

    // Keyboard actions
    bool keyW = false, keyA = false, keyS = false, keyD = false;
    bool keyC = false, keySpace = false;
    bool keyCtrl  = false;
};
