#include "App.h"
// OpenGL
#include <GL/glew.h>
#include <GLFW/glfw3.h>
// Objects
#include "Objects/CameraObject.h"
#include "Objects/LightObject.h"
#include "Objects/RenderObject.h"
// Loaders
#include "Resources/Mesh/MeshLoader.h"
#include "Resources/Shader/ShaderLoader.h"
// Models
#include "res/Models/Box/Box.h"
#include "res/Models/Icosphere/Icosphere.h"
#include "res/Models/Cubemap/CubeMap.h"
// Import
#include <tiny_gltf.h>

Input input;
GLFWwindow *App::_window = nullptr;

// Resolution
float App::WindowWidth = 1920.0f;
float App::WindowHeight = 1080.0f;
// float App::WindowWidth = 2560.0f;
// float App::WindowHeight = 1600.0f;
// float App::WindowWidth = 3840.0f;
// float App::WindowHeight = 2160.0f;

// Modes
bool App::flyMode = true;
bool App::dynamicMode = true;
// Use flags
bool App::useFog = false;
bool App::useFlashLight = false;
// Indexes
int App::stencilIdx = -1;
int App::stencilIdxLast = -1;
int App::cameraIdx = 0;
// Fog
float App::FogColor = 0.5f;
float App::FogColorStep = 0.001f;

// Meshes and models
std::vector<SceneMesh> scene;
std::vector<std::shared_ptr<Mesh>>         Meshes;
std::vector<std::shared_ptr<MeshRenderer>> Renderers;
std::vector<std::shared_ptr<RenderObject>> RenderObjects;

// Shaders, materials, light
Shader shader;
Shader shaderWater;
Shader shaderWhite;
MaterialPGR material;
std::vector<LightObject> lightObjects;

// Camera
CameraObject cameraObject;

// Dynamic boxes
auto boxObjBigT = std::make_shared<RenderObject>(Transform(App::BoxBigTPos, App::BoxBigScale), Box(TypeBox::BoxBigT));
auto boxObjMidT = std::make_shared<RenderObject>(Transform(App::BoxMidTPos, App::BoxMidScale), Box(TypeBox::BoxMidT));
auto boxObjSmlT = std::make_shared<RenderObject>(Transform(App::BoxSmlTPos, App::BoxSmlScale), Box(TypeBox::BoxSmlT));

// Alpha boxes
auto boxObjBigA = std::make_shared<RenderObject>(Transform(App::BoxBigAPos, App::BoxBigScale), Box(TypeBox::BoxBigA));
auto boxObjMidA = std::make_shared<RenderObject>(Transform(App::BoxMidAPos, App::BoxMidScale), Box(TypeBox::BoxMidA));
auto boxObjSmlA = std::make_shared<RenderObject>(Transform(App::BoxSmlAPos, App::BoxSmlScale), Box(TypeBox::BoxSmlA));

// Other objects
auto sphereObj  = std::make_shared<RenderObject>(Transform(App::SpherePos,  App::SphereScale), Icosphere());
auto cubeMapObj = std::make_shared<RenderObject>(Transform(App::CubeMapPos, App::CubeMapScale), CubeMap());

auto waterObj   = std::make_shared<RenderObject>(Transform(App::WaterPos,   App::WaterScale), Water());
auto fireObj    = std::make_shared<RenderObject>(Transform(App::FirePos,   App::FireDir),
                               Fire(App::FireCols,App::FireRows,App::FireFrameDuration));

auto catObj = std::make_shared<RenderObject>(Transform(App::CatPos, App::CatScale), Cat());

