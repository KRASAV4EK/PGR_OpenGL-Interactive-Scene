#include "ShaderLoader.h"

ShaderSource ShaderLoader::LoadShaderSeparate(const std::filesystem::path &vertexPath,
                                              const std::filesystem::path &fragmentPath)
{
    std::ifstream vertexFile(ABSOLUTE_RESOURCE_PATH(vertexPath).string());
    if (!vertexFile)
    {
        LOG_ERROR("Failed to load vertex shader '{}'.", vertexPath.string());
        return ShaderSource("", "");
    }
    std::stringstream vertexStream;
    vertexStream << vertexFile.rdbuf();

    std::ifstream fragmentFile(ABSOLUTE_RESOURCE_PATH(fragmentPath).string());
    if (!fragmentFile)
    {
        LOG_ERROR("Failed to load fragment shader '{}'.", fragmentPath.string());
        return ShaderSource("", "");
    }
    std::stringstream fragmentStream;
    fragmentStream << fragmentFile.rdbuf();

    return {ProcessShaderSource(vertexStream, Vertex), ProcessShaderSource(fragmentStream, Fragment)};
}

std::string ShaderLoader::ProcessShaderSource(std::stringstream &sourceStream, ShaderSourceType type)
{
    std::stringstream shaderOut;

    // Beginning of shader
    // Example: Add version automatically if it isn't present to avoid typing it manually in every shader.

    // Resolve includes
    shaderOut << GenerateIncludes(sourceStream);

    // Shader parsing
    if (type == Vertex)
    {
        // Example: Get or set attribute locations.
    }
    else if (type == Fragment)
    {
        // Example: Add default fragment output variable.
    }
    shaderOut << sourceStream.rdbuf(); // Remove if you're doing shader editing and adding lines manually.

    return shaderOut.str();
}

std::string ShaderLoader::GenerateIncludes(std::stringstream &shaderSource)
{
    std::stringstream sourceOut;
    std::set<std::string> alreadyIncluded;
    std::stack<std::unique_ptr<std::ifstream>> includeFiles;

    std::string line;
    while (shaderSource.good())
    {
        auto &currentSource = includeFiles.empty() ? static_cast<std::istream &>(shaderSource)
                                                   : static_cast<std::istream &>(*includeFiles.top());

        // Skip whitespace characters
        currentSource >> std::ws;

        auto lineStartPos = currentSource.tellg();
        std::getline(currentSource, line);

        if (line.starts_with("//"))
            continue;

        if (line.starts_with("#version"))
        {
            sourceOut << line << std::endl;
            continue;
        }

        if (line.starts_with("#include")) // New include file found
        {
            auto valueStart = line.find_first_not_of(' ', sizeof("#include") - 1) + 1;
            auto valueEnd = line.find_first_of('\"', valueStart + 1);
            auto includedPath = line.substr(valueStart, valueEnd - valueStart);

            if (alreadyIncluded.contains(includedPath))
                continue;

            includeFiles.push(std::make_unique<std::ifstream>(std::filesystem::path(INCLUDE_DIRECTORY) / includedPath));
            alreadyIncluded.insert(includedPath);
        }
        else // End of #include directives, append rest of file
        {
            currentSource.seekg(lineStartPos);

            // If in an include file
            if (!includeFiles.empty())
            {
                sourceOut << currentSource.rdbuf() << std::endl;
                includeFiles.pop();
            }
            else
                break;
        }
    }

    return sourceOut.str();
}
