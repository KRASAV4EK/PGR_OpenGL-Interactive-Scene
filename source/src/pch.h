//----------------------------------------------------------------------------------------
/**
 * \file       pch.h
 * \author     Ilia Timofeev
 * \date       2025/05/20
 * \brief      Precompiled header with common includes and utility macros.
 *
 *  This file serves as the precompiled header for the project, aggregating
 *  frequently used standard and third-party library headers (STL, GLM),
 *  defining type aliases (Ref, unique_ptr_free), resource path macros,
 *  and logging macros for colored console output. It speeds up compilation
 *  and provides common utilities across the codebase.
 *
 */
//----------------------------------------------------------------------------------------

#ifndef PCH_H
#define PCH_H
#define mmstr(s) mstr(s)
#define mstr(x) #x

// Std
#include <algorithm>
#include <array>
#include <initializer_list>
#include <map>
#include <memory>
#include <set>
#include <stack>
#include <stdexcept>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

// String
#include <cstring>
#include <format>
#include <sstream>
#include <string>

// Files
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iosfwd>
#include <iostream>

// GLM
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/transform.hpp>

template <typename T>
using Ref = std::shared_ptr<T>;

struct free_deleter
{
    template <typename T>
    void operator()(T *p) const
    {
        std::free(const_cast<std::remove_const_t<T> *>(p));
    }
};
template <typename T>
using unique_ptr_free = std::unique_ptr<T, free_deleter>;
using void_ptr = unique_ptr_free<void>;

#define ABSOLUTE_RESOURCE_PATH(x) (std::filesystem::path(x).is_relative() ? std::filesystem::absolute(std::filesystem::path(RESOURCES_PATH)/x) : std::filesystem::path(x))

// Logging
#define COLOR_RESET "\033[0m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_RED "\033[31m"

#define INTERNAL_LOG(color, format_str, ...)                                                                           \
    std::cout << color << "[" << std::filesystem::path(__FILE__).stem().string() << "] "                               \
              << std::format(format_str, ##__VA_ARGS__) << COLOR_RESET << std::endl
#define LOG(format_str, ...) INTERNAL_LOG(COLOR_RESET, format_str, ##__VA_ARGS__)
#define LOG_WARNING(format_str, ...) INTERNAL_LOG(COLOR_YELLOW, format_str, ##__VA_ARGS__)
#define LOG_ERROR(format_str, ...) INTERNAL_LOG(COLOR_RED, format_str, ##__VA_ARGS__)
#define LOG_RAW(format_str, ...) std::cout << std::format(format_str, ##__VA_ARGS__) << COLOR_RESET << std::endl
#endif