void LoadShaders()
{
    auto shaderSource = ShaderLoader::LoadShaderSeparate("Shaders/Shader_V.glsl", "Shaders/Shader_F.glsl");
    shader = Shader(shaderSource);
    shader.Load();
    shader.LinkTextures();

    shaderSource = ShaderLoader::LoadShaderSeparate("Shaders/Water_V.glsl", "Shaders/Water_F.glsl");
    shaderWater = Shader(shaderSource);
    shaderWater.LoadWater();
    shaderWater.LinkTexturesWater();

    shaderSource = ShaderLoader::LoadShaderSeparate("Shaders/White_V.glsl", "Shaders/White_F.glsl");
    shaderWhite = Shader(shaderSource);
    shaderWhite.LoadWhite();
}
void LoadObjects()
{
    // Materials
    material = MaterialPGR(shader);
    material.SetValues(); // Bronze

    lightObjects.emplace_back(shader, &cameraObject.GetTransform(), Light(Light::Spot));
    lightObjects.emplace_back(shader, Transform(glm::vec3(-3.0f, 4.5f, -1.0f)), Light(Light::Point));
    lightObjects.emplace_back(shader, Transform(), Light(Light::Ambient));
    lightObjects.emplace_back(shader, Transform(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, -1.0f, 0.0f)), Light(Light::Direct));
    lightObjects.emplace_back(shader, Transform(glm::vec3(0.0f, 3.0f, 0.0f)), Light(Light::Point));

    for (size_t i = 0; i < lightObjects.size(); i++)
    {
        lightObjects[i].SetData(i);
    }

    scene = MeshLoader::LoadScene("res/Models/Scene/scene.glb", shader);
    for (auto& sceneMesh : scene)
    {
        // Create mesh based from MeshLoader
        auto mesh = std::make_unique<Mesh>();
        mesh->CreateGLBuffers(sceneMesh.meshSource);

        // Create meshRenderer from mesh, shader and material
        auto meshRenderer = std::make_unique<MeshRenderer>(*mesh, shader, sceneMesh.material);

        // Create transform from matrix
        Transform transformFromMatrix(sceneMesh.nodeMatrix);

        // Fill vectors
        RenderObjects.emplace_back(std::make_unique<RenderObject>(transformFromMatrix, *meshRenderer));
        Meshes.emplace_back(std::move(mesh));
        Renderers.emplace_back(std::move(meshRenderer));
    }

    Cat::LoadCat(shader);
    cameraObject.SetStaticParent(catObj.get()->GetTransform());
    RenderObjects.emplace_back(catObj);

    sphereObj->GetSphere().LoadSphere();
    RenderObjects.emplace_back(sphereObj);

    fireObj->GetFire().LoadFire();
    RenderObjects.emplace_back(fireObj);

    cubeMapObj->GetCubemap().LoadCubeMap();
    RenderObjects.emplace_back(cubeMapObj);

    waterObj->GetWater().LoadWater();
    RenderObjects.emplace_back(waterObj);

    boxObjBigT->GetBox().LoadBox();
    boxObjMidT->GetTransform().SetParent(&boxObjBigT->GetTransform());
    boxObjSmlT->GetTransform().SetParent(&boxObjMidT->GetTransform());
    boxObjBigA->GetTransform().SetForward(App::BoxBigADir);
    boxObjMidA->GetTransform().SetParent(&boxObjBigA->GetTransform());
    boxObjSmlA->GetTransform().SetParent(&boxObjMidA->GetTransform());
    RenderObjects.emplace_back(boxObjBigT);
    RenderObjects.emplace_back(boxObjMidT);
    RenderObjects.emplace_back(boxObjSmlT);
    RenderObjects.emplace_back(boxObjSmlA);
    RenderObjects.emplace_back(boxObjMidA);
    RenderObjects.emplace_back(boxObjBigA);
}

void App::InitWindow(GLFWwindow* window)
{
    _window = window;

    // Set common settings
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set shaders
    LoadShaders();

    // Set objects
    LoadObjects();
}

void App::Update()
{
    App::UpdateMouse();
    App::UpdateKeyboard();
}
void App::UpdateMouse()
{
    if (input.mouseLooking)
    {
        // Get cursor position
        double xpos, ypos;
        glfwGetCursorPos(_window, &xpos, &ypos);
        double centerX = WindowWidth / 2.0;
        double centerY = WindowHeight / 2.0;

        // Get cursor offset from the center
        double offsetX = xpos - centerX;
        double offsetY = centerY - ypos;

        offsetX *= App::MouseSensitivity;
        offsetY *= App::MouseSensitivity;

        // Update angles
        auto &camTransform = cameraObject.GetTransform();
        camTransform.yaw += static_cast<float>(offsetX);
        camTransform.pitch += static_cast<float>(offsetY);

        // Clamp pitch
        camTransform.pitch = glm::clamp(camTransform.pitch, -89.0f, 89.0f);

        // Update rotation
        camTransform.UpdateRotationFromEuler();

        // Set cursor in the center
        glfwSetCursorPos(_window, centerX, centerY);
    }
}
void App::UpdateKeyboard()
{
    if (!dynamicMode) return;
    if (input.keyW)
    {
        cameraObject.UpdateTransform(Direction::FRONT);
    }
    if (input.keyA)
    {
        cameraObject.UpdateTransform(Direction::LEFT);
    }
    if (input.keyS)
    {
        cameraObject.UpdateTransform(Direction::BACK);
    }
    if (input.keyD)
    {
        cameraObject.UpdateTransform(Direction::RIGHT);
    }
    if (input.keySpace)
    {
        cameraObject.UpdateTransform(Direction::UP);
    }
    if (input.keyC)
    {
        cameraObject.UpdateTransform(Direction::DOWN);
    }
}

