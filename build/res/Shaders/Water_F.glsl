#version 450 core

in vec3 FragPos;
in vec2 TexCoords;

uniform vec3 viewPos;

uniform sampler2D WaterTexture;
uniform float     Time;
uniform vec2      ScrollSpeed;
uniform float     Alpha;

// Fog
uniform bool  useFog;
uniform vec3  fogColor;
uniform float fogStart;
uniform float fogEnd;

out vec4 FragColor;

void main() {
    // move and tile UV
    vec2 uv = fract(TexCoords + ScrollSpeed * Time);

    // get color from texture
    vec4 color = texture(WaterTexture, uv);

    // set alpha
    color.a    = Alpha;

    // apply fog
    if (useFog) {
        float distance = length(FragPos - viewPos);
        float fogFactor = clamp((fogEnd - distance) / (fogEnd - fogStart), 0.0, 1.0);

        vec3 ret = mix(fogColor, vec3(color), fogFactor);
        FragColor = vec4(ret, Alpha);
    } else {
        FragColor = color;
    }
}

