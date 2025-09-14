cmake_minimum_required(VERSION 3.23..3.28)

project(PGR_Project
        VERSION 0.1
        DESCRIPTION "PGR timofili"
        LANGUAGES CXX
)

add_executable(${PROJECT_NAME}
        src/pch.h
        src/main.cpp
        src/App.h src/App.cpp

        # Resources
        src/Resources/Mesh/Mesh.h src/Resources/Mesh/Mesh.cpp
        src/Resources/Mesh/MeshSource.h src/Resources/Mesh/MeshSource.cpp
        src/Resources/Mesh/MeshLoader.h src/Resources/Mesh/MeshLoader.cpp
        src/Resources/Mesh/Loader/GltfLoader.h src/Resources/Mesh/Loader/GltfLoader.cpp

        src/Resources/Shader/Shader.h src/Resources/Shader/Shader.cpp
        src/Resources/Shader/ShaderSource.h src/Resources/Shader/ShaderSource.cpp
        src/Resources/Shader/ShaderLoader.h src/Resources/Shader/ShaderLoader.cpp
        src/Resources/Shader/ShaderUtils.h

        src/Resources/Texture/Texture.h
        src/Resources/Texture/TextureSource.h src/Resources/Texture/TextureSource.cpp
        src/Resources/Texture/TextureInfo.h

        src/Resources/Material/MaterialPGR.h src/Resources/Material/MaterialPGR.cpp

        # Components
        src/Components/Camera.h src/Components/Camera.cpp
        src/Components/Light.h src/Components/Light.cpp
        src/Components/MeshRenderer.h
        src/Components/Transform.h src/Components/Transform.cpp

        # Objects
        src/Objects/CameraObject.h
        src/Objects/LightObject.h src/Objects/LightObject.cpp
        src/Objects/RenderObject.h

        # Models
        res/Models/Box/Box.h res/Models/Box/Box.cpp
        res/Models/Cat/Cat.h res/Models/Cat/Cat.cpp
        res/Models/Cubemap/Cubemap.h
        res/Models/Icosphere/Icosphere.h res/Models/Icosphere/Icosphere.cpp
        res/Models/Fire/Fire.h res/Models/Fire/Fire.cpp
        res/Models/Water/Water.h

        # Utils
        src/Utils/GlfwUtils.h
)

# Copy resources
set(ASSET_DIR ${CMAKE_SOURCE_DIR}/res)
set(ASSET_DST $<TARGET_FILE_DIR:${PROJECT_NAME}>/res)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${ASSET_DIR} ${ASSET_DST}
        COMMENT "Copying resources…")

# CPP
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
target_precompile_headers(${PROJECT_NAME} PUBLIC src/pch.h)

# Compiler flags
if(MSVC)
    option(USE_STATIC_RUNTIME "Use static runtime library (/MT)" ON)

    if(USE_STATIC_RUNTIME)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    else()
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    endif()

    set_property(TARGET ${PROJECT_NAME} PROPERTY
            MSVC_RUNTIME_LIBRARY "${CMAKE_MSVC_RUNTIME_LIBRARY}")

    target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<COMPILE_LANGUAGE:CXX>:
            /W4 /external:W0
            /MP
            /permissive-
            >)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<COMPILE_LANGUAGE:CXX>:
            -Wall -Wextra -pedantic -Wno-sign-compare
            >)
#    set(WAYLAND_PROTOCOLS_DIR "/usr/share/wayland-protocols" CACHE PATH "Wayland protocols directory")
    set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "Build with wayland support")
endif()


# Resources
target_compile_definitions(${PROJECT_NAME} PRIVATE
        "RESOURCES_PATH=\"$<IF:$<CONFIG:Debug>,${CMAKE_CURRENT_SOURCE_DIR}/res/,./res/>\""
)
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_custom_target(copy_resources ALL
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/res/
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/res/
            COMMENT "Copying resources to build directory"
    )

    add_dependencies(${PROJECT_NAME} copy_resources)
endif()


# Libraries
if(MSVC AND USE_STATIC_RUNTIME)
    # GLFW
    set(USE_MSVC_RUNTIME_LIBRARY_DLL OFF CACHE BOOL "Use MSVC runtime library DLL" FORCE)
endif()

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/lib/stb")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/lib/tinygltf")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/lib/glm")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw")

add_definitions(-DGLEW_STATIC)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/lib/glew")

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/lib")
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/lib/stb")
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/lib/glew/include")
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/lib/tinygltf")
find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME}
        PRIVATE stb tinygltf glm glew glfw ${OPENGL_LIBRARIES})

if (WIN32)
    # Use generator expressions to refer to the built targets
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            # GLFW dll
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:glfw> $<TARGET_FILE_DIR:${PROJECT_NAME}>
            # GLEW dll
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:glew> $<TARGET_FILE_DIR:${PROJECT_NAME}>
            COMMENT "Copying GLFW and GLEW DLLs into output directory"
    )
endif()

set(ASSET_DIR ${CMAKE_SOURCE_DIR}/res)
set(ASSET_DST $<TARGET_FILE_DIR:${PROJECT_NAME}>/res)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${ASSET_DIR} ${ASSET_DST}
        COMMENT "Copying resources…"
)