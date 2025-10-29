#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aOffset; // new per-instance position

out vec2 TexCoord;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec3 worldPos = aPos * 0.3 + aOffset; // smaller cube + offset position
    gl_Position = projection * view * vec4(worldPos, 1.0);
    TexCoord = aTexCoord;
}
