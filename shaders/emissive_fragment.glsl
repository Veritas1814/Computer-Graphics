#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform vec3 emissiveColor;

void main()
{
    vec4 texColor = texture(texture_diffuse1, TexCoords);

    FragColor = vec4(texColor.rgb * emissiveColor, 1.0);
}