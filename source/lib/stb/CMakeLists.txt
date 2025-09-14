cmake_minimum_required(VERSION 3.16)
project(stb C)

set(STB_SOURCES stb_image.cpp stb_image_write.cpp)

add_library(stb STATIC ${STB_SOURCES})

set_target_properties(stb PROPERTIES LINKER_LANGUAGE C
        # Match runtime library settings as parent
        MSVC_RUNTIME_LIBRARY "${CMAKE_MSVC_RUNTIME_LIBRARY}"
)

# Make sure STB knows to use the standard library functions
#if(MSVC)
#    target_compile_definitions(stb PRIVATE _CRT_SECURE_NO_WARNINGS)
#endif()

# Include directories
target_include_directories(stb PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})