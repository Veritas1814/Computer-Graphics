#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D floorTexture;

void main()
{
    vec2 tiled = TexCoords * 0.9;
    vec3 color = texture(floorTexture, tiled).rgb;
    FragColor = vec4(color, 1.0);
}
