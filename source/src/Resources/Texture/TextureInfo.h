//----------------------------------------------------------------------------------------
/**
 * \file       TextureInfo.h
 * \author     Ilia Timofeev
 * \date       2025/05/20
 * \brief      Enumerations and settings for texture configuration.
 *
 *  This file defines enums for TextureType, TextureFormat, TextureTiling,
 *  and TextureFiltering, as well as the TextureSettings struct with
 *  default tiling and filtering parameters. It provides human‐readable
 *  names for each enum value.
 *
 */
//----------------------------------------------------------------------------------------

#pragma once


enum class TextureType
{
    Tex2D,
    TexCubemap
};
const inline std::string TextureTypeNames[] = {"2D", "Cube"};

enum class TextureFormat
{
    Invalid = 0,

    RGB8,
    RGBA8,

    SRGB8,
    SRGBA8,

    // HDR
    RGB16F,
    RGBA16F
};

enum class TextureTiling
{
    ClampBorder,
    ClampEdge,
    Repeat,
    MirrorRepeat
};
const inline std::string TextureTilingNames[] = {"ClampBorder", "ClampEdge", "Repeat", "MirrorRepeat"};

enum class TextureFiltering
{
    Nearest,
    Linear,
    LinearMipmapLinear

    // Additional configurations we don't need:
    /* NearestMipmapNearest,
     LinearMipmapNearest,
     NearestMipmapLinear */
};
const inline std::string TextureFilteringNames[] = {"Nearest", "Linear", "LinearMipmapLinear"};

struct TextureSettings
{
    bool mipmaps = true;
    TextureTiling tilingU = TextureTiling::Repeat;
    TextureTiling tilingV = TextureTiling::Repeat;
    TextureFiltering filtering = TextureFiltering::LinearMipmapLinear;

    static TextureSettings Default()
    {
        return {};
    }
};
