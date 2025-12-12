#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aOffset;

uniform mat4 view;
uniform mat4 projection;
uniform float outlineWidth;
uniform float cubeScale;

void main()
{
    vec3 center = aOffset;
    float expansion = 1.0 + outlineWidth;
    vec3 finalPos = (aPos * cubeScale * expansion) + aOffset;
    gl_Position = projection * view * vec4(finalPos, 1.0);
}