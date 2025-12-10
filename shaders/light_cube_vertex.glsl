#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aOffset;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} vs_out;

uniform float cubeScale;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

void main()
{
    vec3 worldPos = aPos * cubeScale + aOffset;

    vs_out.FragPos = worldPos;
    vs_out.Normal  = aNormal; // моделі куба вже в world space
    vs_out.TexCoords = aTexCoord;
    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(worldPos, 1.0);

    gl_Position = projection * view * vec4(worldPos, 1.0);
}
