//----------------------------------------------------------------------------------------
/**
 * \file       Texture.h
 * \author     Ilia Timofeev
 * \date       2025/05/20
 * \brief      Wraps OpenGL texture creation and binding.
 *
 *  This file declares the Texture class, which creates and destroys
 *  OpenGL textures based on a TextureSource and TextureSettings.
 *  It also offers static helpers to bind textures to texture units
 *  and load image files into GPU memory with proper filtering modes.
 *
 */
//----------------------------------------------------------------------------------------

#pragma once
// #define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "GL/glew.h"
#include "TextureSource.h"

// Features
// #define IMPL_TEXTURE
// #define IMPL_TEXTURE_ENABLE_HDR


class Texture
{
public:
    Texture(const TextureSource &source, const TextureSettings &settings);
    ~Texture();

    // Disable moving and copying for simplicity
    Texture(const Texture &other) = delete;
    Texture(Texture &&other) = delete;
    Texture &operator=(const Texture &other) = delete;
    Texture &operator=(Texture &&other) = delete;

    static void Bind(const Texture &texture, int slot);
    static void LoadTextures(unsigned int &textureID, const std::string& texturePath)
    {
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        // set the texture wrapping/filtering options (on the currently bound texture object)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // load and generate the texture
        int width, height, nrChannels;
        unsigned char *data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            GLenum format  = (nrChannels == 1) ? GL_RED  :
                             (nrChannels == 3) ? GL_RGB  :
                             /* 4 */             GL_RGBA;

            glTexImage2D(GL_TEXTURE_2D, 0,
                         format,          // internalFormat
                         width, height, 0,
                         format,          // format
                         GL_UNSIGNED_BYTE, data);

            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);
    }
private:
};
