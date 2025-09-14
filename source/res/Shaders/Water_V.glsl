#version 450 core

layout(location=0) in vec3 aPosition;
layout(location=1) in vec2 aTexCoords;

uniform mat4 ModelM;
uniform mat4 ViewM;
uniform mat4 ProjectionM;

out vec3 FragPos;
out vec2 TexCoords;

void main() {
    FragPos = vec3(ModelM * vec4(aPosition, 1.0));
    TexCoords = aTexCoords;
    gl_Position = ProjectionM * ViewM * ModelM * vec4(aPosition,1.0);
}

