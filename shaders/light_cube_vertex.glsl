#version 330 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aNormal;
layout(location=2) in vec2 aTexCoord;
layout(location=3) in vec3 aOffset;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 view, projection;
uniform float cubeScale;

void main() {
    vec3 wp = aPos * cubeScale + aOffset;
    FragPos = wp;
    Normal  = normalize(aNormal);
    TexCoords = aTexCoord;
    gl_Position = projection * view * vec4(wp, 1.0);
}
