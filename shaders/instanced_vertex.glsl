#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

layout(location = 3) in vec4 instanceModel0;
layout(location = 4) in vec4 instanceModel1;
layout(location = 5) in vec4 instanceModel2;
layout(location = 6) in vec4 instanceModel3;

out vec2 TexCoords;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    mat4 model = mat4(instanceModel0, instanceModel1, instanceModel2, instanceModel3);
    TexCoords = aTexCoords;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
