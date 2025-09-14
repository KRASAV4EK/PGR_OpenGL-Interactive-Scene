#include "TextureSource.h"

TextureSource::TextureSource(TextureType type, TextureFormat format, unsigned int width, unsigned int height,
                             std::vector<void_ptr> &images) :
    _type(type), _format(format), _width(width), _height(height), _images(std::move(images))
{
    if (_type == TextureType::Tex2D && _images.size() != 1)
        throw std::runtime_error("TextureSource Number of images for Tex2D is not 1");
    if (_type == TextureType::TexCubemap && _images.size() != 6)
        throw std::runtime_error("TextureSource Number of images for TexCube is not 6");
}

TextureType TextureSource::GetType() const
{
    return _type;
}

TextureFormat TextureSource::GetFormat() const
{
    return _format;
}

unsigned int TextureSource::GetWidth() const
{
    return _width;
}

unsigned int TextureSource::GetHeight() const
{
    return _height;
}

unsigned int TextureSource::GetImageCount() const
{
    return _images.size();
}

const void *TextureSource::GetImagePixels(unsigned int imageIndex) const
{
    if (imageIndex >= _images.size())
    {
        LOG_ERROR("Image index {} out of range [0, {}].", imageIndex, _images.size());
        return nullptr;
    }

    return _images[imageIndex].get();
}