void ApplyLightAndCamera()
{
    Shader::Bind(shader);

    // Light
    for (auto & lightObject : lightObjects)
    {
        lightObject.ApplyData();
    }

    if (!App::flyMode)
    {
        cameraObject.GetTransform().SetPosition(cameraObject.GetTransform().GetPosition() - glm::vec3(0.0f, 0.1f, 0.0f));
    }

    // Check camera scene position
    {
        const glm::vec3 clampPos = glm::clamp(cameraObject.GetTransform().GetPosition(), App::minBounds, App::maxBounds);
        cameraObject.GetTransform().SetPosition(clampPos);
    }

    // Check camera sphere collision
    {
        const glm::vec3 cameraPosition = cameraObject.GetTransform().GetPosition();
        const glm::vec3 spherePosition = sphereObj->GetTransform().GetPosition();

        if (glm::distance(cameraPosition, spherePosition) < App::CollisionDistance)
        {
            const glm::vec3 dir = glm::normalize(cameraPosition - spherePosition);
            cameraObject.GetTransform().SetPosition(spherePosition + dir * App::CollisionDistance);
        }
    }
}
void ApplyShaderData()
{
    Shader::Bind(shader);

    // Matrices and Uniforms
    Shader::SetMat4(shader._utils.ViewM, cameraObject.GetCamera().GetViewMatrix());
    Shader::SetMat4(shader._utils.ProjectionM, cameraObject.GetCamera().GetProjectionMatrix());

    Shader::SetVec3(shader._utils.ViewPosition, cameraObject.GetTransform().GetWorldPosition());

    Shader::SetInt(shader._utils.lightCount, lightObjects.size());
    Shader::SetInt(shader._utils.useFlashLight, App::useFlashLight);
    Shader::SetInt(shader._utils.useFireLight, Fire::pointFlag);
    Shader::SetInt(shader._utils.useCubeMap, false);

    // Fog
    Shader::SetInt(shader._utils.useFog, App::useFog);
    App::FogColor += App::FogColorStep;
    if (App::FogColor < App::FogColorMin || App::FogColor > App::FogColorMax ) App::FogColorStep *= -1;
    Shader::SetVec3(shader._utils.fogColor, glm::vec3(App::FogColor));
    Shader::SetFloat(shader._utils.fogStart, App::FogStart);
    Shader::SetFloat(shader._utils.fogEnd, App::FogEnd);

    // Water
    Shader::Bind(shaderWater);

    Shader::SetInt(shaderWater._water.WaterTexture, 0);
    Shader::SetVec2(shaderWater._water.ScrollSpeed, App::WaterDir);
    Shader::SetFloat(shaderWater._utils.alpha,      App::WaterAlpha);
    Shader::SetMat4(shaderWater._utils.ModelM, waterObj->GetTransform().GetMatrix());
    Shader::SetMat4(shaderWater._utils.ViewM, cameraObject.GetCamera().GetViewMatrix());
    Shader::SetMat4(shaderWater._utils.ProjectionM, cameraObject.GetCamera().GetProjectionMatrix());

    Shader::SetInt(shaderWater._utils.useFog, App::useFog);
    Shader::SetVec3(shaderWater._utils.fogColor, glm::vec3(App::FogColor));
    Shader::SetFloat(shaderWater._utils.fogStart, App::FogStart);
    Shader::SetFloat(shaderWater._utils.fogEnd, App::FogEnd);
}
void ApplyBoxSettings(RenderObject &obj)
{
    switch (obj.GetBox()._type)
    {
        case(TypeBox::BoxBigT):
        {
            if (obj.GetBox().animFlag)
            {
                RenderObject::UpdateCirclePosition(boxObjBigT->GetTransform());
            }
            break;
        }
        case(TypeBox::BoxMidT):
        {
            if (obj.GetBox().animFlag)
            {
                boxObjMidT->GetTransform().RotateLocal(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(-2.0f));
            }
            break;
        }
        case(TypeBox::BoxSmlT):
        {
            if (obj.GetBox().animFlag)
            {
                boxObjSmlT->GetTransform().RotateLocal(glm::vec3(1.0f, 0.0f, 0.0f), glm::radians(-1.5f));
            }
            break;
        }
        case(TypeBox::BoxBigA):
        {
            Shader::SetInt(shader._utils.useAlpha, true);
            Shader::SetFloat(shader._utils.alpha, App::BoxBigAlpha);
            break;
        }
        case(TypeBox::BoxMidA):
        {
            Shader::SetInt(shader._utils.useAlpha, true);
            Shader::SetFloat(shader._utils.alpha, App::BoxMidAlpha);
            break;
        }
        case(TypeBox::BoxSmlA):
        {
            Shader::SetInt(shader._utils.useAlpha, true);
            Shader::SetFloat(shader._utils.alpha, App::BoxSmlAlpha);
            break;
        }
        default:
            break;
    }
}
void RenderSceneObjects()
{
    for (auto& ptr : RenderObjects)
    {
        // safety check
        if (!ptr) continue;
        RenderObject& obj = *ptr;

        if (obj.GetType() == RenderObject::Type::Box)
        {
            Shader::Bind(shader);
            Shader::SetMat4(shader._utils.ViewM, cameraObject.GetCamera().GetViewMatrix());
            ApplyBoxSettings(obj);
        }
        else if (obj.GetType() == RenderObject::Type::CubeMap)
        {
            Shader::Bind(shader);
            Shader::SetMat4(shader._utils.ViewM, glm::mat4(glm::mat3(cameraObject.GetCamera().GetViewMatrix())));
        }
        else if (obj.GetType() == RenderObject::Type::Water)
        {
            obj.Render(shaderWater);
        }

        obj.Render(shader);
    }
}
void App::Render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    ApplyLightAndCamera();
    ApplyShaderData();

    // Objects
    RenderSceneObjects();

    // Picking
    if ((App::stencilIdx >= 0 || App::stencilIdxLast >= 0))  {

        // Check if user have clicked on the same object -> off white mode
        if (App::stencilIdx == App::stencilIdxLast)
        {
            RenderObjects[stencilIdx]->Render(shader);
            App::stencilIdx = App::stencilIdxLast = -1;
            return;
        }

        int idx;
        // User clicked on new object
        if (App::stencilIdx != -1)
        {
            idx = App::stencilIdx;
            App::stencilIdxLast = idx;
            App::stencilIdx = -1;
        }
        // User did not change selection
        else
        {
            idx = App::stencilIdxLast;
        }

        Shader::Bind(shaderWhite);

        // Set matrices
        Shader::SetMat4(shaderWhite._utils.ViewM, cameraObject.GetCamera().GetViewMatrix());
        Shader::SetMat4(shaderWhite._utils.ProjectionM, cameraObject.GetCamera().GetProjectionMatrix());

        RenderObjects[idx]->Render(shaderWhite);

        // Set primary shader back
        Shader::Bind(shader);
    }
}

