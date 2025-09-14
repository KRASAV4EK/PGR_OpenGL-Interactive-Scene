#define GLFW_INCLUDE_NONE
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "App.h"
#include "Utils/GlfwUtils.h"

// Enable ANSI console formatting on Windows
#ifdef _WIN32
#include <windows.h>

/// Enable ANSI escape sequence (VT100) support
void EnableVTMode()
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}
#else
void EnableVTMode()
{}
#endif

int main()
{
    EnableVTMode();

    // GLFW
    if (!glfwInit())
    {
        LOG_ERROR("Failed to initialize GLFW.");
        return -1;
    }
    GLFWwindow *window = glfwCreateWindow(App::WindowWidth, App::WindowHeight, App::WindowTitle.data(), NULL, NULL);
    if (!window)
    {
        LOG_ERROR("Failed to create GLFW window.");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // GLEW
    if (glewInit() != GLEW_OK)
    {
        LOG_ERROR("Failed to initialize GLEW.");
        glfwTerminate();
        return -1;
    }

    // Logging
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(
            [](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
            {
                if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
                    return;

                switch (severity)
                {
                    case GL_DEBUG_SEVERITY_HIGH:
                        std::cout << COLOR_RED;
                        break;

                    case GL_DEBUG_SEVERITY_MEDIUM:
                    case GL_DEBUG_SEVERITY_LOW:
                        std::cout << COLOR_YELLOW;
                        break;
                    default:
                        // Regular log for notifications if enabled
                        break;
                }

                LOG_RAW("[OpenGL] {}::{} {} (ID: {})", GlfwUtils::DebugSourceToString(source), GlfwUtils::DebugTypeToString(type), message, id);
            },
            nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_HIGH, 0, nullptr, GL_TRUE);

    // Callbacks
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window, int width, int height) { App::OnResize(width, height); });
    glfwSetKeyCallback(window, [](GLFWwindow *window, int key, int scancode, int action, int mods)
    {
        if (action == GLFW_PRESS || action == GLFW_RELEASE)
            App::OnKeyChanged(key, action == GLFW_PRESS);
    });
    glfwSetMouseButtonCallback(window, [](GLFWwindow *window, int button, int action, int mods)
    {
        if (action == GLFW_PRESS || action == GLFW_RELEASE)
            App::OnMouseButtonChanged(button, action == GLFW_PRESS);
    });

    // Start application
    App::InitWindow(window);
    App::OnResize(App::WindowWidth, App::WindowHeight);

    // Window loop
    while (!glfwWindowShouldClose(window))
    {
        App::Update(); // logics update
        App::Render(); // render scene

        glfwSwapBuffers(window); // show next frame
        glfwPollEvents(); // process input
        glFinish(); // wait for GPU to finish
    }

    App::End(); // free shaders and meshes
    glfwTerminate();
    return 0;
}
