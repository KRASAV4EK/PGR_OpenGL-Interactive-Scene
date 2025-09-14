//----------------------------------------------------------------------------------------
/**
 * \file       TextureSource.h
 * \author     Ilia Timofeev
 * \date       2025/05/20
 * \brief      Contains raw image data and metadata for texture creation.
 *
 *  This file defines the TextureSource class, which holds pixel data
 *  for 2D textures or cubemaps along with type, format, width, height,
 *  and number of images. It provides methods to query these properties
 *  and retrieve image pointers for GPU upload.
 *
 */
//----------------------------------------------------------------------------------------

#pragma once
#include "TextureInfo.h"


/// Class containing all necessary data to construct a Texture, including multiple images for cubemaps.
class TextureSource
{
public:
    TextureSource(TextureType type, TextureFormat format, unsigned int width, unsigned int height,
                  std::vector<void_ptr> &images);

    [[nodiscard]] TextureType GetType() const;
    [[nodiscard]] TextureFormat GetFormat() const;

    [[nodiscard]] unsigned int GetWidth() const;
    [[nodiscard]] unsigned int GetHeight() const;
    [[nodiscard]] unsigned int GetImageCount() const;

    /// Get pixel data of a particular image.
    /// @param imageIndex Index of image. Possible values:
    /// 1. 2D - [0]
    /// 2. Cubemap - [0..5] (X+, X-, Y+, Y-, Z+, Z-)
    /// @return Pointer to pixel data
    [[nodiscard]] const void *GetImagePixels(unsigned int imageIndex = 0) const;

private:
    TextureType _type;
    TextureFormat _format;
    unsigned int _width, _height;
    std::vector<void_ptr> _images;
};
