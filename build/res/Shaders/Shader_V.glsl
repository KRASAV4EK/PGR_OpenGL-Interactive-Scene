#version 450 core

// Vertex Attributes
layout (location = 0) in vec3 aPosition;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec2 aTexCoords;

// Vertex Uniforms
uniform mat4  ModelM;
uniform mat4  ViewM;
uniform mat4  ProjectionM;
uniform int   useCubeMap;
uniform int   useToSphere;
uniform float alphaToSphere;

// Vertex Outputs
out vec3 FragPos;
out vec3 Normal;
out vec3 TexCoords3;

vec3 toSphere()
{
    vec3 center = vec3(1.0);
    float radius = 1.2;

    vec3 Q = normalize(aPosition - center) * radius + center;

    return mix(aPosition, Q, alphaToSphere);
}

void main()
{
    FragPos = vec3(ModelM * vec4(aPosition, 1.0));

    if (useCubeMap == 1) {
        vec4 posCubeMap = ProjectionM * ViewM * vec4(aPosition, 1.0);
        gl_Position = posCubeMap.xyww;

        Normal = vec3(0.0);
        TexCoords3 = aPosition;
    } else {
        if (useToSphere == 1)
        {
            gl_Position = ProjectionM * ViewM * ModelM * vec4(toSphere(), 1.0);
        } else
        {
            gl_Position = ProjectionM * ViewM * ModelM * vec4(aPosition, 1.0);
        }
        Normal = mat3(transpose(inverse(ModelM))) * aNormal;
        // Set Texture coordinates
        TexCoords3 = vec3(aTexCoords, 0.0);
    }
}