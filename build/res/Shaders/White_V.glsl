#version 450 core

layout(location=0) in vec3 aPosition;

uniform mat4 ModelM, ViewM, ProjectionM;

void main(){
    gl_Position = ProjectionM * ViewM * ModelM * vec4(aPosition,1);
}