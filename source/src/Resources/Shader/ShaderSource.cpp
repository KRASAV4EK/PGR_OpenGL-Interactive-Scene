#include "ShaderSource.h"

ShaderSource::ShaderSource(std::string vertexSource, std::string fragmentSource) :
    _vertexSource(std::move(vertexSource)), _fragmentSource(std::move(fragmentSource))
{}

const std::string &ShaderSource::GetVertexSource() const
{
    return _vertexSource;
}

const std::string &ShaderSource::GetFragmentSource() const
{
    return _fragmentSource;
}
