#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 3) in vec3 aOffset;

uniform mat4 lightSpaceMatrix;
uniform float cubeScale;

void main()
{
    vec3 worldPos = aPos * cubeScale + aOffset;
    gl_Position = lightSpaceMatrix * vec4(worldPos, 1.0);
}
