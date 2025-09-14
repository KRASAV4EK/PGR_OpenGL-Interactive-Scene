#version 450 core

const int Ambient = 0;
const int Direct = 1;
const int Point = 2;
const int Spot = 3;

const int MAX_LIGHT_COUNT = 8;

struct Material {
    bool useTexture;       // true -> use texture, false -> use vec3
    sampler2D diffuseMap;  // diffuse   texture
    sampler2D specularMap; // specular  texture

    vec3 ambient;          // ambient   color
    vec3 diffuse;          // diffuse   color
    vec3 specular;         // specular  color
    float shininess;       // shininess coefficient
};

struct Light {
    int type;              // Ambient, Direct, Point, Spot

    vec3 position;
    vec3 direction;

    vec3 color;

    vec3 ambient;          // ambient   strength
    vec3 diffuse;          // diffuse   strength
    vec3 specular;         // specular  strength

    // Attenuation
    float constant;
    float linear;
    float quadratic;

    // Cutoff
    float cutOff;
    float outerCutOff;
};

// Fragment Inputs
in vec3 FragPos;           // position of fragment in world
in vec3 Normal;            // normal direction of fragment
in vec3 TexCoords3;        // texture coordinates for fragment

// Fragment Uniforms
uniform vec3 viewPos;      // camera position

uniform Material material;
uniform Light lights[MAX_LIGHT_COUNT];
uniform int lightCount;    // real number of light on the scene
uniform samplerCube cubeMap;
uniform int useCubeMap;    // flag if cubeMap is rendering

// Fragment Output
out vec4 FragColor;        // out color of fragment

vec2 TexCoords2;

// Fog
uniform int   useFog;
uniform vec3  fogColor;
uniform float fogStart;
uniform float fogEnd;

// Fire
uniform int useFire;
uniform sampler2D fireMap;
uniform int frame;
uniform ivec2 pattern = ivec2(14, 1);

// Alpha
uniform int useAlpha;
uniform float alpha;

// Light flags
uniform int useFlashLight;
uniform int useFireLight;

vec3 sampleDiffuse() {
    return material.useTexture
    ? vec3(texture(material.diffuseMap, TexCoords2))
    : material.diffuse;
}
vec3 sampleSpecular() {
    return material.useTexture
    ? vec3(texture(material.specularMap, TexCoords2))
    : material.specular;
}

vec3 calcAmbient(Light light) {
    return light.color * light.ambient * sampleDiffuse();
}
vec3 calcDirect(Light light) {
    vec3 normDir = normalize(Normal);
    vec3 lightDir = normalize(-light.direction);

    float diff  = max(dot(normDir, lightDir), 0.0);

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normDir);
    float spec  = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient  = calcAmbient(light);
    vec3 diffuse  = light.diffuse  * diff * sampleDiffuse();
    vec3 specular = light.specular * spec * sampleSpecular();

    return ambient + diffuse + specular;
}
vec3 calcPoint(Light light) {
    vec3 normDir  = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);

    float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
    light.quadratic * distance * distance);

    float diff  = max(dot(normDir, lightDir), 0.0);

    vec3 V = normalize(viewPos - FragPos);
    vec3 R = reflect(-lightDir, normDir);
    float spec  = pow(max(dot(V, R), 0.0), material.shininess);

    vec3 ambient  = calcAmbient(light);
    vec3 diffuse  = light.diffuse  * diff * sampleDiffuse();
    vec3 specular = light.specular * spec * sampleSpecular();

    return (ambient + diffuse + specular) * attenuation;
}
vec3 calcSpot(Light light) {
    vec3 normDir = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);

    // angle between fragment direction and light direction
    float theta = dot(lightDir, normalize(-light.direction));

    // soft edges
    float epsilon = light.cutOff - light.outerCutOff;
    float edge    = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    // attenuation
    float distance   = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
    light.quadratic * distance * distance);

    float diff  = max(dot(normDir, lightDir), 0.0);

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normDir);
    float spec  = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient  = calcAmbient(light);
    vec3 diffuse  = light.diffuse  * diff * sampleDiffuse();
    vec3 specular = light.specular * spec * sampleSpecular();

    return (ambient + diffuse + specular) * attenuation * edge;
}

void main() {
    TexCoords2 = TexCoords3.xy;
    vec3 color = vec3(0.0);
    vec4 finalColor = vec4(0.0);

    if (useCubeMap == 1)
    {
        finalColor = texture(cubeMap, TexCoords3);
    } else if (useFire == 1)
    {
        vec2 frameUV = TexCoords2 / vec2(14.0, 1.0);
        if (useFireLight == 1)
        {
            frameUV.x += float(frame % 14) / 14.0;
            frameUV.y += float(frame / 1) / 1.0;
        }

        finalColor = texture(fireMap, frameUV);
        if (finalColor.a < 0.1) {
            discard;
        }
    } else
    {
        for (int i = 0; i < lightCount && i < MAX_LIGHT_COUNT; i++) {
            if (i == 0 && useFlashLight == 0) {
                continue;
            }
            if (i == 1 && useFireLight == 0) {
                continue;
            }
            switch (lights[i].type) {
                case(Ambient) : {
                    color += calcAmbient(lights[i]);
                    break;
                };
                case(Direct) : {
                    color += calcDirect(lights[i]);
                    break;
                };
                case(Point) : {
                    color += calcPoint(lights[i]);
                    break;
                };
                case(Spot) : {
                    color += calcSpot(lights[i]);
                    break;
                };
            }
        }

        if (useAlpha == 1) {
            finalColor = vec4(color, alpha);
        } else {
            finalColor = vec4(color, 1.0);
        }
    }

    if (useFog == 1) {
        float distance = length(FragPos - viewPos);
        float fogFactor = clamp((fogEnd - distance) / (fogEnd - fogStart), 0.0, 1.0);

        vec3 ret = mix(fogColor, vec3(finalColor), fogFactor);
        FragColor = vec4(ret, 1.0);
    } else {
        FragColor = finalColor;
    }
}