void DoPicking(const int winX, const int winY) {
    // Enable tests
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_STENCIL_TEST);

    // Enable stencil mask and clear buffers
    glStencilMask(0xFF);
    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // Disable color and enable depth mask
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDepthMask(GL_TRUE);

    // When depth test success then switch stencil to ref
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    int ref = 1;
    for (const auto & RenderObject : RenderObjects) {
        glStencilFunc(GL_ALWAYS, ref, 0xFF);
        RenderObject->Render(shader);
        ++ref;
    }

    // Enable color
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    // glDepthMask(GL_TRUE);
    glDisable(GL_STENCIL_TEST);

    // Read stencil pixel
    unsigned char pixelID = 0;
    glReadPixels(winX, winY, 1, 1,
                 GL_STENCIL_INDEX, GL_UNSIGNED_BYTE,
                 &pixelID);

    App::stencilIdx = (pixelID == 0 ? -1 : pixelID - 1);
    LOG("stencilIdx: {}", App::stencilIdx);

    // Update models based on selected item
    if (App::stencilIdx != -1)
    {
        if (RenderObjects[App::stencilIdx]->GetType() == RenderObject::Type::CatType)
        {
            LOG("Cat selected.");
            Cat::isMoving = !Cat::isMoving;
            App::stencilIdx = -1;
        }
        else if (RenderObjects[App::stencilIdx]->GetType() == RenderObject::Type::Box)
        {
            LOG("Box selected.");
            RenderObjects[App::stencilIdx]->GetBox().animFlag = !RenderObjects[App::stencilIdx]->GetBox().animFlag;
            App::stencilIdx = -1;
        }
        else if (RenderObjects[App::stencilIdx]->GetType() == RenderObject::Type::Fire)
        {
            LOG("Fire selected.");
            Fire::pointFlag = !Fire::pointFlag;
            App::stencilIdx = -1;
        }
        else if (RenderObjects[App::stencilIdx]->GetType() == RenderObject::Type::Sphere)
        {
            LOG("Sphere selected.");
            Icosphere::useToSphere = !Icosphere::useToSphere;
            App::stencilIdx = -1;
        }
    }
}
void App::OnMouseButtonChanged(const int button, const bool pressed)
{
    if (pressed)
    {
        switch (button)
        {
            case GLFW_MOUSE_BUTTON_LEFT:
            {
                if (!dynamicMode)
                {
                    input.mouseLooking = false;
                    return;
                }

                input.mouseLooking = !input.mouseLooking;

                // Hide cursor and set in the middle of the screen
                if (input.mouseLooking)
                {
                    glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                    glfwSetCursorPos(_window, WindowWidth / 2.0, WindowHeight / 2.0);
                }
                else // Reveal cursor
                {
                    glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                }

                break;
            }
            case GLFW_MOUSE_BUTTON_RIGHT:
            {
                if (input.mouseLooking)
                    break;

                double posX, posY;
                glfwGetCursorPos(App::_window, &posX, &posY);

                const int mouseX = static_cast<int>(posX);
                const int mouseY = static_cast<int>(App::WindowHeight) - static_cast<int>(posY); // rotate Y
                DoPicking(mouseX, mouseY);
                break;
            }
            default:
                break;
        }
    }
}

void ChangeCamera(const int buttonIdx)
{
    switch (buttonIdx)
    {
        case 0:
        {
            glfwSetCursorPos(App::_window, App::WindowWidth / 2.0, App::WindowHeight / 2.0);

            App::dynamicMode = true;
            input.mouseLooking = true;
            glfwSetInputMode(App::_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            cameraObject.GetTransforms()[0] = Transform(cameraObject.GetTransform().GetWorldPosition(), cameraObject.GetTransform().GetWorldForward());
            cameraObject.SetTransform(cameraObject.GetTransforms()[0]);
            break;
        }
        case 1:
        {
            App::dynamicMode = false;
            input.mouseLooking = false;
            cameraObject.SetTransform(cameraObject.GetTransforms()[1]);
            break;
        }
        case 2:
        {
            App::dynamicMode = false;
            input.mouseLooking = false;
            cameraObject.SetTransform(cameraObject.GetTransforms()[2]);
            break;
        }
        case 3:
        {
            App::dynamicMode = false;
            input.mouseLooking = false;
            cameraObject.SetTransform(cameraObject.GetTransforms()[3]);
            break;
        }
        default:
            break;
    }
}
void App::OnKeyChanged(const int key, const bool pressed)
{
    if (pressed)
    {
        switch (key)
        {
            case GLFW_KEY_R:
                LoadShaders();
                LOG("Shaders reloaded");
                break;

            case GLFW_KEY_G:
                if (!input.keyCtrl) break;
                useFog = !useFog;
                LOG("Fog switched");
                break;

            case GLFW_KEY_V:
                flyMode = !flyMode;
                LOG("Fly mode switched");
                break;

            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(_window, GLFW_TRUE);
                break;

            case GLFW_KEY_LEFT:
            {
                if (--cameraIdx <= 0) cameraIdx = cameraObject.GetTransforms().size() - 1;
                ChangeCamera(cameraIdx);
                break;
            }

            case GLFW_KEY_RIGHT:
            {
                if (++cameraIdx >= static_cast<int>(cameraObject.GetTransforms().size())) cameraIdx = 1;
                ChangeCamera(cameraIdx);
                break;
            }

            case GLFW_KEY_DOWN:
            {
                ChangeCamera(0);
                break;
            }

            case GLFW_KEY_W:
                input.keyW = true;
                break;

            case GLFW_KEY_A:
                input.keyA = true;
                break;

            case GLFW_KEY_S:
                input.keyS = true;
                break;

            case GLFW_KEY_D:
                input.keyD = true;
                break;

            case GLFW_KEY_SPACE:
                input.keySpace = true;
                break;

            case GLFW_KEY_C:
                input.keyC = true;
                break;

            case GLFW_KEY_LEFT_CONTROL:
                input.keyCtrl = true;
            break;

            case GLFW_KEY_F:
                App::useFlashLight = !App::useFlashLight;
            break;

            case GLFW_KEY_LEFT_SHIFT:
                cameraObject.GetCamera().SetSpeedHorizontal(Camera::DEFAULT_SPEED_H * 2.0f);
                break;

            case GLFW_KEY_1:
                ChangeCamera(0);
                break;

            case GLFW_KEY_2:
                ChangeCamera(1);
                break;

            case GLFW_KEY_3:
                ChangeCamera(2);
                break;

            case GLFW_KEY_4:
                ChangeCamera(3);
                break;

            default:
                break;
        }
    }
    else // released
    {
        switch (key)
        {
            case GLFW_KEY_W:
                input.keyW = false;
                break;

            case GLFW_KEY_A:
                input.keyA = false;
                break;

            case GLFW_KEY_S:
                input.keyS = false;
                break;

            case GLFW_KEY_D:
                input.keyD = false;
                break;

            case GLFW_KEY_SPACE:
                input.keySpace = false;
                break;

            case GLFW_KEY_C:
                input.keyC = false;
                break;

            case GLFW_KEY_LEFT_CONTROL:
                input.keyCtrl = false;
                break;

            case GLFW_KEY_LEFT_SHIFT:
                cameraObject.GetCamera().SetSpeedHorizontal(Camera::DEFAULT_SPEED_H);
                break;

            default:
                break;
        }
    }
}

void App::OnResize(const float width, const float height)
{
    WindowWidth = width;
    WindowHeight = height;
    glViewport(0, 0, static_cast<int>(width), static_cast<int>(height));

    float aspectRatio = width / height;
    cameraObject.GetCamera().SetProjection(aspectRatio, WindowFOV);
}
void App::End()
{
    // Free shaders
    Shader::Delete(shader);
    Shader::Delete(shaderWater);
    Shader::Delete(shaderWhite);

    // Free meshes
    glDeleteBuffers(1, &Box::VBO);
    glDeleteVertexArrays(1, &Box::VAO);

    glDeleteBuffers(1, &Cat::VBOPos);
    glDeleteBuffers(1, &Cat::VBONorm);
    glDeleteBuffers(1, &Cat::VAO);

    glDeleteBuffers(1, &cubeMapObj->GetCubemap().VBO);
    glDeleteVertexArrays(1, &cubeMapObj->GetCubemap().VAO);

    glDeleteBuffers(1, &fireObj->GetFire().EBO);
    glDeleteBuffers(1, &fireObj->GetFire().VBO);
    glDeleteVertexArrays(1, &fireObj->GetFire().VAO);

    glDeleteBuffers(1, &sphereObj->GetSphere().VBO);
    glDeleteVertexArrays(1, &sphereObj->GetSphere().VAO);

    for (const auto& mesh : Meshes)
    {
        mesh->DestroyGLBuffers();
    }

    glDeleteBuffers(1, &waterObj->GetWater().EBO);
    glDeleteBuffers(1, &waterObj->GetWater().VBO);
    glDeleteVertexArrays(1, &waterObj->GetWater().VAO);
}
